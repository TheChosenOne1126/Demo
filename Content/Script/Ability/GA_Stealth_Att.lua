---@type GA_Stealth_Att_C
local M = UnLua.Class("Ability.GameplayAbility")
local Utils = require("Utils")

---@param ActorInfo FGameplayAbilityActorInfo
---@param Handle FGameplayAbilitySpecHandle
---@return bool, FGameplayTagContainer
function M:K2_CanActivateAbility(ActorInfo, Handle)
    local RelevantTags = UE.FGameplayTagContainer()
    if not ActorInfo then
        return false, RelevantTags
    end

    if not Utils.HasImplementInterface(ActorInfo.AvatarActor, "BI_Stealth") then
        return false, RelevantTags
    end

    local Target = ActorInfo.AvatarActor:GetSealthTarget()
    return Target and Target:IsValid()
end

function M:K2_ActivateAbility()
    self.Super.K2_ActivateAbility(self)

    if not self:K2_CommitAbility() then
        self:K2_EndAbility()
        return
    end

    
end

return M