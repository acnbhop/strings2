#===---------------------------------------------------------------------------------------------===
# kstrings.cmake
#===---------------------------------------------------------------------------------------------===
# Setup for kstrings.
#===---------------------------------------------------------------------------------------------===

# Set the project name for configuration to apply the settings
set(KEN_PROJECT_NAME strings2)

# Set standards (from the original project)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Executable
add_executable(strings2
    src/main.cc

    src/strings2/basics.cc
    src/strings2/basics.hh
    src/strings2/binary2strings.cc
    src/strings2/binary2strings.hh
    src/strings2/core.hh
    src/strings2/extracted-string.cc
    src/strings2/extracted-string.hh
    src/strings2/globals.hh
    src/strings2/memory-strings.cc
    src/strings2/memory-strings.hh
    src/strings2/module.cc
    src/strings2/module.hh
    src/strings2/print-buffer.cc
    src/strings2/print-buffer.hh
    src/strings2/string-parser.cc
    src/strings2/string-parser.hh
    src/strings2/utility.cc
    src/strings2/utility.hh
)

# Global configuration
include(${KEN_PROJ_ROOT}/cmake/shared/config.cmake)

#===-------------------------------------===
# Core Definitions
#===-------------------------------------===

# Define unicode if we do not have it already
if (NOT DEFINED UNICODE)
    target_compile_definitions(${KEN_PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()

# Make sure application on MSVC is a "console" application.
if (MSVC)
    set_target_properties(${KEN_PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()

# Define these on windows
if (KEN_PLATFORM_WINDOWS AND KEN_COMPILER_MSVC)
    if (KEN_BUILD_CONFIG STREQUAL "Debug")
        target_compile_definitions(${KEN_PROJECT_NAME} PRIVATE WIN32 _DEBUG)
    endif()

    if( KEN_BUILD_CONFIG STREQUAL "Release")
        target_compile_definitions(${KEN_PROJECT_NAME} PRIVATE WIN32 NDEBUG)
    endif()
endif()

# Release builds will enable intrinsic functions
if (KEN_BUILD_CONFIG STREQUAL "Release")
    if (KEN_COMPILER_MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /Oi)
    else()
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE -march=native)
    endif()
endif()

# Both for basic runtime checks on Debug
if (KEN_BUILD_CONFIG STREQUAL "Debug")
    if (KEN_COMPILER_MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /RTC1)
    else()
        # If it's true
        if (AAO_USE_SANITIZERS STREQUAL "ON")
            target_compile_options(${KEN_PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
            target_link_options(${KEN_PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
        endif()
    endif()
endif()

# Function-level linking on release
if (KEN_BUILD_CONFIG STREQUAL "Release")
    if (KEN_COMPILER_MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /Gy)
    else()
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections)
        target_link_options(${KEN_PROJECT_NAME} PRIVATE -Wl,--gc-sections)
    endif()
endif()
