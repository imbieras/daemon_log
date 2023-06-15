#include "helper.h"
#include <argp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

const char *argp_program_version = "daemon_log 0.1";
error_t argp_err_exit_status = 1;

extern bool stop_loop;

void signal_handler(int signal) {
  if (signal == SIGINT) {
    syslog(LOG_WARNING, "CTRL+C received. Stopping.");
    stop_loop = true;
  }
}

error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = (struct arguments *)state->input;
  switch (key) {
  case 'a':
    arguments->daemon = true;
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
                   "Product id, device id and device secret are all "
                   "required\nCheck with --usage option");
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
    syslog(LOG_ERR, "Failed to fork: %m");
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }

  sid = setsid();
  if (sid < 0) {
    syslog(LOG_ERR, "Failed to setsid: %m");
    exit(EXIT_FAILURE);
  }

  umask(0);

  if (chdir("/") < 0) {
    syslog(LOG_ERR, "Failed to chdir to /: %m");
    exit(EXIT_FAILURE);
  }

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

int execute_command(const char *command, char **dest) {
  FILE *process = popen(command, "r");
  if (process == NULL) {
    syslog(LOG_ERR, "Failed to execute command: %s", command);
    return EXIT_FAILURE;
  }

  char buffer[BUFFER_SIZE];
  size_t bytesRead = fread(buffer, sizeof(char), sizeof(buffer) - 1, process);
  buffer[bytesRead] = '\0';

  syslog(LOG_INFO, "Command output: %s", buffer);

  *dest = strdup(buffer);

  pclose(process);

  return EXIT_SUCCESS;
}
