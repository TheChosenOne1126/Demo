local Utils = {}
local Enum = require("Enum")

function Utils.IsValidEnum(enumTable, value)
    for _, v in pairs(enumTable) do
        if v == value then
            return true
        end
    end
    return false
end

function Utils.LogWarning(Message, Object)
    local WarningMessage = string.format(" [Lua] %s \n", Message)
    UE.UStatics.Log(Object, UE.ELogType.Warning, WarningMessage, true)
end

---@param Message string
---@param Object UObject
function Utils.LogError(Message, Object)
    local ErrorMessage = string.format(" [Lua] %s \n %s", Message, debug.traceback(nil, 2))
    UE.UStatics.Log(Object, UE.ELogType.Error, ErrorMessage, true)
end

local function GetGameDataAsset()
    local AssetManager = UE.UAssetManager.GetIfInitialized()
    if not AssetManager or not AssetManager:IsValid() or not AssetManager:IsA(UE.UDAssetManager) then
        Utils.LogError("nil or invalid AssetManager")
        return nil
    end

    return AssetManager.GameDataAsset
end

---@param PawnTag FGameplayTag
---@return FPawnData
function Utils.GetPawnData(PawnTag)
    if not PawnTag or not PawnTag:IsValid() then
        Utils.LogError("nil or invalid PawnTag")
        return nil
    end

    local GameDataAsset = GetGameDataAsset()
    if not GameDataAsset or not GameDataAsset:IsValid() then
        Utils.LogError("nil or invalid GameDataAsset")
        return nil
    end

    return GameDataAsset.PawnDataMap:FindRef(PawnTag)
end

---@param Tag FGameplayTag
---@return TSubclassOf<UCameraModifier>
function Utils.GetCameraModifierByTag(Tag)
    if not Tag or not Tag:IsValid() then
        Utils.LogError("nil or invalid Tag")
        return nil
    end

    local GameDataAsset = GetGameDataAsset()
    if not GameDataAsset or not GameDataAsset:IsValid() then
        Utils.LogError("nil or invalid GameDataAsset")
        return nil
    end

    return GameDataAsset.CameraModifierMap:FindRef(Tag)
end

---@param Object UObject
---@param InterfaceType Enum.BPInterface
---@return bool
function Utils.HasImplementInterface(Object, InterfaceType)
    if not Utils.IsValidEnum(Enum.BPInterface, InterfaceType) then
        Utils.LogError(string.format("invalid InterfaceType:[%s], not a valid Enum.BPInterface", tostring(InterfaceType)))
        return false
    end

    if not Object or not Object:IsValid() then
        Utils.LogError("invalid Object")
        return false
    end

    local GameInstance = UE.UGameplayStatics.GetGameInstance(Object)
    if not GameInstance or not GameInstance:IsValid() then
        Utils.LogError("invalid GameInstance")
        return false
    end
    
    local BPInterfaceClass = GameInstance.BPInterfaceClass
    if not BPInterfaceClass then
        Utils.LogError("nil BPInterfaceClass")
        return false
    end

    local InterfaceClass = BPInterfaceClass[InterfaceType]
    if not InterfaceClass or not InterfaceClass:IsValid() then
        Utils.LogError(string.format("invalid InterfaceType:[%s]", tostring(InterfaceType)))
        return false
    end

    return UE.UKismetSystemLibrary.DoesImplementInterface(Object, InterfaceClass)
end

return Utils