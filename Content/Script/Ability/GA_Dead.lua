---@type GA_Dead
local M = UnLua.Class("Ability.GameplayAbility")
local GameplayTags = require("GameplayTags")

---@param EventData FGameplayEventData
function M:K2_ActivateAbilityFromEvent(EventData)
    if self:K2_CommitAbility() then
        UE.UStatics.MuteInputForAbilityAvatar(self)
        local deadMontage = self:GetMontageByTag(GameplayTags.Montage_Kwang_Dead)
        if deadMontage and deadMontage:IsValid() then
            local PlayMontageTask = self:PlayMontage(deadMontage)
            if PlayMontageTask and PlayMontageTask:IsValid() then
                PlayMontageTask.OnCompleted:Add(self, self.OnMontageCompleted)
                PlayMontageTask:ReadyForActivation()
            end
        end
    end
end

function M:OnMontageCompleted()
    self:K2_EndAbility()
end

function M:K2_OnEndAbility()
    UE.UStatics.DestroyAbilityAvatar(self)
end
 
return M