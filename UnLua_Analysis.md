# UnLua 插件代码架构全解析

---

## 一、模块总览

UnLua 插件由 **4 个模块** 组成：

| 模块 | 类型 | 职责 |
|---|---|---|
| **UnLua** (Runtime) | 核心运行时 | Lua VM 管理、UE 反射桥接、类型导出、委托、容器 |
| **UnLuaEditor** | 编辑器工具 | 工具栏、IntelliSense 生成、热重载监视、编辑器命令 |
| **UnLuaDefaultParamCollector** | 辅助模块 | 收集 UFUNCTION 的 C++ 默认参数值 |
| **ThirdParty/Lua** | 第三方库 | Lua 5.4 虚拟机本体 |

---

## 二、核心架构总图

```
┌─────────────────────────────────────────────────────────────────────┐
│                         IUnLuaModule                                │
│                    (UnLua 模块入口接口)                              │
│   FUnLuaModule : IUnLuaModule, FUObjectCreateListener,              │
│                  FUObjectDeleteListener                             │
│   ┌──────────┐    ┌──────────────────┐    ┌─────────────────┐      │
│   │UUnLuaSettings│  │ULuaEnvLocator    │    │ULuaModuleLocator│      │
│   │ (配置)     │──>│ (Env定位策略)     │    │ (模块定位策略)   │      │
│   └──────────┘    └───────┬──────────┘    └─────────────────┘      │
│                           │ Locate()                                │
│                           ▼                                         │
│              ┌─────────────────────────────┐                        │
│              │        FLuaEnv              │ ◄── 核心！一切的入口     │
│              │  (Lua 虚拟机环境)            │                        │
│              │                             │                        │
│              │  lua_State* L               │                        │
│              │  UUnLuaManager* Manager     │                        │
│              │  7 个 Registry              │                        │
│              │  FDanglingCheck             │                        │
│              │  FDeadLoopCheck             │                        │
│              │  FObjectReferencer ×2       │                        │
│              └─────────┬───────────────────┘                        │
│                        │ 拥有(owns)                                  │
│         ┌──────────────┼──────────────────────────┐                 │
│         ▼              ▼              ▼            ▼                 │
│  ┌────────────┐ ┌────────────┐ ┌──────────┐ ┌──────────┐          │
│  │FClassRegistry│FObjectRegistry│FDelegateRegistry│FFunctionRegistry│  │
│  │FEnumRegistry │FContainerRegistry│FPropertyRegistry│              │
│  └────────────┘ └────────────┘ └──────────┘ └──────────┘          │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 三、各层详解

### 3.1 启动层 (Module / Locator / Settings)

```
IUnLuaModule (Public 接口)
    │
    └── FUnLuaModule (Private 实现)
            │  实现 FUObjectCreateListener + FUObjectDeleteListener
            │  监听所有 UObject 的创建/销毁
            │
            ├── UUnLuaSettings (UObject, Config=UnLua)
            │     StartupModuleName        — 启动 Lua 入口模块名
            │     DeadLoopCheck            — 死循环检测超时(秒)
            │     DanglingCheck            — 悬空指针检测开关
            │     EnvLocatorClass          — Env 定位器类
            │     ModuleLocatorClass       — 模块定位器类
            │     PreBindClasses           — 启动时预绑定的 UClass 列表
            │
            ├── ULuaEnvLocator (可继承, 策略模式)
            │     │  Locate(UObject*) → FLuaEnv*
            │     │  为每个 UObject 决定它属于哪个 FLuaEnv
            │     │
            │     └── ULuaEnvLocator_ByGameInstance
            │           每个 GameInstance 独立一个 FLuaEnv（多实例支持）
            │
            └── ULuaModuleLocator (可继承, 策略模式)
                  │  Locate(UObject*) → FString (Lua模块路径)
                  │  为每个 UObject 决定它对应的 Lua 文件
                  │
                  └── ULuaModuleLocator_ByPackage
                        按 UPackage 路径缓存映射
```

**生命周期流程:**
```
StartupModule()
  → 创建 ULuaEnvLocator
  → 注册 UObject 创建/销毁监听
  → SetActive(true)
      → FLuaEnv 创建 (lua_State 初始化)

UObject 被创建时:
  → NotifyUObjectCreated()
    → 检查是否实现 IUnLuaInterface
    → EnvLocator->Locate(Object) 找到 FLuaEnv
    → FLuaEnv::TryBind(Object) 绑定

ShutdownModule()
  → SetActive(false)
  → FLuaEnv 销毁
