---@type CM_ZoomInOrOut_C
local M = UnLua.Class()

---@param InFOVOffset float
---@param InInterpSpeed float
function M:Init(InFOVOffset, InInterpSpeed)
    self.FOVOffset = InFOVOffset or 0
    self.InterpSpeed = InInterpSpeed or 0
    self.CurrentZoom = self.CurrentZoom or 0
    self.FOVDelta = self.FOVDelta or 0
    self.LerpTarget = self.LerpTarget or 0
end

function M:ToggleZoom()
    ---@comment 0 - Zooming out, 1 - Zooming in
    self.LerpTarget = 1 - self.LerpTarget
end

---@param DeltaTime float
---@param ViewLocation FVector
---@param ViewRotation FRotator
---@param FOV float
---@return FVector, FRotator, float
function M:BlueprintModifyCamera(DeltaTime, ViewLocation, ViewRotation, FOV)
    if self.CurrentZoom == self.LerpTarget then
        if self.FOVDelta <= 0 then
            self:DisableModifier(false)
        end

        return ViewLocation, ViewRotation, FOV + self.FOVDelta
    end

    self.CurrentZoom = UE.UKismetMathLibrary.FInterpTo(self.CurrentZoom, self.LerpTarget, DeltaTime, self.InterpSpeed)
    local EffectAlpha = self.CurrentZoom * self.Alpha
    local NewFOV = FOV + self.FOVOffset * EffectAlpha
    self.FOVDelta = NewFOV - FOV

    return ViewLocation, ViewRotation, NewFOV
end

return M