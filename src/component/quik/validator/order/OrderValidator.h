//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_ORDERVALIDATOR_H
#define QUIK_CONNECTOR_ORDERVALIDATOR_H

#include "string"
#include "../../../../dto/quik/connector/request/RequestDto.h"

using namespace std;

Option<string> validateCancelStopOrderData(CancelStopOrderRequestDto& cancelStopOrderRequest);

#endif //QUIK_CONNECTOR_ORDERVALIDATOR_H