```

---

### 3.2 核心环境层 (FLuaEnv)

`FLuaEnv` 是整个 UnLua 的中枢，持有 lua_State 和所有子系统。

```
FLuaEnv : FUObjectArray::FUObjectDeleteListener
│
├── lua_State* L                           — Lua 主线程
├── UUnLuaManager* Manager                — UObject 绑定管理器
├── ULuaModuleLocator* ModuleLocator      — Lua 模块文件定位
│
├── FClassRegistry* ClassRegistry         — UClass/UScriptStruct → Lua metatable
├── FObjectRegistry* ObjectRegistry       — UObject ↔ Lua table 映射
├── FDelegateRegistry* DelegateRegistry   — 委托绑定管理
├── FFunctionRegistry* FunctionRegistry   — ULuaFunction 调用分发
├── FContainerRegistry* ContainerRegistry — TArray/TSet/TMap 包装
├── FPropertyRegistry* PropertyRegistry   — 属性类型接口创建
├── FEnumRegistry* EnumRegistry           — UEnum → Lua table
│
├── FDanglingCheck* DanglingCheck         — 悬空指针检测
├── FDeadLoopCheck* DeadLoopCheck         — 死循环检测
│
├── FObjectReferencer AutoObjectReference — 自动 GC 引用（防止 UObject 被 UE GC）
├── FObjectReferencer ManualObjectReference — 手动 GC 引用
│
├── TMap<lua_State*, int32> ThreadToRef   — 协程 → ref 映射
├── TMap<int32, lua_State*> RefToThread   — ref → 协程 映射
│
├── TArray<FLuaFileLoader> CustomLoaders  — 自定义文件加载器链
├── TMap<FString, lua_CFunction> BuiltinLoaders — 内置模块加载器
│
└── 关键方法:
      TryBind(UObject*)          — 将 UObject 绑定到 Lua
      TryReplaceInputs(UObject*) — 替换输入绑定
      DoString(Chunk)            — 执行 Lua 代码
      GC()                       — 触发 Lua GC
      HotReload(ModuleNames)     — 热重载 Lua 模块
      NotifyUObjectDeleted()     — UObject 销毁回调
      FindEnv(lua_State*) → FLuaEnv*  — 静态查找
```

---

### 3.3 注册表层 (Registries)

7 个 Registry 各司其职，都持有 `FLuaEnv*` 反向引用。

#### 3.3.1 FClassRegistry — 类型注册

```
FClassRegistry
│
├── TMap<UStruct*, FClassDesc*> Classes
├── TMap<FName, FClassDesc*> Name2Classes
│
├── Register(UStruct*)        — 注册 UClass/UScriptStruct
├── RegisterReflectedType()   — 按名称延迟注册
├── PushMetatable(L, name)    — 将 metatable 推入栈
├── Find(TypeName/UStruct*)   — 查找已注册的类型描述
│
└──→ FClassDesc (描述一个 UClass 或 UScriptStruct)
      │  ClassName, Size, UserdataPadding
      │  bIsScriptStruct / bIsClass / bIsInterface / bIsNative
      │
      ├── TMap<FName, TSharedPtr<FFieldDesc>> Fields    — 字段缓存
      ├── TArray<TSharedPtr<FPropertyDesc>> Properties  — 属性列表
      ├── TArray<TSharedPtr<FFunctionDesc>> Functions   — 函数列表
      │
      └── RegisterField(Env, FieldName)
            → 返回 FFieldDesc (字段描述)
               │  FieldIndex > 0 → Property
               │  FieldIndex < 0 → Function
               │  IsInherited() — 是否来自父类
               │
               ├── AsProperty() → FPropertyDesc
               └── AsFunction() → FFunctionDesc
```

#### 3.3.2 FObjectRegistry — 对象注册

```
FObjectRegistry
│
├── TMap<UObject*, int32> ObjectRefs    — UObject → lua_ref 映射
│
├── Push(L, UObject*)           — 推送 UObject 到 Lua 栈
├── Bind(UObject*) → int       — 绑定，返回 Lua 引用
├── Unbind(UObject*)            — 解绑
├── IsBound(UObject*) → bool   — 是否已绑定
├── GetBoundRef(UObject*) → int — 获取绑定引用
├── AddManualRef(L, UObject*)   — 添加手动引用
├── RemoveManualRef(UObject*)   — 移除手动引用
└── NotifyUObjectDeleted()      — 对象销毁时清理
```

#### 3.3.3 FDelegateRegistry — 委托注册

```
FDelegateRegistry
│
├── TMap<void*, FDelegateInfo> Delegates
│     FDelegateInfo:
│       FProperty* / FDelegateProperty* / FMulticastDelegateProperty*
│       UFunction* SignatureFunction
│       TSharedPtr<FFunctionDesc> Desc
│       TWeakObjectPtr<UObject> Owner
│       TSet<TWeakObjectPtr<ULuaDelegateHandler>> Handlers
│       bIsMulticast
│
├── TMap<FLuaDelegatePair, TWeakObjectPtr<ULuaDelegateHandler>> CachedHandlers
│
├── Register(Delegate, Property) — 注册委托
├── Bind(L, Index, Delegate)     — 绑定 Lua 函数到单播委托
├── Add(L, Index, Delegate)      — 添加 Lua 函数到多播委托
├── Remove(L, Object, Delegate)  — 移除
├── Broadcast(L, Delegate)       — 广播多播委托
├── Execute(Handler, Params)     — 执行回调
├── Clear(Delegate)              — 清空所有绑定
│
└──→ ULuaDelegateHandler : UObject
      │  充当 UE 委托的接收者（UObject），内部转发到 Lua
      │
      ├── int32 LuaRef           — Lua 回调函数引用
      ├── TWeakObjectPtr<UObject> SelfObject — self 对象
      ├── void* Delegate         — 关联的委托指针
      │
      ├── ProcessEvent()         — 被 UE 委托系统调用，转发到 Lua
      ├── BindTo(FScriptDelegate*) — 绑定到单播
      ├── AddTo(Property, Delegate) — 添加到多播
      └── RemoveFrom()           — 从多播移除
