# Create quik connector output directory
set(QUIK_CONNECTOR_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/quik-connector)

file(REMOVE_RECURSE ${QUIK_CONNECTOR_OUTPUT_DIR})

# Generate LUA entrypoint file depends on build type and copy to output directory
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/template/quik-connector.lua.in"
    "${PROJECT_SOURCE_DIR}/quik-connector/quik-connector.lua"
)

file(COPY ${PROJECT_SOURCE_DIR}/config DESTINATION ${QUIK_CONNECTOR_OUTPUT_DIR})
file(COPY data/db/migrations DESTINATION ${QUIK_CONNECTOR_OUTPUT_DIR}/db)

# Copy OpenSSL DLL files if enabled
if (QUIK_INCLUDE_OPENSSL)
    file(COPY ${CMAKE_CURRENT_BINARY_DIR}/../external/openssl-${OPENSSL_VERSION}/build/libssl-1_1-x64.dll DESTINATION ${QUIK_CONNECTOR_OUTPUT_DIR}/bin)
    file(COPY ${CMAKE_CURRENT_BINARY_DIR}/../external/openssl-${OPENSSL_VERSION}/build/libcrypto-1_1-x64.dll DESTINATION ${QUIK_CONNECTOR_OUTPUT_DIR}/bin)
endif()

set(QUIK_CONNECTOR_DLL_PATH ${PROJECT_BINARY_DIR}/${CMAKE_BUILD_TYPE})

option(CLION_IDE "Use Clion IDE therefore we will copy DLL from: ${PROJECT_BINARY_DIR} directory" OFF)

if (CLION_IDE)
    set(QUIK_CONNECTOR_DLL_PATH ${PROJECT_BINARY_DIR})

    message(STATUS "QUIK connector DLL path (by Clion IDE): ${QUIK_CONNECTOR_DLL_PATH}")
endif(CLION_IDE)

if(QUIK_BUILD_TOOLS)
    add_custom_command(
        TARGET quik-flags-debug
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${QUIK_CONNECTOR_DLL_PATH}/quik-flags-debug.exe ${QUIK_CONNECTOR_OUTPUT_DIR}/bin/quik-flags-debug.exe
    )
endif(QUIK_BUILD_TOOLS)

add_custom_command(
    TARGET quik-connector
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${QUIK_CONNECTOR_DLL_PATH}/quik-connector.dll ${QUIK_CONNECTOR_OUTPUT_DIR}/bin/quik-connector.dll
    COMMENT "\nNow you can copy quik connector directory to quik from: ${QUIK_CONNECTOR_OUTPUT_DIR} and use it\n\n"
)
