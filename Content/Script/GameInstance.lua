---@type UDGameInstance
local M = UnLua.Class()
local Utils = require("Utils")

function M:ReceiveInit()
    Utils.InitializeBPInterfaceClass()
end

return M