```

#### 3.3.4 FFunctionRegistry — 函数注册

```
FFunctionRegistry
│
├── TMap<ULuaFunction*, FFunctionInfo> LuaFunctions
│     FFunctionInfo:
│       lua_Integer LuaRef     — Lua 函数引用
│       TUniquePtr<FFunctionDesc> Desc
│
├── Invoke(ULuaFunction*, Context, Stack, RESULT_DECL)
│     — 当 ULuaFunction::execCallLua 被调用时触发
│     — 查找 LuaRef 并调用对应 Lua 函数
│
└── NotifyUObjectDeleted() — 清理已销毁的 UFunction
```

#### 3.3.5 FContainerRegistry — 容器注册

```
FContainerRegistry
│
├── int MapRef  — Lua 中的弱表引用（缓存容器映射）
│
├── NewArray(L, ElementType, Flag)   → FLuaArray*
├── NewSet(L, ElementType, Flag)     → FLuaSet*
├── NewMap(L, KeyType, ValueType, Flag) → FLuaMap*
│
├── FindOrAdd(L, FScriptArray*, Type)  — 查找或创建 Array 包装
├── FindOrAdd(L, FScriptSet*, Type)    — 查找或创建 Set 包装
├── FindOrAdd(L, FScriptMap*, Key, Value) — 查找或创建 Map 包装
│
└── Remove(Container) — 移除缓存
```

#### 3.3.6 FEnumRegistry — 枚举注册

```
FEnumRegistry
│
├── TMap<UEnum*, FEnumDesc*> Enums
├── TMap<FName, FEnumDesc*> Name2Enums
│
├── Register(UEnum*) → FEnumDesc*
├── Find(name/UEnum*) → FEnumDesc*
│
└──→ FEnumDesc
      │  EnumPathName, bUserDefined
      │  GetValue(EntryName) → int64
      │  支持 UserDefinedEnum (蓝图枚举)
      └── Load() / UnLoad() — 延迟加载
```

#### 3.3.7 FPropertyRegistry — 属性类型注册

```
FPropertyRegistry
│
├── CreateTypeInterface(L, Index) → TSharedPtr<ITypeInterface>
│     根据 Lua 栈上的值类型创建对应的 ITypeInterface
│
├── 缓存常用类型:
│     BoolProperty, IntProperty, FloatProperty
│     StringProperty, NameProperty, TextProperty
│     TMap<UField*, TSharedPtr<ITypeInterface>> FieldProperties
│
└── UScriptStruct* PropertyCollector  — 用于默认参数收集
```

---

### 3.4 反射描述层 (ReflectionUtils)

这一层将 UE 的反射信息抽象为 C++ 描述对象，供 Lua metatable 使用。

```
┌─────────────────────────────────────────────────────┐
│ FClassDesc                                           │
│   描述一个 UClass / UScriptStruct / UInterface       │
│   持有 Properties[] 和 Functions[]                    │
│   RegisterField() → FFieldDesc                       │
│                                                      │
│   ┌───────────┐     ┌────────────┐                  │
│   │ FFieldDesc │     │ FFieldDesc  │                  │
│   │ FieldIndex>0│     │ FieldIndex<0│                  │
│   │ → Property │     │ → Function  │                  │
│   └─────┬─────┘     └─────┬──────┘                  │
│         ▼                  ▼                          │
│   FPropertyDesc      FFunctionDesc                    │
└─────────────────────────────────────────────────────┘

FPropertyDesc : ITypeInterface
│  描述一个 FProperty
│  GetValue(L, ContainerPtr)    — 从 C++ 读值推送到 Lua
│  SetValue(L, ContainerPtr, Index) — 从 Lua 读值写入 C++
│  IsOutParameter() / IsReturnParameter() / IsReferenceParameter()
│  Create(FProperty*) → FPropertyDesc* — 工厂方法，根据属性类型创建子类

FFunctionDesc
│  描述一个 UFunction
│  CallUE(L, NumParams)          — 从 Lua 调用 C++ UFunction
│  CallLua(L, FuncRef, SelfRef)  — 从 C++ 调用 Lua 函数
│  ExecuteDelegate(L, ...)       — 触发委托
│  BroadcastMulticastDelegate()  — 广播多播委托
│  PreCall() / PostCall()        — 参数的序列化/反序列化
│  Properties[]                  — 参数描述列表
│  ReturnPropertyIndex           — 返回值属性索引
│  LatentPropertyIndex           — Latent 属性索引（异步）
│  OutPropertyIndices[]          — 输出参数索引列表

