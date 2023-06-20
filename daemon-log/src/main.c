#include "helper.h"
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

const char *command = "echo 'Hello world'";

tuya_mqtt_context_t client_instance;
tuya_mqtt_context_t *client = &client_instance;

static struct argp argp = {options, parse_opt, 0, doc};

bool stop_loop = false;
char *response_filepath = NULL;

int main(int argc, char **argv) {
  int ret = OPRT_OK;

  openlog("daemon_log", LOG_PID, LOG_DAEMON);

  struct arguments arguments = {false, NULL, NULL, NULL};

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  signal(SIGINT, signal_handler);

  if (arguments.daemon) {
    daemonize();
    syslog(LOG_INFO, "Daemon started");
  }

  response_filepath = path_from_home("/response.json");

  client_init(client, arguments.device_id, arguments.device_secret);

  while (!stop_loop) {
    if ((ret = tuya_mqtt_loop(client)) != OPRT_OK) {
      syslog(LOG_ERR, "Connection was dropped");
      return ret;
    }

    process_command(client, command, arguments);
  }

  client_deinit(client);

  if (response_filepath != NULL)
    free(response_filepath);

  closelog();

  return ret;
}