//
// Created by Sergey on 01.07.2021.
//

#ifndef QUIK_CONNECTOR_CANDLEMAPPER_H
#define QUIK_CONNECTOR_CANDLEMAPPER_H

#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../service/lua/Lua.h"
#include "../date/DateMapper.h"
#include "../../dto/candle/CandleDto.h"
#include "../../dto/option/Option.h"
#include "../../dto/connector/subscription/QuikSubscriptionDto.h"
#include "../../dto/candle/ChangedCandleDto.h"

using namespace nlohmann;
using namespace std;

bool toCandleDto(QuikSubscriptionDto *candleSubscription, CandleDto *candle, int candleFirstIndex, int candleLastIndex);

Option<ChangedCandleDto> toChangedCandleDto(Option<CandleDto> *candle);

json toCandleJson(Option<CandleDto> *candle);

json toChangedCandleJson(Option<ChangedCandleDto> *changedCandleOption);

#endif //QUIK_CONNECTOR_CANDLEMAPPER_H
