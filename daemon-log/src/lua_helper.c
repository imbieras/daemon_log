#include "lua_helper.h"
#include "helper.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

extern struct loaded_script loaded_scripts[MAX_SCRIPTS];
int num_loaded_scripts = 0;

void load_script(struct loaded_script *scripts, int *num_scripts,
                 const char *script_path) {
  if (*num_scripts >= MAX_SCRIPTS) {
    syslog(LOG_ERR, "Maximum number of scripts reached. Ignoring script: %s",
           script_path);
    return;
  }

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  if (luaL_loadfile(L, script_path) != LUA_OK) {
    syslog(LOG_ERR, "Failed to load script: %s", lua_tostring(L, -1));
    lua_close(L);
    return;
  }

  if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
    syslog(LOG_ERR, "Failed to execute script: %s", lua_tostring(L, -1));
    lua_close(L);
    return;
  }

  lua_getglobal(L, "get_data");
  if (lua_isfunction(L, -1)) {
    lua_CFunction get_data_hook = lua_tocfunction(L, -1);
    scripts[*num_scripts].get_data_hook = get_data_hook;
  } else {
    syslog(LOG_ERR,
           "Script missing mandatory 'get_data' function. Ignoring script: %s",
           script_path);
    lua_close(L);
    return;
  }
  lua_pop(L, 1);

  lua_getglobal(L, "init");
  if (lua_isfunction(L, -1)) {
    lua_CFunction init_hook = lua_tocfunction(L, -1);
    scripts[*num_scripts].init_hook = init_hook;
    // Call the init hook with the Lua state as an argument
    lua_pushlightuserdata(L, (void *)L);
    lua_call(L, 1, 0);
  } else {
    scripts[*num_scripts].init_hook = NULL;
  }
  lua_pop(L, 1);

  lua_getglobal(L, "destroy");
  if (lua_isfunction(L, -1)) {
    lua_CFunction destroy_hook = lua_tocfunction(L, -1);
    scripts[*num_scripts].destroy_hook = destroy_hook;
  } else {
    scripts[*num_scripts].destroy_hook = NULL;
  }
  lua_pop(L, 1);

  scripts[*num_scripts].L = L;
  extract_filename(script_path, scripts[*num_scripts].file_name);
  syslog(LOG_INFO, "Successfully loaded script: %s, name: %s", script_path,
         scripts[*num_scripts].file_name);
  (*num_scripts)++;
}

void call_get_data_hook(lua_State *L, lua_CFunction get_data_hook,
                        char *response) {
  lua_pushcfunction(L, get_data_hook);
  syslog(LOG_INFO, "a");
  if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
    syslog(LOG_ERR, "Error calling get_data hook: %s", lua_tostring(L, -1));
    lua_pop(L, 1);
    return;
  }
  syslog(LOG_INFO, "b");
  const char *json = lua_tostring(L, -1);
  syslog(LOG_INFO, "c");
  snprintf(response, BUFFER_SIZE, "%s", json);
  syslog(LOG_INFO, "d");
  lua_pop(L, 1);
  syslog(LOG_INFO, "e");
}

void execute_destroy_hooks(struct loaded_script *scripts, int num_scripts) {
  for (int i = 0; i < num_scripts; i++) {
    if (scripts[i].destroy_hook) {
      scripts[i].destroy_hook(scripts[i].L);
      syslog(LOG_INFO, "Called destroy on: %s", scripts[i].file_name);
    }
  }
}

void load_lua_scripts(struct loaded_script *scripts, int *num_scripts) {
  char *scripts_dir = malloc((strlen(SCRIPTS_DIR)) + 9);
  if (scripts_dir == NULL) {
    syslog(LOG_ERR, "Failed to allocate memory for directory path");
    return;
  }
  snprintf(scripts_dir, strlen(SCRIPTS_DIR) + 9, "%s%s", "/usr/bin",
           SCRIPTS_DIR);
  DIR *dir;
  struct dirent *entry;
  dir = opendir(scripts_dir);
  if (dir == NULL) {
    syslog(LOG_ERR, "Failed to open scripts directory: %s", scripts_dir);
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
          syslog(LOG_ERR, "Failed to allocate memory for file path");
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
    scripts[i].get_data_hook = NULL;
    scripts[i].init_hook = NULL;
    scripts[i].destroy_hook = NULL;
    lua_close(scripts[i].L);
  }
}