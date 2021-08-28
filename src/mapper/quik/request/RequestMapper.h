//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_REQUESTMAPPER_H
#define QUIK_CONNECTOR_REQUESTMAPPER_H

#include <string>
#include <nlohmann/json.hpp>
#include "../../../dto/option/Option.h"
#include "../../../dto/quik/connector/request/RequestDto.h"

using namespace nlohmann;
using namespace std;

static const size_t CANDLE_REQUEST_DTO_TYPE = typeid(CandlesRequestDto).hash_code();

static Option<CandlesRequestDto> toCandlesRequestDto(const json& jsonData);

template<class T> Option<T> toRequestDto(const json& jsonData) {
    try {
        const size_t dtoType = typeid(T).hash_code();

        if (dtoType == CANDLE_REQUEST_DTO_TYPE) {
            return toCandlesRequestDto(jsonData);
        }
    } catch (json::parse_error& exception) {
        LOGGER->error("Could not convert queue command: {} to request dto! Reason: {}!", jsonData.dump().c_str(), exception.what());
    }
    return Option<T>();
}

Option<TickersRequestDto> toGetTickersRequestDto(const json& jsonData) {
    TickersRequestDto tickersRequest;
    tickersRequest.classCode = jsonData["classCode"];

    return {tickersRequest};
}

Option<CandlesRequestDto> toCandlesRequestDto(const json& jsonData) {
    CandlesRequestDto candlesRequest;
    candlesRequest.classCode = jsonData["classCode"];
    candlesRequest.ticker = jsonData["ticker"];
    candlesRequest.interval = QuikUtils::getIntervalByName(jsonData["interval"]);

    return {candlesRequest};
}

#endif //QUIK_CONNECTOR_REQUESTMAPPER_H
