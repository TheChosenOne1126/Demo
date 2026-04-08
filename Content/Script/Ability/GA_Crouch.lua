---@type GA_Crouch_C
local M = UnLua.Class("Ability.GameplayAbility")
local Utils = require("Utils")
local GameplayTags = require("GameplayTags")

local ZoomOffset = 30
local ZoomSpeed = 3

---@param ActorInfo FGameplayAbilityActorInfo
function M:OnInitialized(ActorInfo)
    self.Super.OnInitialized(self, ActorInfo)

    if ActorInfo.PlayerController and ActorInfo.PlayerController:IsValid() then
        self:OnPlayerControllerSet(ActorInfo.PlayerController)
    else
        local Asc = ActorInfo.AbilitySystemComponent
        if not Asc or not Asc:IsValid() then
            Utils.LogError("invalid UDAbilitySystemComponent")
            return
        end

        if not Asc:IsA(UE.UDAbilitySystemComponent) then
            Utils.LogError("AbilitySystemComponent is not UDAbilitySystemComponent")
            return
        end

        Asc.PlayerControllerSet:Add(self, self.OnPlayerControllerSet)
    end
end

---@param PlayerController APlayerController
function M:OnPlayerControllerSet(PlayerController)
    if self.CM_Zoom then
        return
    end

    if not PlayerController or not PlayerController:IsValid() then
        Utils.LogError("invalid PlayerController")
        return
    end

    if not PlayerController:IsLocalController() then
        return
    end

    local PlayerCameraManager = PlayerController.PlayerCameraManager
    if not PlayerCameraManager or not PlayerCameraManager:IsValid() then
        Utils.LogError("invalid PlayerCameraManager")
        return
    end

    local ZoomClass = Utils.GetCameraModifierByTag(GameplayTags.Camera_Zoom)
    if not ZoomClass or not ZoomClass:IsValid() then
        Utils.LogError("invalid ZoomClass")
        return
    end

    self.CM_Zoom = PlayerCameraManager:AddNewCameraModifier(ZoomClass)
    if not self.CM_Zoom or not self.CM_Zoom:IsValid() then
        return
    end
    self.CM_Zoom:DisableModifier(true)
    self.CM_Zoom:Init(ZoomOffset, ZoomSpeed)
end

---@param ActorInfo FGameplayAbilityActorInfo
function M:OnDeInitialized(ActorInfo)
    local PlayerController = ActorInfo.PlayerController

    if PlayerController and PlayerController:IsValid() and PlayerController:IsLocalController() then
        local PlayerCameraManager = PlayerController.PlayerCameraManager
        if PlayerCameraManager and PlayerCameraManager:IsValid() and self.CM_Zoom and self.CM_Zoom:IsValid() then
            PlayerCameraManager:RemoveCameraModifier(self.CM_Zoom)
            self.CM_Zoom = nil
        end
    end

    self.Super.OnDeInitialized(self)
end

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

    return true, RelevantTags
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

    if self:IsLocallyControlled() then
        local Params = UE.FGameplayCueParameters()
        Params.Instigator = self:GetPlayerControllerFromActorInfo()

        if Avatar:CanCrouch() then
            Avatar:Crouch(false)
        else
            Avatar:UnCrouch(false)
        end
        
        local Task = UE.UAbilityTask_WaitGameplayEvent.WaitGameplayEvent(self, GameplayTags.Ability_Input_Release)
        if Task and Task:IsValid() then
            Task.EventReceived:Add(self, self.OnEventReceived)
            Task:ReadyForActivation()
        end

        self:K2_ExecuteGameplayCueWithParams(GameplayTags.GameplayCue_Crouch, Params)
    end
end

---@param Payload FGameplayEventData
function M:OnEventReceived(Payload)
    if not Payload.EventTag:MatchesTagExact(GameplayTags.Ability_Input_Release) then
        return
    end

    if not Payload.InstigatorTags:HasTagExact(GameplayTags.InputTag_Crouch) then
        return
    end

    self:K2_EndAbility()
end

return M