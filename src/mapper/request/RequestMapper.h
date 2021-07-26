//
// Created by Sergey on 13.07.2021.
//

#ifndef QUIK_CONNECTOR_REQUESTMAPPER_H
#define QUIK_CONNECTOR_REQUESTMAPPER_H

#include <string>
#include <nlohmann/json.hpp>
#include "../../dto/option/Option.h"
#include "../../dto/connector/request/RequestDto.h"

using namespace nlohmann;
using namespace std;

static const size_t LAST_CANDLE_REQUEST_DTO_TYPE = typeid(LastCandleRequestDto).hash_code();

static Option<LastCandleRequestDto> toLastCandleRequestDto(const json& jsonData);

template<class T> Option<T> toRequestDto(const json& jsonData) {
    try {
        const size_t dtoType = typeid(T).hash_code();

        if (dtoType == LAST_CANDLE_REQUEST_DTO_TYPE) {
            return toLastCandleRequestDto(jsonData);
        }
    } catch (json::parse_error& exception) {
        LOGGER->error("Could not convert queue command: {} to request dto! Reason: {}!", jsonData.dump().c_str(), exception.what());
    }
    return Option<T>();
}

Option<LastCandleRequestDto> toLastCandleRequestDto(const json& jsonData) {
    LastCandleRequestDto lastCandleRequest;
    lastCandleRequest.classCode = jsonData["classCode"];
    lastCandleRequest.ticker = jsonData["ticker"];
    lastCandleRequest.interval = QuikUtils::getIntervalByName(jsonData["interval"]);

    return Option<LastCandleRequestDto>(lastCandleRequest);
}

#endif //QUIK_CONNECTOR_REQUESTMAPPER_H
