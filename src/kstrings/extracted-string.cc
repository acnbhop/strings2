// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/extracted-string.hh"

__NS_BEGIN_KSTRINGS

std::wstring_convert<std::codecvt_utf8<wchar_t>> _converter;

CExtractedString::CExtractedString()
{
    m_Type = TYPE_UNDETERMINED;
    m_String = (std::string) NULL;
    m_SizeInBytes = 0;
    m_OffsetStart = 0;
    m_OffsetEnd = 0;
}

CExtractedString::CExtractedString( const char* string, size_t size_in_bytes, eStringType type, s32 offset_start, s32 offset_end )
{
    m_Type = type;
    m_String = std::string( string, size_in_bytes );
    m_SizeInBytes = size_in_bytes;
    m_OffsetStart = offset_start;
    m_OffsetEnd = offset_end;
}

CExtractedString::CExtractedString( const wchar_t* string, size_t size_in_bytes, eStringType type, s32 offset_start, s32 offset_end )
{
    m_Type = type;

    // Convert to UTF8 string
    m_String = _converter.to_bytes( string, string + size_in_bytes / 2 );
    //m_String = _wchar_to_utf8(string, size_in_bytes);
    m_SizeInBytes = size_in_bytes;
    m_OffsetStart = offset_start;
    m_OffsetEnd = offset_end;
}

f32 CExtractedString::GetProbaInteresting()
{
    // Returns a probability of the string being interesting, 0.0 to 1.0.
    // An interesting string is non-gibberish. Gibberish is mostly erroneous
    // short extracted strings from binary content.

    // The model is trained to only support strings of length 4 to 7. Longer
    // strings are asssumed to be interesting, shorter assumed gibberish..
    int l = (int) m_String.length();
    if ( l > 16 )
        return 1.0f;
    if ( l < 4 )
        return 0.0f;

    // Score the features
    // 	118 character unigrams (character ranges 0x9 to 0x7e)
    // 	118 + 118*118 character bigrams
    // 	1 for the total number of characters in string
    // 	1 for the total number of > 0x128 ascii code
    //  1 for distinct character count
    f32 score = (f32) string_model::fBias;
    std::unordered_set<wchar_t> CharacterCounts; // Character counts 
    for ( int i = 0; i < l; i++ )
    {
        // Count distinct characters
        CharacterCounts.insert( m_String[i] );
        if ( m_String[i] >= 0x9 && m_String[i] <= 0x7E )
        {
            // Unigram
            score += (f32) string_model::fWeights[m_String[i] - 0x9];

            // Bigram
            if ( i + 1 < l && m_String[i + 1] >= 0x9 && m_String[i + 1] <= 0x7E )
            {
                score += (f32) string_model::fWeights[118 + (m_String[i] - 0x9) + 118 * (m_String[i + 1] - 0x9)];
            }
        }
        else
        {
            // Number of non-latin unicode characters
            score += (f32) string_model::fWeights[118 + 118 + 118 * 118 + 1];
        }
    }

    // Add the string length weight
    score += (f32) string_model::fWeights[118 + 118 + 118 * 118] * (float) l;

    // Add the distinct character count weight
    score += (f32) string_model::fWeights[118 + 118 + 118 * 118 + 2] * (float) CharacterCounts.size();

    // Convert it to a probability
    return 1.0f / (1.0f + exp( -score ));
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
    if ( m_Type == TYPE_UTF8 )
    {
        return "UTF8";
    }
    else if ( m_Type == TYPE_WIDE_STRING )
    {
        return "WIDE_STRING";
    }
    else
    {
        return "UNDETERMINED";
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

__NS_END_KSTRINGS
