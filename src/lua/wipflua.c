/* Windows IP Filter: Lua interface */

#define _WIN32_WINNT	0x0600

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h>
#include <fwpmu.h>

#define LUA_LIB

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "../common.h"

#include "../wipflog.c"


/*
 * Returns: string
 */
static int
wipf_lua_device_name (lua_State *L)
{
  lua_pushliteral(L, WIPF_DEVICE_NAME);
  return 1;
}

/*
 * Returns: number
 */
static int
wipf_lua_ioctl_getlog (lua_State *L)
{
  lua_pushinteger(L, WIPF_IOCTL_GETLOG);
  return 1;
}

/*
 * Returns: number
 */
static int
wipf_lua_ioctl_setconf (lua_State *L)
{
  lua_pushinteger(L, WIPF_IOCTL_SETCONF);
  return 1;
}

/*
 * Returns: number
 */
static int
wipf_lua_buffer_size (lua_State *L)
{
  lua_pushinteger(L, WIPF_BUFFER_SIZE);
  return 1;
}

/*
 * Arguments: output (ludata), remote_ip (number),
 *	pid (number), path (string)
 * Returns: length (number)
 */
static int
wipf_lua_log_write (lua_State *L)
{
  char *out = lua_touserdata(L, 1);
  const int remote_ip = lua_tointeger(L, 2);
  const int pid = lua_tointeger(L, 3);
  size_t path_len;
  const char *path = lua_tolstring(L, 4, &path_len);

  if (out) {
    wipf_log_write(out, remote_ip, pid, path_len, path);

    lua_pushinteger(L, WIPF_LOG_SIZE(path_len));
    return 1;
  }
  return 0;
}


static luaL_Reg wipf_lib[] = {
  {"device_name",	wipf_lua_device_name},
  {"ioctl_getlog",	wipf_lua_ioctl_getlog},
  {"ioctl_setconf",	wipf_lua_ioctl_setconf},
  {"buffer_size",	wipf_lua_buffer_size},
  {"log_write",		wipf_lua_log_write},
  {NULL, NULL}
};


LUALIB_API int
luaopen_wipflua (lua_State *L)
{
  luaL_register(L, "wipflua", wipf_lib);
  return 1;
}
