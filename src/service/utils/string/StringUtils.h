//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_STRINGUTILS_H
#define QUIK_CONNECTOR_STRINGUTILS_H

#include <set>

using namespace std;

set<string> stringSplitByDelimeter(const string& str, const char delimiter);

void convertToUtf8(const char *src, char *dst);

string stringRepeatWithoutFirstNChars(string value, int numRepeat, int removeFirstNChars);

string stringRepeatWithoutLastNChars(string value, int numRepeat, int removeLastNChars);

#endif //QUIK_CONNECTOR_STRINGUTILS_H
