---@type GA_Vault_C
local M = UnLua.Class("Ability.GameplayAbility")
local Utils = require("Utils")
local GameplayTags = require("GameplayTags")

local TraceCount = 3

local function TriggerVault(self)
    if not self.VaultLand or not self.VaultStart or not self.VaultMiddle then
        self:K2_EndAbility()
        return
    end

    local bOk, Avatar, Montage, MotionWarping = xpcall(function(self)
        local Montage = self:GetMontageByTag(GameplayTags.Montage_Vault)
        if not Montage or not Montage:IsValid() then
            error("invalid Montage")
        end

        local Avatar = self:GetAvatarActorFromActorInfo()
        if not Utils.HasImplementInterface(Avatar, "BI_MotionWarp") then
            error("Avatar not implement BI_MotionWarp")
        end

        local MotionWarping = Avatar:GetMotionWarpComp()
        if not MotionWarping or not MotionWarping:IsValid() then
            error("invalid MotionWarpingComponent")
        end

        return Avatar, Montage, MotionWarping
    end, function(Err)
        Utils.LogError(Err)
    end, self)

    if not bOk then
        self:K2_EndAbility()
        return
    end

    local Rotation = Avatar:K2_GetActorRotation()
    local MotionWarpingInfos = {
        VaultStart = self.VaultStart,
        VaultMiddle = self.VaultMiddle,
        VaultLand = self.VaultLand
    }

    for MotionWarpingName, HitResult in pairs(MotionWarpingInfos) do
        local WarpingTarget = UE.FMotionWarpingTarget()
        WarpingTarget.Name = MotionWarpingName
        WarpingTarget.Rotation = Rotation
        WarpingTarget.Location = HitResult.Location
        MotionWarping:AddOrUpdateWarpTarget(WarpingTarget)
    end
        
    local Task = UE.UAbilityTask_PlayMontageAndWait.CreatePlayMontageAndWaitProxy(self, nil, Montage)
    if Task and Task:IsValid() then
        Task.OnCompleted:Add(self, self.OnMontageCompleted)
        Task:ReadyForActivation()
    end

    if self:IsPredictingClient() then
        if not self.SyncTargetDataTask or not self.SyncTargetDataTask:IsValid() then
            Utils.LogError("invalid SyncTargetDataTask")
            return
        end

        local Data = UE.FGameplayAbilityTargetDataHandle()
        for _, HitResult in pairs(MotionWarpingInfos) do
            local TargetData = UE.UAbilitySystemBlueprintLibrary.AbilityTargetDataFromHitResult(HitResult)
            Data = UE.UAbilitySystemBlueprintLibrary.AppendTargetDataHandle(Data, TargetData)
        end
        self.SyncTargetDataTask:CallServerSetTargetData(Data, UE.FGameplayTag())
    end
end

---@param ActorInfo FGameplayAbilityActorInfo
function M:OnInitialized(ActorInfo)
    UnLua.ConsoleCommand.Add("Ability.Vault.DrawDebugType", "Draw vault sweep trace deubg type", function(Args)
        local Type = tonumber(Args[1])
        if Type == 0 then
            self.DrawDebugType = UE.EDrawDebugTrace.None
        elseif Type == 1 then
            self.DrawDebugType = UE.EDrawDebugTrace.ForOneFrame
        elseif Type == 2 then
            self.DrawDebugType = UE.EDrawDebugTrace.ForDuration
        elseif Type == 3 then
            self.DrawDebugType = UE.EDrawDebugTrace.Persistent
        else
            Utils.LogError(string.format("invalid DrawDebugType: %d", Type))
        end
    end)

    self.ActivationCancelTags = UE.FGameplayTagContainer.Make_Table({
        GameplayTags.Event_Dead,
        GameplayTags.Event_OnHit
    })

    self.Super.OnInitialized(self, ActorInfo)
end

