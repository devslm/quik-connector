//
// Created by Sergey on 26.06.2021.
//

#ifndef QUIK_CONNECTOR_QUIKUTILS_H
#define QUIK_CONNECTOR_QUIKUTILS_H

#include <string>
#include <list>
#include <map>
#include "../../lua/Lua.h"
#include "../../../dto/option/Option.h"
#include "../../../dto/trade/TradeDto.h"
#include "../../../dto/order/OrderCommissionDto.h"

using namespace std;

const string BOND_CLASS_NAME = "BOND";
const string STOCK_CLASS_NAME = "STOCK";
const string FUTURE_CLASS_NAME = "FUTURE";
const string OPTION_CLASS_NAME = "OPTION";
const string ETF_CLASS_NAME = "ETF";
const string CURRENCY_CLASS_NAME = "CURRENCY";
const string INCOMPLETE_LOT_CLASS_NAME = "INCOMPLETE_LOT";
const string INDEX_CLASS_NAME = "INDEX";
const string PAI_CLASS_NAME = "PAI";

const string ORDER_STATUS_SUCCESS_COMPLETED = "SUCCESS_COMPLETED";
const string ORDER_STATUS_ACTIVE = "ACTIVE";
const string ORDER_STATUS_INACTIVE = "INACTIVE";
const string ORDER_STATUS_CANCELED = "CANCELED";
const string ORDER_STATUS_REJECTED = "REJECTED";
const string ORDER_STATUS_UNDEFINED = "UNDEFINED";

const string ORDER_TYPE_BUY = "BUY";
const string ORDER_TYPE_SELL = "SELL";

enum Interval {
    INTERVAL_TICK,
    INTERVAL_M1,
    INTERVAL_M2,
    INTERVAL_M3,
    INTERVAL_M5,
    INTERVAL_M10,
    INTERVAL_M15,
    INTERVAL_M30,
    INTERVAL_H1,
    INTERVAL_H2,
    INTERVAL_H4,
    INTERVAL_D1
};

class QuikUtils {
public:
    static string getIntervalName(Interval interval);

    static Interval getIntervalByName(const string& interval);

    static string createCandlesMapKey(string classCode, string ticker, string interval);

    static Option<string> getClassTypeByCode(string& classCode);

    static string getCurrency(string& currency);

    static string getOrderStatus(double orderFlags);

    static string getOrderType(double orderFlags);

    static string getStopOrderConditionType(uint64_t conditionNumber);

    static string getStopOrderType(uint64_t stopOrderType);

    static void decodeFlags(double flags);

    static bool bitTest(double number, int bitNumber);

private:
    static const map<const string, const string> classCodes;
};


#endif //QUIK_CONNECTOR_QUIKUTILS_H
