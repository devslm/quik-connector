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
    FunctionArgType type;
    std::string valueString;
    int valueInt;
    double valueDouble;
    bool valueBoolean;
} FunctionArgDto;

class FunctionArgDtos {
public:
    FunctionArgType getType() {
        return this->type;
    }

protected:
    FunctionArgType type;
};

class StringFunctionArgDto: public FunctionArgDtos {
public:
    StringFunctionArgDto(string value) : FunctionArgDtos() {
        this->type = STRING_TYPE;
        this->value = value;
    }

    string getValue() {
        return this->value;
    }

private:
    string value;
};

class NumberFunctionArgDto: public FunctionArgDtos {
public:
    NumberFunctionArgDto(double value) : FunctionArgDtos() {
        this->type = DOUBLE_TYPE;
        this->value = value;
    }

    double getValue() {
        return this->value;
    }

private:
    double value;
};

class IntegerFunctionArgDto: public FunctionArgDtos {
public:
    IntegerFunctionArgDto(int value) : FunctionArgDtos() {
        this->type = INTEGER_TYPE;
        this->value = value;
    }

    int getValue() {
        return this->value;
    }

private:
    int value;
};

class BooleanFunctionArgDto: public FunctionArgDtos {
public:
    BooleanFunctionArgDto(bool value) : FunctionArgDtos() {
        this->type = BOOLEAN_TYPE;
        this->value = value;
    }

    bool getValue() {
        return this->value;
    }

private:
    bool value;
};

#endif //TEST_FUNCTION_ARG_DTO_H
