cmake_minimum_required(VERSION 3.19)

project(quik-connector)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(MSVC_COMPILE_FLAGS "/MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_COMPILE_FLAGS}")

option(USE_LUA_VERSION_5_4 "Build QUIK DLL with LUA 5.4.1 otherwise with LUA 5.3.5" OFF)

if (USE_LUA_VERSION_5_4)
    set(LUA_VERSION 5.4.1)
    set(LUA_LIB_VERSION 54)
else()
    set(LUA_VERSION 5.3.5)
    set(LUA_LIB_VERSION 53)
endif()

message(STATUS "Build QUIK connector with LUA version: ${LUA_VERSION}")

set(SQLITE_CPP_WRAPPER_VERSION 3.1.1)
set(LZMA_VERSION 5.2.5)
set(JSON_VERSION 3.9.1)
set(YAML_CPP_VERSION 0.7.0)
set(SPDLOG_VERSION 1.9.0)
set(CPP_REDIS_VERSION 4.3.1)
set(GOOGLE_TEST_FRAMEWORK_VERSION 1.11.0)

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

# Include GoogleTest test framework (https://github.com/google/googletest)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-${GOOGLE_TEST_FRAMEWORK_VERSION}
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE INCLUDE_FILES "src/*.h")
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Remove library.h and .cpp, quik-tools.cpp files to define them in separate variables
list(REMOVE_ITEM INCLUDE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/library.h")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/library.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/quik-tools.cpp")

set(LIBRARY_INC_FILE "src/library.h")
set(LIBRARY_SRC_FILE "src/library.cpp")
set(VERSION_FILE "src/version.rc")

add_library(quik-connector SHARED ${INCLUDE_FILES} ${SRC_FILES} ${LIBRARY_INC_FILE} ${LIBRARY_SRC_FILE} ${VERSION_FILE})

add_library(quik-connector-static STATIC ${INCLUDE_FILES} ${SRC_FILES} ${LIBRARY_INC_FILE} ${LIBRARY_SRC_FILE} ${VERSION_FILE})

include_directories(lua-${LUA_VERSION}/src ${SQLiteCpp_SOURCE_DIR}/include)

set(
    LINK_LIBRARIES
    liblzma
    sqlite3
    SQLiteCpp
    spdlog
    nlohmann_json::nlohmann_json
    yaml-cpp
    cpp_redis
    ${CMAKE_CURRENT_BINARY_DIR}/../lua-${LUA_VERSION}/build/lua${LUA_LIB_VERSION}.lib
)

target_link_libraries(quik-connector ${LINK_LIBRARIES})

option(QUIK_BUILD_TOOLS "Build quik tools executable files" OFF)

if(QUIK_BUILD_TOOLS)
    target_link_libraries(quik-connector-static ${LINK_LIBRARIES})

    add_executable(quik-tools "src/quik-tools.cpp")

    target_link_libraries(quik-tools PUBLIC quik-connector-static)
endif(QUIK_BUILD_TOOLS)

# Create quik connector output directory
set(QUIK_CONNECTOR_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/quik-connector)

file(REMOVE_RECURSE ${QUIK_CONNECTOR_OUTPUT_DIR})

file(COPY ${PROJECT_SOURCE_DIR}/quik-connector.lua DESTINATION ${QUIK_CONNECTOR_OUTPUT_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/config DESTINATION ${QUIK_CONNECTOR_OUTPUT_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/db/migrations DESTINATION ${QUIK_CONNECTOR_OUTPUT_DIR}/db)

set(QUIK_CONNECTOR_DLL_PATH ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})

option(CLION_IDE "Use Clion IDE therefore we will copy DLL from: ${PROJECT_BINARY_DIR} directory" OFF)

if (CLION_IDE)
    set(QUIK_CONNECTOR_DLL_PATH ${PROJECT_BINARY_DIR})

    message(STATUS "QUIK connector DLL path (by Clion IDE): ${QUIK_CONNECTOR_DLL_PATH}")
endif(CLION_IDE)

add_custom_command(
    TARGET quik-connector
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${QUIK_CONNECTOR_DLL_PATH}/quik-connector.dll ${QUIK_CONNECTOR_OUTPUT_DIR}/bin/quik-connector.dll
    COMMENT "\nNow you can copy quik connector directory to quik from: ${QUIK_CONNECTOR_OUTPUT_DIR} and use it\n\n"
)
