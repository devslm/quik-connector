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

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

add_library(
    quik_connector
    SHARED
    library.cpp library.h
        src/service/log/log.c src/service/log/log.h
        src/service/log/Logger.cpp src/service/log/Logger.h
        src/service/quik/Quik.cpp src/service/quik/Quik.h
        src/dto/trade/TradeDto.h
        src/mapper/trade/TradeMapper.cpp
        src/dto/option/Option.h
        src/service/lua/Lua.cpp src/service/lua/Lua.h
        src/mapper/trade/TradeMapper.h
        src/mapper/date/DateMapper.cpp src/mapper/date/DateMapper.h
        src/service/utils/debug/DebugFunctions.cpp src/service/utils/debug/DebugFunctions.h
        src/service/quik/utils/QuikUtils.cpp src/service/quik/utils/QuikUtils.h
        src/dto/config/Config.h
        src/service/utils/string/StringUtils.cpp src/service/utils/string/StringUtils.h
        src/dto/class/ClassInfoDto.h
        src/mapper/class/ClassInfoMapper.cpp src/mapper/class/ClassInfoMapper.h
        src/dto/candle/CandleSubscriptionDto.h
        src/service/quik/candle/QuikCandleService.h src/service/quik/candle/QuikCandleService.cpp
        src/dto/candle/CandleDto.h
        src/mapper/candle/CandleMapper.cpp src/mapper/candle/CandleMapper.h
        src/service/queue/QueueService.cpp src/service/queue/QueueService.h
        src/dto/quik/QuikConnectionStatusDto.h
        src/mapper/quik/QuikServerMapper.cpp src/mapper/quik/QuikServerMapper.h
        src/mapper/request/RequestMapper.cpp src/mapper/request/RequestMapper.h
        src/dto/connector/request/RequestDto.h
        src/dto/quik/QuikUserInfoDto.h src/dto/quote/TickerQuoteDto.h src/mapper/quote/QuoteMapper.cpp src/mapper/quote/QuoteMapper.h src/dto/candle/ChangedCandleDto.h src/service/config/ConfigService.cpp src/service/config/ConfigService.h src/dto/order/OrderDto.h src/mapper/order/OrderMapper.cpp src/mapper/order/OrderMapper.h src/service/quik/order/QuikOrderService.cpp src/service/quik/order/QuikOrderService.h src/dto/ticker/TickerDto.h src/mapper/ticker/TickerMapper.cpp src/mapper/ticker/TickerMapper.h src/dto/order/OrderCommissionDto.h src/component/redis/Redis.cpp src/component/redis/Redis.h)

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
