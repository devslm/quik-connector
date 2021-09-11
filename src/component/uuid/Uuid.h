//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_UUID_H
#define QUIK_CONNECTOR_UUID_H

#include <Guid.hpp>

using namespace std;

namespace slmdev {
    class Uuid {
    public:
        static string createRandom();
    };
}

#endif //QUIK_CONNECTOR_UUID_H
