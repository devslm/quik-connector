//
// Created by Sergey on 21.07.2021.
//

#ifndef QUIK_CONNECTOR_ORDERCOMMISSIONDTO_H
#define QUIK_CONNECTOR_ORDERCOMMISSIONDTO_H

typedef struct OrderCommissionDto {
    double broker;
    double clearing;
    double techCenter;
    double exchange;
} OrderCommissionDto;

#endif //QUIK_CONNECTOR_ORDERCOMMISSIONDTO_H
