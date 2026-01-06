#pragma once

// Core header
#include "kstrings/core.hh"

// kstrings headers
#include "kstrings/print-buffer.hh"
#include "kstrings/binary2strings.hh"

// Standard headers
#include <algorithm>

// External headers
#include "extern/json.hpp"

__NS_BEGIN_KSTRINGS

constexpr auto iMaxStringSize = 0x2000;
constexpr auto iBlockSize = 5e+7; // 50MB

struct sStringOptions
{
    bool bPrintUTF8 = true;
    bool bPrintWideString = true;
    bool bPrintStringType = false;
    bool bPrintInteresting = true;
    bool bPrintNotInteresting = false;
    bool bPrintFilename = false;
    bool bPrintFilepath = false;
    bool bPrintSpan = false;
    bool bPrintJson = false;
    bool bEscapeNewLines = false;
    s32 iMinChars = 4;
    usize iOffsetStart = 0;
    usize iOffsetEnd = 0;
};

class CStringParser
{
    // Maybe add XOR methods for extracting strings?
    enum eExtractType
    {
        EXTRACT_RAW,
        EXTRACT_ASM
    };

    enum eStringType
    {
        TYPE_UNDETERMINED,
        TYPE_ASCII,
        TYPE_UNICODE
    };

    sStringOptions m_Options;
    CPrintBuffer* m_Printer;

public:
    CStringParser( sStringOptions options );
    bool ParseBlock( unsigned char* buffer, u32 buffer_length, std::string name_short, std::string name_long, u64 base_address );
    bool ParseStream( FILE* fh, std::string name_short, std::string name_long );
    ~CStringParser( void );
};

__NS_END_KSTRINGS
