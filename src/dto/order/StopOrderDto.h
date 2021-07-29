//
//

#ifndef QUIK_CONNECTOR_STOPORDERDTO_H
#define QUIK_CONNECTOR_STOPORDERDTO_H

#include <string>

using namespace std;

typedef struct StopOrderDto {
    uint64_t orderNum;
    uint64_t date;
    double flags = 0.0;
    string brokerRef;
    string firmId;
    string account;
    string clientCode;
    string classCode;
    string classType;
    string ticker;
    string name;
    uint64_t condition;
    string conditionType;
    double conditionPrice = 0.0;
    double price = 0.0;
    double qty = 0.0;
    uint64_t linkedOrder;
    uint64_t expiry;
    uint64_t transId;
    double coOrderNum = 0.0;
    double coOrderPrice = 0.0;
    string type;
    uint64_t orderDate;
    uint64_t allTradeNum;
    double stopFlags = 0.0;
    double offset = 0.0;
    double spread = 0.0;
    double balance = 0.0;
    uint64_t uid;
    double filledQty = 0.0;
    uint64_t withdrawTime;
    double conditionPrice2 = 0.0;
    uint64_t activeFromTime;
    uint64_t activeToTime;
    // condition_sec_code
    string conditionTicker;
    string conditionClassCode;
    uint64_t canceledUid;
    uint64_t orderDateTime;
    uint64_t withdrawDate;
    uint64_t activationDate;
    string currency;
    double lotSize = 0.0;
    string status;
} StopOrderDto;

#endif //QUIK_CONNECTOR_STOPORDERDTO_H
