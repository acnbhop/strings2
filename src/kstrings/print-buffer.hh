//===------------------------------------------------------------------------------------------===//
// kstrings/print-buffer.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

// Core header
#include "kstrings/core.hh"

#include <stdio.h>
#include <string>

__NS_BEGIN_KSTRINGS

class CPrintBuffer
{
    bool m_bIsStart = true;         // Is this the start of the json array
    bool m_bAddJsonClose = false;   // Should we add the json array closing bracket
    s32 m_iBufferSize;              // Size of the buffer
    s32 m_iSpaceUsed;               // Space used in the buffer
    char* m_pBuffer;                // Pointer to the buffer
public:
    // Add string with specified length
    void AddString( const char* string, size_t length );
    // Add string
    void AddString( const char* string );
    // Add string std::string
    void AddString( std::string string );

    // Add json string
    void AddJsonString( std::string json );
    // Digest the buffer to stdout
    void Digest();

    // Constructor
    CPrintBuffer( s32 buffer_size );
    // Destructor
    ~CPrintBuffer( void );
};

__NS_END_KSTRINGS
