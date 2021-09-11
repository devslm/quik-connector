//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "Uuid.h"

string Uuid::createRandom() {
    UUID uuid;
    UuidCreate(&uuid);

    char *str;

    UuidToStringA(&uuid, (RPC_CSTR*)&str);

    auto uuidString = string(str);

    delete str;

    return uuidString;
}
