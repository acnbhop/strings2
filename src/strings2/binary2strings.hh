//===------------------------------------------------------------------------------------------===//
// strings2/binary2strings.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

// Core header
#include "strings2/core.hh"

// strings2 headers
#include "strings2/extracted-string.hh"

// Standard headers
#include <string>
#include <vector>

NAMESPACE_BEGIN_STRINGS2

#define WINDOW_SIZE 11

// Tries to parse a utf8 character at the specified offset in the buffer.
size_t TryUTF8CharStep(const u8* pBuffer, usize iBufferSize, isize iOffset);

// Returns the language group of a unicode wchar.
s32 GetLanguageGroup(wchar_t c);

// Tries to extract a string at the specified offset in the buffer.
CExtractedString* TryExtractString(const u8* pBuffer, usize iBufferSize, isize iOffset,
                                   usize iMinChars);

// Tries to extract a string at the specified offset in the buffer, returning a tuple.
std::tuple<std::string, std::string, std::pair<s32, s32>, bool>
TryExtractStringTuple(const u8* pBuffer, usize iBufferSize, isize iOffset, usize iMinChars,
                      bool bOnlyInteresting);

// Extracts all strings from the specified binary buffer.
std::vector<std::tuple<std::string, std::string, std::pair<s32, s32>, bool>>
ExtractAllStrings(const u8 pBuffer[], usize iBufferSize, usize iMinChars, bool bOnlyInteresting);

NAMESPACE_END_STRINGS2
