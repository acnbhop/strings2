//===------------------------------------------------------------------------------------------===//
// strings2/core.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

#if defined(__clang__)
#define AAO_COMPILER_CLANG 1
#if defined(__apple_build_version__)
#define AAO_COMPILER_APPLE_CLANG 1
#else
#define AAO_COMPILER_APPLE_CLANG 0
#endif
#else
#define AAO_COMPILER_CLANG 0
#define AAO_COMPILER_APPLE_CLANG 0
#endif

#if AAO_COMPILER_CLANG && defined(_MSC_VER)
#undef AAO_COMPILER_CLANG
#define AAO_COMPILER_CLANG_CL 1
#else
#define AAO_COMPILER_CLANG_CL 0
#endif

#if !AAO_COMPILER_CLANG && (defined(__GNUC__) || defined(__GNUG__))
#define AAO_COMPILER_GCC 1
#else
#define AAO_COMPILER_GCC 0
#endif

#if defined(__x86_64__) || defined(__amd64__) || defined(_M_X64) || defined(_M_AMD64)
#define AAO_CPU_X86_64 1
#else
#define AAO_CPU_X86_64 0
#endif

#if defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM64)
#define AAO_CPU_ARM64 1
#else
#define AAO_CPU_ARM64 0
#endif

#if defined(_WIN32) || defined(_WIN64)
#define AAO_PLATFORM_WINDOWS 1
#if defined(_WIN64)
#define AAO_PLATFORM_WIN64 1
#else
#define AAO_PLATFORM_WIN64 0
#endif
#if !AAO_PLATFORM_WIN64 && (defined(_WIN32))
#define AAO_PLATFORM_WIN32 1
#else
#define AAO_PLATFORM_WIN32 0
#endif
#else
#define AAO_PLATFORM_WINDOWS 0
#define AAO_PLATFORM_WIN64 0
#define AAO_PLATFORM_WIN32 0
#endif

#if defined(__linux__)
#define AAO_PLATFORM_LINUX 1
#else
#define AAO_PLATFORM_LINUX 0
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define AAO_PLATFORM_DARWIN 1
#else
#define AAO_PLATFORM_DARWIN 0
#endif

// Silence C++17 deprecation for codecvt
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#if AAO_PLATFORM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#if !AAO_COMPILER_GCC
#pragma comment(lib, "Shlwapi.lib")
#endif
#define NOMINMAX
#include <Windows.h>
#include <Psapi.h>
#include <errno.h>
#include <tlhelp32.h>
#if !AAO_COMPILER_GCC
#pragma comment(lib, "Psapi.lib")
#endif
#endif

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>

// External headers
#include "extern/json.hpp"

// Apple Clang needs this for std::stringstream
#if AAO_COMPILER_APPLE_CLANG
#include <sstream>
#endif

namespace strings2
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
template <typename T> constexpr const T& min(const T& a, const T& b) { return (a < b) ? a : b; }

// max
template <typename T> constexpr const T& max(const T& a, const T& b) { return (a > b) ? a : b; }

// exp
template <typename T> constexpr T exp(T x)
{
    T result = 1;
    T term = 1;
    for (usize n = 1; n < 20; ++n)
    {
        term *= x / static_cast<T>(n);
        result += term;
    }
    return result;
}

} // namespace strings2

#define NAMESPACE_BEGIN_STRINGS2                                                                   \
    namespace strings2                                                                             \
    {
#define NAMESPACE_END_STRINGS2 }

#include "strings2/globals.hh"
