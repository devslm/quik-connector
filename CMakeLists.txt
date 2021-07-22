cmake_minimum_required(VERSION 3.19)
project(quik_connector)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)

# Include JSON library (https://github.com/nlohmann/json)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.9.1
)

FetchContent_GetProperties(json)

if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Include YAML library (https://github.com/jbeder/yaml-cpp)
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.7.0
)
set(YAML_CPP_BUILD_TESTS OFF)
FetchContent_MakeAvailable(yaml-cpp)

add_library(
    quik_connector
    SHARED
    library.cpp library.h
    service/log/log.c service/log/log.h
    service/log/Logger.cpp service/log/Logger.h
    service/quik/Quik.cpp service/quik/Quik.h
    dto/trade/TradeDto.h
    mapper/trade/TradeMapper.cpp
    dto/option/Option.h
    service/lua/Lua.cpp service/lua/Lua.h
    mapper/trade/TradeMapper.h
    mapper/date/DateMapper.cpp mapper/date/DateMapper.h
    service/utils/debug/DebugFunctions.cpp service/utils/debug/DebugFunctions.h
    service/quik/utils/QuikUtils.cpp service/quik/utils/QuikUtils.h
    dto/config/Config.h
    service/utils/string/StringUtils.cpp service/utils/string/StringUtils.h
    dto/class/ClassInfoDto.h
    mapper/class/ClassInfoMapper.cpp mapper/class/ClassInfoMapper.h
    dto/candle/CandleSubscriptionDto.h
    service/quik/candle/QuikCandleService.h service/quik/candle/QuikCandleService.cpp
    dto/candle/CandleDto.h
    mapper/candle/CandleMapper.cpp mapper/candle/CandleMapper.h
    service/queue/QueueService.cpp service/queue/QueueService.h
    dto/quik/QuikConnectionStatusDto.h
    mapper/quik/QuikServerMapper.cpp mapper/quik/QuikServerMapper.h
    mapper/request/RequestMapper.cpp mapper/request/RequestMapper.h
    dto/connector/request/RequestDto.h
    dto/quik/QuikUserInfoDto.h dto/quote/TickerQuoteDto.h mapper/quote/QuoteMapper.cpp mapper/quote/QuoteMapper.h dto/candle/ChangedCandleDto.h service/config/ConfigService.cpp service/config/ConfigService.h dto/order/OrderDto.h mapper/order/OrderMapper.cpp mapper/order/OrderMapper.h service/quik/order/QuikOrderService.cpp service/quik/order/QuikOrderService.h dto/ticker/TickerDto.h mapper/ticker/TickerMapper.cpp mapper/ticker/TickerMapper.h dto/order/OrderCommissionDto.h component/redis/Redis.cpp component/redis/Redis.h)

link_directories(
    C:/lua-5.3.6
    C:/cpp_redis/includes
    C:/cpp_redis/tacopie/includes
)

include_directories(
    C:/lua-5.3.6
    C:/cpp_redis/includes
    C:/cpp_redis/tacopie/includes
)

target_link_libraries(
    quik_connector
    nlohmann_json::nlohmann_json
    yaml-cpp
    C:/lua-5.3.6/lua53.lib
    C:/cpp_redis/msvc15/x64/Release/cpp_redis.lib
    C:/cpp_redis/msvc15/x64/Release/tacopie.lib
)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
