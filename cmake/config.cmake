#
# config.cmake
#       Common configuration settings.
#

# Add src as an include directory
target_include_directories(${KEN_PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src")

# Make sure application on MSVC is a "console" application.
if (MSVC)
    set_target_properties(${KEN_PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif()

# Warning level three
if (MSVC)
    target_compile_options(${KEN_PROJECT_NAME} PRIVATE /W3 /permissive-)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${KEN_PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Link "Shlwapi.lib" on Windows
if (WIN32)
    target_link_libraries(${KEN_PROJECT_NAME} PRIVATE Shlwapi)
endif()
