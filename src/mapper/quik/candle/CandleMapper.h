//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_CANDLEMAPPER_H
#define QUIK_CONNECTOR_CANDLEMAPPER_H

#include <string>
#include <mutex>
#include <nlohmann/json.hpp>
#include "../../../service/lua/Lua.h"
#include "../date/DateMapper.h"
#include "../../../dto/quik/candle/CandleDto.h"
#include "../../../dto/option/Option.h"
#include "../../../dto/quik/connector/subscription/QuikSubscriptionDto.h"
#include "../../../dto/quik/candle/ChangedCandleDto.h"

using namespace nlohmann;
using namespace std;

extern shared_ptr<spdlog::logger> logger;

bool toCandleDto(QuikSubscriptionDto *candleSubscription, CandleDto *candle, int candleFirstIndex, int candleLastIndex);

Option<ChangedCandleDto> toChangedCandleDto(Option<CandleDto>& candle);

json toCandleJson(const list<CandleDto>& candles);

json toCandleJson(Option<CandleDto>& candle);

json toChangedCandleJson(Option<ChangedCandleDto>& changedCandleOption);

#endif //QUIK_CONNECTOR_CANDLEMAPPER_H
