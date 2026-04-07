---@type ADPlayerController
local M = UnLua.Class()
local GameplayTags = require("GameplayTags")
local Utils = require("Utils")

function M:ReceiveUnPossess()
    self:UnBindInputActions()
end

function M:Initialize()
    self.Handles = {}
    self.bBoundInput = false
end

---@param NewPawn ADCharacter
function M:OnClientRestart(NewPawn)
    if not NewPawn:IsA(UE.ADCharacter) then
        Utils.LogError("NewPawn type is not ADCharacter")
        return
    end

    self:BindInputActions(NewPawn.PawnTag)
end

function M:OnPostProcessInput()
    if not self.bBoundInput then
        return
    end

    local Asc = UE.UAbilitySystemBlueprintLibrary.GetAbilitySystemComponent(self.PlayerState)
    if not Asc or not Asc:IsValid() then
        --Utils.LogError("invalid UDAbilitySystemComponent")
        return
    end

    Asc:ProcessAbilityInput()
end

---@param PlayerTag FGameplayTag
function M:BindInputActions(PlayerTag)
    if self.bBoundInput then
        return
    end

    self.bBoundInput = true

    if not self:IsLocalController() then
        Utils.LogError("not local controller")
        return
    end

    if not PlayerTag then
        Utils.LogError("nil PlayerTag")
        return
    end

    if type(PlayerTag) ~= "userdata" or getmetatable(PlayerTag) ~= UE.FGameplayTag then
        Utils.LogError("PlayerTag type is not FGameplayTag")
        return
    end

    if not PlayerTag:IsValid() then
        Utils.LogError("invalid PlayerTag")
        return
    end

    if not self.InputComponent or not self.InputComponent:IsValid() then
        Utils.LogError("invalid InputComponent")
        return
    end

    if not self.InputComponent:IsA(UE.UEnhancedInputComponent) then
        Utils.LogError("InputComponent is not UEnhancedInputComponent")
        return
    end
    
    local PawnData = Utils.GetPawnData(PlayerTag)
    if not PawnData then
        Utils.LogError(string.format("nil PawnData for PlayerTag: %s", PlayerTag.TagName))
        return
    end

    local InputDataAsset = PawnData.InputDataAsset
    if not InputDataAsset or not InputDataAsset:IsValid() then
        Utils.LogError(string.format("invalid InputDataAsset for PlayerTag: %s", PlayerTag.TagName))
        return
    end

    local InputMoveAction = InputDataAsset.NativeInputMap:FindRef(GameplayTags.InputTag_Move)
    if not InputMoveAction or not InputMoveAction:IsValid() then
        Utils.LogError(string.format("invalid InputAction Move for PlayerTag: %s", PlayerTag.TagName))
    else
        table.insert(self.Handles, self.InputComponent:BindAction(InputMoveAction, UE.ETriggerEvent.Triggered, self, self.OnInputMove))
    end

    local InputLookMouseAction = InputDataAsset.NativeInputMap:FindRef(GameplayTags.InputTag_Look_Mouse)
    if not InputLookMouseAction or not InputLookMouseAction:IsValid() then
        Utils.LogError(string.format("invalid InputAction Look_Mouse for PlayerTag: %s", PlayerTag.TagName))
    else
        table.insert(self.Handles, self.InputComponent:BindAction(InputLookMouseAction, UE.ETriggerEvent.Triggered, self, self.OnInputLookMouse))
    end

    for InputTag, InputAction in pairs(InputDataAsset.AbilityInputMap) do
        if not InputTag then
            Utils.LogError(string.format("nil InputTag for PlayerTag: %s", PlayerTag.TagName))
            goto continue
        end

        if not InputTag:IsValid() then
            Utils.LogError(string.format("invalid InputTag for PlayerTag: %s", PlayerTag.TagName))
            goto continue
        end
    
        if not InputAction or not InputAction:IsValid() then
            Utils.LogError(string.format("InputTag: %s invalid InputAction for PlayerTag: %s", InputTag.TagName, PlayerTag.TagName))
            goto continue
        end

        table.insert(self.Handles, self.InputComponent:BindAction(InputAction, UE.ETriggerEvent.Triggered, self, self.OnAbilityInputPressed, InputTag))
        table.insert(self.Handles, self.InputComponent:BindAction(InputAction, UE.ETriggerEvent.Completed, self, self.OnAbilityInputReleased, InputTag))
        ::continue::
    end

    local Subsystem = UE.USubsystemBlueprintLibrary.GetLocalPlayerSubsystem(self, UE.UEnhancedInputLocalPlayerSubsystem)
    if not Subsystem or not Subsystem:IsValid() then
        Utils.LogError(string.format("invalid EnhancedInputLocalPlayerSubsystem for PlayerTag: %s", PlayerTag.TagName))
        return
    end

    Subsystem:AddMappingContext(InputDataAsset.InputMappingData.MappingContext, InputDataAsset.InputMappingData.Priority, UE.FModifyContextOptions())
