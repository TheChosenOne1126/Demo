local M = UnLua.Class()

local MeleeAttackInfo = {
    {
        Damage = 1,
        ExtraDamage = 0.25,
        MontageTag = GameplayTags.Montage_Kwang_MeleeAttackA
    },
    {
        Damage = 1.25,
        ExtraDamage = 0.5,
        MontageTag = GameplayTags.Montage_Kwang_MeleeAttackB
    },
    {
        Damage = 1.5,
        ExtraDamage = 0.75,
        MontageTag = GameplayTags.Montage_Kwang_MeleeAttackC
    },
    {
        Damage = 1.7,
        ExtraDamage = 1,
        MontageTag = GameplayTags.Montage_Kwang_MeleeAttackD
    }
}

local ComboIndex = 1
local bShouldNextCombo = false
local bComboInputReceived = false
local SweepTraceTask = nil

---@param ... any 
---[1]Rate number [2]StartSection string [3]StartTimeSeconds number
local function PlayMeleeMontage(Ability, ...)
    if not MeleeAttackInfo or not ComboIndex or not MeleeAttackInfo[ComboIndex] then
        return
    end

    local tag = MeleeAttackInfo[ComboIndex].MontageTag
    local montage = UE.UStatics.GetMontageByTag(Ability, tag)
    if not montage or not montage:IsValid() then
        return
    end

    local rate = select(1, ...) or 1
    local startSection = select(2, ...) or nil
    local startTimeSeconds = select(3, ...) or 0
    Ability:LuaPlayMontage(montage, rate, startSection, startTimeSeconds)
end

function M:Initialize(Initializer)
    local TagContainer = UE.FGameplayTagContainer()
    Utils.MakeGameplayTagContainer(TagContainer, {
        GameplayTags.Event_Dead,
        GameplayTags.Event_OnHit
    })
    self:SetupActivationCancelTags(TagContainer)
end

function M:K2_ActivateAbility()
    if self:K2_CommitAbility() then
        ComboIndex = 1
        bShouldNextCombo = false
        bComboInputReceived = false

        SweepTraceTask = UE.UDAbilityTask_SweepTrace.CreateSweepTrace(self)
        SweepTraceTask.ValidData:Add(self, self.SweepTraceValidData)
        SweepTraceTask:ReadyForActivation()

        local EventTags = UE.FGameplayTagContainer()
        Utils.MakeGameplayTagContainer(EventTags, {
            GameplayTags.Event_Combo_Start,
            GameplayTags.Event_Combo_End,
            GameplayTags.Event_Sweep_Start,
            GameplayTags.Event_Sweep_End
        })
        self:LuaWaitGameplayEvent(EventTags, false)

        local InputEventTags = UE.FGameplayTagContainer()
        Utils.MakeGameplayTagContainer(InputEventTags, {
            GameplayTags.Ability_Input_Press
        })
        self:LuaWaitGameplayEvent(InputEventTags, true)

        PlayMeleeMontage(self)
    end
end

function M:SweepTraceValidData(Data)
    local Info = MeleeAttackInfo[ComboIndex]
    if Info then
        local effectClass = UE.UStatics.GetGameplayEffectByTag(GameplayTags.Effect_Cooldown)

        ---@type FGameplayEffectSpecHandle
        local effectSpecHandle = self:MakeOutgoingGameplayEffectSpec(effectClass)

        local function SetByCallerMagnitude(Tag, Magnitude)
            return UE.UAbilitySystemBlueprintLibrary.AssignTagSetByCallerMagnitude(effectSpecHandle, Tag, Magnitude)
        end

        effectSpecHandle = SetByCallerMagnitude(GameplayTags.SetByCaller_Attribute_Percentage_Damage, Info.Damage)
        effectSpecHandle = SetByCallerMagnitude(GameplayTags.SetByCaller_Attribute_Percentage_ExtraDamage, Info.ExtraDamage)

        self:K2_ApplyGameplayEffectSpecToTarget(effectSpecHandle, Data)

        local EventData = UE.FGameplayEventData()
        EventData.ContextHandle = self:GetContextFromOwner(Data)

        local targetActors = UE.UAbilitySystemBlueprintLibrary.GetAllActorsFromTargetData(Data)
        for i = 1, targetActors:Length() do
            UE.UAbilitySystemBlueprintLibrary.SendGameplayEventToActor(targetActors:Get(i), GameplayTags.Event_OnHit, EventData)
        end
    end
end

function M:LuaOnMontageEnd(Montage)
    self:K2_EndAbility()
end

function M:LuaOnEventHandle(Tag, Data)
    if Utils.TagExactMatch(Tag, GameplayTags.Event_Combo_Start) then
        bComboInputReceived = true
    elseif Utils.TagExactMatch(Tag, GameplayTags.Event_Combo_End) then
        bComboInputReceived = false

        if bShouldNextCombo then
            ComboIndex = ComboIndex % #MeleeAttackInfo + 1
            PlayMeleeMontage(self)
            bShouldNextCombo = false
        end
    elseif Utils.TagExactMatch(Tag, GameplayTags.Ability_Input_Press) then
        if Utils.TagExactMatch(Data.EventTag, GameplayTags.InputTag_MeleeAttack) and bComboInputReceived then
            bShouldNextCombo = true
        end
    elseif Utils.TagExactMatch(Tag, GameplayTags.Event_Sweep_Start) then
        if not SweepTraceTask then
            return
        end
        SweepTraceTask:StartLogic(false)
    elseif Utils.TagExactMatch(Tag, GameplayTags.Event_Sweep_End) then
        if not SweepTraceTask then
            return
        end
        SweepTraceTask:StopLogic()
    end
end

return M