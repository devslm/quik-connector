//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_RESPONSEDTO_H
#define QUIK_CONNECTOR_RESPONSEDTO_H

#include <string>
#include <nlohmann/json.hpp>
#include "../../component/uuid/Uuid.h"

using namespace std;
using namespace nlohmann;

static const string RESPONSE_QUIK_LUA_ERROR = "QUIK_LUA_ERROR";
static const string RESPONSE_COMMAND_REQUEST_ERROR = "COMMAND_REQUEST_ERROR";

static const string RESPONSE_DEFAULT_ERROR_MESSAGE = "Details not available!";

typedef struct SuccessResponseDto {
    SuccessResponseDto(const json data) {
        this->id = slmdev::Uuid::createRandom();
        this->data = move(data);
    }
    SuccessResponseDto(const string id, const json data) {
        this->id = move(id);
        this->data = move(data);
    }
    string id;
    int status;
    json data;
} SuccessResponseDto;

typedef struct ErrorResponseDto {
    ErrorResponseDto(const string code, const string detail) {
        this->id = slmdev::Uuid::createRandom();
        this->code = move(code);
        this->detail = move(detail);
    }
    ErrorResponseDto(const string id, const string code, const string detail) {
        this->id = move(id);
        this->code = move(code);
        this->detail = move(detail);
    }
    string id;
    string code;
    string detail;
} ErrorResponseDto;

#endif //QUIK_CONNECTOR_RESPONSEDTO_H
