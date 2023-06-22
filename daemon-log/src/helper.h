#ifndef HELPER_H
#define HELPER_H

#include <argp.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024
#define PATH_MAX 4096

static char doc[] = "A daemon program that accepts arguments using argp and "
                    "logs Ubus information using syslog.";

static struct argp_option options[] = {
    {"daemon", 'a', 0, 0},
    {"product_id", 'p', "<product_id>", 0, 0},
    {"device_id", 'd', "<device_id>", 0, 0},
    {"device_secret", 's', "<device_secret>", 0},
    {0}};

struct arguments {
  bool daemon;
  char *product_id;
  char *device_id;
  char *device_secret;
};

void signal_handler(int signal);
error_t parse_opt(int key, char *arg, struct argp_state *state);
void daemonize();
int write_json_to_file(const char *message, const char *filepath);
char *path_from_home(char *filepath);
void cleanup(char *response_filepath);

#endif // HELPER_H
