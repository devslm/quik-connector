//
// Created by Sergey on 26.06.2021.
//

#include "DebugFunctions.h"

const int AS_DECIMAL = 10;

void debugLuaFunctionArgsToString(FunctionArgDto *functionArgs, int functionNumArgs, char *resultBuffer, int bufferSize) {
    memset(resultBuffer, 0, bufferSize);

    if (functionArgs == NULL) {
        strcpy(resultBuffer, "NULL");
        return;
    }

    for (int i = 0; i < functionNumArgs; ++i) {
        strcat(resultBuffer, "{");

        char argType[16] = {0};
        char argValue[128] = {0};
        FunctionArgDto arg = functionArgs[i];

        switch (arg.type) {
            case STRING_TYPE:
                strcpy(argType, "STRING");
                strcpy(argValue, arg.valueString.c_str());
                break;
            case INTEGER_TYPE:
                strcpy(argType, "INTEGER");
                ltoa(arg.valueInt, argValue, AS_DECIMAL);
                break;
            case DOUBLE_TYPE:
                strcpy(argType, "DOUBLE");
                sprintf(argValue, "%f", arg.valueDouble);
                break;
            case BOOLEAN_TYPE:
                strcpy(argType, "BOOLEAN");

                if (arg.valueBoolean == true) {
                    strcpy(argValue, "true");
                } else {
                    strcpy(argValue, "false");
                }
                break;
            default:
                LOGGER->error("Could not debug function arguments because argument: #{} has unknown type: {}", i + 1, arg.type);

                strcpy(argType, "UNKNOWN");
                strcpy(argValue, "UNKNOWN");

                return;
        }
        strcat(resultBuffer, "type: ");
        strcat(resultBuffer, argType);
        strcat(resultBuffer, ", value: ");
        strcat(resultBuffer, argValue);
        strcat(resultBuffer, "}");

        if (i < functionNumArgs - 1) {
            strcat(resultBuffer, ", ");
        }
    }
}

void debugDateTimeStructure(const tm& date) {
    json jsonObject;
    jsonObject["year"] = date.tm_year;
    jsonObject["month"] = date.tm_mon;
    jsonObject["weekDay"] = date.tm_wday;
    jsonObject["day"] = date.tm_mday;
    jsonObject["hour"] = date.tm_hour;
    jsonObject["minutes"] = date.tm_min;
    jsonObject["seconds"] = date.tm_sec;


    LOGGER->info("Date structure fields: {}", jsonObject.dump());
}
