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

# Include crossguid library (https://github.com/graeme-hill/crossguid)
FetchContent_Declare(
    crossguid
    GIT_REPOSITORY https://github.com/graeme-hill/crossguid
    GIT_TAG v${CROSSGUID_VERSION}
)

FetchContent_GetProperties(crossguid)

if(NOT crossguid_POPULATED)
    FetchContent_Populate(crossguid)

    add_subdirectory(${crossguid_SOURCE_DIR} ${crossguid_BINARY_DIR})
endif()

# Include GoogleTest test framework (https://github.com/google/googletest)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-${GOOGLE_TEST_FRAMEWORK_VERSION}
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
