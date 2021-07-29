//
// Created by Sergey on 25.06.2021.
//

#include <cstdint>
#include "Lua.h"

using namespace std;

static lua_State *luaMainThreadState;
static recursive_mutex mutexLock;

void luaInit(lua_State *L) {
    if (luaMainThreadState != nullptr) {
        free(luaMainThreadState);
    }
    luaMainThreadState = L;
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
        return Option<string>();
    }
    isSuccess = luaGetString(luaState, &path);

    if (isSuccess) {
        return Option<string>(path);
    }
    return Option<string>();
}

const char *luaGetType(lua_State *L, int index) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    return lua_typename(L, lua_type(L, index));
}

const char *luaGetErrorMessage(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    return lua_tostring(luaState, -1);
}

void luaGcCollect(lua_State *luaState) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    lua_gc(luaState, LUA_GCCOLLECT, 0);
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
    int result = lua_pcall(L, numArgs, numReturns, 0);

    if (LUA_OK != result) {
        LOGGER->error("Could not call lua function!");

        return false;
    }
    return true;
}

bool luaGetPlainBooleanField(lua_State *L, const char *fieldName, bool *buffer) {
    LOGGER->debug("Get lua plain boolean field for: {}", fieldName);

    lock_guard<recursive_mutex> lockGuard(mutexLock);

    //int intBuffer;

    //bool isSuccess = luaGetPlainNumberField(L, functionName, &intBuffer);

    //if (isSuccess) {
    //    *buffer = (intBuffer == 1 ? true : false);
    //}
    return true;
}

bool luaGetTableNumberFieldAsInt(lua_State *L, const char *key, int *buffer) {
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

int luaLoadReference(lua_State *luaState, int index) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    // Push reference onto the stack
    lua_rawgeti(luaState, LUA_REGISTRYINDEX, index);
    // Get reference

    return lua_gettop(luaState);
}
