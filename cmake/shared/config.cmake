if(KEN_PROJECT_NAME STREQUAL "")
    status(FATAL_ERROR "Cannot use this file without not specifying a project name!")
else()

# Add src as an include directory
target_include_directories(${KEN_PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src")

#===-------------------------------------===
# Multi Processor Configuration
#===-------------------------------------===
ProcessorCount(KEN_NUM_OF_PROCESSORS)

# Turn on multi processor compilation
if (KEN_COMPILER_MSVC AND NOT KEN_COMPILER_CLANG_CL)
    target_compile_options(${KEN_PROJECT_NAME} PRIVATE /MP)
else()
    # Set makefile generator flags for parallel builds
    #
    # Make, Ninja, and others support the -j flag to specify
    # the number of parallel jobs to use.
    if (CMAKE_GENERATOR MATCHES "Makefiles" OR CMAKE_GENERATOR STREQUAL "Ninja")
        if (KEN_NUM_OF_PROCESSORS GREATER 1)
            set(CMAKE_MAKE_PROGRAM "${CMAKE_MAKE_PROGRAM} -j${KEN_NUM_OF_PROCESSORS}")
        endif()
    endif()
endif()

# Debug builds have no optimizations
if (KEN_BUILD_CONFIG STREQUAL "Debug")
    if (KEN_COMPILER_MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /Od)
    else()
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE -O0)
    endif()
endif()

# Release / Final builds favor maximum optimization
if (KEN_BUILD_CONFIG STREQUAL "Release" OR KEN_BUILD_CONFIG STREQUAL "Final")
    if (KEN_COMPILER_MSVC)
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE /O2)
    else()
        target_compile_options(${KEN_PROJECT_NAME} PRIVATE -O3)
    endif()
endif()

# Multi-threaded debug on "debug"
if (KEN_BUILD_CONFIG STREQUAL "Debug" AND KEN_COMPILER_MSVC)
    target_compile_options(${KEN_PROJECT_NAME} PRIVATE /MDd)
endif()

# Multi-threaded on "release"
if (KEN_BUILD_CONFIG STREQUAL "Release" AND KEN_COMPILER_MSVC)
    target_compile_options(${KEN_PROJECT_NAME} PRIVATE /MD)
endif()

# Warning level three
if (KEN_COMPILER_MSVC)
    target_compile_options(${KEN_PROJECT_NAME} PRIVATE /W3 /permissive-)
else()
    target_compile_options(${KEN_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Link "Shlwapi.lib" on Windows
if (KEN_PLATFORM_WINDOWS)
    target_link_libraries(${KEN_PROJECT_NAME} PRIVATE Shlwapi)
endif()

endif() # KEN_PROJECT_NAME STREQUAL ""
