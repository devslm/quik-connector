//
// Created by Sergey on 26.06.2021.
//

#ifndef QUIK_CONNECTOR_QUIKUTILS_H
#define QUIK_CONNECTOR_QUIKUTILS_H

#include <string>
#include "../../../service/lua/Lua.h"

using namespace std;

enum Interval {
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
};


#endif //QUIK_CONNECTOR_QUIKUTILS_H
