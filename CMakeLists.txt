cmake_minimum_required(VERSION 3.19)

project(quik_connector)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(MSVC_COMPILE_FLAGS "/MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_COMPILE_FLAGS}")

set(SQLITE_CPP_WRAPPER_VERSION 3.1.1)
set(LZMA_VERSION 5.2.5)
set(JSON_VERSION 3.9.1)
set(YAML_CPP_VERSION 0.7.0)
set(SPDLOG_VERSION 1.9.0)
set(CPP_REDIS_VERSION 4.3.1)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)

    message(STATUS "Starting build Release...")
endif()

include(FetchContent)

# Include SQLITE3 C++ wrapper library (https://github.com/SRombauts/SQLiteCpp)
FetchContent_Declare(
    SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG ${SQLITE_CPP_WRAPPER_VERSION}
)

FetchContent_GetProperties(SQLiteCpp)

if(NOT SQLiteCpp_POPULATED)
    FetchContent_Populate(SQLiteCpp)

    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/_deps/sqlitecpp-src ${CMAKE_CURRENT_BINARY_DIR}/_deps/sqlitecpp-build)
endif()

# Include LZMA Compression library (https://git.tukaani.org/xz.git)
FetchContent_Declare(
    liblzma
    GIT_REPOSITORY https://git.tukaani.org/xz.git
    GIT_TAG v${LZMA_VERSION}
)

FetchContent_GetProperties(liblzma)

if(NOT liblzma_POPULATED)
    FetchContent_Populate(liblzma)

    add_subdirectory(${liblzma_SOURCE_DIR} ${liblzma_BINARY_DIR})
endif()

# Include JSON library (https://github.com/nlohmann/json)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v${JSON_VERSION}
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
    GIT_TAG yaml-cpp-${YAML_CPP_VERSION}
)
set(YAML_CPP_BUILD_TESTS OFF)

FetchContent_MakeAvailable(yaml-cpp)

# Include logging library (https://github.com/gabime/spdlog)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v${SPDLOG_VERSION}
)

FetchContent_MakeAvailable(spdlog)

target_include_directories(spdlog INTERFACE ${spdlog_SOURCE_DIR}/include)

# Include Redis library (https://github.com/cpp-redis/cpp_redis)
FetchContent_Declare(
    cpp_redis
    GIT_REPOSITORY https://github.com/cpp-redis/cpp_redis.git
    GIT_TAG ${CPP_REDIS_VERSION}
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

add_library(quik_connector SHARED ${INCLUDE_QUIK_CONNECTOR} ${SRC_QUIK_CONNECTOR} src/version.rc)

include_directories(lua-5.3.5/src ${SQLiteCpp_SOURCE_DIR}/include)

target_link_libraries(
    quik_connector
    liblzma
    sqlite3
    SQLiteCpp
    spdlog
    nlohmann_json::nlohmann_json
    yaml-cpp
    cpp_redis
    ${CMAKE_CURRENT_BINARY_DIR}/../lua-5.3.5/build/lua53.lib
)

# Create quik connector output directory
set(QUIK_CONNECTOR_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/quik-connector)

file(REMOVE_RECURSE ${QUIK_CONNECTOR_OUTPUT_DIR})

file(COPY ${PROJECT_SOURCE_DIR}/quik-connector.lua DESTINATION ${QUIK_CONNECTOR_OUTPUT_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/config DESTINATION ${QUIK_CONNECTOR_OUTPUT_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/db/migrations DESTINATION ${QUIK_CONNECTOR_OUTPUT_DIR}/db)

set(QUIK_CONNECTOR_DLL_PATH ${PROJECT_BINARY_DIR})

#if (MSVC)
#    set(QUIK_CONNECTOR_DLL_PATH ${QUIK_CONNECTOR_DLL_PATH}/${CMAKE_BUILD_TYPE})

#    message(STATUS "QUIK connector DLL path: ${QUIK_CONNECTOR_DLL_PATH}")
#endif()

add_custom_command(
    TARGET quik_connector
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${QUIK_CONNECTOR_DLL_PATH}/quik_connector.dll ${QUIK_CONNECTOR_OUTPUT_DIR}/bin/quik_connector.dll
    COMMENT "\nNow you can copy quik connector directory to quik from: ${QUIK_CONNECTOR_OUTPUT_DIR} and use it\n\n"
)
