#ifndef LUA_HELPER_H
#define LUA_HELPER_H

#include "helper.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#define MAX_SCRIPTS 20
#define SCRIPTS_DIR "/scripts"
#define LUA_FILE_EXTENSION ".lua"
#define LUA_OK 0

struct loaded_script {
  char file_name[BUFFER_SIZE];
  lua_State *L;
  lua_CFunction init_hook;
  lua_CFunction destroy_hook;
  lua_CFunction get_data_hook;
};

void load_script(struct loaded_script *scripts, int *num_scripts,
                 const char *script_path);
void call_get_data_hook(lua_State *L, lua_CFunction get_data_hook, char* response);
void execute_destroy_hooks(struct loaded_script *scripts, int num_scripts);
void load_lua_scripts(struct loaded_script *scripts, int *num_scripts);
void unload_lua_scripts(struct loaded_script *scripts, int num_scripts);

#endif // LUA_HELPER_H
