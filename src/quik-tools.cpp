//
// Copyright (c) 2021 SLM <sergey.s.mareychev@gmail.com>. All rights reserved.
//

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "service/quik/utils/QuikUtils.h"

std::shared_ptr<spdlog::logger> logger;

static void loggerInit() {
    logger = spdlog::stdout_color_mt("quik-utils");
    logger->flush_on(spdlog::level::info);

    spdlog::set_pattern("%v");
}

// TODO Implement QUIK tools application
int main(int argc, char *argv[]) {
    loggerInit();

    QuikUtils::decodeFlags(10280.0);

    logger->info(""); // Empty line

    system("pause");

    return 0;
}
