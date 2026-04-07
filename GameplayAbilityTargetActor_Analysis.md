# AGameplayAbilityTargetActor 设计分析

## 一、核心职责

`AGameplayAbilityTargetActor` 是 GAS 中**目标选取系统**的核心基类，负责：

1. **目标选取逻辑** — 如何选目标（射线、范围、地面放置等）
2. **生成标准化目标数据** — 产出 `FGameplayAbilityTargetDataHandle`，供后续 AbilityTask 消费
3. **网络同步** — 处理客户端→服务器的目标数据复制
4. **可视化反馈** — 通过 `ReticleClass`（准星/指示器）给玩家视觉提示

---

## 二、工作流程

```
GameplayAbility 激活
       ↓
AbilityTask_WaitTargetData 创建并 Spawn TargetActor
       ↓
TargetActor::StartTargeting() → 开始选取逻辑（Tick中持续更新）
       ↓
玩家确认 / 取消
       ↓
TargetDataReadyDelegate / CanceledDelegate 广播目标数据
       ↓
AbilityTask 收到数据 → 传递给 Ability 的后续逻辑（如 ApplyGameplayEffect）
```

---

## 三、关键成员分析

| 成员 | 作用 |
|------|------|
| `ShouldProduceTargetDataOnServer` | 若为 true，目标数据完全在服务器生成，客户端只需发送"确认"信号，不需要发送实际目标数据 |
| `StartLocation` | 描述目标选取的起点（通常是角色位置或某个Socket） |
| `TargetDataReadyDelegate` | 目标数据就绪时广播 |
| `CanceledDelegate` | 选取被取消时广播 |
| `Filter` | `FGameplayTargetDataFilterHandle` — 过滤不符合条件的目标 |
| `ReticleClass` | 准星/指示器类，跟随目标显示 |
| `bDebug` | 是否绘制调试信息 |
| `PrimaryPC` | 拥有此 TargetActor 的玩家控制器 |
| `OwningAbility` | 创建此 TargetActor 的技能实例 |

---

## 四、引擎内置的子类继承层次

```
AGameplayAbilityTargetActor（基类，abstract）
├── AGameplayAbilityTargetActor_Trace（射线类基类，abstract）
│   ├── AGameplayAbilityTargetActor_SingleLineTrace — 单条射线检测
│   └── AGameplayAbilityTargetActor_GroundTrace — 地面射线检测
│       └── AGameplayAbilityTargetActor_ActorPlacement — 在地面放置Actor
└── AGameplayAbilityTargetActor_Radius — 范围（球形重叠）选取
```

---

## 五、各子类功能说明

### 1. AGameplayAbilityTargetActor_Trace（射线类基类）
- 提供 `MaxRange`（最大射程）、`TraceProfile`（碰撞配置）
- 内置 `AimWithPlayerController()` — 从相机方向计算射线
- 内置 `ClipCameraRayToAbilityRange()` — 将相机射线裁剪到技能范围内
- 每 Tick 调用 `PerformTrace()` 纯虚函数更新目标
- 子类只需实现 `PerformTrace()` 即可

### 2. AGameplayAbilityTargetActor_SingleLineTrace（单射线）
- **最简单的实现**：从起点向准星方向做一条 LineTrace
- 适用场景：**枪械瞄准、技能指向性射击**

### 3. AGameplayAbilityTargetActor_GroundTrace（地面检测）
- 从相机射线打到地面，然后在命中点做球形/胶囊体碰撞检测
- 有 `CollisionRadius` 和 `CollisionHeight`
- 适用场景：**AOE技能地面指示器**（如LOL的技能范围圈）

### 4. AGameplayAbilityTargetActor_ActorPlacement（Actor放置）
- 继承自 GroundTrace，额外持有 `PlacedActorClass` 和 `PlacedActorMaterial`
- 在地面位置显示要放置的 Actor 预览
- 适用场景：**放置建筑/陷阱/召唤物**

### 5. AGameplayAbilityTargetActor_Radius（范围选取）
- 在起始位置周围做球形重叠检测
- 有 `Radius` 参数
- 适用场景：**以自身为中心的AOE技能**（如群体治疗、震地攻击）

---

## 六、实际使用示例

### 示例1：枪械射击 — 单射线目标选取

```cpp
// 在 GameplayAbility 中使用 AbilityTask 配合 SingleLineTrace
void UGA_FireWeapon::ActivateAbility(...)
{
    // 创建 WaitTargetData 任务，使用 SingleLineTrace 类型
    UAbilityTask_WaitTargetData* Task = UAbilityTask_WaitTargetData::WaitTargetData(
        this,
        FName("AimTarget"),
        EGameplayTargetingConfirmation::Instant,  // 立即确认，无需玩家手动确认
        AGameplayAbilityTargetActor_SingleLineTrace::StaticClass()
    );
    
    Task->ValidData.AddDynamic(this, &UGA_FireWeapon::OnTargetDataReady);
    Task->Cancelled.AddDynamic(this, &UGA_FireWeapon::OnTargetDataCancelled);
    Task->ReadyForActivation();
}

void UGA_FireWeapon::OnTargetDataReady(const FGameplayAbilityTargetDataHandle& Data)
{
    // 拿到目标数据后，对命中目标施加 GameplayEffect（伤害）
    ApplyGameplayEffectToTarget(GetCurrentAbilitySpecHandle(), 
        GetCurrentActorInfo(), GetCurrentActivationInfo(),
        Data, DamageEffectClass, 1.0f);
}
```

