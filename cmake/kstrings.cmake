# Set the project name for configuration to apply the settings
set(KEN_PROJECT_NAME kstrings)

# Set standards (from the original project)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Executable
add_executable(kstrings
    src/main.cc

    src/kstrings/basics.cc
    src/kstrings/basics.hh
    src/kstrings/binary2strings.cc
    src/kstrings/binary2strings.hh
    src/kstrings/core.hh
    src/kstrings/extracted-string.cc
    src/kstrings/extracted-string.hh
    src/kstrings/globals.hh
    src/kstrings/memory-strings.cc
    src/kstrings/memory-strings.hh
    src/kstrings/module.cc
    src/kstrings/module.hh
    src/kstrings/print-buffer.cc
    src/kstrings/print-buffer.hh
    src/kstrings/string-parser.cc
    src/kstrings/string-parser.hh
    src/kstrings/utility.cc
    src/kstrings/utility.hh
)

# Global configuration
include(${PROJ_ROOT}/cmake/shared/config.cmake)

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