//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#include "OrderValidator.h"

Option<string> validateCancelStopOrderData(CancelStopOrderRequestDto& cancelStopOrderRequest) {
    string errorMessage;

    if (cancelStopOrderRequest.stopOrderId == 0) {
        errorMessage += "id must be > 0 ";
    }
    if (cancelStopOrderRequest.account.empty()) {
        errorMessage += (errorMessage.empty() ? ", " : "");
        errorMessage += "account is required";
    }
    if (cancelStopOrderRequest.clientCode.empty()) {
        errorMessage += (errorMessage.empty() ? ", " : "");
        errorMessage += "client code is required ";
    }
    if (cancelStopOrderRequest.classCode.empty()) {
        errorMessage += (errorMessage.empty() ? ", " : "");
        errorMessage += "class code is required ";
    }
    if (cancelStopOrderRequest.ticker.empty()) {
        errorMessage += (errorMessage.empty() ? ", " : "");
        errorMessage += "ticker is required ";
    }

    if (!errorMessage.empty()) {
        return Option<string>(errorMessage);
    }
    return Option<string>();
}
