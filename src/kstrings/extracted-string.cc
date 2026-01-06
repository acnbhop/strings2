//===------------------------------------------------------------------------------------------===//
// kstrings/extracted-string.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/extracted-string.hh"

NAMESPACE_BEGIN_KSTRINGS

// gcc-disable: -Wdeprecated-declarations
#if KEN_COMPILER_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

// apple-clang-disable: -Wdeprecated-declarations
#if KEN_COMPILER_APPLE_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

std::wstring_convert<std::codecvt_utf8<wchar_t>> _converter;

#if KEN_COMPILER_GCC
    #pragma GCC diagnostic pop
#endif

#if KEN_COMPILER_APPLE_CLANG
    #pragma clang diagnostic pop
#endif

CExtractedString::CExtractedString()
{
    m_Type = eStringType::Undetermined;
    m_String = (std::string) NULL;
    m_SizeInBytes = 0;
    m_OffsetStart = 0;
    m_OffsetEnd = 0;
}

CExtractedString::CExtractedString( const char* szString, size_t iSizeInBytes, eStringType eType, s32 iOffsetStart, s32 iOffsetEnd )
{
    m_Type = eType;
    m_String = std::string( szString, iSizeInBytes );
    m_SizeInBytes = iSizeInBytes;
    m_OffsetStart = iOffsetStart;
    m_OffsetEnd = iOffsetEnd;
}

CExtractedString::CExtractedString( const wchar_t* szString, size_t iSizeInBytes, eStringType eType, s32 iOffsetStart, s32 iOffsetEnd )
{
    m_Type = eType;

    // Convert to UTF8 string
    m_String = _converter.to_bytes( szString, szString + iSizeInBytes / 2 );
    //m_String = _wchar_to_utf8(string, size_in_bytes);
    m_SizeInBytes = iSizeInBytes;
    m_OffsetStart = iOffsetStart;
    m_OffsetEnd = iOffsetEnd;
}

f32 CExtractedString::GetProbaInteresting()
{
    // Returns a probability of the string being interesting, 0.0 to 1.0.
    // An interesting string is non-gibberish. Gibberish is mostly erroneous
    // short extracted strings from binary content.

    // The model is trained to only support strings of length 4 to 7. Longer
    // strings are asssumed to be interesting, shorter assumed gibberish..
    usize iLength = (int) m_String.length();
    if ( iLength > 16 )
        return 1.0f;
    if ( iLength < 4 )
        return 0.0f;

    // Score the features
    // 	118 character unigrams (character ranges 0x9 to 0x7e)
    // 	118 + 118*118 character bigrams
    // 	1 for the total number of characters in string
    // 	1 for the total number of > 0x128 ascii code
    //  1 for distinct character count
    f32 fScore = (f32) string_model::fBias;
    std::unordered_set<wchar_t> CharacterCounts; // Character counts 

    // gcc-disable: -Wsign-compare
    #if KEN_COMPILER_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wsign-compare"
    #endif

    for ( usize i = 0; i < iLength; i++ )
    {
        // Count distinct characters
        CharacterCounts.insert( m_String[i] );
        if ( m_String[i] >= 0x9 && m_String[i] <= 0x7E )
        {
            // Unigram
            fScore += (f32) string_model::fWeights[m_String[i] - 0x9];

            // Bigram
            if ( i + 1 < iLength && m_String[i + 1] >= 0x9 && m_String[i + 1] <= 0x7E )
            {

            #if KEN_COMPILER_GCC
                #pragma GCC diagnostic pop
            #endif

                fScore += (f32) string_model::fWeights[118 + (m_String[i] - 0x9) + 118 * (m_String[i + 1] - 0x9)];
            }
        }
        else
        {
            // Number of non-latin unicode characters
            fScore += (f32) string_model::fWeights[118 + 118 + 118 * 118 + 1];
        }
    }

    // Add the string length weight
    fScore += (f32) string_model::fWeights[118 + 118 + 118 * 118] * (float) iLength;

    // Add the distinct character count weight
    fScore += (f32) string_model::fWeights[118 + 118 + 118 * 118 + 2] * (float) CharacterCounts.size();

    // Convert it to a probability
    return 1.0f / (1.0f + kstrings::exp( -fScore ));
}

usize CExtractedString::GetSizeInBytes()
{
    return m_SizeInBytes;
}

std::string CExtractedString::GetString()
{
    return m_String;
}

bool CExtractedString::IsInteresting()
{
    return GetProbaInteresting() > 0.5f;
}

eStringType CExtractedString::GetType()
{
    return m_Type;
}

std::string CExtractedString::GetTypeString()
{
    if ( m_Type == eStringType::UTF8 )
    {
        return "UTF8";
    }
    else if ( m_Type == eStringType::WideString )
    {
        return "WideString";
    }
    else
    {
        return "Undetermined";
    }
}

int CExtractedString::GetOffsetStart()
{
    return m_OffsetStart;
}

int CExtractedString::GetOffsetEnd()
{
    return m_OffsetEnd;
}

CExtractedString::~CExtractedString()
{
    // Nothing to do
}

NAMESPACE_END_KSTRINGS
