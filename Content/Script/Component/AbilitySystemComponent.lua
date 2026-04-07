---@type UDAbilitySystemComponent
local M = UnLua.Class()
local GameplayTags = require("GameplayTags")
local Utils = require("Utils")

function M:Initialize()
    self.InputPressedHandles = {}
    self.InputReleasedHandles = {}
    self.InputHeldHandles = {}
end

---@param InputTag FGameplayTag
function M:AbilityForInputPressed(InputTag)
    if not InputTag then
        Utils.LogError("nil InputTag")
        return
    end

    if type(InputTag) ~= "userdata" or getmetatable(InputTag) ~= UE.FGameplayTag then
        Utils.LogError("InputTag type is not FGameplayTag")
        return
    end

    if not InputTag:IsValid() then
        Utils.LogError("invalid InputTag")
        return
    end

    local EventData = UE.FGameplayEventData()
    EventData.InstigatorTags:AddTag(InputTag)
    self:HandleGameplayEvent(GameplayTags.Ability_Input_Press, EventData)

    self:IncrementAbilityListLock()
    for _, Spec in pairs(self.ActivatableAbilities.Items) do
        if not Spec.DynamicAbilityTags:HasTagExact(InputTag) then
            goto continue
        end

        if not Spec.Ability or not Spec.Ability:IsValid() then
            Utils.LogError(string.format("invalid Ability for InputTag: %s", InputTag.TagName))
            goto continue
        end

        ---@type FGameplayAbilitySpecHandle
        local SpecHandle = Spec.Handle
        ---@type integer
        local IntHandle = SpecHandle.Handle

        if Spec.DynamicAbilityTags:HasTagExact(GameplayTags.Ability_Activation_InputPressed) then
            self.InputPressedHandles[IntHandle] = self.InputPressedHandles[IntHandle] or SpecHandle
        elseif Spec.DynamicAbilityTags:HasTagExact(GameplayTags.Ability_Activation_InputReleased) then
            self.InputHeldHandles[IntHandle] = self.InputHeldHandles[IntHandle] or SpecHandle
        else
            Utils.LogError(string.format("invalid activation tag for InputTag: %s", InputTag.TagName))
        end

        ::continue::
    end
    self:DecrementAbilityListLock()
end

---@param InputTag FGameplayTag
function M:AbilityForInputReleased(InputTag)
    if not InputTag then
        Utils.LogError("nil InputTag")
        return
    end

    if type(InputTag) ~= "userdata" or getmetatable(InputTag) ~= UE.FGameplayTag then
        Utils.LogError("InputTag type is not FGameplayTag")
        return
    end

    if not InputTag:IsValid() then
        Utils.LogError("invalid InputTag")
        return
    end

    local EventData = UE.FGameplayEventData()
    EventData.InstigatorTags:AddTag(InputTag)
    self:HandleGameplayEvent(GameplayTags.Ability_Input_Release, EventData)
    
    self:IncrementAbilityListLock()
    for _, Spec in pairs(self.ActivatableAbilities.Items) do
        if not Spec.DynamicAbilityTags:HasTagExact(InputTag) then
            goto continue
        end

        if not Spec.Ability or not Spec.Ability:IsValid() then
            Utils.LogError(string.format("invalid Ability for InputTag: %s", InputTag.TagName))
            goto continue
        end

        ---@type FGameplayAbilitySpecHandle
        local SpecHandle = Spec.Handle
        ---@type integer
        local IntHandle = SpecHandle.Handle
        self.InputReleasedHandles[IntHandle] = self.InputReleasedHandles[IntHandle] or SpecHandle
        self.InputHeldHandles[IntHandle] = self.InputHeldHandles[IntHandle] and nil

        ::continue::
    end
    self:DecrementAbilityListLock()
end

function M:ProcessAbilityInput()
    local AbilitySpecHandlesToActivate = {}

    for _, SpecHandle in pairs(self.InputHeldHandles) do
        local Spec = self:FindAbilitySpecFromHandle(SpecHandle)
        if not Spec then
            Utils.LogError("nil AbilitySpec for Held")
            goto continue
        end

        self:AbilitySpecInputPressed(Spec)

        if Spec:IsActive() then
            goto continue
        end

        AbilitySpecHandlesToActivate[SpecHandle.Handle] = AbilitySpecHandlesToActivate[SpecHandle.Handle] or SpecHandle

        ::continue::
    end

    for _, SpecHandle in pairs(self.InputPressedHandles) do
        local Spec = self:FindAbilitySpecFromHandle(SpecHandle)
        if not Spec then
            Utils.LogError("nil AbilitySpec for Pressed")
            goto continue
        end

        self:AbilitySpecInputPressed(Spec)

        if Spec:IsActive() then
            goto continue
        end

        AbilitySpecHandlesToActivate[SpecHandle.Handle] = AbilitySpecHandlesToActivate[SpecHandle.Handle] or SpecHandle

        ::continue::
    end

    for _, SpecHandle in pairs(AbilitySpecHandlesToActivate) do
        self:TryActivateAbility(SpecHandle, true)
    end

    for _, SpecHandle in pairs(self.InputReleasedHandles) do
        local Spec = self:FindAbilitySpecFromHandle(SpecHandle)
        if not Spec then
            Utils.LogError("nil AbilitySpec for Released")
            goto continue
        end

        self:AbilitySpecInputReleased(Spec)

        ::continue::
    end

    self.InputPressedHandles = {}
    self.InputReleasedHandles = {}
end

return M