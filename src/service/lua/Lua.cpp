//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#include <cstdint>
#include "Lua.h"

using namespace std;

static lua_State *luaMainThreadState;
static recursive_mutex mutexLock;
static ConfigDto config;

void luaInit(lua_State *L) {
    if (luaMainThreadState != nullptr) {
        free(luaMainThreadState);
    }
    luaMainThreadState = L;
    config = configService->getConfig();
}

lua_State* luaGetState() {
    return luaMainThreadState;
}

recursive_mutex* luaGetMutex() {
    return &mutexLock;
}

Option<string> luaGetScriptPath(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    string path;
    bool isSuccess = luaCallFunction(luaState, "getScriptPath", 0, 1, nullptr);

    if (!isSuccess) {
        return {};
    }
    isSuccess = luaGetString(luaState, &path);

    if (isSuccess) {
        return {path};
    }
    return {};
}

const char *luaGetType(lua_State *L, int index) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    return lua_typename(L, lua_type(L, index));
}

string luaGetErrorMessage(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    string errorMessage;

    if (luaGetString(luaState, &errorMessage)) {
        return errorMessage;
    }
    return "<<Can't read LUA error message>>";
}

void luaGcCollect(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    lua_gc(luaState, LUA_GCCOLLECT, 0);
}

bool luaGetGlobal(lua_State *luaState, const string& key) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    lua_getglobal(luaState, key.c_str());

    if (!lua_isnil(luaState, -1)) {
        return true;
    }
    return false;
}

bool luaGetField(lua_State *luaState, int index, const string& key) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    if (lua_isnil(luaState, index)) {
        LOGGER->error("Could not get lua field: {} with index: 0 because index 0 is null!", key);
        return false;
    }
    lua_getfield(luaState, index, key.c_str());

    if (!lua_isnil(luaState, -1)) {
        return true;
    }
    return false;
}

bool luaCallFunctionWithTableArg(lua_State *luaState,
                                 const char *name,
                                 int numArgs,
                                 uint8_t numReturns,
                                 map<string, string>& argTable) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    lua_getglobal(luaState, name);

    if (!lua_isfunction(luaState, -1)) {
        LOGGER->error("Could not call lua function: {} and map args! Current stack value type is: <<{}>> but required function!",
            name, luaGetType(luaState, -1));

        return false;
    }
    lua_newtable(luaState);

    for(auto argIterator = argTable.begin(); argIterator != argTable.end(); ++argIterator) {
        lua_pushstring(luaState, argIterator->first.c_str());
        lua_pushstring(luaState, argIterator->second.c_str());
        lua_settable(luaState, -3);
    }
    int result = lua_pcall(luaState, numArgs, numReturns, 0);

    if (LUA_OK != result) {
        string errorMessage;

        luaGetString(luaState, &errorMessage);

        LOGGER->error("Could not call lua function: {}! Reason: {}!", name, errorMessage);

        return false;
    }
    return true;
}

bool luaCallFunction(lua_State *L, const char *name, int numArgs, uint8_t numReturns, FunctionArgDto *functionArgs) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    lua_getglobal(L, name);

    if (!lua_isfunction(L, -1)) {
        char argsString[512] = {0};

        debugLuaFunctionArgsToString(functionArgs, numArgs, argsString, 512);

        LOGGER->error("Could not call lua function: {} and args: {}! Current stack value type is: <<{}>> but required function!",
             name, argsString, luaGetType(L, -1));

        return false;
    }

    if (functionArgs != nullptr) {
        FunctionArgDto arg;

        for (int i = 0; i < numArgs; ++i) {
            arg = functionArgs[i];

            switch (arg.type) {
                case STRING_TYPE:
                    lua_pushstring(L, arg.valueString.c_str());
                    break;
                case INTEGER_TYPE:
                    lua_pushinteger(L, arg.valueInt);
                    break;
                case DOUBLE_TYPE:
                    lua_pushnumber(L, arg.valueDouble);
                    break;
                case BOOLEAN_TYPE:
                    lua_pushboolean(L, arg.valueBoolean);
                    break;
                default:
                    LOGGER->error("Could not call ticker function: {} because argument: #{} has unknown type: {}",
                        name, i + 1, arg.type);

                    return false;
            }
        }
    }
    int result = lua_pcall(L, numArgs, numReturns, 0);

    if (LUA_OK != result) {
        string errorMessage;

        luaGetString(L, &errorMessage);

        LOGGER->error("Could not call lua function: {}! Reason: {}!", name, errorMessage);

        return false;
    }
    return true;
}

bool luaGetTableBooleanField(lua_State *L, const char *key, bool *buffer) {
    LOGGER->debug("Get lua table boolean field with key: {} from table", key);

    lock_guard<recursive_mutex> lockGuard(mutexLock);

    double value;

    bool isSuccess = luaGetTableNumberField(L, key, &value);

    if (isSuccess) {
        *buffer = ((int)value > 0 ? true : false);
    }
    return true;
}

