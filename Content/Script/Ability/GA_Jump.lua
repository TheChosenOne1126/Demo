---@type GA_Jump
local M = UnLua.Class("Ability.GameplayAbility")
local Utils = require("Utils")
local GameplayTags = require("GameplayTags")

---@param ActorInfo FGameplayAbilityActorInfo
---@param Handle FGameplayAbilitySpecHandle
---@return bool, FGameplayTagContainer
function M:K2_CanActivateAbility(ActorInfo, Handle)
    local RelevantTags = UE.FGameplayTagContainer()
    if not ActorInfo then
        return false, RelevantTags
    end

    local Avatar = ActorInfo.AvatarActor
    if not Avatar or not Avatar:IsValid() then
        return false, RelevantTags
    end

    if not Avatar:IsA(UE.ACharacter) then
        return false, RelevantTags
    end

    return Avatar:CanJump(), RelevantTags
end

function M:K2_ActivateAbility()
    self.Super.K2_ActivateAbility(self)
    
    if not self:K2_CommitAbility() then
        self:K2_EndAbility()
        return
    end

    local bOk, Avatar = xpcall(function(self)
        local Avatar = self:GetAvatarActorFromActorInfo()
        if not Avatar or not Avatar:IsValid() then
            error("invalid Avatar")
        end

        if not Avatar:IsA(UE.ACharacter) then
            error("Avatar is not ACharacter")
        end

        return Avatar
    end, function(Err)
        Utils.LogError(Err)
        return nil
    end, self)

    if not bOk then
        self:K2_EndAbility()
        return
    end

    if self:IsLocallyControlled() and not Avatar.bPressedJump then
        Avatar:Jump()
        
        local Task = UE.UAbilityTask_WaitGameplayEvent.WaitGameplayEvent(self, GameplayTags.Ability_Input_Release)
        if Task and Task:IsValid() then
            Task.EventReceived:Add(self, self.OnEventReceived)
            Task:ReadyForActivation()
        end
    end
end

---@param Payload FGameplayEventData
function M:OnEventReceived(Payload)
    if not Payload.EventTag:MatchesTagExact(GameplayTags.Ability_Input_Release) then
        return
    end

    if not Payload.InstigatorTags:HasTagExact(GameplayTags.InputTag_Jump) then
        return
    end

    local Avatar = self:GetAvatarActorFromActorInfo()
    if not Avatar or not Avatar:IsValid() or not Avatar:IsA(UE.ACharacter) then
        return
    end

    Avatar:StopJumping()
    self:K2_EndAbility()
end

return M