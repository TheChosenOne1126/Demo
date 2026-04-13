---@type GA_Stealth_Att_C
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

    local bIsOk, Target, TargetCmc, MotionWarping = xpcall(function(self)
        local OutAvatar = self:GetAvatarActorFromActorInfo()
        if not OutAvatar or not OutAvatar:IsValid() then
            error("invalid Avatar")
        end

        if not Utils.HasImplementInterface(OutAvatar, "BI_Stealth") then
            error("Avatar is not implement BI_Stealth")
        end

        local OutTarget = OutAvatar:GetSealthTarget()
        if not OutTarget or not OutTarget:IsValid() then
            error("invalid Target")
        end

        if not OutTarget:IsA(UE.ADCharacter) then
            error("Target is not ACharacter")
        end

        local OutTargetCmc = OutTarget.CharacterMovement
        if not OutTargetCmc or not OutTargetCmc:IsValid() then
            error("invalid Target CharacterMovement")
        end

        if not Utils.HasImplementInterface(OutAvatar, "BI_MotionWarp") then
            error("Avatar not implement BI_MotionWarp")
        end

        local OutMotionWarping = OutAvatar:GetMotionWarpComp()
        if not OutMotionWarping or not OutMotionWarping:IsValid() then
            error("invalid MotionWarping")
        end

        return OutTarget, OutTargetCmc, OutMotionWarping
    end, function(err)
        Utils.LogError(err)
    end, self)

    if not bIsOk then
        self:K2_EndAbility()
        return
    end

    TargetCmc:DisableMovement()
    MotionWarping:AddOrUpdateWarpTarget(Target:GetSealthWarpingTarget())
    Target:SetActorEnableCollision(false)

    -- local PawnData = Utils.GetPawnData(Target.PawnTag)
    -- if not PawnData then
    --     return nil
    -- end

    -- local MontageDataAsset = PawnData.MontageDataAsset
    -- if not MontageDataAsset or not MontageDataAsset:IsValid() then
    --     return nil
    -- end

    -- local SealthVicMontage = MontageDataAsset.MontageDataMap:FindRef(GameplayTags.Montage_Vault)
    -- Target:PlayAnimMontage(SealthVicMontage)

    -- local SealthAttMontage = self:GetMontageByTag(GameplayTags.Montage_Vault)
    -- local Task = UE.UAbilityTask_PlayMontageAndWaitCreatePlayMontageAndWaitProxy(self, nil, SealthAttMontage)
end

return M