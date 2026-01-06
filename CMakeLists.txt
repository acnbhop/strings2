#
# CMakeLists.txt
#   Primary CMake configuration for kstrings
#

# Setup the project
cmake_minimum_required(VERSION 3.10)
project(kstrings VERSION 1.0 LANGUAGES C CXX)

# Set standards (from the original project)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(cmake/kstrings.cmake)
include(cmake/kstringslib.cmake)
