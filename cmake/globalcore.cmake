#
# globalcore.cmake
#    Global core configuration settings.
#

#===-------------------------------------===
# Core Definitions
#===-------------------------------------===

# Define unicode if we do not have it already
if (NOT DEFINED UNICODE)
    target_compile_definitions(${KEN_PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()

# If on windows and we are on debug, define "WIN32" and "_DEBUG"
if (WIN32 AND KEN_BUILD_CONFIG STREQUAL "Debug")
    target_compile_definitions(${KEN_PROJECT_NAME} PRIVATE WIN32 _DEBUG)
endif()

# If on windows and we are on release, define "WIN32" and "NDEBUG"
if (WIN32 AND KEN_BUILD_CONFIG STREQUAL "Release")
    target_compile_definitions(${KEN_PROJECT_NAME} PRIVATE WIN32 NDEBUG)
endif()

# Release builds will enable intrinsic functions
if (KEN_BUILD_CONFIG STREQUAL "Release")
    if (MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /Oi)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE -march=native)
    endif()
endif()

# Debug builds have no optimizations
if (KEN_BUILD_CONFIG STREQUAL "Debug")
    if (MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /Od)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE -O0)
    endif()
endif()

# Both for basic runtime checks on Debug
if (KEN_BUILD_CONFIG STREQUAL "Debug")
    if (MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /RTC1)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # If it's true
        if (AAO_USE_SANITIZERS STREQUAL "ON")
            target_compile_options(${KEN_PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
            target_link_options(${KEN_PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
        endif()
    endif()
endif()

# Multi-threaded debug on "debug"
if (KEN_BUILD_CONFIG STREQUAL "Debug")
    if (MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /MDd)
    endif()
endif()

# Multi-threaded on "release"
if (KEN_BUILD_CONFIG STREQUAL "Release")
    if (MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /MD)
    endif()
endif()

# Function-level linking on release
if (KEN_BUILD_CONFIG STREQUAL "Release")
    if (MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /Gy)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections)
        target_link_options(${KEN_PROJECT_NAME} PRIVATE -Wl,--gc-sections)
    endif()
endif()

# Release builds favor maximum optimization
if (KEN_BUILD_CONFIG STREQUAL "Release")
    if (MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /O2)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE -O3)
    endif()
endif()