FEnumDesc
│  描述一个 UEnum
│  GetValue(EntryName) → int64
│  支持 UserDefinedEnum（蓝图枚举）
```

---

### 3.5 UFunction 覆写层 (ULuaFunction)

```
ULuaFunction : UFunction
│  UnLua 动态创建的 UFunction 子类，替换原始 UFunction 的 NativeFunc
│
├── static Override(UFunction*, UClass*, NewName)
│     将原始 UFunction 的 NativeFunc 替换为 execCallLua
│     创建 ULuaFunction 插入到 UClass 的函数链中
│
├── static RestoreOverrides(UClass*)
│     退出 PIE 时还原所有覆写
│
├── static SuspendOverrides(UClass*)
│     PIE 保存时临时挂起（避免序列化 ULuaFunction）
│
├── static ResumeOverrides(UClass*)
│     PIE 保存后恢复
│
├── static GetOverridableFunctions(UClass*, Functions)
│     获取所有可覆写的 UFUNCTION
│
├── DECLARE_FUNCTION(execCallLua)
│     自定义 thunk 函数 → FFunctionRegistry::Invoke()
│     → FFunctionDesc::CallLua() → lua_pcall
│
├── TWeakObjectPtr<UFunction> Overridden  — 被覆写的原始 UFunction
└── TSharedPtr<FFunctionDesc> Desc        — 函数描述
```

**覆写流程:**
```
UObject 绑定时:
  UUnLuaManager::Bind()
    → BindClass(UClass*, ModuleName)
      → 加载 Lua 模块, 获取 Lua 函数列表
      → 遍历 UClass 的 UFUNCTION
      → 对每个在 Lua 中有同名函数的 UFUNCTION:
          ULuaFunction::Override(OriginalFunc, Class, LuaFuncName)
            → 创建 ULuaFunction
            → 设置 NativeFunc = execCallLua
            → 挂到 UClass FuncMap 中

运行时 UFunction 被调用:
  UObject::ProcessEvent(ULuaFunction)
    → execCallLua (thunk)
      → FFunctionRegistry::Invoke(ULuaFunction, Context, Stack)
        → FFunctionDesc::CallLua(L, LuaRef, SelfRef)
          → lua_pcall 调用 Lua 函数
```

---

### 3.6 UObject 绑定层 (UUnLuaManager)

```
UUnLuaManager : UObject
│
├── FLuaEnv* Env
│
├── Bind(UObject*, ModuleName, InitializerTableRef)
│     核心绑定入口:
│     1. BindClass() — 确保 UClass 已注册（覆写 UFunction）
│     2. ObjectRegistry->Bind() — 在 Lua 中为对象创建 table
│     3. 调用 Lua 模块的构造函数
│
├── BindClass(UClass*, ModuleName, Error)
│     1. require(ModuleName) 加载 Lua 模块
│     2. 遍历 Lua table 函数名
│     3. 对每个与 UFUNCTION 同名的函数: ULuaFunction::Override()
│
├── ReplaceInputs(Actor, InputComponent)
│     替换输入绑定（Action/Axis/Key/Touch/Gesture）
│     将 BP 的输入事件转发到 Lua 函数
│
├── GetDefaultInputs() / CleanupDefaultInputs()
│     管理默认输入映射
│
├── NotifyUObjectDeleted(Object)
│     对象销毁时清理绑定
│
├── OnLatentActionCompleted(LinkID)
│     Latent Action 完成回调
│
├── FClassBindInfo — 每个 UClass 的绑定信息
│     UClass*, ModuleName, TableRef
│     TSet<FName> LuaFunctions       — Lua 中定义的函数名集合
│     TMap<FName, UFunction*> UEFunctions — UE 侧的 UFUNCTION 映射
│
└── UFUNCTION(BlueprintImplementableEvent)
      InputAction, InputAxis, InputTouch 等
      — 用于输入替换的占位函数签名
```

---

### 3.7 绑定接口层 (IUnLuaInterface)

```
IUnLuaInterface (UINTERFACE)
│
├── GetModuleName() → FString
│     返回该类对应的 Lua 模块路径
│     例如: "Weapon.BP_DefaultProjectile_C"
│
└── RunInEditor() → bool  (Editor Only)
      是否在编辑器中运行

使用方式:
  UCLASS()
  class AMyActor : public AActor, public IUnLuaInterface
  {
      GENERATED_BODY()
      FString GetModuleName_Implementation() const override
      {
          return TEXT("MyActor");
      }
  };
```

---

### 3.8 Lua 值包装层 (LuaValue)

```
UnLua::FLuaIndex
│  Env + Index (栈索引)
│
├── UnLua::FLuaValue : FLuaIndex
│     通用 Lua 值包装
│     Value<T>() — 提取为 C++ 类型
│     operator T() — 隐式转换
│     GetType() → LUA_Txxx
│
├── UnLua::FLuaTable : FLuaIndex
│     Lua table 包装
│     operator[](int/double/string/pointer/index) → FLuaValue
│     Call(FuncName, Args...) → FLuaRetValues
│     Length() → int32
│
├── UnLua::FLuaFunction
│     Lua 函数包装（持有 registry ref）
│     构造: FLuaFunction(Env, "GlobalFunc")
│            FLuaFunction(Env, "Table", "Func")
│            FLuaFunction(Env, FLuaTable, "Func")
│            FLuaFunction(Env, FLuaValue)
│     Call(Args...) → FLuaRetValues
│     析构时自动 luaL_unref
│
└── UnLua::FLuaRetValues
      函数返回值集合
      operator[](i) → FLuaValue
      Num() → int32
      IsValid() → bool
      析构时自动 pop 栈
