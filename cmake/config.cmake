#
# config.cmake
#   Common configuration settings.
#

# Add src as an include directory
target_include_directories(${AAO_PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src")

# Define unicode if we do not have it already
if (NOT DEFINED UNICODE)
    target_compile_definitions(${AAO_PROJECT_NAME} PRIVATE UNICODE _UNICODE)
endif()

# Get how many processors are available
include(ProcessorCount)
ProcessorCount(NUMBER_OF_PROCESSORS_AVAILABLE)

# See if we are clang-cl
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MSVC)
    set(IS_CLANG_CL TRUE)
else()
    set(IS_CLANG_CL FALSE)
endif()


# Turn on multi processor compilation
if (MSVC AND NOT IS_CLANG_CL)
    target_compile_options(${AAO_PROJECT_NAME} PRIVATE /MP)
else()
    # Set makefile generator flags for parallel builds
    #
    # Make, Ninja, and others support the -j flag to specify
    # the number of parallel jobs to use.
    if (CMAKE_GENERATOR MATCHES "Makefiles" OR CMAKE_GENERATOR STREQUAL "Ninja")
        if (NUMBER_OF_PROCESSORS_AVAILABLE GREATER 1)
            set(CMAKE_MAKE_PROGRAM "${CMAKE_MAKE_PROGRAM} -j${NUMBER_OF_PROCESSORS_AVAILABLE}")
        endif()
    endif()
endif()

# Make sure application on MSVC is a "console" application.
if (MSVC)
    set_target_properties(${AAO_PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()

# If on windows and we are on debug, define "WIN32" and "_DEBUG"
if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${AAO_PROJECT_NAME} PRIVATE WIN32 _DEBUG)
endif()

# If on windows and we are on release, define "WIN32" and "NDEBUG"
if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${AAO_PROJECT_NAME} PRIVATE WIN32 NDEBUG)
endif()

# Warning level three
if (MSVC)
    target_compile_options(${AAO_PROJECT_NAME} PRIVATE /W3 /permissive-)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${AAO_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Release builds will enable intrinsic functions
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(${AAO_PROJECT_NAME} PRIVATE /Oi)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${AAO_PROJECT_NAME} PRIVATE -march=native)
    endif()
endif()

# Debug builds have no optimizations
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        target_compile_options(${AAO_PROJECT_NAME} PRIVATE /Od)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${AAO_PROJECT_NAME} PRIVATE -O0)
    endif()
endif()

# Both for basic runtime checks on Debug
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        target_compile_options(${AAO_PROJECT_NAME} PRIVATE /RTC1)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # If it's true
        if (AAO_USE_SANITIZERS STREQUAL "ON")
            target_compile_options(${AAO_PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
            target_link_options(${AAO_PROJECT_NAME} PRIVATE -fsanitize=address -fsanitize=undefined)
        endif()
    endif()
endif()

# Multi-threaded debug on "debug"
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (MSVC)
        target_compile_options(${AAO_PROJECT_NAME} PRIVATE /MDd)
    endif()
endif()

# Multi-threaded on "release"
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(${AAO_PROJECT_NAME} PRIVATE /MD)
    endif()
endif()

# Function-level linking on release
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(${AAO_PROJECT_NAME} PRIVATE /Gy)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${AAO_PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections)
        target_link_options(${AAO_PROJECT_NAME} PRIVATE -Wl,--gc-sections)
    endif()
endif()

# Release builds favor maximum optimization
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    if (MSVC)
        target_compile_options(${AAO_PROJECT_NAME} PRIVATE /O2)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${AAO_PROJECT_NAME} PRIVATE -O3)
    endif()
endif()

# Link "Shlwapi.lib" on Windows
if (WIN32)
    target_link_libraries(${AAO_PROJECT_NAME} PRIVATE Shlwapi)
endif()
