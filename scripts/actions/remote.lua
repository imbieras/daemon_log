require "ubus"
require "socket"
local json = require "json"

local conn = nil

function on(param)
    conn = ubus.connect()
    if not conn then
        error("Failed to connect to ubus")
        return
    end

    local table_param = json.decode(param)

    if not table_param.pin or not table_param.port then
        error("Missing required 'pin' or 'port' in input parameters")
        conn:close()
        conn = nil
        return
    end

    conn:call("esp_control", "on", table_param)

    conn:close()
    conn = nil
end

function off(param)
    conn = ubus.connect()
    if not conn then
        error("Failed to connect to ubus")
        return
    end

    local table_param = json.decode(param)

    if not table_param.pin or not table_param.port then
        error("Missing required 'pin' or 'port' in input parameters")
        conn:close()
        conn = nil
        return
    end

    conn:call("esp_control", "off", table_param)

    conn:close()
    conn = nil
end

function blink(param)
    conn = ubus.connect()
    if not conn then
        error("Failed to connect to ubus")
        return
    end

    local table_param = json.decode(param)

    if not table_param.pin or not table_param.port or not table_param.timer or not table_param.repeat_times then
        error("Missing required parameters: 'pin', 'port', 'timer', or 'repeat_times'")
        conn:close()
        conn = nil
        return
    end

    for i = 1, table_param.repeat_times do
        conn:call("esp_control", "on", table_param)
        socket.sleep(table_param.timer / 1000)
        conn:call("esp_control", "off", table_param)
        socket.sleep(table_param.timer / 1000)
    end

    conn:close()
    conn = nil
end
