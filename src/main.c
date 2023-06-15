#include "../tuya-iot-core-sdk/include/tuyalink_core.h"
#include "../tuya-iot-core-sdk/utils/tuya_error_code.h"
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
#include <unistd.h>

const char *command = "echo 'Hello world'";

tuya_mqtt_context_t client_instance;
tuya_mqtt_context_t *client = &client_instance;

static struct argp argp = {options, parse_opt, 0, doc};

bool stop_loop = false;

int main(int argc, char **argv) {
  char report[BUFFER_SIZE];

  int ret = OPRT_OK;

  struct arguments arguments = {false, NULL, NULL, NULL};
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  openlog("daemon_program", LOG_PID, LOG_DAEMON);

  signal(SIGINT, signal_handler);

  if (arguments.daemon) {
    daemonize();
    syslog(LOG_INFO, "Daemon started");
  }

  syslog(LOG_INFO, "Product ID: %s", arguments.product_id);
  syslog(LOG_INFO, "Device ID: %s", arguments.device_id);
  syslog(LOG_INFO, "Secret: %s", arguments.device_secret);

  client_init(client, arguments.device_id, arguments.device_secret);

  while (!stop_loop) {
    if ((ret = tuya_mqtt_loop(client)) != OPRT_OK) {
      syslog(LOG_ERR, "Connection was dropped");
      return ret;
    }

    // execute_command(command, &report);
    // "{\"temperature\":{\"value\":%s,\"time\":%lld}}"
    // send_command_report(client, arguments.device_id, report);
  }

  client_deinit(client);

  closelog();

  return ret;
}
