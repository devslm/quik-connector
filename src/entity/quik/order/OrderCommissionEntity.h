//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_ORDERCOMMISSIONENTITY_H
#define QUIK_CONNECTOR_ORDERCOMMISSIONENTITY_H

typedef struct OrderCommissionEntity {
    double broker = 0.0;
    double clearing = 0.0;
    double techCenter = 0.0;
    double exchange = 0.0;
} OrderCommissionEntity;

#endif //QUIK_CONNECTOR_ORDERCOMMISSIONENTITY_H
