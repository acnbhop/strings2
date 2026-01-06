// Class for extracted strings
#pragma once

// Core header
#include "kstrings/core.hh"

#include <string>
#include <codecvt>
#include <unordered_set>

__NS_BEGIN_KSTRINGS

enum eStringType
{
    TYPE_UNDETERMINED,
    TYPE_UTF8,
    TYPE_WIDE_STRING
};

class CExtractedString
{
private:
    eStringType m_Type;
    std::string m_String; // Supports Utf8
    usize m_SizeInBytes;
    s32 m_OffsetStart;
    s32 m_OffsetEnd;
public:
    CExtractedString();
    CExtractedString(const char* string, usize size_in_bytes, eStringType type, 
        s32 offset_start, s32 offset_end);
    CExtractedString(const wchar_t* string, usize size_in_bytes, eStringType type, 
        s32 offset_start, s32 offset_end);

    f32 GetProbaInteresting();
    usize GetSizeInBytes();
    std::string GetString();
    eStringType GetType();
    std::string GetTypeString();
    s32 GetOffsetStart();
    s32 GetOffsetEnd();
    bool IsInteresting();

    ~CExtractedString();
};

__NS_END_KSTRINGS
