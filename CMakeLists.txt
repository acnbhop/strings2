# Setup the project
cmake_minimum_required(VERSION 3.10)
project(strings2 VERSION 1.0 LANGUAGES C CXX)

# Set standards (from the original project)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Primary executable
add_executable(strings2
    src/basics.cpp
    src/basics.h
    src/binary2strings.cpp
    src/binary2strings.hpp
    src/dirent.h
    src/extracted_string.cpp
    src/extracted_string.hpp
    src/json.hpp
    src/memory_strings.cpp
    src/memory_strings.h
    src/module.cpp
    src/module.h
    src/print_buffer.cpp
    src/print_buffer.h
    src/string_model.h
    src/string_parser.cpp
    src/string_parser.h
    src/strings.cpp
    src/targetver.h
)

# Define unicode if we do not have it already
if (NOT DEFINED UNICODE)
    target_compile_definitions(strings2 PRIVATE UNICODE _UNICODE)
endif()

# Make sure application on MSVC is a "console" application.
if (MSVC)
    set_target_properties(strings2 PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()

# If on windows and we are on debug, define "WIN32" and "_DEBUG"
if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(strings2 PRIVATE WIN32 _DEBUG)
endif()

# If on windows and we are on release, define "WIN32" and "NDEBUG"
if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(strings2 PRIVATE WIN32 NDEBUG)
endif()

# Warning level three
if (MSVC)
    target_compile_options(strings2 PRIVATE /W3 /permissive-)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(strings2 PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Release builds will enable intrinsic functions
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(strings2 PRIVATE /Oi)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(strings2 PRIVATE -march=native)
    endif()
endif()

# Debug builds have no optimizations
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        target_compile_options(strings2 PRIVATE /Od)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(strings2 PRIVATE -O0)
    endif()
endif()

# Both for basic runtime checks on Debug
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        target_compile_options(strings2 PRIVATE /RTC1)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(strings2 PRIVATE -fsanitize=address -fsanitize=undefined)
        target_link_libraries(strings2 PRIVATE -fsanitize=address -fsanitize=undefined)
    endif()
endif()

# Multi-threaded debug on "debug"
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        target_compile_options(strings2 PRIVATE /MDd)
    endif()
endif()

# Multi-threaded on "release"
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(strings2 PRIVATE /MD)
    endif()
endif()

# Function-level linking on release
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(strings2 PRIVATE /Gy)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(strings2 PRIVATE -ffunction-sections -fdata-sections)
        target_link_options(strings2 PRIVATE -Wl,--gc-sections)
    endif()
endif()

# Release builds favor maximum optimization
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(strings2 PRIVATE /O2)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(strings2 PRIVATE -O3)
    endif()
endif()

# Link "Shlwapi.lib" on Windows
if (WIN32)
    target_link_libraries(strings2 PRIVATE Shlwapi)
endif()