end

function M:UnBindInputActions()
    if not self.bBoundInput then
        return
    end

    self.bBoundInput = false

    if not self:IsLocalController() then
        Utils.LogError("not local controller")
        return
    end

    if not self.InputComponent or not self.InputComponent:IsValid() then
        Utils.LogError("invalid InputComponent")
        return
    end

    if not self.InputComponent:IsA(UE.UEnhancedInputComponent) then
        Utils.LogError("InputComponent is not UEnhancedInputComponent")
        return
    end

    for _, Handle in pairs(self.Handles) do
        self.InputComponent:RemoveBindingByHandle(Handle)
    end

    self.Handles = {}
end

---@param InputActionValue FInputActionValue
function M:OnInputMove(InputActionValue)
    local PlayerPawn = self:K2_GetPawn()
    if not PlayerPawn or not PlayerPawn:IsValid() then
        Utils.LogError("invalid Pawn")
        return
    end

    local InputVector = UE.UEnhancedInputLibrary.Conv_InputActionValueToAxis2D(InputActionValue)
    local MoveRotator = UE.FRotator(0, self:GetControlRotation().Yaw, 0)

    if InputVector.X ~= 0 then
        PlayerPawn:AddMovementInput(MoveRotator:RotateVector(UE.FVector(0, 1, 0)), InputVector.X, false)
    end

    if InputVector.Y ~= 0 then
        PlayerPawn:AddMovementInput(MoveRotator:RotateVector(UE.FVector(1, 0, 0)), InputVector.Y, false)
    end
end

---@param InputActionValue FInputActionValue
function M:OnInputLookMouse(InputActionValue)
    local InputVector = UE.UEnhancedInputLibrary.Conv_InputActionValueToAxis2D(InputActionValue)

    if InputVector.X ~= 0 then
        self:AddYawInput(InputVector.X)
    end

    if InputVector.Y ~= 0 then
        self:AddPitchInput(InputVector.Y)
    end
end

---@param InputTag FGameplayTag
function M:OnAbilityInputPressed(InputTag)
    local Asc = UE.UAbilitySystemBlueprintLibrary.GetAbilitySystemComponent(self.PlayerState)
    if not Asc or not Asc:IsValid() then
        Utils.LogError("invalid UDAbilitySystemComponent")
        return
    end

    Asc:AbilityForInputPressed(InputTag)
end

---@param InputTag FGameplayTag
function M:OnAbilityInputReleased(InputTag)
    local Asc = UE.UAbilitySystemBlueprintLibrary.GetAbilitySystemComponent(self.PlayerState)
    if not Asc or not Asc:IsValid() then
        Utils.LogError("invalid UDAbilitySystemComponent")
        return
    end

    Asc:AbilityForInputReleased(InputTag)
end

return M