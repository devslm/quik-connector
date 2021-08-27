//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "MoneyLimitMapper.h"

bool toMoneyLimitDto(lua_State *luaState, MoneyLimitDto* moneyLimit) {
    if (!lua_istable(luaState, -1)) {
        LOGGER->error("Could not get table for money limit data! Current stack value type is: <<{}>> but required table!",
            luaGetType(luaState, -1));

        return false;
    }

    if (!luaGetTableNumberField(luaState, "money_open_limit", &moneyLimit->moneyOpenLimit)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "money_limit_locked_nonmarginal_value", &moneyLimit->moneyLimitLockedNonMarginalValue)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "money_limit_locked", &moneyLimit->moneyLimitLocked)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "money_open_balance", &moneyLimit->moneyOpenBalance)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "money_current_limit", &moneyLimit->moneyCurrentLimit)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "money_current_balance", &moneyLimit->moneyCurrentBalance)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "money_limit_available", &moneyLimit->moneyLimitAvailable)) {
        return false;
    }
    return true;
}

json toMoneyLimitJson(Option<MoneyLimitDto>& moneyLimitOption) {
    json jsonObject;

    if (moneyLimitOption.isEmpty()) {
        return jsonObject;
    }
    MoneyLimitDto moneyLimit = moneyLimitOption.get();
    jsonObject["moneyOpenLimit"] = moneyLimit.moneyOpenLimit;
    jsonObject["moneyLimitLockedNonMarginalValue"] = moneyLimit.moneyLimitLockedNonMarginalValue;
    jsonObject["moneyLimitLocked"] = moneyLimit.moneyLimitLocked;
    jsonObject["moneyOpenBalance"] = moneyLimit.moneyOpenBalance;
    jsonObject["moneyCurrentLimit"] = moneyLimit.moneyCurrentLimit;
    jsonObject["moneyCurrentBalance"] = moneyLimit.moneyCurrentBalance;
    jsonObject["moneyLimitAvailable"] = moneyLimit.moneyLimitAvailable;

    return jsonObject;
}
