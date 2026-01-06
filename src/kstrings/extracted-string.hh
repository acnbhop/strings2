//===------------------------------------------------------------------------------------------===//
// kstrings/extracted-string.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

// Core header
#include "kstrings/core.hh"

#include <string>
#include <codecvt>
#include <unordered_set>

NAMESPACE_BEGIN_KSTRINGS

// String types
enum class eStringType
{
    Undetermined = 0,
    UTF8,
    WideString
};

// Extracted string class
class CExtractedString
{
private:
    eStringType m_Type;     // String type
    std::string m_String;   // Supports Utf8
    usize m_SizeInBytes;    // Size in bytes
    s32 m_OffsetStart;      // Offset start in the buffer
    s32 m_OffsetEnd;        // Offset end in the buffer
public:
    // Constructor
    CExtractedString();
    // Constructor for char strings
    CExtractedString( const char* szString, usize iSizeInBytes, eStringType eType,
                      s32 iOffsetStart, s32 iOffsetEnd );
    // Constructor for wide char strings
    CExtractedString( const wchar_t* szString, usize iSizeInBytes, eStringType eType,
                      s32 iOffsetStart, s32 iOffsetEnd );
    f32 GetProbaInteresting();          // Probability of being an interesting string
    usize GetSizeInBytes();             // Size in bytes
    std::string GetString();            // Get the string value
    eStringType GetType();              // Get the string type
    std::string GetTypeString();        // Get the string type as string
    s32 GetOffsetStart();               // Get the offset start
    s32 GetOffsetEnd();                 // Get the offset end
    bool IsInteresting();               // Is the string interesting

    // Destructor
    ~CExtractedString();
};

NAMESPACE_END_KSTRINGS
