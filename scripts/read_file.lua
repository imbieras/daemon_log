local json = require("json")

local file_path = "/etc/hosts"
local file = nil

function init()
  file = io.open(file_path, "r")
  if not file then
    error("Failed to open file: " .. file_path)
  end
  
  -- Add any additional initialization code here
end

function get_data() 
  if not file then
    error("File is not open")
  end
  
  local content = file:read("*a")
  
  local data = { content = content }
  
  local json_data = json.encode(data)
  
  return json_data
end

function destroy() 
  if file then
    file:close()
    file = nil
  end
  
  -- Add any additional cleanup code here
end
