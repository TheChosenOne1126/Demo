---@type UDGameInstance
local M = UnLua.Class()
local Enum = require("Enum")

function M:ReceiveInit()
    self.BPInterfaceClass = {
        [Enum.BPInterface.BI_MotionWarp] = LoadClass("/Game/Blueprints/Interface/BI_MotionWarp.BI_MotionWarp_C"),
        [Enum.BPInterface.BI_Stealth] = LoadClass("/Game/Blueprints/Interface/BI_Stealth.BI_Stealth_C")
    }
end

function M:ReceiveShutdown()
    self.BPInterfaceClass = nil
end

return M