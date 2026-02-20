//
// core.hpp
//

#pragma once

#ifdef _WIN32
#define PLATFORM_WINDOWS 1
#endif

#ifdef __linux__
#define PLATFORM_LINUX 1
#endif

#ifdef __APPLE__
#define PLATFORM_DARWIN 1
#endif

#ifdef __FreeBSD__
#define PLATFORM_FREEBSD 1
#endif

//
// System specific headers
//
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <Shlwapi.h>
#include <TlHelp32.h>
#endif

#if defined(PLATFORM_LINUX) || defined(PLATFORM_DARWIN) || defined(PLATFORM_FREEBSD)
#include <unistd.h>
#endif

//
// Standard Headers
//
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <codecvt>
#include <atomic>
#include <mutex>
#include <thread>
#include <algorithm>
#include <unordered_set>

#include "json.hpp"

#include "string_model.hpp"

#define _NS_BEGIN_STRINGS2      namespace strings2 {
#define _NS_END_STRINGS2        }

_NS_BEGIN_STRINGS2

using byte = std::byte;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;
using uptr = std::uintptr_t;
using iptr = std::intptr_t;
using umax = std::uintmax_t;
using imax = std::intmax_t;
using size = std::size_t;
using ptrdiff = std::ptrdiff_t;
using isize = std::ptrdiff_t;

using f32 = float;
using f64 = double;

_NS_END_STRINGS2
