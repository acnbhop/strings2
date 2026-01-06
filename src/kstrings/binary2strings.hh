#pragma once

// Core header
#include "kstrings/core.hh"

// kstrings headers
#include "kstrings/extracted-string.hh"
// Moved to other file since it's expensive to render on screen in here.
// kstrings/globals.hh

// Standard headers
#include <string>
#include <vector>

__NS_BEGIN_KSTRINGS

#define WINDOW_SIZE 11

size_t TryUTF8CharStep(const unsigned char* buffer, usize buffer_size, isize offset);

s32 GetLanguageGroup(wchar_t c);

CExtractedString* TryExtractString(const unsigned char* buffer, usize buffer_size, long offset, 
    usize min_chars);

std::tuple<std::string, std::string, std::pair<int, int>, bool> 
try_extract_string_tuple(const unsigned char* buffer, usize buffer_size, long offset, 
    usize min_chars, bool only_interesting);

std::vector<std::tuple<std::string, std::string, std::pair<int, int>, bool>> 
ExtractAllStrings(const unsigned char buffer[], usize buffer_size, usize min_chars, 
    bool only_interesting);

__NS_END_KSTRINGS
