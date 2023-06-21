#ifndef UBUS_HELPER_H
#define UBUS_HELPER_H

#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <stdint.h>

struct MemData {
  int total;
  int free;
  int shared;
  int buffered;
};

enum {
  TOTAL_MEMORY,
  FREE_MEMORY,
  SHARED_MEMORY,
  BUFFERED_MEMORY,
  __MEMORY_MAX,
};

enum {
  MEMORY_DATA,
  __INFO_MAX,
};

static const struct blobmsg_policy memory_policy[__MEMORY_MAX] = {
    [TOTAL_MEMORY] = {.name = "total", .type = BLOBMSG_TYPE_INT64},
    [FREE_MEMORY] = {.name = "free", .type = BLOBMSG_TYPE_INT64},
    [SHARED_MEMORY] = {.name = "shared", .type = BLOBMSG_TYPE_INT64},
    [BUFFERED_MEMORY] = {.name = "buffered", .type = BLOBMSG_TYPE_INT64},
};

static const struct blobmsg_policy info_policy[__INFO_MAX] = {
    [MEMORY_DATA] = {.name = "memory", .type = BLOBMSG_TYPE_TABLE},
};

void board_cb(struct ubus_request *req, int type, struct blob_attr *msg);
int ubus_info_to_json(struct ubus_context *ctx, uint32_t *id,
                      struct MemData *memory, char *response);
int ubus_init(struct ubus_context *ctx);
int ubus_deinit(struct ubus_context *ctx);

#endif // UBUS_HELPER_H