```

---

### 3.9 类型导出层 (Static Export System)

通过宏在 C++ 中静态导出类型到 Lua，无需 UFUNCTION/UPROPERTY 反射。

```
接口层次:
  IExportedClass    — 导出类接口
  IExportedFunction — 导出函数接口
  IExportedProperty — 导出属性接口（: ITypeOps）
  IExportedEnum     — 导出枚举接口
  ITypeInterface    — 类型接口（: ITypeOps）
  ITypeOps          — 类型操作基接口（Read/Write Lua栈）

┌─ IExportedClass ─────────────────────────────────────────────┐
│                                                               │
│  TExportedClassBase<bIsReflected>                             │
│    │  Name, SuperClassName                                    │
│    │  Properties[], Functions[], GlueFunctions[]               │
│    │  Register(L) — 创建/注册 metatable                       │
│    │  AddLib(luaL_Reg*) — 添加 C 函数库                       │
│    │                                                          │
│    └── TExportedClass<bIsReflected, ClassType, CtorArgs...>   │
│          AddProperty()         ← ADD_PROPERTY(Prop)           │
│          AddStaticProperty()   ← ADD_STATIC_PROPERTY(Prop)    │
│          AddFunction()         ← ADD_FUNCTION(Func)           │
│          AddStaticFunction()   ← ADD_STATIC_FUNCTION(Func)    │
│          AddStaticCFunction()  ← ADD_STATIC_CFUNTION(Func)    │
│          AddBitFieldBoolProperty()                             │
│          AddSharedPtrConstructor<Mode>()                       │
└──────────────────────────────────────────────────────────────┘

┌─ IExportedFunction ──────────────────────────────────────────┐
│  TExportedFunction<RetType, ArgType...>    — 全局函数         │
│  TExportedMemberFunction<Class,Ret,Args>   — 成员函数         │
│  TExportedStaticMemberFunction<Ret,Args>   — 静态成员函数     │
│  TConstructor<ClassType, ArgType...>       — 构造函数         │
│  TSmartPtrConstructor<SmartPtr, Class, Args> — 智能指针构造   │
│  TDestructor<ClassType>                    — 析构函数         │
│  FGlueFunction (lua_CFunction 包装)       — 胶水函数          │
└──────────────────────────────────────────────────────────────┘

┌─ IExportedProperty ──────────────────────────────────────────┐
│  FExportedProperty (基类, TSharedFromThis)                    │
│  TExportedProperty<T>              — 普通属性                 │
│  TExportedStaticProperty<T>        — 静态属性                 │
│  TExportedArrayProperty<T>         — 数组属性                 │
│  FExportedBitFieldBoolProperty     — 位域 bool 属性           │
└──────────────────────────────────────────────────────────────┘

全局注册函数 (Binding.h):
  ExportClass(IExportedClass*)
  ExportEnum(IExportedEnum*)
  ExportFunction(IExportedFunction*)
  AddType(Name, TypeInterface)
  FindExportedClass / FindExportedReflectedClass / FindExportedNonReflectedClass
```

**常用导出宏:**
```cpp
// 导出反射类（UClass/UScriptStruct），自动关联 UE 反射 metatable
BEGIN_EXPORT_REFLECTED_CLASS(UMyComponent)
    ADD_FUNCTION(MyFunc)
    ADD_FUNCTION_EX("Overloaded", int, MyFunc, float, int)
    ADD_PROPERTY(MyProperty)
    ADD_LIB(MyLib)  // luaL_Reg 数组
END_EXPORT_CLASS()
IMPLEMENT_EXPORTED_CLASS(UMyComponent)

// 导出非反射类
BEGIN_EXPORT_CLASS(FMyStruct)
    ADD_FUNCTION(Method)
    ADD_STATIC_FUNCTION(StaticMethod)
    ADD_EXTERNAL_FUNCTION_EX("ExtFunc", RetType, ExternalFunction, ArgTypes...)
END_EXPORT_CLASS()
IMPLEMENT_EXPORTED_CLASS(FMyStruct)

// 导出枚举
BEGIN_EXPORT_ENUM(EMyEnum)
    ADD_SCOPED_ENUM_VALUE(Value1)
    ADD_SCOPED_ENUM_VALUE(Value2)
END_EXPORT_ENUM(EMyEnum)
```

---

### 3.10 容器包装层 (Containers)

```
FLuaArray
│  包装 FScriptArray (与 TArray 内存布局相同)
│  OwnedBySelf / OwnedByOther 标记
│  Num(), Add(), Remove(), Insert(), Get(), Set(), Clear()
│  ElementCache — 单元素缓存
│  Inner: TSharedPtr<ITypeInterface> — 元素类型

