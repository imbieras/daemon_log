local json = require("json")

local file_path = "/etc/hosts"
file = nil

function init()
  file = io.open(file_path, "r")
  if not file then
    error("Failed to open file: " .. file_path)
    return 1
  end
  -- Add any additional initialization code here
  return 0
end

function get_data() 
  if not file then
    error("File is not open")
  end
  
  local content = file:read("*a")
  
  local data = { response = content }
  
  local json_data = json.encode(data)
  
  file:seek("set", 0)
  
  return json_data
end

function destroy() 
  if file then
    file:close()
    file = nil
    return 0
  end
  -- Add any additional cleanup code here
end
