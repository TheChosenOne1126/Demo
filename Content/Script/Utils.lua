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

return Utils