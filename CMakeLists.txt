cmake_minimum_required(VERSION 3.15)
project(NumberStore)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler specific options
if(MSVC)
    add_compile_options(/W4)
    add_compile_definitions(_WIN32_WINNT=0x0600)  # Windows Vista+
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Utils Library
add_library(numberstore-utils
    utils/ErrorCodes.cxx
    utils/Config.cxx
    utils/Validator.cxx
    utils/TimeUtils.cxx
    utils/Logger.cxx
    utils/SingleInstanceManager.cxx
)

# Protocol Library
add_library(numberstore-protocol
    protocol/Message.cxx
    protocol/Command.cxx
    protocol/Response.cxx
    protocol/MessageSerializer.cxx
)

target_link_libraries(numberstore-protocol numberstore-utils)

# IPC Library
add_library(numberstore-ipc
    ipc/NamedPipeConnection.cxx
    ipc/NamedPipeServer.cxx
    ipc/NamedPipeClient.cxx
)

target_link_libraries(numberstore-ipc numberstore-utils)

# Storage Library
add_library(numberstore-storage
    storage/NumberStore.cxx
    storage/SnapshotManager.cxx
)

target_link_libraries(numberstore-storage numberstore-utils)

# Daemon Library
add_library(numberstore-daemon-lib
    daemon/SignalHandler.cxx
    daemon/CommandProcessor.cxx
    daemon/ClientHandler.cxx
    daemon/ConnectionManager.cxx
    daemon/DaemonServer.cxx
)

target_link_libraries(numberstore-daemon-lib 
    numberstore-storage 
    numberstore-protocol 
    numberstore-ipc 
    numberstore-utils
)

# CLI Library
add_library(numberstore-cli-lib
    cli/DaemonClient.cxx
    cli/CLIApplication.cxx
)

target_link_libraries(numberstore-cli-lib 
    numberstore-protocol 
    numberstore-ipc 
    numberstore-utils
)

# Daemon Executable
add_executable(numberstore-daemon
    daemon/DaemonMain.cxx
)

target_link_libraries(numberstore-daemon numberstore-daemon-lib)

# CLI Executable
add_executable(numberstore-cli
    cli/CLIMain.cxx
)

target_link_libraries(numberstore-cli numberstore-cli-lib)

# Platform specific libraries
if(WIN32)
    target_link_libraries(numberstore-ipc ws2_32 kernel32)
    target_link_libraries(numberstore-daemon ws2_32 kernel32)
    target_link_libraries(numberstore-cli ws2_32 kernel32)
endif()

# Set output directories
set_target_properties(numberstore-daemon PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(numberstore-cli PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Create directories for output
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Installation rules
install(TARGETS numberstore-daemon numberstore-cli
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Print build configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Create a custom target to build both executables
add_custom_target(all-executables ALL
    DEPENDS numberstore-daemon numberstore-cli
)
