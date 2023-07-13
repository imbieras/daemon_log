#include "ubus_helper.h"
#include "helper.h"
#include <cJSON.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>

struct ubus_context *ctx = NULL;
char devices[BUFFER_SIZE];

int ubus_init() {
  ctx = ubus_connect(NULL);
  if (!(ctx)) {
    syslog(LOG_ERR, "Failed to connect to ubus\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int ubus_deinit() {
  if (!ctx) {
    syslog(LOG_ERR, "Failed to connect to ubus\n");
    return EXIT_FAILURE;
  }
  ubus_free(ctx);
  return EXIT_SUCCESS;
}

int ubus_esp_control_on(char *port, int pin) {
  struct blob_buf b = {};
  blob_buf_init(&b, 0);
  blobmsg_add_string(&b, "port", port);
  blobmsg_add_u32(&b, "pin", pin);

  uint32_t id;

  if (ubus_lookup_id(ctx, "esp_control", &id) != EXIT_SUCCESS) {
    syslog(LOG_ERR, "Failed to find ubus id");
    return EXIT_FAILURE;
  }
  if (ubus_invoke(ctx, id, "on", b.head, NULL, NULL, 3000) != EXIT_SUCCESS) {
    syslog(LOG_ERR, "Failed to invoke ubus method");
    return EXIT_FAILURE;
  }
  syslog(LOG_DEBUG, "Successfully invoked ubus 'on' method");
  blob_buf_free(&b);
  return EXIT_SUCCESS;
}

int ubus_esp_control_off(char *port, int pin) {
  struct blob_buf b = {};
  blob_buf_init(&b, 0);
  blobmsg_add_string(&b, "port", port);
  blobmsg_add_u32(&b, "pin", pin);

  uint32_t id;

  if (ubus_lookup_id(ctx, "esp_control", &id) != EXIT_SUCCESS) {
    syslog(LOG_ERR, "Failed to find ubus id");
    return EXIT_FAILURE;
  }
  if (ubus_invoke(ctx, id, "off", b.head, NULL, NULL, 3000) != EXIT_SUCCESS) {
    syslog(LOG_ERR, "Failed to invoke ubus method");
    return EXIT_FAILURE;
  }

  syslog(LOG_DEBUG, "Successfully invoked ubus 'off' method");
  blob_buf_free(&b);
  return EXIT_SUCCESS;
}

static void get_devices_cb(struct ubus_request *req, int type,
                           struct blob_attr *msg) {
  char *device_data = *(char **)req->priv;
  struct blob_attr *tb[__ESP_DEVICES_ATTR_MAX];

  blobmsg_parse(devices_policy, __ESP_DEVICES_ATTR_MAX, tb, blob_data(msg),
                blob_len(msg));

  if (!tb[ESP_DEVICES_ATTR_DEVICES]) {
    syslog(LOG_ERR, "Failed to parse devices");
    return;
  }

  char *json_str = blobmsg_format_json(tb[ESP_DEVICES_ATTR_DEVICES], true);
  snprintf(devices, BUFFER_SIZE, "%s", json_str);
  free(json_str);
}

int ubus_info_to_json(char *response) {
  int ret;
  uint32_t id;

  if (ubus_lookup_id(ctx, "esp_control", &id) != EXIT_SUCCESS) {
    syslog(LOG_ERR, "Failed to find ubus id");
    return EXIT_FAILURE;
  }

  if (ubus_invoke(ctx, id, "devices", NULL, get_devices_cb, &devices, 3000) !=
      EXIT_SUCCESS) {
    syslog(LOG_ERR, "Failed to invoke ubus method");
    return EXIT_FAILURE;
  }

  syslog(LOG_DEBUG, "Successfully invoked ubus 'devices' method");

  cJSON *root = cJSON_Parse(devices);
  if (root == NULL) {
    syslog(LOG_ERR, "Error parsing JSON");
    return EXIT_FAILURE;
  }

  cJSON *array = cJSON_CreateArray();

  int i;
  int array_size = cJSON_GetArraySize(root);
  for (i = 0; i < array_size; i++) {
    cJSON *item = cJSON_GetArrayItem(root, i);
    if (item != NULL) {
      cJSON *port = cJSON_GetObjectItem(item, "port");
      cJSON *pid = cJSON_GetObjectItem(item, "pid");
      cJSON *vid = cJSON_GetObjectItem(item, "vid");

      if (port != NULL && pid != NULL && vid != NULL) {
        char *string_item = (char *)malloc(sizeof(char) * 256);
        snprintf(string_item, 256,
                 "{\"port\":\"%s\",\"pid\":\"%s\",\"vid\":\"%s\"}",
                 port->valuestring, pid->valuestring, vid->valuestring);
        cJSON_AddItemToArray(array, cJSON_CreateString(string_item));
        free(string_item);
      }
    }
  }

  char *json_str = cJSON_Print(array);

  ret = snprintf(response, BUFFER_SIZE, "{\"devices\": %s}", json_str);

  cJSON_Delete(root);
  cJSON_Delete(array);
  free(json_str);

  if (ret < 0 || ret >= BUFFER_SIZE) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}