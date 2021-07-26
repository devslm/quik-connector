//
// Created by Sergey on 26.06.2021.
//

#include "QuikUtils.h"

string QuikUtils::getIntervalName(Interval interval) {
    switch (interval) {
        case INTERVAL_M1:
            return "INTERVAL_M1";
        case INTERVAL_M2:
            return "INTERVAL_M2";
        case INTERVAL_M3:
            return "INTERVAL_M3";
        case INTERVAL_M5:
            return "INTERVAL_M5";
        case INTERVAL_M10:
            return "INTERVAL_M10";
        case INTERVAL_M15:
            return "INTERVAL_M15";
        case INTERVAL_M30:
            return "INTERVAL_M30";
        case INTERVAL_H1:
            return "INTERVAL_H1";
        case INTERVAL_H2:
            return "INTERVAL_H2";
        case INTERVAL_H4:
            return "INTERVAL_H4";
        case INTERVAL_D1:
            return "INTERVAL_D1";
        default:
            LOGGER->error("Could not find interval name! Because interval: {} is undefined!", interval);
            return "INTERVAL_D1";
    }
}

Interval QuikUtils::getIntervalByName(const string& interval) {
    if (interval == "INTERVAL_M1") {
        return INTERVAL_M1;
    } else if (interval == "INTERVAL_M2") {
        return INTERVAL_M2;
    } else if (interval == "INTERVAL_M3") {
        return INTERVAL_M3;
    } else if (interval == "INTERVAL_M5") {
        return INTERVAL_M5;
    } else if (interval == "INTERVAL_M10") {
        return INTERVAL_M10;
    } else if (interval == "INTERVAL_M15") {
        return INTERVAL_M15;
    } else if (interval == "INTERVAL_M30") {
        return INTERVAL_M30;
    } else if (interval == "INTERVAL_H1") {
        return INTERVAL_H1;
    } else if (interval == "INTERVAL_H2") {
        return INTERVAL_H2;
    } else if (interval == "INTERVAL_H4") {
        return INTERVAL_H4;
    } else if (interval == "INTERVAL_D1") {
        return INTERVAL_D1;
    }
    LOGGER->error("Could not find interval type! Because interval name: {} is undefined!", interval.c_str());

    return INTERVAL_D1;
}

const map<const string, const string> QuikUtils::classCodes = {
    {"TQCB", BOND_CLASS_NAME},
    {"TQOB", BOND_CLASS_NAME},
    {"TQOD", BOND_CLASS_NAME},
    {"TQOE", BOND_CLASS_NAME},
    {"TQRD", BOND_CLASS_NAME},
    {"TQUD", BOND_CLASS_NAME},
    {"TQED", BOND_CLASS_NAME},
    {"TQBR", STOCK_CLASS_NAME},
    {"TQBE", STOCK_CLASS_NAME},
    {"TQDE", STOCK_CLASS_NAME},
    {"FQBR", STOCK_CLASS_NAME},
    {"SPBXM", STOCK_CLASS_NAME},
    {"SPBFUT", FUTURE_CLASS_NAME},
    {"FUTSPREAD", FUTURE_CLASS_NAME},
    {"SPBOPT", OPTION_CLASS_NAME},
    {"TQTF", ETF_CLASS_NAME},
    {"TQTD", ETF_CLASS_NAME},
    {"TQTE", ETF_CLASS_NAME},
    {"CETS", CURRENCY_CLASS_NAME},
    {"CROSSRATE", CURRENCY_CLASS_NAME},
    {"AFXCURR1", CURRENCY_CLASS_NAME},
    {"USDRUB", CURRENCY_CLASS_NAME},
    {"AFXCURR1", CURRENCY_CLASS_NAME},
    {"SMAL", INCOMPLETE_LOT_CLASS_NAME},
    {"INDX", INDEX_CLASS_NAME},
    {"INDXC", INDEX_CLASS_NAME},
    {"RTSIDX", INDEX_CLASS_NAME},
    {"TQIF", PAI_CLASS_NAME},
    {"TQFD", PAI_CLASS_NAME},
    {"TQFE", PAI_CLASS_NAME}
};

string QuikUtils::createCandlesMapKey(string classCode, string ticker, string interval) {
    return classCode + ":" + ticker + ":" + interval;
}

Option<string> QuikUtils::getClassTypeByCode(string& classCode) {
    if (classCodes.find(classCode) != end(classCodes)) {
        return Option<string>(classCodes.at(classCode));
    }
    LOGGER->error("Could not get class type by code: {} because code is undefined!", classCode);

    return Option<string>();
}

string QuikUtils::getCurrency(string& currency) {
    if (currency == "SUR") {
        return "RUB";
    }
    return currency;
}

string QuikUtils::getOrderStatus(double orderFlags) {
    if (bitTest(orderFlags, 1)) {
        return ORDER_STATUS_CANCELED;
    } else if (!bitTest(orderFlags, 1)) {
        return ORDER_STATUS_SUCCESS_COMPLETED;
    } else if (bitTest(orderFlags, 0)) {
        return ORDER_STATUS_ACTIVE;
    } else if (!bitTest(orderFlags, 0)) {
        return ORDER_STATUS_INACTIVE;
    }
    return ORDER_STATUS_UNDEFINED;
}

string QuikUtils::getOrderType(double orderFlags) {
    if (bitTest(orderFlags, 2)) {
        return ORDER_TYPE_SELL;
    }
    return ORDER_TYPE_BUY;
}

bool QuikUtils::bitTest(double number, int bitNumber) {
    uint64_t mask =  1 << bitNumber;
    uint64_t maskedN = (uint64_t)number & mask;
    int bitValue = maskedN >> bitNumber;

    return (bitValue > 0 ? true : false);
}
