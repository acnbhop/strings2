//===------------------------------------------------------------------------------------------===//
// kstrings/print-buffer.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/print-buffer.hh"

__NS_BEGIN_KSTRINGS

CPrintBuffer::CPrintBuffer( s32 buffer_size )
{
    this->m_iBufferSize = buffer_size;
    this->m_pBuffer = new char[buffer_size];
    this->m_iSpaceUsed = 0;
}

void CPrintBuffer::AddString( const char* string, size_t length )
{
    // Digest the buffer if it is full
    if ( m_iSpaceUsed + (s32) length + 1 >= m_iBufferSize )
        Digest();
    // Copy the string if there is room
    if ( m_iSpaceUsed + (s32) length + 1 >= m_iBufferSize )
    {
        // Digest this string without buffering it
        fwrite( string, length, 1, stdout );
    }
    else
    {
        // Add it to the buffer
        memcpy( m_pBuffer + m_iSpaceUsed, string, length );
        m_iSpaceUsed += (s32) length;
        m_pBuffer[m_iSpaceUsed] = 0;
    }
}

void CPrintBuffer::AddString( const char* string )
{
    s32 length = (s32) strlen( string );
    AddString( string, length );
}

void CPrintBuffer::AddString( std::string string )
{
    AddString( string.c_str(), string.length() );
}

void CPrintBuffer::AddJsonString( std::string json )
{
    // Json string building is formed as an array of json objects.
    // We prepend "[" at the first log, comma delmit, then post-fix a "]" upon completion.
    if ( m_bIsStart )
    {
        AddString( "[" + json );
        m_bIsStart = false;
        m_bAddJsonClose = true;
    }
    else
    {
        AddString( "," + json );
    }
}

void CPrintBuffer::Digest()
{
    if ( m_iSpaceUsed > 0 )
    {
        // Print the current buffer
        fwrite( m_pBuffer, 1, m_iSpaceUsed, stdout );
        fflush( stdout );
        m_pBuffer[0] = 0;
        m_iSpaceUsed = 0;
    }
}

CPrintBuffer::~CPrintBuffer( void )
{
    if ( m_bAddJsonClose )
        AddString( "]" );
    Digest();
    delete[] m_pBuffer;
}

__NS_END_KSTRINGS
