//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "TransactionMapper.h"

const string TRANSACTION_ACTIVE_CODE = "ACTIVE";
const string TRANSACTION_COMPLETED_SUCCESSFULLY_CODE = "COMPLETED_SUCCESSFULLY";
const string TRANSACTION_ERROR_CODE = "ERROR";

bool toTransactionReplyDto(lua_State *luaState, TransactionReplyDto *transactionReply) {
    if (!lua_istable(luaState, -1)) {
        logger->error("Could not get table for transaction reply data! Current stack value type is: <<{}>> but required table!",
            luaGetType(luaState, -1));

        return false;
    }

    if (!luaGetTableIntegerField(luaState, "trans_id", &transactionReply->transId)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "status", &transactionReply->status)) {
        return false;
    }

    if (transactionReply->status < 2) {
        transactionReply->statusCode = TRANSACTION_ACTIVE_CODE;
    } else if (transactionReply->status == 3) {
        transactionReply->statusCode = TRANSACTION_COMPLETED_SUCCESSFULLY_CODE;
    } else if (transactionReply->status < 15) {
        transactionReply->statusCode = TRANSACTION_ERROR_CODE;
    } else {
        transactionReply->statusCode = string("UNKNOWN (QUIK status: ").append(to_string(transactionReply->status)).append(")");
    }

    if (!luaGetTableStringField(luaState, "result_msg", &transactionReply->resultMsg)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "uid", &transactionReply->uid)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "flags", &transactionReply->flags)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "server_trans_id", &transactionReply->serverTransId)) {
        return false;
    }
    if (!luaGetTableIntegerField(luaState, "order_num", &transactionReply->orderNum)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "price", &transactionReply->price)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "quantity", &transactionReply->quantity)) {
        return false;
    }
    if (!luaGetTableNumberField(luaState, "balance", &transactionReply->balance)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "firm_id", &transactionReply->firmId)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "account", &transactionReply->account)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "client_code", &transactionReply->clientCode)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "brokerref", &transactionReply->brokerRef)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "class_code", &transactionReply->classCode)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "sec_code", &transactionReply->ticker)) {
        return false;
    }
    if (!luaGetTableStringField(luaState, "exchange_code", &transactionReply->exchangeCode)) {
        return false;
    }
    if (!toDateMillis(luaState, "date_time", &transactionReply->date)) {
        return false;
    }
    lua_pop(luaState, 1);

    return true;
}

json toTransactionReplyJson(Option<TransactionReplyDto>& transactionReplyOption) {
    json jsonObject;

    if (transactionReplyOption.isEmpty()) {
        return jsonObject;
    }
    TransactionReplyDto transactionReply = transactionReplyOption.get();
    jsonObject["transId"] = transactionReply.transId;
    jsonObject["status"] = transactionReply.status;
    jsonObject["statusCode"] = transactionReply.statusCode;
    jsonObject["resultMsg"] = transactionReply.resultMsg;
    jsonObject["date"] = transactionReply.date;
    jsonObject["uid"] = transactionReply.uid;
    jsonObject["flags"] = transactionReply.flags;
    jsonObject["serverTransId"] = transactionReply.serverTransId;
    jsonObject["orderNum"] = transactionReply.orderNum;
    jsonObject["price"] = transactionReply.price;
    jsonObject["quantity"] = transactionReply.quantity;
    jsonObject["balance"] = transactionReply.balance;
    jsonObject["firmId"] = transactionReply.firmId;
    jsonObject["account"] = transactionReply.account;
    jsonObject["clientCode"] = transactionReply.clientCode;
    jsonObject["brokerRef"] = transactionReply.brokerRef;
    jsonObject["classCode"] = transactionReply.classCode;
    jsonObject["ticker"] = transactionReply.ticker;
    jsonObject["exchangeCode"] = transactionReply.exchangeCode;

    return jsonObject;
}
