//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#ifndef QUIK_CONNECTOR_TELEGRAM_H
#define QUIK_CONNECTOR_TELEGRAM_H

#ifdef OPENSSL_LIB
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif

#include <string>
#include "httplib.h"
#include "../../service/log/Logger.h"

using namespace std;

extern shared_ptr<spdlog::logger> logger;

class Telegram {
public:
    explicit Telegram(const string& apiKey);

    ~Telegram();

    void sendMessage(string& chatId, string& message);

private:
    const string host = "https://api.telegram.org";
    string apiKey;
    httplib::Client *httpClient;
};

#endif //QUIK_CONNECTOR_TELEGRAM_H
