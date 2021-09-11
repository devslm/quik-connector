//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "Uuid.h"

string slmdev::Uuid::createRandom() {
    return xg::newGuid().str();
}
