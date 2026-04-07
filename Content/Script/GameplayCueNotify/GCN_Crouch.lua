---@type GCN_Crouch_C
local M = UnLua.Class()
local GameplayTags = require("GameplayTags")
local Utils = require("Utils")

---@param Target AActor
---@param Parameters FGameplayCueParameters
---@param SpawnResults FGameplayCueNotify_SpawnResult
function M:OnBurst(Target, Parameters, SpawnResults)
    local PlayerController = Parameters.Instigator
    if not PlayerController or not PlayerController:IsValid() or not PlayerController:IsA(UE.APlayerController) then
        return
    end

    local PlayerCameraManager = PlayerController.PlayerCameraManager
    if not PlayerCameraManager or not PlayerCameraManager:IsValid() then
        return
    end

    local ZoomClass = Utils.GetCameraModifierByTag(GameplayTags.Camera_Zoom)
    if not ZoomClass or not ZoomClass:IsValid() then
        Utils.LogError("invalid ZoomClass")
        return
    end

    local CM_Zoom = PlayerCameraManager:FindCameraModifierByClass(ZoomClass)
    if not CM_Zoom or not CM_Zoom:IsValid() then
        return
    end

    CM_Zoom:ToggleZoom()

    if CM_Zoom:IsDisabled() then
        CM_Zoom:EnableModifier()
    end
end

return M