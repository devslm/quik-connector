//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_LUA_H
#define QUIK_CONNECTOR_LUA_H

#include <string>
#include <mutex>
#include "../../dto/quik/lua/FunctionArgDto.h"
#include "../utils/debug/DebugFunctions.h"
#include "../utils/string/StringUtils.h"
#include "../../dto/option/Option.h"
#include "../config/ConfigService.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
};

using namespace std;

extern ConfigService *configService;

void luaInit(lua_State *L);

lua_State* luaGetState();

recursive_mutex* luaGetMutex();

Option<string> luaGetScriptPath(lua_State *luaState);

const char *luaGetType(lua_State *L, int index);

bool luaCallFunctionWithTableArg(lua_State *luaState,
                                 const char *name,
                                 int numArgs,
                                 uint8_t numReturns,
                                 map<string, string>& argTable);

bool luaCallFunction(lua_State *L, const char *name, int numArgs, uint8_t numReturns, FunctionArgDto *functionArgs);

string luaGetErrorMessage(lua_State *luaState);

void luaGcCollect(lua_State *luaState);

bool luaGetGlobal(lua_State *luaState, const string& key);

bool luaGetField(lua_State *luaState, int index, const string& key);

bool luaGetTableBooleanField(lua_State *L, const char *key, bool *buffer);

bool luaGetTableNumberFieldAsInt(lua_State *L, const char *key, int *buffer);

bool luaGetTableNumberField(lua_State *L, const char *key, double *buffer);

bool luaGetTableIntegerField(lua_State *L, const char *key, uint64_t *buffer);

bool luaGetTableStringField(lua_State *L, const char *key, string *buffer);

bool luaGetString(lua_State *L, string *buffer, int fieldIndexInStack = -1);

bool luaGetNumber(lua_State *L, double *buffer);

bool luaGetBoolean(lua_State *L, bool *buffer);

// Save reference to avoid GC remove
int luaSaveReference(lua_State *luaState);

// Push saved reference onto the stack
int luaLoadReference(lua_State *luaState, int index);

void luaRemoveReference(lua_State *luaState, int index);

void luaPrintStackSize(lua_State *luaState, const string& functionName);

#endif //QUIK_CONNECTOR_LUA_H
