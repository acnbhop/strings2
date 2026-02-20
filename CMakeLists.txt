cmake_minimum_required(VERSION 3.20)
if(NOT MSVC_IDE AND NOT XCODE)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()
project(strings2 VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/basics.cpp
    src/basics.h
    src/binary2strings.cpp
    src/binary2strings.hpp
    src/dirent.h
    src/extracted_string.cpp
    src/extracted_string.hpp
    src/json.hpp
    src/memory_strings.cpp
    src/memory_strings.h
    src/module.cpp
    src/module.h
    src/print_buffer.cpp
    src/print_buffer.h
    src/resource.h
    src/stdafx.cpp
    src/stdafx.h
    src/strings.cpp
    src/string_model.h
    src/string_parser.cpp
    src/string_parser.h
    src/targetver.h
)

add_executable(strings2 ${SOURCES})

if(WIN32)
    target_link_libraries(strings2 PRIVATE Shlwapi)
endif()
