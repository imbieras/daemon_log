map = Map("daemon_log")

enableSection = map:section(NamedSection, "daemon_log_sct", "daemon_log", "Enable program")
flag = enableSection:option(Flag, "enable", "Enable", "Enable program")

argumentSection = map:section(NamedSection, "daemon_log_sct", "daemon_log", "Basic device information")
product_id = argumentSection:option(Value, "product_id", "Product ID")
product_id.size = 16
product_id.maxlength = 16

device_id = argumentSection:option(Value, "device_id", "Device ID")
device_id.size = 22
device_id.maxlength = 22

device_secret = argumentSection:option(Value, "device_secret", "Device Secret")
device_secret.size = 22
device_secret.maxlength = 16

return map
