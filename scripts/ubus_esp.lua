require "ubus"
local json = require("json")

local conn = nil

function init()
	conn = ubus.connect()
	if not conn then
		error("Failed to connect to ubus")
		return 1
	end
	return 0
end

function get_data()
    local data = {}
    local status = conn:call("esp_control", "devices", {})
    for k, v in pairs(status) do
        if k == "devices" then
            for i, device in ipairs(v) do
                table.insert(data, json.encode(device))
            end
        end
    end
    local joined_data = table.concat(data, ",")
    local response = { response = joined_data }
    local json_data = json.encode(response)
    return json_data
end

function destroy()
	if conn then
		conn:close()
		conn = nil
		return 0
	end
end
