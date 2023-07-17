#include "lua_helper.h"
#include "helper.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

extern struct loaded_script loaded_scripts[MAX_SCRIPTS];

void load_script(struct loaded_script *scripts, int *num_scripts,
                 const char *script_path) {
  if (*num_scripts >= MAX_SCRIPTS) {
    printf("Maximum number of scripts reached.\n");
    return;
  }

  struct loaded_script *script = &scripts[*num_scripts];
  strncpy(script->file_name, script_path, BUFFER_SIZE);

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  if (luaL_dofile(L, script_path) != LUA_OK) {
    printf("Failed to load and execute the Lua file: %s\n",
           lua_tostring(L, -1));
    lua_close(L);
    return;
  }

  lua_getglobal(L, "get_data");
  if (!lua_isfunction(L, -1)) {
    syslog(LOG_ERR,
           "Script missing mandatory 'get_data' function. Ignoring script: %s",
           script_path);
    lua_close(L);
    return;
  }
  lua_pop(L, 1);

  lua_getglobal(L, "init");
  if (lua_isfunction(L, -1)) {
    lua_call(L, 0, 0);
  }

  lua_getglobal(L, "destroy");
  if (lua_isfunction(L, -1)) {
    lua_pop(L, 1);
  }

  scripts[*num_scripts].L = L;
  extract_filename(script_path, scripts[*num_scripts].file_name);
  printf("Successfully loaded script: %s, name: %s\n", script_path,
         scripts[*num_scripts].file_name);
  (*num_scripts)++;
}

void call_get_data_hook(lua_State *L, char *response) {
  lua_getglobal(L, "get_data");
  lua_call(L, 0, 1);

  const char *json = lua_tostring(L, -1);
  snprintf(response, BUFFER_SIZE, "%s", json);
  lua_pop(L, 1);
}

void call_destory_hooks(struct loaded_script *scripts, int num_scripts) {
  for (int i = 0; i < num_scripts; i++) {
    lua_getglobal(scripts[i].L, "destroy");
    lua_call(scripts[i].L, 0, 0);
  }
}

void load_lua_scripts(struct loaded_script *scripts, int *num_scripts) {
  char *scripts_dir = malloc(strlen(SCRIPTS_DIR) + 1);
  if (scripts_dir == NULL) {
    return;
  }
  snprintf(scripts_dir, strlen(SCRIPTS_DIR) + 1, "%s", SCRIPTS_DIR);
  DIR *dir;
  struct dirent *entry;
  dir = opendir(scripts_dir);
  if (dir == NULL) {
    free(scripts_dir);
    return;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      char *filename = entry->d_name;
      size_t filename_len = strlen(filename);
      size_t extension_len = strlen(LUA_FILE_EXTENSION);

      if (filename_len > extension_len &&
          strcmp(filename + filename_len - extension_len, LUA_FILE_EXTENSION) ==
              0) {
        char *filepath =
            malloc((strlen(scripts_dir) + filename_len + 2) * sizeof(char));
        if (filepath == NULL) {
          continue;
        }
        snprintf(filepath, strlen(scripts_dir) + filename_len + 2, "%s/%s",
                 scripts_dir, filename);
        load_script(scripts, num_scripts, filepath);
        free(filepath);
      }
    }
  }

  closedir(dir);
  free(scripts_dir);
}

void unload_lua_scripts(struct loaded_script *scripts, int num_scripts) {
  for (int i = 0; i < num_scripts; i++) {
    lua_close(scripts[i].L);
  }
}
