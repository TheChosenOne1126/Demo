---@type BP_Player_C
local M = UnLua.Class()
local Utils = require("Utils")

local StealthOffset = {
    Forward = -80,
    Right = -25,
    Up = -90
}

function M:UserConstructionScript()
    if self:HasAuthority() then
        self.MotionWarping = self:AddComponentByClass(UE.UMotionWarpingComponent, false, self:GetTransform(), false)
        self.MotionWarping:SetComponentTickEnabled(true)

        if not UE.UKismetSystemLibrary.IsStandalone(self) then
            self.MotionWarping:SetIsReplicated(false)
        end
    end
end

function M:OnRep_Controller()
    self.MotionWarping = self:AddComponentByClass(UE.UMotionWarpingComponent, false, self:GetTransform(), false)
    self.MotionWarping:SetComponentTickEnabled(true)
end

---@comment override BI_Stealth
---@return FMotionWarpingTarget
function M:GetSealthWarpingTarget()
    local WarpingTarget = UE.FMotionWarpingTarget()
    WarpingTarget.Name = "Stealth"
    WarpingTarget.Rotation = self:K2_GetActorRotation()
    WarpingTarget.Location = self:K2_GetActorLocation()
        + self:GetActorForwardVector() * StealthOffset.Forward
        + self:GetActorRightVector() * StealthOffset.Right
        + self:GetActorUpVector() * StealthOffset.Up
    return WarpingTarget
end

---@comment override BI_MotionWarp
---@return UMotionWarpingComponent
function M:GetMotionWarpComp()
    return self.MotionWarping
end

---@comment override BI_Stealth
---@return AActor
function M:GetStealthTarget()
    return self.StealthTarget
end

return M