//
// Created by Sergey on 25.06.2021.
//

#ifndef QUIK_CONNECTOR_LUA_H
#define QUIK_CONNECTOR_LUA_H

#include <string>
#include <mutex>
#include "../../dto/lua/FunctionArgDto.h"
#include "../utils/debug/DebugFunctions.h"
#include "../utils/string/StringUtils.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "../../service/log/log.h"
};

using namespace std;

void luaInit(lua_State *L);

lua_State* luaGetState();

recursive_mutex *luaGetMutex();

bool luaGetScriptPath(lua_State *luaState, string *buffer);

const char *luaGetType(lua_State *L, int index);

bool luaCallFunction(lua_State *L, const char *name, int numArgs, uint8_t numReturns, FunctionArgDto *functionArgs);

const char *luaGetErrorMessage(lua_State *luaState);

void luaGcCollect(lua_State *luaState);

bool luaGetTableNumberField(lua_State *L, const char *key, double *buffer);

bool luaGetTableStringField(lua_State *L, const char *key, string *buffer);

bool luaGetString(lua_State *L, string *buffer);

int luaLoadReference(lua_State *luaState, int index);

#endif //QUIK_CONNECTOR_LUA_H
