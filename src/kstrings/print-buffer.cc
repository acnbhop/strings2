//===------------------------------------------------------------------------------------------===//
// kstrings/print-buffer.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/print-buffer.hh"

NAMESPACE_BEGIN_KSTRINGS

CPrintBuffer::CPrintBuffer(s32 iBufferSize)
{
    this->m_iBufferSize = iBufferSize;
    this->m_pBuffer = new char[iBufferSize];
    this->m_iSpaceUsed = 0;
}

void CPrintBuffer::AddString(const char* szString, usize iLength)
{
    // Digest the buffer if it is full
    if (m_iSpaceUsed + (s32)iLength + 1 >= m_iBufferSize)
        Digest();
    // Copy the string if there is room
    if (m_iSpaceUsed + (s32)iLength + 1 >= m_iBufferSize)
    {
        // Digest this string without buffering it
        fwrite(szString, iLength, 1, stdout);
    }
    else
    {
        // Add it to the buffer
        memcpy(m_pBuffer + m_iSpaceUsed, szString, iLength);
        m_iSpaceUsed += (s32)iLength;
        m_pBuffer[m_iSpaceUsed] = 0;
    }
}

void CPrintBuffer::AddString(const char* szString)
{
    s32 iLength = (s32)strlen(szString);
    AddString(szString, iLength);
}

void CPrintBuffer::AddString(std::string szString)
{
    AddString(szString.c_str(), szString.length());
}

void CPrintBuffer::AddJsonString(std::string szJson)
{
    // Json string building is formed as an array of json objects.
    // We prepend "[" at the first log, comma delmit, then post-fix a "]" upon completion.
    if (m_bIsStart)
    {
        AddString("[" + szJson);
        m_bIsStart = false;
        m_bAddJsonClose = true;
    }
    else
    {
        AddString("," + szJson);
    }
}

void CPrintBuffer::Digest()
{
    if (m_iSpaceUsed > 0)
    {
        // Print the current buffer
        fwrite(m_pBuffer, 1, m_iSpaceUsed, stdout);
        fflush(stdout);
        m_pBuffer[0] = 0;
        m_iSpaceUsed = 0;
    }
}

CPrintBuffer::~CPrintBuffer(void)
{
    if (m_bAddJsonClose)
        AddString("]");
    Digest();
    delete[] m_pBuffer;
}

NAMESPACE_END_KSTRINGS
