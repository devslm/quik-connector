//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_CLASSINFOMAPPER_H
#define QUIK_CONNECTOR_CLASSINFOMAPPER_H

#include <string>
#include <nlohmann/json.hpp>
#include "../../../service/lua/Lua.h"
#include "../../../dto/quik/class/ClassInfoDto.h"
#include "../../../dto/option/Option.h"

using namespace nlohmann;
using namespace std;

bool toClassInfoDto(lua_State *L, ClassInfoDto *classInfo);

string toClassInfoJson(Option<ClassInfoDto> *classInfo);

#endif //QUIK_CONNECTOR_CLASSINFOMAPPER_H
