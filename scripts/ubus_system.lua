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
	local data
	local status = conn:call("system", "info", {})
	for k, v in pairs(status) do
		if k == "memory" then
			data = json.encode(v)
		end
	end
	local temp_data = { response = data }
	local json_data = json.encode(temp_data)
	return json_data
end

function destroy()
	if conn then
		conn:close()
		conn = nil
		return 0
	end
end