### 示例2：AOE技能 — 地面范围指示器

```cpp
// MOBA类技能：选择一个地面位置释放AOE
void UGA_Meteor::ActivateAbility(...)
{
    // 使用 GroundTrace，玩家手动确认位置
    UAbilityTask_WaitTargetData* Task = UAbilityTask_WaitTargetData::WaitTargetData(
        this,
        FName("GroundTarget"),
        EGameplayTargetingConfirmation::UserConfirmed,  // 需要玩家点击确认
        AGameplayAbilityTargetActor_GroundTrace::StaticClass()
    );
    
    Task->ValidData.AddDynamic(this, &UGA_Meteor::OnLocationConfirmed);
    Task->ReadyForActivation();
}

void UGA_Meteor::OnLocationConfirmed(const FGameplayAbilityTargetDataHandle& Data)
{
    // 从目标数据中取出命中位置，在该位置生成陨石
    if (Data.Num() > 0)
    {
        FVector TargetLocation = Data.Get(0)->GetEndPoint();
        // 在 TargetLocation 生成陨石特效和伤害区域...
    }
}
```

### 示例3：范围治疗 — 以自身为中心的Radius选取

```cpp
// 治疗技能：治疗周围所有友军
void UGA_GroupHeal::ActivateAbility(...)
{
    UAbilityTask_WaitTargetData* Task = UAbilityTask_WaitTargetData::WaitTargetData(
        this,
        FName("RadiusTarget"),
        EGameplayTargetingConfirmation::Instant,
        AGameplayAbilityTargetActor_Radius::StaticClass()
    );
    
    Task->ValidData.AddDynamic(this, &UGA_GroupHeal::OnTargetsFound);
    Task->ReadyForActivation();
}

void UGA_GroupHeal::OnTargetsFound(const FGameplayAbilityTargetDataHandle& Data)
{
    // 对范围内所有目标施加治疗 GameplayEffect
    ApplyGameplayEffectToTarget(..., Data, HealEffectClass, 1.0f);
}
```

### 示例4：放置建筑/陷阱

```cpp
// 策略类技能：在地面放置防御塔
void UGA_PlaceTurret::ActivateAbility(...)
{
    UAbilityTask_WaitTargetData* Task = UAbilityTask_WaitTargetData::WaitTargetData(
        this,
        FName("PlacementTarget"),
        EGameplayTargetingConfirmation::UserConfirmed,
        AGameplayAbilityTargetActor_ActorPlacement::StaticClass()
    );
    // TargetActor Spawn后会显示建筑预览模型，玩家确认后返回放置位置
    Task->ValidData.AddDynamic(this, &UGA_PlaceTurret::OnPlacementConfirmed);
    Task->ReadyForActivation();
}
```

### 示例5：Lyra项目 — 交互检测（自定义子类）

Lyra 项目实现了 `AGameplayAbilityTargetActor_Interact`，继承自 Trace 类：
- 重写 `PerformTrace()` 实现自定义的交互物检测逻辑
- 用于检测玩家面前可交互的物体（如门、开关、拾取物）
- 这是一个很好的**自定义 TargetActor** 的参考

---

## 七、设计优缺点

### 优点
- **标准化数据流**：所有目标选取统一产出 `FGameplayAbilityTargetDataHandle`，与 GAS 的 Effect/Cue 系统无缝配合
- **网络复制内建**：`ShouldProduceTargetDataOnServer` + `OnReplicatedTargetDataReceived` 处理了客户端/服务器之间的目标同步
- **可视化支持**：内建 Reticle（准星）系统
- **蓝图友好**：标记 `Blueprintable`，可在蓝图中子类化

### 缺点（引擎源码注释也提到了）
- **每次技能激活都 Spawn Actor，性能开销大** — 源码注释明确警告
- 实际项目中通常需要：
  - 使用对象池复用 TargetActor
  - 或者直接在 GameplayAbility/AbilityTask 中写目标选取逻辑，绕开 TargetActor
  - 或者用 `WaitTargetDataUsingActor` 复用已有实例

---

## 八、总结

`AGameplayAbilityTargetActor` 本质上是一个 **"目标选取策略"的 Actor 化封装**。它把"怎么选目标"这件事从 Ability 中解耦出来，让不同技能可以灵活组合不同的目标选取方式。在原型开发和学习 GAS 时非常有用，但在正式项目中需要注意性能优化。