bool luaGetTableNumberFieldAsInt(lua_State *L, const char *key, int *buffer) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    double value;
    auto isSuccess = luaGetTableNumberField(L, key, &value);

    if (!isSuccess) {
        return false;
    }
    *buffer = (int)value;

    return true;
}

bool luaGetTableNumberField(lua_State *L, const char *key, double *buffer) {
    LOGGER->debug("Get lua table number field with key: {} from table", key);

    lock_guard<recursive_mutex> lockGuard(mutexLock);

    int fieldIndexInStack = -1;

    if (!lua_istable(L, fieldIndexInStack)) {
        LOGGER->error("Could not get table number field with key: {}! Current stack value type is: <<{}>> but required table!",
             key, luaGetType(L, fieldIndexInStack));

        *buffer = 0.0;

        return false;
    }
    lua_getfield(L, fieldIndexInStack, key);

    if (!lua_isnumber(L, fieldIndexInStack)) {
        LOGGER->error("Could not get table number field with key: {} from table! Current stack value type is: <<{}>> but required number!",
             key, luaGetType(L, fieldIndexInStack));

        return false;
    }
    *buffer = lua_tonumber(L, fieldIndexInStack);

    lua_pop(L,1);

    return true;
}

bool luaGetTableIntegerField(lua_State *L, const char *key, uint64_t *buffer) {
    LOGGER->debug("Get lua table integer field with key: {} from table", key);

    lock_guard<recursive_mutex> lockGuard(mutexLock);

    int fieldIndexInStack = -1;

    if (!lua_istable(L, fieldIndexInStack)) {
        LOGGER->error("Could not get table integer field with key: {}! Current stack value type is: <<{}>> but required table!",
             key, luaGetType(L, fieldIndexInStack));

        *buffer = 0.0;

        return false;
    }
    lua_getfield(L, fieldIndexInStack, key);

    if (!lua_isinteger(L, fieldIndexInStack)) {
        LOGGER->error("Could not get table integer field with key: {} from table! Current stack value type is: <<{}>> but required integer!",
             key, luaGetType(L, fieldIndexInStack));

        return false;
    }
    *buffer = (uint64_t)lua_tointeger(L, fieldIndexInStack);

    lua_pop(L,1);

    return true;
}

bool luaGetTableStringField(lua_State *L, const char *key, string *buffer) {
    LOGGER->debug("Get lua table string field with key: {} from table", key);

    lock_guard<recursive_mutex> lockGuard(mutexLock);

    int fieldIndexInStack = -1;

    if (!lua_istable(L, fieldIndexInStack)) {
        LOGGER->error("Could not get table string field with key: {}! Current stack value type is: <<{}>> but required table!",
             key, luaGetType(L, fieldIndexInStack));

        return false;
    }
    lua_getfield(L, fieldIndexInStack, key);

    return luaGetString(L, buffer);
}

bool luaGetString(lua_State *L, string *buffer) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    int fieldIndexInStack = -1;

    if (!lua_isstring(L, fieldIndexInStack)) {
        LOGGER->error("Could not get plain string value! Current stack value type is: <<{}>> but required string!",
             luaGetType(L, fieldIndexInStack));

        return false;
    }
    char tmpBuffer[2048] = {0};

    convertToUtf8(lua_tostring(L, fieldIndexInStack), tmpBuffer);

    *buffer = tmpBuffer;

    lua_pop(L,1);

    return true;
}

bool luaGetNumber(lua_State *L, double *buffer) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    int fieldIndexInStack = -1;

    if (!lua_isnumber(L, fieldIndexInStack)) {
        LOGGER->error("Could not get plain number value! Current stack value type is: <<{}>> but required number!",
             luaGetType(L, fieldIndexInStack));

        return false;
    }
    *buffer = lua_tonumber(L, fieldIndexInStack);

    lua_pop(L,1);

    return true;
}

bool luaGetBoolean(lua_State *L, bool *buffer) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    int fieldIndexInStack = -1;

    if (!lua_isboolean(L, fieldIndexInStack)) {
        LOGGER->error("Could not get plain boolean value! Current stack value type is: <<{}>> but required boolean!",
            luaGetType(L, fieldIndexInStack));

        return false;
    }
    int value = lua_toboolean(L, fieldIndexInStack);

    *buffer = (value > 0 ? true : false);

    return true;
}

int luaSaveReference(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    return luaL_ref(luaState, LUA_REGISTRYINDEX);
}

int luaLoadReference(lua_State *luaState, int index) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    // Push reference onto the stack
    lua_rawgeti(luaState, LUA_REGISTRYINDEX, index);
    // Get reference
    return lua_gettop(luaState);
}

void luaRemoveReference(lua_State *luaState, int index) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    luaL_unref(luaState, LUA_REGISTRYINDEX, index);
}

void luaPrintStackSize(lua_State *luaState, const string& functionName) {
    if (config.debug.printLuaStack) {
        lock_guard<recursive_mutex> lockGuard(mutexLock);

        LOGGER->info("Stack size for <<{}>> is: {}", functionName, lua_gettop(luaState));
    }
}
