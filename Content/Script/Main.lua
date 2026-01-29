local GameplayTags = require("GameplayTags")
_G.GameplayTags = GameplayTags

local Utils = require("Utils")
_G.Utils = Utils

function _G.GetTableLength(tb)
    if not tb then
        UnLua.LogWarn("TableLength: input is nil")
        return 0
    end

    if type(tb) ~= "table" then
        UnLua.LogWarn("TableLength: input is not a table")
        return 0
    end

    local count = 0
    for _ in pairs(tb) do
        count = count + 1
    end

    return count
end