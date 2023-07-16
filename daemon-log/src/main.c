#include "helper.h"
#include "lua_helper.h"
#include "tuya_helper.h"
#include <argp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <syslog.h>
#include <time.h>
#include <tuya_error_code.h>
#include <tuyalink_core.h>
#include <unistd.h>

tuya_mqtt_context_t client_instance;
tuya_mqtt_context_t *client = &client_instance;

static struct argp argp = {options, parse_opt, 0, doc};

bool stop_loop = false;

struct loaded_script scripts[MAX_SCRIPTS];

char *response_filepath = NULL;

int main(int argc, char **argv) {
  int ret = OPRT_OK;

  openlog("daemon_log", LOG_PID, LOG_DAEMON);

  struct arguments arguments = {false, NULL, NULL, NULL};

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  if (arguments.daemon) {
    daemonize();
    syslog(LOG_INFO, "Daemon started");
  }

  int num_scripts = 0;

  load_lua_scripts(scripts, &num_scripts);

  syslog(LOG_INFO, "Number of scripts: %d", num_scripts);
  for (int i = 0; i < num_scripts; i++) {
    syslog(LOG_INFO, "#%d script name: %s", i+1, scripts[i].file_name);
  }

  response_filepath = path_from_home("/response.json");

  if ((ret = client_init(client, arguments.device_id,
                         arguments.device_secret)) != OPRT_OK) {
    syslog(LOG_ERR, "Failed to initialize client: %d", ret);
    cleanup(response_filepath);
    return ret;
  }

  char response[BUFFER_SIZE];

  while (!stop_loop) {
    if ((ret = tuya_mqtt_loop(client)) != OPRT_OK) {
      syslog(LOG_ERR, "Connection was dropped");
      return ret;
    }
    syslog(LOG_INFO, "Trying to execute get_data hooks for %d scripts", num_scripts);
    for (int i = 0; i < num_scripts; i++) {
      char response[BUFFER_SIZE];
      call_get_data_hook(scripts[i].L, scripts[i].get_data_hook, response);
      if (strlen(response) > 0) {
        process_command(client, arguments, response);
      }
    }
  }

  execute_destroy_hooks(scripts, num_scripts);

  cleanup(response_filepath);
  client_deinit(client);
  unload_lua_scripts(scripts, num_scripts);

  closelog();

  return ret;
}
