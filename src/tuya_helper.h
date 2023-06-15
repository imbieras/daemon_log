#ifndef TUYA_HELPER_H
#define TUYA_HELPER_H

#include "../tuya-iot-core-sdk/include/tuyalink_core.h"

void on_connected(tuya_mqtt_context_t *context, void *user_data);
void on_disconnect(tuya_mqtt_context_t *context, void *user_data);
void on_messages(tuya_mqtt_context_t *context, void *user_data,
                 const tuyalink_message_t *msg);
int client_init(tuya_mqtt_context_t *client, char *deviceId,
                char *deviceSecret);
int client_deinit(tuya_mqtt_context_t *client);
int send_command_report(tuya_mqtt_context_t *client, char *device_id,
                        char *report);

#endif // TUYA_HELPER_H