FLuaSet
│  包装 FScriptSet (与 TSet 内存布局相同)
│  Num(), Add(), Remove(), Contains(), Clear()
│  ElementInterface: TSharedPtr<ITypeInterface>

FLuaMap
│  包装 FScriptMap (与 TMap 内存布局相同)
│  Num(), Add(), Remove(), Find(), Clear()
│  KeyInterface + ValueInterface: TSharedPtr<ITypeInterface>
│  支持迭代器 FLuaMapEnumerator

TLuaContainerInterface<T> (LuaContainerInterface.h)
│  持有 Inner/Extra 的 ITypeInterface
│  用于在不知道具体模板参数时操作容器
```

---

### 3.11 Lua ↔ C++ 类型转换层 (UnLuaLegacy.h)

```
namespace UnLua:

Push(L, Value, bCopy) — 将 C++ 值推送到 Lua 栈
  │  基本类型: int8~int64, uint8~uint64, float, double, bool → lua_pushinteger/number/boolean
  │  字符串: char*, FString, FName, FText → lua_pushstring
  │  指针: void*, UObject* → lua_pushlightuserdata / PushUObject
  │  容器: TArray<T>, TSet<T>, TMap<K,V> → PushArray/Set/Map
  │  枚举: TIsEnum<T> → lua_pushinteger
  │  结构体: 其他类型 → TGenericTypeHelper::Push
  │           bCopy=true: NewUserdata + placement new (复制)
  │           bCopy=false: PushPointer (引用)
  │  智能指针: TSharedPtr/TSharedRef → PushSmartPointer
  │  TSubclassOf<T> → PushUObject(V.Get())

Get(L, Index, TType<T>) — 从 Lua 栈获取 C++ 值
  │  基本类型 → lua_tointeger/tonumber/toboolean
  │  字符串 → lua_tostring → FString/FName/FText
  │  指针 → GetPointer / GetUObject
  │  容器 → GetArray/Set/Map

TPointerHelper<T, IsUObject>
  │  true  → PushUObject / Cast<T>(GetUObject)
  │  false → PushPointer / GetPointer

TGenericTypeHelper<T, IsEnum>
  │  Enum  → lua_pushinteger / lua_tointeger
  │  Other → NewUserdata(bCopy) or PushPointer
