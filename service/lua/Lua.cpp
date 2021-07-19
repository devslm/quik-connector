//
// Created by Sergey on 25.06.2021.
//

#include <cstdint>
#include "Lua.h"

using namespace std;

static lua_State *luaState;
static recursive_mutex mutexLock;

void luaInit(lua_State *L) {
    luaState = L;
}

lua_State* luaGetState() {
    return luaState;
}

recursive_mutex *luaGetMutex() {
    return &mutexLock;
}

bool luaGetScriptPath(lua_State *luaState, string *buffer) {
    lock_guard<recursive_mutex> lockGuard(mutexLock);

    bool isSuccess = luaCallFunction(luaState, "getScriptPath", 0, 1, nullptr);

    if (!isSuccess) {
        return false;
    }
    isSuccess = luaGetString(luaState, buffer);

    if (isSuccess) {
        return true;
    }
    return false;
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

        logError("Could not call lua function: %s and args: %s! Current stack value type is: <<%s>> but required function!",
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
                logError("Could not call ticker function: %s because argument: #%d has unknown type: %d",
                     name, i + 1, arg.type);

                return false;
        }
    }
    int result = lua_pcall(L, numArgs, numReturns, 0);

    if (LUA_OK != result) {
        logError("Could not call lua function!");

        return false;
    }
    return true;
}

bool luaGetPlainBooleanField(lua_State *L, const char *fieldName, bool *buffer) {
    logDebug("Get lua plain boolean field for: %s", fieldName);

    lock_guard<recursive_mutex> lockGuard(mutexLock);

    //int intBuffer;

    //bool isSuccess = luaGetPlainNumberField(L, functionName, &intBuffer);

    //if (isSuccess) {
    //    *buffer = (intBuffer == 1 ? true : false);
    //}
    return true;
}

bool luaGetTableNumberField(lua_State *L, const char *key, double *buffer) {
    logDebug("Get lua table number field with key: %s from table", key);

    lock_guard<recursive_mutex> lockGuard(mutexLock);

    int fieldIndexInStack = -1;

    if (!lua_istable(L, fieldIndexInStack)) {
        logError("Could not get table number field with key: %s! Current stack value type is: <<%s>> but required table!",
             key, luaGetType(L, fieldIndexInStack));

        *buffer = 0.0;

        return false;
    }
    lua_getfield(L, fieldIndexInStack, key);

    if (!lua_isnumber(L, fieldIndexInStack)) {
        logError("Could not get table string field with key: %s from table! Current stack value type is: <<%s>> but required number!",
             key, luaGetType(L, fieldIndexInStack));

        return false;
    }
    *buffer = lua_tonumber(L, fieldIndexInStack);

    lua_pop(L,1);

    return true;
}

bool luaGetTableStringField(lua_State *L, const char *key, string *buffer) {
    logDebug("Get lua table string field with key: %s from table", key);

    lock_guard<recursive_mutex> lockGuard(mutexLock);

    int fieldIndexInStack = -1;

    if (!lua_istable(L, fieldIndexInStack)) {
        logError("Could not get table string field with key: %s! Current stack value type is: <<%s>> but required table!",
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
        logError("Could not get plain string value! Current stack value type is: <<%s>> but required string!",
             luaGetType(L, fieldIndexInStack));

        return false;
    }
    char tmpBuffer[2048] = {0};

    convertToUtf8(lua_tostring(L, fieldIndexInStack), tmpBuffer);

    *buffer = tmpBuffer;

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
