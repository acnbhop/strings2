//===------------------------------------------------------------------------------------------===//
// kstrings/core.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

#if defined(__clang__)
#define KEN_COMPILER_CLANG 1
#else
#define KEN_COMPILER_CLANG 0
#endif

#if KEN_COMPILER_CLANG && defined(_MSC_VER)
#undef KEN_COMPILER_CLANG
#define KEN_COMPILER_CLANG_CL 1
#else
#define KEN_COMPILER_CLANG_CL 0
#endif

#if !KEN_COMPILER_CLANG && (defined(__GNUC__) || defined(__GNUG__))
#define KEN_COMPILER_GCC 1
#else
#define KEN_COMPILER_GCC 0
#endif

#if defined(__x86_64__) || defined(__amd64__) || defined(_M_X64) || defined(_M_AMD64)
#define KEN_CPU_X86_64 1
#else
#define KEN_CPU_X86_64 0
#endif

#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
#define KEN_CPU_ARM64 1
#else
#define KEN_CPU_ARM64 0
#endif

#if defined(_WIN32) || defined(_WIN64)
#define KEN_PLATFORM_WINDOWS 1
#if defined(_WIN64)
#define KEN_PLATFORM_WIN64 1
#else
#define KEN_PLATFORM_WIN64 0
#endif
#if !KEN_PLATFORM_WIN64 && (defined(_WIN32))
#define KEN_PLATFORM_WIN32 1
#else
#define KEN_PLATFORM_WIN32 0
#endif
#else
#define KEN_PLATFORM_WINDOWS 0
#define KEN_PLATFORM_WIN64 0
#define KEN_PLATFORM_WIN32 0
#endif

#if defined(__linux__)
#define KEN_PLATFORM_LINUX 1
#else
#define KEN_PLATFORM_LINUX 0
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define KEN_PLATFORM_DARWIN 1
#else
#define KEN_PLATFORM_DARWIN 0
#endif

// Silence C++17 deprecation for codecvt
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#if KEN_PLATFORM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#if !KEN_COMPILER_GCC
#pragma comment(lib, "Shlwapi.lib")
#endif
#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <errno.h>
#if !KEN_COMPILER_GCC
#pragma comment(lib, "Psapi.lib")
#endif
#endif

#include <cstdint>
#include <cstddef>

#include <filesystem>
#include <string>
#include <iostream>

namespace kstrings
{

using f32 = float;
using f64 = double;

using s8 = std::int8_t;
using u8 = std::uint8_t;
using s16 = std::int16_t;
using u16 = std::uint16_t;
using s32 = std::int32_t;
using u32 = std::uint32_t;
using s64 = std::int64_t;
using u64 = std::uint64_t;

using byte = u8;

using uptr = std::uintptr_t;
using sptr = std::intptr_t;
using smax = std::ptrdiff_t;
using umax = std::size_t;
using usize = std::size_t;
using isize = std::ptrdiff_t;

// min
template<typename T>
constexpr const T& min( const T& a, const T& b )
{
    return ( a < b ) ? a : b;
}

// max
template<typename T>
constexpr const T& max( const T& a, const T& b )
{
    return ( a > b ) ? a : b;
}

// exp
template<typename T>
constexpr T exp( T x )
{
    T result = 1;
    T term = 1;
    for ( usize n = 1; n < 20; ++n )
    {
        term *= x / static_cast<T>( n );
        result += term;
    }
    return result;
}

} // namespace kstrings

#define NAMESPACE_BEGIN_KSTRINGS      namespace kstrings {
#define NAMESPACE_END_KSTRINGS        }

#include "kstrings/globals.hh"
