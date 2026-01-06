#pragma once

// Core header
#include "kstrings/core.hh"

#include <stdio.h>
#include <string>

__NS_BEGIN_KSTRINGS

class CPrintBuffer
{
    bool m_bIsStart = true;
    bool m_bAddJsonClose = false;
    s32 m_iBufferSize;
    s32 m_iSpaceUsed;
    char* m_pBuffer;
public:
    void AddString(const char* string, size_t length);
    void AddString(const char* string);
    void AddString(std::string string);

    void AddJsonString(std::string json);
    void Digest();
    
    CPrintBuffer(s32 buffer_size);
    ~CPrintBuffer(void);
};

__NS_END_KSTRINGS
