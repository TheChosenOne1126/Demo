---@type GA_Kwang_MeleeAttack_C
local M = UnLua.Class("Ability.GameplayAbility")
local GameplayTags = require("GameplayTags")
local Utils = require("Utils")

---@param self GA_Kwang_MeleeAttack_C
local function PlayMeleeMontage(self)
    if not self.MeleeAttackInfo or not self.ComboIndex or not self.MeleeAttackInfo[self.ComboIndex] then
        return
    end

    local Tag = self.MeleeAttackInfo[self.ComboIndex].MontageTag
    local Montage = self:GetMontageByTag(Tag)
    if not Montage or not Montage:IsValid() then
        Utils.LogError("invalid Montage for tag: " .. tostring(Tag))
        return
    end

    local Task = UE.UAbilityTask_PlayMontageAndWait.CreatePlayMontageAndWaitProxy(self, nil, Montage)
    if Task and Task:IsValid() then
        Task.OnCompleted:Add(self, self.OnMontageCompleted)
        Task:ReadyForActivation()
    end
end

---@param ActorInfo FGameplayAbilityActorInfo
function M:OnInitialized(ActorInfo)
    self.MeleeAttackInfo = self.MeleeAttackInfo or {
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

    local TagContainer = UE.FGameplayTagContainer.Make_Table({
        GameplayTags.Event_Dead,
        GameplayTags.Event_OnHit
    })

    self.ActivationCancelTags:AppendTags(TagContainer)
    self.Super.OnInitialized(self, ActorInfo)
end

---@param ActorInfo FGameplayAbilityActorInfo
function M:OnDeInitialized(ActorInfo)
    self.Super.OnDeInitialized(self)
    self.MeleeAttackInfo = nil
end

function M:K2_ActivateAbility()
    self.Super.K2_ActivateAbility(self)

    if self:K2_CommitAbility() then
        self.ComboIndex = 1
        self.bShouldNextCombo = false
        self.bComboInputReceived = false

        self.SweepTraceTask = UE.UDAbilityTask_SweepTrace.CreateSweepTrace(self)
        self.SweepTraceTask.ValidData:Add(self, self.SweepTraceValidData)
        self.SweepTraceTask:ReadyForActivation()

        local EventTags = {
            GameplayTags.Event_Combo_Start,
            GameplayTags.Event_Combo_End,
            GameplayTags.Event_Sweep_Start,
            GameplayTags.Event_Sweep_End,
            GameplayTags.Ability_Input_Press
        }

        for _, Tag in pairs(EventTags) do
            local Task = UE.UAbilityTask_WaitGameplayEvent.WaitGameplayEvent(self, Tag)
            if Task and Task:IsValid() then
                Task.EventReceived:Add(self, self.OnEventReceived)
                Task:ReadyForActivation()
            end
        end

        self.SyncTargetDataTask = UE.UAbilityTask_SyncTargetData.SyncTargetData(self, false)
        if self.SyncTargetDataTask and self.SyncTargetDataTask:IsValid() then
            self.SyncTargetDataTask.SyncData:Add(self, self.OnSyncTargetData)
            self.SyncTargetDataTask:ReadyForActivation()
        end

        PlayMeleeMontage(self)
    end
end

---@param Data FGameplayAbilityTargetDataHandle
---@param Tag FGameplayTag
function M:OnSyncTargetData(Data, Tag)
    
end

function M:SweepTraceValidData(Data)
    local Info = self.MeleeAttackInfo[self.ComboIndex]
    if Info then
        local effectClass = UE.UStatics.GetGameplayEffectByTag(GameplayTags.Effect_Cooldown)

        local effectSpecHandle = self:MakeOutgoingGameplayEffectSpec(effectClass)
        effectSpecHandle:AssignTagSetByCallerMagnitude(GameplayTags.SetByCaller_Attribute_Percentage_Damage, Info.Damage)
        effectSpecHandle:AssignTagSetByCallerMagnitude(GameplayTags.SetByCaller_Attribute_Percentage_ExtraDamage, Info.ExtraDamage)

        self:K2_ApplyGameplayEffectSpecToTarget(effectSpecHandle, Data)

        local EventData = UE.FGameplayEventData()
        EventData.ContextHandle = self:GetContextFromOwner(Data)

        local targetActors = UE.UAbilitySystemBlueprintLibrary.GetAllActorsFromTargetData(Data)
        for i = 1, targetActors:Length() do
            UE.UAbilitySystemBlueprintLibrary.SendGameplayEventToActor(targetActors:Get(i), GameplayTags.Event_OnHit, EventData)
        end
    end
end

function M:OnMontageCompleted()
    self:K2_EndAbility()
end

---@param bWasCancelled bool
function M:K2_OnEndAbility(bWasCancelled)
    if self:IsForRemoteClient() then
        self.ReplicatedDataInvoked:Remove(self, self.OnReplicatedDataInvoked)
    end
end

---@param Payload FGameplayEventData
function M:OnEventReceived(Payload)
    if Payload.EventTag:MatchesTagExact(GameplayTags.Event_Combo_Start) then
        self.bComboInputReceived = true
    elseif Payload.EventTag:MatchesTagExact(GameplayTags.Event_Combo_End) then
        self.bComboInputReceived = false

        if self.bShouldNextCombo then
            self.bShouldNextCombo = false
            self.ComboIndex = self.ComboIndex % #self.MeleeAttackInfo + 1
            PlayMeleeMontage(self)
        end
    elseif Payload.EventTag:MatchesTagExact(GameplayTags.Ability_Input_Press) then
        if Payload.InstigatorTags:HasTagExact(GameplayTags.InputTag_MeleeAttack) and self.bComboInputReceived then
            self.bShouldNextCombo = true
        end

        if self:IsPredictingClient() then
            local Asc = self:GetAbilitySystemComponentFromActorInfo()
            if not Asc or not Asc:IsValid() then
                Utils.LogError("invalid AbilitySystemComponent")
                return
            end

            if not Asc:IsA(UE.UDAbilitySystemComponent) then
                Utils.LogError("AbilitySystemComponent is not UDAbilitySystemComponent")
                return
            end
        end
    elseif Payload.EventTag:MatchesTagExact(GameplayTags.Event_Sweep_Start) then
        if not self.SweepTraceTask then
            return
        end
        self.SweepTraceTask:StartLogic(false)
    elseif Payload.EventTag:MatchesTagExact(GameplayTags.Event_Sweep_End) then
        if not self.SweepTraceTask then
            return
        end
        self.SweepTraceTask:StopLogic()
    end
end

return M