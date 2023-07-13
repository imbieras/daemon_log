#include "helper.h"
#include <argp.h>
#include <cJSON.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

const char *argp_program_version = "daemon_log 0.1";
error_t argp_err_exit_status = 1;
extern bool stop_loop;

void signal_handler(int signal) {
  if (signal == SIGINT || signal == SIGTERM) {
    syslog(LOG_WARNING, "Signal received. Stopping.");
    stop_loop = true;
  }
}

error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = (struct arguments *)state->input;
  switch (key) {
  case 'a':
    arguments->daemon = true;
    break;
  case 'p':
    arguments->product_id = arg;
    break;
  case 'd':
    arguments->device_id = arg;
    break;
  case 's':
    arguments->device_secret = arg;
    break;
  case ARGP_KEY_END:
    if (arguments->device_id == NULL || arguments->product_id == NULL ||
        arguments->device_secret == NULL) {
      argp_failure(state, 1, 0,
                   "product_id, device_id and device_secret are all "
                   "required\nTry `daemon_log --help' or `daemon_log --usage' "
                   "for more information.");
    }
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

void daemonize() {
  pid_t pid, sid;

  pid = fork();
  if (pid < 0) {
    syslog(LOG_ERR, "Failed to fork: %d", pid);
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  sid = setsid();
  if (sid < 0) {
    syslog(LOG_ERR, "Failed to setsid: %d", sid);
    exit(EXIT_FAILURE);
  }

  umask(0);

  if (chdir("/") < 0) {
    syslog(LOG_ERR, "Failed to chdir to /");
    exit(EXIT_FAILURE);
  }

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

int write_json_to_file(const char *message, const char *filepath) {
  cJSON *json = cJSON_Parse(message);
  if (json == NULL) {
    syslog(LOG_ERR, "Failed to parse JSON message.");
    return EXIT_FAILURE;
  }
  FILE *fd = fopen(filepath, "a");
  if (fd == NULL) {
    syslog(LOG_ERR, "Failed to open file: %s", filepath);
    cJSON_Delete(json);
    return EXIT_FAILURE;
  }
  char *json_str = cJSON_Print(json);
  fprintf(fd, "%s\n", json_str);
  fclose(fd);
  cJSON_Delete(json);
  free(json_str);
  return EXIT_SUCCESS;
}

char *path_from_home(char *filepath) {
  char *homedir;
  if ((homedir = getenv("HOME")) == NULL) {
    homedir = getpwuid(getuid())->pw_dir;
  }

  char *filepath_from_home = malloc((PATH_MAX * sizeof(char)) + 1);
  if (filepath_from_home == NULL) {
    syslog(LOG_ERR, "Failed to allocate memory for file path from home");
    return NULL;
  }
  snprintf(filepath_from_home, PATH_MAX, "%s%s", homedir, filepath);
  return filepath_from_home;
}

void cleanup(char *response_filepath) {
  if (response_filepath != NULL) {
    free(response_filepath);
  }
}
