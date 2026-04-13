---@type UDGameplayAbility
local M = UnLua.Class()
local Utils = require("Utils")

---@param ActorInfo FGameplayAbilityActorInfo
function M:OnInitialized(ActorInfo)
    if not self.ActivationCancelTags then
        self.ActivationCancelTags = UE.FGameplayTagContainer()
    end
    self.ActivationBlockedTags:AppendTags(self.ActivationCancelTags)
end

---@param ActorInfo FGameplayAbilityActorInfo
function M:OnDeInitialized(ActorInfo)
    self.ActivationBlockedTags:RemoveTags(self.ActivationCancelTags)
    self.ActivationCancelTags:Reset(0)
end

function M:K2_ActivateAbility()
    if not self.ActivationCancelTags:IsValid() then
        return
    end

    local TagQuery = UE.UBlueprintGameplayTagLibrary.MakeGameplayTagQuery_MatchAnyTags(self.ActivationCancelTags)
    local Task = UE.UAbilityTask_WaitGameplayTagQuery.WaitGameplayTagQuery(self, TagQuery)
    if Task and Task:IsValid() then
        Task.Triggered:Add(self, self.OnTriggered)
        Task:ReadyForActivation()
    end
end

function M:OnTriggered()
    self:K2_CancelAbility()
end

---@param MontageTag FGameplayTag
---@return UAnimMontage
function M:GetMontageByTag(MontageTag)
    if not MontageTag or not MontageTag:IsValid() then
        return nil
    end

    local Avatar = self:GetAvatarActorFromActorInfo()
    if not Avatar or not Avatar:IsValid() then 
        return nil
    end

    local PawnData = Utils.GetPawnData(Avatar.PawnTag)
    if not PawnData then
        return nil
    end

    local MontageDataAsset = PawnData.MontageDataAsset
    if not MontageDataAsset or not MontageDataAsset:IsValid() then
        return nil
    end

    return MontageDataAsset.MontageDataMap:FindRef(MontageTag)
end

function M:GetPlayerControllerFromActorInfo()
    local ActorInfo = self:GetActorInfo()
    if not ActorInfo then
        return nil
    end

    return ActorInfo.PlayerController
end

return M