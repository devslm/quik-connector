//
// Created by Sergey on 26.06.2021.
//

#ifndef QUIK_CONNECTOR_DEBUGFUNCTIONS_H
#define QUIK_CONNECTOR_DEBUGFUNCTIONS_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "../../../dto/lua/FunctionArgDto.h"
#include "../../log/Logger.h"

extern shared_ptr<spdlog::logger> LOGGER;

void debugLuaFunctionArgsToString(FunctionArgDto *functionArgs, int functionNumArgs, char *resultBuffer, int bufferSize);

#endif //QUIK_CONNECTOR_DEBUGFUNCTIONS_H
