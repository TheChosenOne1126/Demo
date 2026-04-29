---@type AN_GenericEventByTag_C
local M = UnLua.Class()

---@return FString
function M:GetNotifyName()
    return self.NotifyName
end

---@param MeshComp USkeletalMeshComponent
---@param Animation UAnimSequenceBase
---@param TotalDuration float
---@param EventReference FAnimNotifyEventReference
---@return bool
function M:Received_Notify(MeshComp, Animation, TotalDuration, EventReference)
    if not self.EventTag or not self.EventTag:IsValid() then
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
    EventData.EventTag = self.EventTag
    Asc:HandleGameplayEvent(self.EventTag, EventData)
    return true
end

return M