function M:K2_ActivateAbility()
    self.Super.K2_ActivateAbility(self)

    if not self:K2_CommitAbility() then
        self:K2_EndAbility()
        return
    end
    
    local VaultTags = {
        GameplayTags.Event_Vault_Start,
        GameplayTags.Event_Vault_End
    }

    for _, Tag in pairs(VaultTags) do
        local Task = UE.UAbilityTask_WaitGameplayEvent.WaitGameplayEvent(self, Tag)
        if Task and Task:IsValid() then
            Task.EventReceived:Add(self, self.OnEventReceived)
            Task:ReadyForActivation()
        end
    end

    self.SyncTargetDataTask = UE.UAbilityTask_SyncTargetData.SyncTargetData(self, true)
    if self.SyncTargetDataTask and self.SyncTargetDataTask:IsValid() then
        self.SyncTargetDataTask.SyncData:Add(self, self.OnSyncTargetData)
        self.SyncTargetDataTask:ReadyForActivation()
    end

    if self:IsLocallyControlled() then
        local bOk, Avatar, CapsuleComp = xpcall(function(self)
            local Avatar = self:GetAvatarActorFromActorInfo()
            if not Avatar or not Avatar:IsValid() then
                error("invalid Avatar")
            end

            if not Avatar:IsA(UE.ACharacter) then
                error("Avatar is not ACharacter")
            end

            local CapsuleComp = Avatar.CapsuleComponent
            if not CapsuleComp or not CapsuleComp:IsValid() then
                error("invalid CapsuleComponent")
            end

            return Avatar, CapsuleComp
        end, function(Err)
            Utils.LogError(Err)
        end, self)

        if not bOk then
            self:K2_EndAbility()
            return
        end

        local Radius, HalfHeight = CapsuleComp:GetScaledCapsuleSize()
        local Channel = UE.ETraceTypeQuery.Visibility

        local AvatarLocation = Avatar:K2_GetActorLocation()
        local AvatarForward = Avatar:GetActorForwardVector()
        AvatarForward:Normalize()
        local AvatarUp = Avatar:GetActorUpVector()
        AvatarUp:Normalize()

        local Start = AvatarLocation - AvatarUp * HalfHeight * 0.5
        local End = Start + AvatarForward * HalfHeight * 0.5
        local bHit, HitResult = UE.UKismetSystemLibrary.SphereTraceSingle(
            Avatar, Start, End, HalfHeight * 0.5, Channel,
            false, nil, self.DrawDebugType,
            nil, true, UE.FLinearColor(1, 1, 0))
        if not bHit then
            self:K2_EndAbility()
            return
        end

        local TraceStart = UE.FVector(HitResult.Location.X, HitResult.Location.Y, HitResult.Location.Z) + AvatarUp * HalfHeight
        local TraceEnd = TraceStart + AvatarUp * HalfHeight * 2 * -1
        for i = 1, TraceCount do
            local bTraceHit, TraceHitResult = UE.UKismetSystemLibrary.SphereTraceSingle(
                Avatar, TraceStart, TraceEnd, Radius / 2,
                Channel, false, nil, self.DrawDebugType)
            if bTraceHit == nil or TraceHitResult == nil then
                Utils.LogError("nil TraceHitResult")
                break
            end

            if not bTraceHit then
                local LandStart = TraceStart + AvatarForward * Radius * 2
                local LandEnd = LandStart + AvatarUp * HalfHeight * 100 * -1
                local bLineHit, LineHitResult = UE.UKismetSystemLibrary.LineTraceSingle(
                    Avatar, LandStart, LandEnd, Channel,
                    false, nil, self.DrawDebugType, nil,
                    true, UE.FLinearColor(0, 0, 1))
                if bLineHit and LineHitResult then
                    self.VaultLand = self.VaultLand or LineHitResult
                    break
                end
            else
                TraceEnd:Set(TraceHitResult.Location.X, TraceHitResult.Location.Y, TraceHitResult.Location.Z)
            end

            local Color = UE.FLinearColor()
            if not self.VaultStart then
                self.VaultStart = TraceHitResult
                Color:Set(1, 0, 0)
            else
                self.VaultMiddle = TraceHitResult
                Color:Set(1, 1, 0)
            end

            if self.DrawDebugType ~= UE.EDrawDebugTrace.None then
                UE.UKismetSystemLibrary.DrawDebugSphere(
                    self, TraceHitResult.Location,
                    Radius / 2, 12, Color, 10, 2)
            end

            TraceStart = TraceStart + AvatarForward * i * Radius * 2
            TraceEnd = TraceEnd + AvatarForward * i * Radius * 2
        end

        TriggerVault(self)
    end
end

---@param Payload FGameplayEventData
function M:OnEventReceived(Payload)
    local Avatar = self:GetAvatarActorFromActorInfo()
    if not Avatar or not Avatar:IsValid() then
        Utils.LogError("invalid Avatar")
        return
    end

    if not Avatar:IsA(UE.ACharacter) then
        Utils.LogError("Avatar is not ACharacter")
        return
    end

    local Cmc = Avatar.CharacterMovement
    if not Cmc or not Cmc:IsValid() then
        Utils.LogError("invalid CharacterMovementComponent")
        return
    end

    if Payload.EventTag:MatchesTagExact(GameplayTags.Event_Vault_Start) then
        Cmc:SetMovementMode(UE.EMovementMode.MOVE_Flying)
    elseif Payload.EventTag:MatchesTagExact(GameplayTags.Event_Vault_End) then
        Cmc:SetMovementMode(UE.EMovementMode.MOVE_Walking)
    end
end

---@param Data FGameplayAbilityTargetDataHandle
---@param Tag FGameplayTag
function M:OnSyncTargetData(Data, Tag)
    local Num = UE.UAbilitySystemBlueprintLibrary.GetDataCountFromTargetData(Data)
    if Num ~= 3 then
        Utils.LogError(string.format("TargetData count: [%d] not 3", Num))
        return
    end

    for i = 1, Num do
        local HitResult = UE.UAbilitySystemBlueprintLibrary.GetHitResultFromTargetData(Data, i - 1)
        if not HitResult then
            Utils.LogError(string.format("invalid HitResult at index %d", i - 1))
            return
        end

        if i == 1 then
            self.VaultStart = HitResult
        elseif i == 2 then
            self.VaultMiddle = HitResult
        elseif i == 3 then
            self.VaultLand = HitResult
        end
    end

    TriggerVault(self)
end

function M:OnMontageCompleted()
    self:K2_EndAbility()
end

function M:K2_OnEndAbility(bWasCancelled)
    self.VaultStart = nil
    self.VaultMiddle = nil
    self.VaultLand = nil

    if self.SyncTargetDataTask and self.SyncTargetDataTask:IsValid() then
        self.SyncTargetDataTask:EndTask()
    end
    self.SyncTargetDataTask = nil
end

return M