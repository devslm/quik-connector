//
//

#ifndef QUIK_CONNECTOR_ORDERVALIDATOR_H
#define QUIK_CONNECTOR_ORDERVALIDATOR_H

#include "string"
#include "../../../dto/connector/request/RequestDto.h"

using namespace std;

Option<string> validateCancelStopOrderData(CancelStopOrderRequestDto& cancelStopOrderRequest);

#endif //QUIK_CONNECTOR_ORDERVALIDATOR_H
