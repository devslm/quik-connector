//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#include <ctime>
#include "DateMapper.h"

using namespace std;

bool toDateMillis(lua_State *luaState, const char *fieldName, uint64_t *dateTime) {
    if (!lua_istable(luaState, -1)) {
        LOGGER->error("Could not get table for date data! Current stack value type is: <<{}>> but required table!", luaGetType(luaState, -1));

        return false;
    }
    lua_getfield(luaState, -1, fieldName);

    return toPlainDateMillis(luaState, dateTime);
}

bool toPlainDateMillis(lua_State *luaState, uint64_t *dateTime) {
    struct tm date = {0};

    if (!luaGetTableNumberFieldAsInt(luaState, "sec", &date.tm_sec)) {
        return false;
    }
    if (!luaGetTableNumberFieldAsInt(luaState, "min", &date.tm_min)) {
        return false;
    }
    if (!luaGetTableNumberFieldAsInt(luaState, "hour", &date.tm_hour)) {
        return false;
    }
    if (!luaGetTableNumberFieldAsInt(luaState, "day", &date.tm_mday)) {
        return false;
    }
    if (!luaGetTableNumberFieldAsInt(luaState, "month", &date.tm_mon)) {
        return false;
    }
    if (!luaGetTableNumberFieldAsInt(luaState, "year", &date.tm_year)) {
        return false;
    }
    date.tm_mon = date.tm_mon - 1;
    date.tm_year = date.tm_year - 1900;

    double milliseconds;

    if (!luaGetTableNumberField(luaState, "ms", &milliseconds)) {
        return false;
    }
    auto result = mktime(&date);

    if (result > -1) {
        *dateTime = (result * (uint64_t)1000) + (uint64_t)milliseconds;
    } else {
        *dateTime = 0;
    }
    lua_pop(luaState, 1);

    return true;
}
