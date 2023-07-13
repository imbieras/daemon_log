#ifndef TUYA_HELPER_H
#define TUYA_HELPER_H

#include "ubus_helper.h"
#include <libubus.h>
#include <stdint.h>
#include <tuyalink_core.h>

void on_connected(tuya_mqtt_context_t *context, void *user_data);
void on_disconnect(tuya_mqtt_context_t *context, void *user_data);
void on_messages(tuya_mqtt_context_t *context, void *user_data,
                 const tuyalink_message_t *msg);
int client_init(tuya_mqtt_context_t *client, char *deviceId,
                char *deviceSecret);
int client_deinit(tuya_mqtt_context_t *client);
int send_command_report(tuya_mqtt_context_t *client, char *device_id,
                        char *report);
void process_command(tuya_mqtt_context_t *client, struct arguments arguments);
void handle_action(char *json_str);

#endif // TUYA_HELPER_H
