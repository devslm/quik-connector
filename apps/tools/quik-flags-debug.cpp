//
// Copyright (c) 2021 SLM Dev <https://slm-dev.com/quik-connector/>. All rights reserved.
//

#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "../../src/service/quik/utils/QuikUtils.h"

using namespace std;

shared_ptr<spdlog::logger> logger;

static void loggerInit() {
    logger = spdlog::stdout_color_mt("quik-flags-debug");
    logger->flush_on(spdlog::level::info);

    spdlog::set_pattern("    %v");
}

int main(int argc, char *argv[]) {
    loggerInit();

    string flags;

    while (true) {
        if (flags == "q") {
            break;
        }
        cout <<"    Enter flags (or enter: q to exit): ";

        cin >> flags;

        logger->info("\n");

        if (!flags.empty()) {
            QuikUtils::decodeFlags(std::stod(flags));
        }
        logger->info("\n\n");
    }
    return 0;
}
