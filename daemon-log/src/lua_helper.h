#ifndef LUA_HELPER_H
#define LUA_HELPER_H

#include "helper.h"
#include "tuya_helper.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <time.h>

#define MAX_SCRIPTS 20
#define SCRIPTS_DIR "/usr/bin/scripts/"
#define LUA_FILE_EXTENSION ".lua"
#define LUA_OK 0

struct loaded_script {
  char file_name[BUFFER_SIZE];
  lua_State *L;
  int interval;
  time_t last_execution;
};

void load_script(struct loaded_script *scripts, int *num_scripts,
                 const char *script_path);
void call_get_data_hook(lua_State *L, char *response);
void call_destory_hooks(struct loaded_script *scripts, int num_scripts);
void execute_scripts(tuya_mqtt_context_t *client, struct arguments argument,
                     struct loaded_script *scripts, int num_scripts);
void execute_action(char *lua_script, char *action, char *input_params);
void load_lua_scripts(struct loaded_script *scripts, int *num_scripts);
void unload_lua_scripts(struct loaded_script *scripts, int num_scripts);

#endif // LUA_HELPER_H
