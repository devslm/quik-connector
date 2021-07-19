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
            logError("Could not find interval name! Because interval: %d is undefined!", interval);
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
    logError("Could not find interval type! Because interval name: %d is undefined!", interval.c_str());

    return INTERVAL_D1;
}

string QuikUtils::createCandlesMapKey(string classCode, string ticker, string interval) {
    return classCode + ":" + ticker + ":" + interval;
}
