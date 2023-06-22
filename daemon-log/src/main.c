#include "helper.h"
#include "tuya_helper.h"
#include "ubus_helper.h"
#include <argp.h>
#include <libubus.h>
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

  if ((ret = client_init(client, arguments.device_id,
                         arguments.device_secret)) != OPRT_OK) {
    syslog(LOG_ERR, "Failed to initialize client: %d", ret);
    cleanup(response_filepath);
    return ret;
  }

  struct ubus_context *ctx = NULL;

  if (ubus_init(&ctx) != EXIT_SUCCESS) {
    syslog(LOG_ERR, "Failed to initialize UBus context");
    cleanup(response_filepath);
    client_deinit(client);
    return EXIT_FAILURE;
  }

  while (!stop_loop) {
    if ((ret = tuya_mqtt_loop(client)) != OPRT_OK) {
      syslog(LOG_ERR, "Connection was dropped");
      return ret;
    }

    process_command(ctx, client, arguments);
  }

  cleanup(response_filepath);
  ubus_deinit(ctx);
  client_deinit(client);

  closelog();

  return ret;
}
