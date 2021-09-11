//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "ResponseMapper.h"

json toResponseJson(SuccessResponseDto& response) {
    json jsonObject;
    jsonObject["id"] = response.id;
    jsonObject["data"] = response.data;

    return jsonObject;
}

json toResponseJson(ErrorResponseDto& response) {
    json jsonObject;
    jsonObject["id"] = response.id;

    json errorObject;
    errorObject["code"] = response.code;
    errorObject["detail"] = response.detail;

    jsonObject["errors"] = errorObject;

    return jsonObject;
}
