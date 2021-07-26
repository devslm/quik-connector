cmake_minimum_required(VERSION 3.19)
project(quik_connector)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

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

# Include logging library (https://github.com/gabime/spdlog)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.9.0
)

FetchContent_MakeAvailable(spdlog)

target_include_directories(spdlog INTERFACE ${spdlog_SOURCE_DIR}/include)

# Include Redis library (https://github.com/cpp-redis/cpp_redis)
FetchContent_Declare(
    cpp_redis
    GIT_REPOSITORY https://github.com/cpp-redis/cpp_redis.git
    GIT_TAG 4.3.1
)

FetchContent_GetProperties(cpp_redis)

if(NOT cpp_redis_POPULATED)
    FetchContent_Populate(cpp_redis)

    add_subdirectory(${cpp_redis_SOURCE_DIR} ${cpp_redis_BINARY_DIR})

    target_include_directories(cpp_redis INTERFACE ${cpp_redis_SOURCE_DIR}/includes)
    target_include_directories(cpp_redis INTERFACE ${cpp_redis_SOURCE_DIR}/tacopie/includes)
endif()

file(GLOB_RECURSE INCLUDE_QUIK_CONNECTOR "src/*.h")
file(GLOB_RECURSE SRC_QUIK_CONNECTOR "src/*.cpp")

add_library(quik_connector SHARED ${INCLUDE_QUIK_CONNECTOR} ${SRC_QUIK_CONNECTOR})

include_directories(lua-5.3.5/src)

target_link_libraries(
    quik_connector
    spdlog
    nlohmann_json::nlohmann_json
    yaml-cpp
    cpp_redis
    ${CMAKE_CURRENT_BINARY_DIR}/../lua-5.3.5/build/lua53.lib
)

# Create quik connector output directory
set(QUIK_CONNECTOR_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/quik-connector)

add_custom_command(
    TARGET quik_connector
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/quik-connector.lua ${QUIK_CONNECTOR_OUTPUT_DIR}/quik-connector.lua
)

add_custom_command(
    TARGET quik_connector
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/quik_connector.dll ${QUIK_CONNECTOR_OUTPUT_DIR}/quik_connector.dll
)

add_custom_command(
    TARGET quik_connector
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/../config/config.yml ${QUIK_CONNECTOR_OUTPUT_DIR}/config/config.yml
    COMMENT "\nNow you can copy quik connector directory to quik from: ${QUIK_CONNECTOR_OUTPUT_DIR} and use it\n\n"
)
