//
// kstrings/core.hh
//
#pragma once

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

#if KEN_PLATFORM_WINDOWS
#pragma comment(lib, "Shlwapi.lib")
#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <errno.h>

#pragma comment(lib, "Psapi.lib")
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

} // namespace kstrings

#define __NS_BEGIN_KSTRINGS      namespace kstrings {
#define __NS_END_KSTRINGS        }

#include "kstrings/globals.hh"
