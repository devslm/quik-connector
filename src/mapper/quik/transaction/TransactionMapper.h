//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_TRANSACTIONMAPPER_H
#define QUIK_CONNECTOR_TRANSACTIONMAPPER_H

#include <nlohmann/json.hpp>
#include "../../../service/lua/Lua.h"
#include "../../../dto/quik/transaction/TransactionReplyDto.h"
#include "../date/DateMapper.h"
#include "../../../dto/option/Option.h"

using namespace nlohmann;

bool toTransactionReplyDto(lua_State *luaState, TransactionReplyDto *transactionReply);

json toTransactionReplyJson(Option<TransactionReplyDto>& transactionReplyOption);

#endif //QUIK_CONNECTOR_TRANSACTIONMAPPER_H
