//
// Created by Sergey on 04.06.2021.
//

#ifndef TEST_FUNCTION_ARG_DTO_H
#define TEST_FUNCTION_ARG_DTO_H

#include <string>

typedef enum FunctionArgType {
    STRING_TYPE,
    INTEGER_TYPE,
    DOUBLE_TYPE,
    BOOLEAN_TYPE,
} FunctionArgType;

typedef struct FunctionArgDto {
    FunctionArgType type;
    std::string valueString;
    int valueInt;
    double valueDouble;
    bool valueBoolean;
} FunctionArgDto;

#endif //TEST_FUNCTION_ARG_DTO_H
