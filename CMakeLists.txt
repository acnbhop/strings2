# Setup the project
cmake_minimum_required(VERSION 3.10)
project(kstrings VERSION 1.0 LANGUAGES C CXX)

# Set standards (from the original project)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Primary executable
add_executable(kstrings
    src/main.cc
)

add_library(kstringslib STATIC
    src/kstrings/basics.cc
    src/kstrings/basics.hh
    src/kstrings/binary2strings.cc
    src/kstrings/binary2strings.hh
    src/kstrings/extracted-string.cc
    src/kstrings/extracted-string.hh
    src/kstrings/memory-strings.cc
    src/kstrings/memory-strings.hh
    src/kstrings/module.cc
    src/kstrings/module.hh
    src/kstrings/print-buffer.cc
    src/kstrings/print-buffer.hh
    src/kstrings/string-model.hh
    src/kstrings/string-parser.cc
    src/kstrings/string-parser.hh
)

# Forceinclude "src/core.hh" for all files in kstrings and kstringslib
if(MSVC)
    target_compile_options(kstringslib PRIVATE /FI"${CMAKE_SOURCE_DIR}/src/core.hh")
    target_compile_options(kstrings PRIVATE /FI"${CMAKE_SOURCE_DIR}/src/core.hh")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(kstringslib PRIVATE -include "${CMAKE_SOURCE_DIR}/src/core.hh")
    target_compile_options(kstrings PRIVATE -include "${CMAKE_SOURCE_DIR}/src/core.hh")
endif()

# Add src as an include directory
target_include_directories(kstringslib PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(kstrings PUBLIC "${CMAKE_SOURCE_DIR}/src")

# Link kstringslib into kstrings executable
target_link_libraries(kstrings PRIVATE kstringslib)

# Define unicode if we do not have it already
if (NOT DEFINED UNICODE)
    target_compile_definitions(kstrings PRIVATE UNICODE _UNICODE)
endif()

# Make sure application on MSVC is a "console" application.
if (MSVC)
    set_target_properties(kstrings PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()

# If on windows and we are on debug, define "WIN32" and "_DEBUG"
if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(kstrings PRIVATE WIN32 _DEBUG)
endif()

# If on windows and we are on release, define "WIN32" and "NDEBUG"
if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(kstrings PRIVATE WIN32 NDEBUG)
endif()

# Warning level three
if (MSVC)
    target_compile_options(kstrings PRIVATE /W3 /permissive-)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(kstrings PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Release builds will enable intrinsic functions
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(kstrings PRIVATE /Oi)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(kstrings PRIVATE -march=native)
    endif()
endif()

# Debug builds have no optimizations
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        target_compile_options(kstrings PRIVATE /Od)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(kstrings PRIVATE -O0)
    endif()
endif()

# Both for basic runtime checks on Debug
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        target_compile_options(kstrings PRIVATE /RTC1)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(kstrings PRIVATE -fsanitize=address -fsanitize=undefined)
        target_link_libraries(kstrings PRIVATE -fsanitize=address -fsanitize=undefined)
    endif()
endif()

# Multi-threaded debug on "debug"
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        target_compile_options(kstrings PRIVATE /MDd)
    endif()
endif()

# Multi-threaded on "release"
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(kstrings PRIVATE /MD)
    endif()
endif()

# Function-level linking on release
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(kstrings PRIVATE /Gy)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(kstrings PRIVATE -ffunction-sections -fdata-sections)
        target_link_options(kstrings PRIVATE -Wl,--gc-sections)
    endif()
endif()

# Release builds favor maximum optimization
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(kstrings PRIVATE /O2)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(kstrings PRIVATE -O3)
    endif()
endif()

# Link "Shlwapi.lib" on Windows
if (WIN32)
    target_link_libraries(kstrings PRIVATE Shlwapi)
endif()
