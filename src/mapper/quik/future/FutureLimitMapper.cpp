//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "FutureLimitMapper.h"

bool toFutureLimitDto(lua_State *luaState, FutureLimitDto* futureLimit) {
    if (!lua_istable(luaState, -1)) {
        logger->error("Could not get table for futures limit data! Current stack value type is: <<{}>> but required table!",
            luaGetType(luaState, -1));

        lua_pop(luaState, 1);

        return false;
    }

    if (!luaGetTableStringField(luaState, "firmid", &futureLimit->firmId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "trdaccid", &futureLimit->trdAccId)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "limit_type", &futureLimit->limitType)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "liquidity_coef", &futureLimit->liquidityCoef)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "cbp_prev_limit", &futureLimit->cbpPrevLimit)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "cbplused", &futureLimit->cbplUsed)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "cbplplanned", &futureLimit->cbplPlanned)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "varmargin", &futureLimit->varMargin)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "accruedint", &futureLimit->accruedInt)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "cbplused_for_orders", &futureLimit->cbplUsedForOrders)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "cbplused_for_positions", &futureLimit->cbplUsedForPositions)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "options_premium", &futureLimit->optionsPremium)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "ts_comission", &futureLimit->tsComission)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "kgo", &futureLimit->kgo)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "currcode", &futureLimit->currencyCode)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "real_varmargin", &futureLimit->realVarMargin)) {
        return false;
    }
    lua_pop(luaState, 1);

    luaPrintStackSize(luaState, (string)__FUNCTION__);

    return true;
}

json toFutureLimitJson(Option<FutureLimitDto>& futureLimitOption) {
    json jsonObject;

    if (futureLimitOption.isEmpty()) {
        return jsonObject;
    }
    FutureLimitDto futureLimit = futureLimitOption.get();
    jsonObject["firmId"] = futureLimit.firmId;
    jsonObject["trdAccId"] = futureLimit.trdAccId;
    jsonObject["limitType"] = futureLimit.limitType;
    jsonObject["liquidityCoef"] = futureLimit.liquidityCoef;
    jsonObject["cbpPrevLimit"] = futureLimit.cbpPrevLimit;
    jsonObject["cbplUsed"] = futureLimit.cbplUsed;
    jsonObject["cbplPlanned"] = futureLimit.cbplPlanned;
    jsonObject["varMargin"] = futureLimit.varMargin;
    jsonObject["accruedInt"] = futureLimit.accruedInt;
    jsonObject["cbplUsedForOrders"] = futureLimit.cbplUsedForOrders;
    jsonObject["cbplUsedForPositions"] = futureLimit.cbplUsedForPositions;
    jsonObject["optionsPremium"] = futureLimit.optionsPremium;
    jsonObject["tsComission"] = futureLimit.tsComission;
    jsonObject["kgo"] = futureLimit.kgo;
    jsonObject["currencyCode"] = futureLimit.currencyCode;
    jsonObject["realVarMargin"] = futureLimit.realVarMargin;

    return jsonObject;
}
