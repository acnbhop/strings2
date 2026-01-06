#
# kstringslib.cmake
#
#       Configuration for kstringslib
#

add_library(kstringslib STATIC
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

# Set the project name for configuration to apply the settings
set(KEN_PROJECT_NAME kstringslib)

# Apply configuration settings
include(${KEN_ROOT}/cmake/globalcore.cmake)
include(${KEN_ROOT}/cmake/config.cmake)
