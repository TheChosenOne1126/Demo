local GameplayTags = require("GameplayTags")
local M = UnLua.Class()

function M:K2_ActivateAbilityFromEvent(EventData)
    if self:K2_CommitAbility() then
        UE.UStatics.MuteInputForAbilityAvatar(self)
        local deadMontage = UE.UStatics.GetMontageByTag(self, GameplayTags.Montage_Kwang_Dead)
        if deadMontage and deadMontage:IsValid() then
            self:AbilityPlayMontage(deadMontage)
        end
    end
end

function M:OnMontageEnded(Montage)
    self:K2_EndAbility()
end

function M:K2_OnEndAbility(bWasCancelled)
    UE.UStatics.DestroyAbilityAvatar(self)
end

return M