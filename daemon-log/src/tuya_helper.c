#include "helper.h"
#include "tuya_cacert.h"
#include <stdlib.h>
#include <syslog.h>
#include <time.h>
#include <tuya_error_code.h>
#include <tuyalink_core.h>

extern char *response_filepath;

void on_connected(tuya_mqtt_context_t *context, void *user_data) {
  syslog(LOG_INFO, "Client connected");
}

void on_disconnect(tuya_mqtt_context_t *context, void *user_data) {
  syslog(LOG_INFO, "Client disconnected");
}

void on_messages(tuya_mqtt_context_t *context, void *user_data,
                 const tuyalink_message_t *msg) {
  if (response_filepath == NULL) {
    syslog(LOG_ERR, "Response file path is not set");
    return;
  }

  syslog(LOG_INFO, "On message id:%s, type:%d, code:%d", msg->msgid, msg->type,
         msg->code);
  switch (msg->type) {
  case THING_TYPE_PROPERTY_REPORT_RSP:
    syslog(LOG_INFO, "Cloud received and replied: id:%s, type:%d", msg->msgid,
           msg->type);
    break;
  case THING_TYPE_PROPERTY_SET:
    syslog(LOG_INFO, "Device received id:%s, type:%d", msg->msgid, msg->type);
    write_json_to_file(msg->data_string, response_filepath);
    break;
  default:
    break;
  }
  printf("\r\n");
}

int client_init(tuya_mqtt_context_t *client, char *deviceId,
                char *deviceSecret) {
  int ret = OPRT_OK;

  ret = tuya_mqtt_init(
      client, &(const tuya_mqtt_config_t){.host = "m1.tuyacn.com",
                                          .port = 8883,
                                          .cacert = tuya_cacert_pem,
                                          .cacert_len = sizeof(tuya_cacert_pem),
                                          .device_id = deviceId,
                                          .device_secret = deviceSecret,
                                          .keepalive = 100,
                                          .timeout_ms = 2000,
                                          .on_connected = on_connected,
                                          .on_disconnect = on_disconnect,
                                          .on_messages = on_messages});

  if (ret != OPRT_OK) {
    syslog(LOG_ERR, "Failed to initialize");
    return ret;
  }

  if ((ret = tuya_mqtt_connect(client)) != OPRT_OK) {
    syslog(LOG_ERR, "Failed to connect");
    return ret;
  }

  syslog(LOG_INFO, "Client was succesfuly initialized");
  return ret;
}

int client_deinit(tuya_mqtt_context_t *client) {
  int ret = OPRT_OK;

  if ((ret = tuya_mqtt_disconnect(client)) != OPRT_OK) {
    syslog(LOG_ERR, "Failed to disconnect");
    return ret;
  }

  if ((ret = tuya_mqtt_deinit(client)) != OPRT_OK) {
    syslog(LOG_ERR, "Failed to deinitialize");
    return ret;
  }

  syslog(LOG_INFO, "Client was succesfuly deinitialized");
  return ret;
}

int send_command_report(tuya_mqtt_context_t *client, char *device_id,
                        char *report) {
  int ret = OPRT_OK;
  if ((ret = tuyalink_thing_property_report_with_ack(
           client, device_id, report)) == OPRT_INVALID_PARM) {
    syslog(LOG_ERR, "Failed to send report");
    return ret;
  }

  syslog(LOG_INFO, "Command report was sent successfully");
  return OPRT_OK;
}

void process_command(tuya_mqtt_context_t *client, const char *command,
                     struct arguments arguments) {
  char report[BUFFER_SIZE];
  if ((execute_command(command, report)) == EXIT_SUCCESS) {
    time_t current_time = time(NULL);
    char response[BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "{\"response\":{\"value\":\"%s\", \"time\":%lld}}", report,
             (long long)current_time);
    send_command_report(client, arguments.device_id, response);
  }
}
