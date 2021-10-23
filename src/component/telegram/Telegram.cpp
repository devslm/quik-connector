//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include "Telegram.h"

Telegram::Telegram(const string& apiKey) {
    if (apiKey.empty()) {
        throw runtime_error("Could not start connector because telegram api key is empty!");
    }
    this->apiKey = apiKey;
    this->httpClient = new httplib::Client(host);
}

Telegram::~Telegram() {
    delete httpClient;
}

void Telegram::sendMessage(string& chatId, string& message) {
    httplib::Params params;
    params.emplace("chat_id", chatId);
    params.emplace("parse_mode", "Markdown");
    params.emplace("text", message);

    auto url = "/bot" + apiKey + "/sendMessage";
    auto response = httpClient->Post(url.c_str(), params);

    if (response->status >= 400) {
        logger->error("Could not send Telegram message to channel: {}! "
            "Reason: response status code: {} and message: {}", response->status, response->body);
    } else {
        logger->debug("Successfully sent Telegram message to channel: {} with response status code: {}", response->status);
    }
}
