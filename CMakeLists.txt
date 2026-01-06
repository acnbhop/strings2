#===---------------------------------------------------------------------------------------------===
# CMakeLists.txt
#===---------------------------------------------------------------------------------------------===
# Primary file for configuring.
#===---------------------------------------------------------------------------------------------===
cmake_minimum_required(VERSION 3.10)
project(kstrings VERSION 1.0 LANGUAGES C CXX)

#
# Global CMake modules
#
# Note: Always include these for AAO projects that use 'shared' cmake files.
#
include(CheckCSourceCompiles)       # For checking C source compile features
include(ProcessorCount)             # For counting processors
include(FetchContent)               # For fetching content

# The root directory of the project
set(KEN_PROJ_ROOT ${CMAKE_SOURCE_DIR})

# Global setup
include(${KEN_PROJ_ROOT}/cmake/shared/setup.cmake)

# kstrings setup
include(${KEN_PROJ_ROOT}/cmake/kstrings.cmake)
