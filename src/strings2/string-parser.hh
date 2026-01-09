//===------------------------------------------------------------------------------------------===//
// strings2/string-parser.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

// Core header
#include "strings2/core.hh"

// kstrings headers
#include "strings2/binary2strings.hh"
#include "strings2/print-buffer.hh"

NAMESPACE_BEGIN_STRINGS2

// The maximum string size
constexpr s32 iMaxStringSize = 0x2000;
// The block size to read from files,
// this is around 50 MB.
constexpr f64 iBlockSize = 5e+7;

struct sStringOptions
{
    bool bPrintUTF8 = true;            // Print UTF8 strings
    bool bPrintWideString = true;      // Print Wide strings
    bool bPrintStringType = false;     // Print the string type (UTF8/WIDE)
    bool bPrintInteresting = true;     // Print only interesting strings
    bool bPrintNotInteresting = false; // Print non-interesting strings
    bool bPrintFilename = false;       // Print the filename
    bool bPrintFilepath = false;       // Print the filepath
    bool bPrintSpan = false;           // Print the string span (offsets)
    bool bPrintJson = false;           // Print output as JSON
    bool bEscapeNewLines = false;      // Escape new lines in strings
    s32 iMinChars = 4;                 // Minimum characters to consider a string
    usize iOffsetStart = 0;            // Start offset in the file to start parsing
    usize iOffsetEnd = 0;              // End offset in the file to stop parsing (0 = EOF)
};

// String parser class
class CStringParser
{
public:
    // Constructor
    CStringParser(sStringOptions Options);
    // Parse a memory block
    bool ParseBlock(u8* pBuffer, u32 iBufferLength, std::string szNameShort, std::string szNameLong,
                    u64 iBaseAddress);
    // Parse a file stream
    bool ParseStream(FILE* pFileHandle, std::string szNameShort, std::string szNameLong);
    // Destructor
    ~CStringParser(void);
private:
    enum class eExtractType
    {
        EXT_Raw, // Extract raw strings
        EXT_Asm  // Extract strings from assembly (x86/x64)
    };

    enum class eStringType
    {
        EST_Undetermined, // Undetermined string type
        EST_ASCII,        // ASCII string type
        EST_Unicode       // Unicode string type
    };

    sStringOptions m_Options; // String parsing options
    CPrintBuffer* m_Printer;  // Print buffer
};

NAMESPACE_END_STRINGS2
