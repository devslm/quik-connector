//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_RESPONSEMAPPER_H
#define QUIK_CONNECTOR_RESPONSEMAPPER_H

#include <string>
#include <nlohmann/json.hpp>
#include "../../dto/option/Option.h"
#include "../../dto/response/ResponseDto.h"

using namespace std;
using namespace nlohmann;

json toResponseJson(SuccessResponseDto& response);

json toResponseJson(ErrorResponseDto& response);

#endif //QUIK_CONNECTOR_RESPONSEMAPPER_H
