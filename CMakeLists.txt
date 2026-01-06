#
# CMakeLists.txt
#       Primary CMake configuration for kstrings
#

# Setup the project
cmake_minimum_required(VERSION 3.10)
project(kstrings VERSION 1.0 LANGUAGES C CXX)

# Global CMake modules
include(CheckCSourceCompiles)       # For checking C source compile features
include(ProcessorCount)             # For counting processors
include(FetchContent)               # For fetching content

# The root directory of the project
set(PROJ_ROOT ${CMAKE_SOURCE_DIR})

# Global setup
include(${PROJ_ROOT}/cmake/shared/setup.cmake)

# kstrings setup
include(${PROJ_ROOT}/cmake/kstrings.cmake)
