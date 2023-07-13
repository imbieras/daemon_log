#ifndef UBUS_HELPER_H
#define UBUS_HELPER_H

#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <stdint.h>

enum {
  ESP_DEVICES_ATTR_DEVICES,
  __ESP_DEVICES_ATTR_MAX,
};

static const struct blobmsg_policy devices_policy[__ESP_DEVICES_ATTR_MAX] = {
    [ESP_DEVICES_ATTR_DEVICES] = {.name = "devices", .type = BLOBMSG_TYPE_ARRAY},
};

int ubus_init();
int ubus_deinit();
int ubus_esp_control_on(char *port, int pin);
int ubus_esp_control_off(char *port, int pin);
static void get_devices_cb(struct ubus_request *req, int type,
                           struct blob_attr *msg);
int ubus_info_to_json(char *response);

#endif // UBUS_HELPER_H
