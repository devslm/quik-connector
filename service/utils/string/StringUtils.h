//
// Created by Sergey on 27.06.2021.
//

#ifndef QUIK_CONNECTOR_STRINGUTILS_H
#define QUIK_CONNECTOR_STRINGUTILS_H

#include <set>

using namespace std;

set<string> stringSplitByDelimeter(const char *str, const char delimiter);

void convertToUtf8(const char *src, char *dst);

#endif //QUIK_CONNECTOR_STRINGUTILS_H