```

---

### 3.12 底层工具层 (LuaCore.h)

```
LuaCore.h (Private):
  ── Userdata 管理 ──
  NewUserdataWithPadding(L, Size, MetatableName, Padding)
  NewTypedUserdata(L, Type)   — 宏，= NewUserdataWithPadding(sizeof(T), #T, ...)
  GetUserdata(L, Index)       — 获取 userdata 指针
  GetCppInstance(L, Index)    — 获取 C++ 实例指针
  NewUserdataWithTwoLvPtrTag() — 两级指针 userdata
  MarkUserdataTwoLvPtrTag()    — 标记两级指针
  SetUserdataFlags()           — 设置 userdata 标记

  ── UObject/函数调用 ──
  PushObjectCore(L, UObject*)      — 内部推送 UObject
  PushFunction(L, Object, FuncName)  — 推送函数 + self (遍历 Super 链)
  PushFunction(L, Object, FuncRef)   — 推送函数 ref + self
  CallFunction(L, NumArgs, NumResults) — lua_pcall + 错误处理
  GetDelegateInfo(L, Index, Object, Function)
  GetObjectMapping(L, Object)       — 获取 UObject 的 Lua 映射

  ── 脚本容器 ──
  NewScriptContainer / CacheScriptContainer / GetScriptContainer
  RemoveCachedScriptContainer

  ── Lua metatable 操作 ──
  SetTableForClass(L, Name)
  TryToSetMetatable(L, MetatableName)
  Class_Index / Class_NewIndex / Class_CallUFunction / Class_CallLatentFunction
  Class_StaticClass / Class_Cast
  ScriptStruct_New / ScriptStruct_Delete / ScriptStruct_Copy
  Enum_Index / Enum_Delete / Enum_GetMaxValue

  ── 碰撞枚举注册 ──
  RegisterECollisionChannel / RegisterEObjectTypeQuery / RegisterETraceTypeQuery

LowLevel.h (Public):
  LowLevel::AbsIndex(L, Index)
  LowLevel::PushEmptyIterator(L)
  LowLevel::IsReleasedPtr(Ptr)
  LowLevel::CreateWeakKeyTable / CreateWeakValueTable
  LowLevel::GetMetatableName(UObject* / UStruct*)
  LowLevel::GetFunctionNames(L, TableRef, FunctionNames)
  LowLevel::GetLoadedModule(L, ModuleName)
  LowLevel::CheckPropertyOwner(L, Property, ContainerPtr)
```

---

### 3.13 安全检查层

```
FDanglingCheck (悬空指针检测)
│  在 UFunction 调用 Lua 前后使用 RAII Guard
│  CaptureStruct(L, Value) — 捕获当前帧访问的 struct 指针
│  CaptureContainer(L, Value) — 捕获容器指针
│  Guard 析构时检查这些指针是否仍有效
│  开关: UUnLuaSettings::DanglingCheck / FDanglingCheck::Enabled

FDeadLoopCheck (死循环检测)
│  使用独立线程监视 Lua 执行时间
│  FDeadLoopCheck::FRunner : FRunnable — 监视线程
│  FDeadLoopCheck::FGuard — RAII Guard
│    超时 → lua_sethook 中断 → 抛出 Lua error
│  配置: UUnLuaSettings::DeadLoopCheck (秒)
│  Timeout=0 表示关闭
```

---

### 3.14 其他辅助类

```
FObjectReferencer : FGCObject
│  TSet<UObject*> ReferencedObjects
│  防止被 UE GC 回收（手动/自动引用计数）
│  AddReferencedObjects() → 告诉 UE GC 这些对象还在用

FUnLuaDelegates (全局委托集合)
│  OnLuaStateCreated          — Lua state 创建后
│  OnLuaContextInitialized    — Lua 上下文初始化后
│  OnPreLuaContextCleanup     — 清理前
│  OnPostLuaContextCleanup    — 清理后
│  OnPreStaticallyExport      — 静态导出前
│  OnObjectBinded             — UObject 绑定后
│  OnObjectUnbinded           — UObject 解绑后
│  HotfixLua                  — 热更新委托
│  ReportLuaCallError         — 错误报告委托
│  ConfigureLuaGC             — GC 配置委托
│  CustomLoadLuaFile          — 自定义文件加载

FLuaDynamicBinding
│  运行时动态绑定 UClass → Lua 模块
│  支持栈式 Push/Pop（SpawnActor 时临时绑定）
│  FScopedLuaDynamicBinding — RAII 作用域绑定

UUnLuaLatentAction : UObject, FTickableGameObject
│  Latent Action 包装
│  Callback → OnCompleted → FLuaEnv 恢复协程
│  支持暂停时 Tick

UUnLuaFunctionLibrary : UBlueprintFunctionLibrary
│  GetScriptRootPath()        — 获取脚本根目录
│  HotReload()                — 蓝图可调用的热重载
│  AddConsoleCommand / RemoveConsoleCommand

FUnLuaConsoleCommands
│  UnLua.Do "code"            — 执行 Lua 代码
│  UnLua.DoFile "path"        — 执行 Lua 文件
│  UnLua.CollectGarbage       — 触发 Lua GC

IHotReloadWatcher (接口)
│  Watch(Directories) — 监视文件变化
│  └── EditorHotReloadWatcher (Editor模块实现)

FLuaDebugValue / FLuaVariable (调试)
│  用于调试器获取 Lua 局部变量和 upvalue
│  递归展开 table/userdata/UStruct/TArray/TMap/TSet
```

---

### 3.15 BaseLib / MathLib / UELib (内置 Lua 库)

这些是 UnLua 为常用 UE 类型在 Lua 侧注册的 C 函数库。

```
BaseLib/ (基础类型)
  LuaLib_Object.cpp       — UObject: GetName, GetClass, IsValid, Cast 等
  LuaLib_Class.cpp        — UClass: Load, GetDefaultObject, ClassOf 等
  LuaLib_World.cpp        — UWorld: SpawnActor, GetTimeSeconds 等
  LuaLib_Delegate.cpp     — FScriptDelegate: Bind, Execute, Unbind
  LuaLib_MulticastDelegate.cpp — 多播委托: Add, Remove, Broadcast, Clear
  LuaLib_Array.cpp        — TArray: Add, Remove, Get, Set, Num, Insert 等
  LuaLib_Set.cpp          — TSet: Add, Remove, Contains, Num 等
  LuaLib_Map.cpp          — TMap: Add, Remove, Find, Num, Keys, Values 等
  LuaLib_PrimitiveTypes.cpp — 基本类型元方法
  LuaLib_DataTable.cpp    — DataTable 支持
  LuaLib_Key.cpp          — FKey 支持
  LuaLib_File.cpp         — 文件操作

MathLib/ (数学类型)
  LuaLib_FVector.cpp      — FVector: +, -, *, /, Dot, Cross, Normalize 等
  LuaLib_FVector2.cpp     — FVector2D
  LuaLib_FVector4.cpp     — FVector4
  LuaLib_FRotator.cpp     — FRotator
  LuaLib_FQuat.cpp        — FQuat
  LuaLib_FTransform.cpp   — FTransform
  LuaLib_FColor.cpp       — FColor
  LuaLib_FLinearColor.cpp — FLinearColor
  LuaLib_FIntPoint.cpp    — FIntPoint
  LuaLib_FIntVector.cpp   — FIntVector

UELib.cpp (全局 UE 库)
  注册到 Lua 全局表 "UE":
  UE.Log, UE.NewObject, UE.GetWorld, UE.IsValid, UE.LoadClass 等
```

---

## 四、完整类型关系图

```
                                ┌──────────────┐
                                │ IUnLuaModule  │
                                │ (模块入口)    │
                                └──────┬───────┘
                                       │
                        ┌──────────────┤
                        │              │
                ┌───────┴──────┐ ┌─────┴───────────┐
                │ULuaEnvLocator│ │  UUnLuaSettings  │
                │(Env定位策略) │ │  (配置)          │
                └───────┬──────┘ └─────────────────┘
                        │
                        ▼
                 ┌──────────────┐
                 │   FLuaEnv    │◄──── 核心中枢
                 └──┬───────────┘
                    │
    ┌───────────────┼───────────────────────────────────┐
    │               │               │                    │
    ▼               ▼               ▼                    ▼
┌─────────┐  ┌───────────┐  ┌────────────┐  ┌─────────────────┐
│FClass   │  │FObject    │  │FDelegate   │  │FFunction        │
│Registry │  │Registry   │  │Registry    │  │Registry         │
└────┬────┘  └─────┬─────┘  └─────┬──────┘  └────────┬────────┘
     │             │              │                   │
     ▼             │              ▼                   ▼
┌─────────┐        │      ┌──────────────┐   ┌───────────────┐
│FClassDesc│        │      │ULuaDelegate  │   │ ULuaFunction  │
│ ├ FFieldDesc      │      │  Handler     │   │ : UFunction   │
│ ├ FPropertyDesc   │      └──────────────┘   │ execCallLua   │
│ └ FFunctionDesc   │                         └───────────────┘
└─────────┘        │
                    ▼
    ┌───────────────────────┐
    │   UUnLuaManager       │
    │   Bind / BindClass    │
    │   ReplaceInputs       │
    │   (绑定协调器)         │
    └───────────────────────┘

    ┌────────────────┐  ┌──────────────┐  ┌──────────────┐
    │FContainerRegistry│  │FEnumRegistry │  │FPropertyRegistry│
    │ ├ FLuaArray     │  │ ├ FEnumDesc   │  │ ├ ITypeInterface│
    │ ├ FLuaSet       │  │ └ UEnum映射   │  │ └ 按类型缓存    │
    │ └ FLuaMap       │  └──────────────┘  └──────────────┘
    └────────────────┘

静态导出体系 (编译期):
    IExportedClass ← TExportedClassBase ← TExportedClass
    IExportedFunction ← TExportedFunction / TExportedMemberFunction / FGlueFunction
    IExportedProperty ← FExportedProperty ← TExportedProperty<T>
    IExportedEnum ← FExportedEnum
    ITypeInterface ← TTypeInterface<T> / FPropertyDesc
    ITypeOps ← ITypeInterface / IExportedProperty

Lua 值包装:
    FLuaIndex ← FLuaValue
              ← FLuaTable
    FLuaFunction (独立, 持有 FunctionRef)
    FLuaRetValues (函数返回值集合)

安全机制:
    FDanglingCheck + FDanglingCheck::FGuard (RAII)
    FDeadLoopCheck + FDeadLoopCheck::FGuard + FDeadLoopCheck::FRunner (线程)

辅助:
    FObjectReferencer : FGCObject (防 UE GC)
    FLuaDynamicBinding / FScopedLuaDynamicBinding (运行时动态绑定)
    FUnLuaDelegates (全局事件委托)
    UUnLuaLatentAction (异步 Action)
    ULuaModuleLocator (模块路径定位)
    IHotReloadWatcher (热重载文件监视)
    FUnLuaConsoleCommands (控制台命令)
    UnLua::LowLevel (底层工具函数)
```

---

## 五、数据流：从 Lua 调用 UFunction 全流程

```
Lua: self:DoSomething(arg1, arg2)
│
├─① Lua __index → Class_Index(L)
│     查找 FFieldDesc → IsFunction() → FFunctionDesc
│     push Class_CallUFunction 作为 closure (upvalue = FFunctionDesc*)
│
├─② Lua call → Class_CallUFunction(L)
│     取出 FFunctionDesc* 从 upvalue
│     调用 FFunctionDesc::CallUE(L, NumParams)
│
├─③ FFunctionDesc::CallUE()
│     PreCall(): 从 Lua 栈读取参数 → 写入 UFunction 参数缓冲区
│     UObject::ProcessEvent(UFunction, ParamBuffer)  ← 真正调用 UE
│     PostCall(): 将 out 参数 / 返回值推回 Lua 栈
│
└─④ 返回值回到 Lua
```

## 六、数据流：从 C++ 调用 Lua 函数全流程

```
C++: UObject::ProcessEvent(ULuaFunction, Params)
│
├─① ULuaFunction::execCallLua (thunk)
│     被 UE 的 ProcessEvent 调用
│
├─② FFunctionRegistry::Invoke(ULuaFunction, Context, Stack, RESULT_DECL)
│     查找 LuaRef (函数在 Lua registry 中的引用)
│     查找 SelfRef (UObject 对应的 Lua table 引用)
│
├─③ FFunctionDesc::CallLua(L, FuncRef, SelfRef, Stack, RESULT_DECL)
│     从 UE 的 FFrame 栈中读取 C++ 参数
│     Push 到 Lua 栈
│     lua_pcall(L, NumArgs, NumResults, ErrHandler)
│
├─④ Lua 函数执行
│     function MyActor:DoSomething(arg1, arg2)
│         ...
│         return result
│     end
│
└─⑤ 返回值从 Lua 栈读取，写回 RESULT_DECL / OutParms
```
