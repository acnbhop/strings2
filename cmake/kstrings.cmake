#
# kstrings.cmake
#
#       Configuration for kstrings executable
#

add_executable(kstrings
    src/main.cc
)

target_link_libraries(kstrings PRIVATE kstringslib)

# Set the project name for configuration to apply the settings
set(AAO_PROJECT_NAME kstrings)

# Apply configuration settings
include(cmake/config.cmake)
