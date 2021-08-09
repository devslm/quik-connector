//
// Created by Sergey on 04.06.2021.
//

#ifndef TEST_FUNCTION_ARG_DTO_H
#define TEST_FUNCTION_ARG_DTO_H

#include <string>

using namespace std;

typedef enum FunctionArgType {
    STRING_TYPE,
    INTEGER_TYPE,
    DOUBLE_TYPE,
    BOOLEAN_TYPE,
} FunctionArgType;

typedef struct FunctionArgDto {
    FunctionArgDto() {

    }

    FunctionArgDto(const string& value) {
        this->type = STRING_TYPE;
        this->valueString = value;
    }

    FunctionArgDto(string& value) {
        this->type = STRING_TYPE;
        this->valueString = value;
    }

    FunctionArgDto(char value[]) {
        this->type = STRING_TYPE;
        this->valueString = string(value);
    }

    FunctionArgDto(const char value[]) {
        this->type = STRING_TYPE;
        this->valueString = string(value);
    }

    FunctionArgDto(int value) {
        this->type = INTEGER_TYPE;
        this->valueInt = value;
    }

    FunctionArgDto(double value) {
        this->type = DOUBLE_TYPE;
        this->valueDouble = value;
    }

    FunctionArgDto(bool value) {
        this->type = BOOLEAN_TYPE;
        this->valueBoolean = value;
    }
    FunctionArgType type;
    string valueString;
    int valueInt = 0;
    double valueDouble = 0.0;
    bool valueBoolean = false;
} FunctionArgDto;

#endif //TEST_FUNCTION_ARG_DTO_H
