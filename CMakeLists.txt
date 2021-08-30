cmake_minimum_required(VERSION 3.19)

project(quik-connector)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(MSVC_COMPILE_FLAGS "/MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_COMPILE_FLAGS}")

option(USE_LUA_VERSION_5_4 "Build QUIK connector with LUA 5.4.1 otherwise with LUA 5.3.5" OFF)

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

    message(STATUS "Starting build QUIK connector Release...")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build-dependencies.cmake)

# Add definition QUIK_LUA_VERSION for version.h file
add_definitions(-DQUIK_LUA_VERSION="${LUA_VERSION}")

file(GLOB_RECURSE INCLUDE_FILES "src/*.h")
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Remove library.h and .cpp, quik-tools.cpp files to define them in separate variables
list(REMOVE_ITEM INCLUDE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/library.h")
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/library.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/quik-tools.cpp")

set(LIBRARY_INC_FILE "src/library.h")
set(LIBRARY_SRC_FILE "src/library.cpp")
set(VERSION_FILE "src/version.rc")

add_library(quik-connector SHARED ${INCLUDE_FILES} ${SRC_FILES} ${LIBRARY_INC_FILE} ${LIBRARY_SRC_FILE} ${VERSION_FILE})

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
    add_library(quik-connector-static STATIC ${INCLUDE_FILES} ${SRC_FILES} ${LIBRARY_INC_FILE} ${LIBRARY_SRC_FILE} ${VERSION_FILE})

    target_link_libraries(quik-connector-static ${LINK_LIBRARIES})

    add_executable(quik-tools "src/quik-tools.cpp")

    target_link_libraries(quik-tools PUBLIC quik-connector-static)
endif(QUIK_BUILD_TOOLS)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/create-output-directory-tree.cmake)
