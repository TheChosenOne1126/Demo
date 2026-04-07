local Utils = {}

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

local function GetPawnDataAsset()
    local AssetManager = UE.UAssetManager.GetIfInitialized()
    if not AssetManager or not AssetManager:IsValid() or not AssetManager:IsA(UE.UDAssetManager) then
        Utils.LogError("nil or invalid AssetManager")
        return nil
    end

    return AssetManager:GetPawnDataAsset()
end

---@param PawnTag FGameplayTag
---@return FPawnData
function Utils.GetPawnData(PawnTag)
    if not PawnTag or not PawnTag:IsValid() then
        Utils.LogError("nil or invalid PawnTag")
        return nil
    end

    local PawnDataAsset = GetPawnDataAsset()
    if not PawnDataAsset or not PawnDataAsset:IsValid() then
        Utils.LogError("nil or invalid PawnDataAsset")
        return nil
    end

    return PawnDataAsset.PawnDataMap:FindRef(PawnTag)
end

---@param Tag FGameplayTag
---@return TSubclassOf<UCameraModifier>
function Utils.GetCameraModifierByTag(Tag)
    if not Tag or not Tag:IsValid() then
        Utils.LogError("nil or invalid Tag")
        return nil
    end

    local PawnDataAsset = GetPawnDataAsset()
    if not PawnDataAsset or not PawnDataAsset:IsValid() then
        Utils.LogError("nil or invalid PawnDataAsset")
        return nil
    end

    return PawnDataAsset.CameraModifierMap:FindRef(Tag)
end

function Utils.InitializeBPInterfaceClass()
    Utils.BI_MotionWarp = LoadClass("/Game/Blueprints/Interface/BI_MotionWarp.BI_MotionWarp_C")
    Utils.BI_Stealth = LoadClass("/Game/Blueprints/Interface/BI_Stealth.BI_Stealth_C")
end

return Utils