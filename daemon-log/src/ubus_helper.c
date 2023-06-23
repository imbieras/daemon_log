#include "ubus_helper.h"
#include "helper.h"
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslog.h>
#include <time.h>

static void get_memory_cb(struct ubus_request *req, int type,
                          struct blob_attr *msg) {
  struct MemData *memoryData = (struct MemData *)req->priv;
  struct blob_attr *tb[__INFO_MAX];
  struct blob_attr *memory[__MEMORY_MAX];

  blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

  if (!tb[MEMORY_DATA]) {
    syslog(LOG_ERR, "No memory data received\n");
    return;
  }

  blobmsg_parse(memory_policy, __MEMORY_MAX, memory,
                blobmsg_data(tb[MEMORY_DATA]),
                blobmsg_data_len(tb[MEMORY_DATA]));

  memoryData->total = blobmsg_get_u64(memory[TOTAL_MEMORY]);
  memoryData->free = blobmsg_get_u64(memory[FREE_MEMORY]);
  memoryData->shared = blobmsg_get_u64(memory[SHARED_MEMORY]);
  memoryData->buffered = blobmsg_get_u64(memory[BUFFERED_MEMORY]);
}

int ubus_info_to_json(struct ubus_context *ctx, char *response) {
  struct MemData memory = {0};
  uint32_t id;

  if (ubus_lookup_id(ctx, "system", &id) ||
      ubus_invoke(ctx, id, "info", NULL, get_memory_cb, &memory, 3000)) {
    syslog(LOG_ERR, "Cannot request memory info from procd\n");
    return EXIT_FAILURE;
  }
  time_t current_time = time(NULL);
  snprintf(response, BUFFER_SIZE,
           "{\"response\": {\"total\": %d, \"free\": %d, \"shared\": %d, "
           "\"buffered\": %d, \"time\": %lld}}",
           memory.total, memory.free, memory.shared, memory.buffered,
           (long long)current_time);

  return EXIT_SUCCESS;
}

int ubus_init(struct ubus_context **ctx) {
  *ctx = ubus_connect(NULL);
  if (!(*ctx)) {
    syslog(LOG_ERR, "Failed to connect to ubus\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int ubus_deinit(struct ubus_context *ctx) {
  if (!ctx) {
    syslog(LOG_ERR, "Failed to connect to ubus\n");
    return EXIT_FAILURE;
  }
  ubus_free(ctx);
  return EXIT_SUCCESS;
}
