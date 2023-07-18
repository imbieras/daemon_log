#include "tuya_helper.h"
#include "helper.h"
#include "lua_helper.h"
#include "tuya_cacert.h"
#include <cJSON.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

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
  case THING_TYPE_ACTION_EXECUTE:
    syslog(LOG_INFO, "Trying to handle action for: %s", msg->data_string);

    if (strlen(msg->data_string) == 0) {
      syslog(LOG_ERR, "Empty JSON string");
      break;
    }

    handle_action(msg->data_string);
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

  syslog(LOG_INFO, "Client was successfully initialized");
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

  syslog(LOG_INFO, "Client was successfully deinitialized");
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

void process_command(tuya_mqtt_context_t *client, struct arguments arguments,
                     char *response) {
  send_command_report(client, arguments.device_id, response);
}

void handle_action(char *json_str) {
  cJSON *root = cJSON_Parse(json_str);
  if (root == NULL) {
    syslog(LOG_ERR, "Error parsing JSON");
    return;
  }

  cJSON *action_code = cJSON_GetObjectItem(root, "actionCode");
  if (action_code == NULL) {
    syslog(LOG_ERR, "Error retrieving action code from JSON");
    cJSON_Delete(root);
    return;
  }

  cJSON *input_params = cJSON_GetObjectItem(root, "inputParams");
  if (input_params == NULL) {
    syslog(LOG_ERR, "Error retrieving input parameters from JSON");
    cJSON_Delete(root);
    return;
  }

  char *input_params_str = NULL;
  input_params_str = cJSON_Print(input_params);

  char *action_code_str = NULL;
  action_code_str = action_code->valuestring;

  const char *delim = "_";

  char *tok = strtok(action_code_str, delim);
  char *lua_script = strdup(tok);
  tok = strtok(NULL, delim);
  char *action = strdup(tok);

  if ((lua_script == NULL) || (action == NULL)) {
    syslog(LOG_ERR, "Could not get script name or action from actionCode");
    free(action);
    free(lua_script);
    cJSON_free(input_params_str);
    cJSON_Delete(root);
    return;
  }

  size_t lua_script_len = strlen(lua_script);
  size_t dir_len = strlen(SCRIPTS_DIR);

  size_t new_len =
      dir_len + 1 + lua_script_len + strlen(LUA_FILE_EXTENSION) + 1;

  char *new_lua_script = malloc(new_len);
  if (new_lua_script == NULL) {
    syslog(LOG_ERR, "Failed to allocate memory for new_lua_script");
    free(action);
    free(lua_script);
    cJSON_free(input_params_str);
    cJSON_Delete(root);
    return;
  }

  sprintf(new_lua_script, "%s%s%s%s", SCRIPTS_DIR, ACTIONS_SUBDIR, lua_script,
          LUA_FILE_EXTENSION);

  free(lua_script);
  lua_script = new_lua_script;

  syslog(LOG_INFO, "Launching action %s in lua script file %s with params: %s",
         action, lua_script, input_params_str);
  execute_action(lua_script, action, input_params_str);

  free(action);
  free(lua_script);
  cJSON_free(input_params_str);
  cJSON_Delete(root);
}
