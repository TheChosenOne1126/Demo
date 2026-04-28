---@type ANS_GenericEventByTag_C
local M = UnLua.Class()

---@private
---@param self ANS_GenericEventByTag_C
---@param EventTag FGameplayTag
---@param MeshComp USkeletalMeshComponent
---@return bool
local function TriggerEvent(self, EventTag, MeshComp)
    if not EventTag or not EventTag:IsValid() then
        return false
    end

    if not MeshComp or not MeshComp:IsValid() then
        return false
    end

    local Asc = UE.UAbilitySystemBlueprintLibrary.GetAbilitySystemComponent(MeshComp:GetOwner())
    if not Asc or not Asc:IsValid() then
        return false
    end

    local EventData = UE.FGameplayEventData()
    EventData.EventTag = EventTag
    Asc:HandleGameplayEvent(EventTag, EventData)
    return true
end

---@return FString
function M:GetNotifyName()
    return self.NotifyName
end

---@param MeshComp USkeletalMeshComponent
---@param Animation UAnimSequenceBase
---@param TotalDuration float
---@param EventReference FAnimNotifyEventReference
---@return bool
function M:Received_NotifyBegin(MeshComp, Animation, TotalDuration, EventReference)
    return TriggerEvent(self, self.StartEventTag, MeshComp)
end

---@param MeshComp USkeletalMeshComponent
---@param Animation UAnimSequenceBase
---@param TotalDuration float
---@param EventReference FAnimNotifyEventReference
---@return bool
function M:Received_NotifyEnd(MeshComp, Animation, TotalDuration, EventReference)
    return TriggerEvent(self, self.EndEventTag, MeshComp)
end

return M