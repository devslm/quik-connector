//
// Created by Sergey on 25.06.2021.
//

#include <time.h>
#include "DateMapper.h"

using namespace std;

bool toDateMillis(lua_State *L, const char *fieldName, uint64_t *dateTime) {
    if (!lua_istable(L, -1)) {
        logError("Could not get table for date data! Current stack value type is: <<%s>> but required table!", luaGetType(L, -1));

        return false;
    }
    lua_getfield(L, -1, fieldName);

    return toPlainDateMillis(L, dateTime);
}

bool toPlainDateMillis(lua_State *L, uint64_t *dateTime) {
    struct tm date = {0};

    lua_getfield(L, -1, "sec");
    date.tm_sec = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "min");
    date.tm_min = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "hour");
    date.tm_hour = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "day");
    date.tm_mday = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "week_day");
    date.tm_wday = lua_tonumber(L, -1) - 1;
    lua_pop(L, 1);

    lua_getfield(L, -1, "month");
    date.tm_mon = lua_tonumber(L, -1) - 1;
    lua_pop(L, 1);

    lua_getfield(L, -1, "year");
    date.tm_year = (int)lua_tonumber(L, -1) - 1900;
    lua_pop(L, 1);

    lua_getfield(L, -1, "ms");
    auto milliseconds = (uint64_t)lua_tonumber(L, -1);
    lua_pop(L, 1);

    auto result = mktime(&date);

    if (result > -1) {
        *dateTime = (result * (uint64_t)1000) + milliseconds;
    } else {
        *dateTime = 0;
    }
    lua_pop(L, 1);

    return true;
}
