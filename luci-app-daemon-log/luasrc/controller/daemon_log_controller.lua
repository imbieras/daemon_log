module("luci.controller.daemon_log_controller", package.seeall)

function index()
	entry({ "admin", "services", "daemon_log" }, cbi("daemon_log_model"), "Tuya IoT", 100)
end
