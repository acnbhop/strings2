#===---------------------------------------------------------------------------------------------===
# shared/setup.cmake
#===---------------------------------------------------------------------------------------------===
# Core setup across all projects.
#===---------------------------------------------------------------------------------------------===

# Multiconfig or singleconfig generator?
if(CMAKE_CONFIGURATION_TYPES)
    set(KEN_MULTICONFIG TRUE)
else()
    set(KEN_MULTICONFIG FALSE)
endif()

#===-------------------------------------===
# Build Configurations
#===-------------------------------------===

# Define the build configurations
#set(KEN_BUILD_CONFIGURATIONS Debug Profile Beta BankRelease Release Final)
set(KEN_BUILD_CONFIGURATIONS Debug Release)

# Setup for either multiconfig or singleconfig scenarios
if(KEN_MULTICONFIG)
    # For multiconfig generators, set the configurations
    set(CMAKE_CONFIGURATION_TYPES ${KEN_BUILD_CONFIGURATIONS} CACHE STRING "Reset Build Configurations" FORCE)
else()
    # For singleconfig generators, set the default build configuration to Debug
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Configuration")
    endif()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${KEN_BUILD_CONFIGURATIONS})

    # Alias the build configuration
    set(KEN_BUILD_CONFIG ${CMAKE_BUILD_TYPE})
endif()

#===-------------------------------------===
# Compilers 
#===-------------------------------------==
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(KEN_COMPILER_MSVC TRUE)
else()
    set(KEN_COMPILER_MSVC FALSE)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND MSVC)
    set(KEN_COMPILER_CLANG_CL TRUE)
else()
    set(KEN_COMPILER_CLANG_CL FALSE)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT MSVC)
    set(KEN_COMPILER_CLANG TRUE)
else()
    set(KEN_COMPILER_CLANG FALSE)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(KEN_COMPILER_GCC TRUE)
else()
    set(KEN_COMPILER_GCC FALSE)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(KEN_COMPILER_APPLE_CLANG TRUE)
else()
    set(KEN_COMPILER_APPLE_CLANG FALSE)
endif()

#===-------------------------------------===
# Platforms
#===-------------------------------------===
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(KEN_PLATFORM_WINDOWS TRUE)
else()
    set(KEN_PLATFORM_WINDOWS FALSE)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(KEN_PLATFORM_LINUX TRUE)
else()
    set(KEN_PLATFORM_LINUX FALSE)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(KEN_PLATFORM_MACOS TRUE)
    set(KEN_PLATFORM_DARWIN TRUE)
else()
    set(KEN_PLATFORM_MACOS FALSE)
    set(KEN_PLATFORM_DARWIN FALSE)
endif()

#===-------------------------------------===
# CPUs
#===-------------------------------------===
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(KEN_64BIT TRUE)
    set(KEN_32BIT FALSE)
else()
    set(KEN_64BIT FALSE)
    set(KEN_32BIT TRUE)
endif()

# AMD64/x86_64
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
    set(KEN_CPU_AMD64 TRUE)
    set(KEN_CPU_X86_64 TRUE)
else()
    set(KEN_CPU_AMD64 FALSE)
    set(KEN_CPU_X86_64 FALSE)
endif()

# ARM64/AArch64
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|ARM64|arm64)$")
    set(KEN_CPU_ARM64 TRUE)
    set(KEN_CPU_AARCH64 TRUE)
else()
    set(KEN_CPU_ARM64 FALSE)
    set(KEN_CPU_AARCH64 FALSE)
endif()

# i386/x86_32
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i386|i486|i586|i686|x86|i86pc|x86_32)$")
    set(KEN_CPU_I386 TRUE)
    set(KEN_CPU_X86_32 TRUE)
else()
    set(KEN_CPU_I386 FALSE)
    set(KEN_CPU_X86_32 FALSE)
endif()

# ARM32
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm|ARM|arm32)$")
    set(KEN_CPU_ARM32 TRUE)
else()
    set(KEN_CPU_ARM32 FALSE)
endif()

# x86 CPU
if(KEN_CPU_AMD64 OR KEN_CPU_X86_64 OR KEN_CPU_I386 OR KEN_CPU_X86_32)
    set(KEN_CPU_X86 TRUE)
else()
    set(KEN_CPU_X86 FALSE)
endif()

# ARM CPU
if(KEN_CPU_ARM64 OR KEN_CPU_AARCH64 OR KEN_CPU_ARM32)
    set(KEN_CPU_ARM TRUE)
else()
    set(KEN_CPU_ARM FALSE)
endif()
