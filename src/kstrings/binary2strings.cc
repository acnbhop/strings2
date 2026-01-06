//===------------------------------------------------------------------------------------------===//
// kstrings/binary2strings.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/binary2strings.hh"

NAMESPACE_BEGIN_KSTRINGS

usize TryUTF8CharStep( const u8* pBuffer, usize iBufferSize, isize iOffset )
{
    // Returns 0 if it's not likely a valid utf8 character. For ascii range of characters it requires
    // the character to be a displayable character.
    if ( iBufferSize < (usize) iOffset + 1 )
    {
        return 0;
    }

    u8 iFirstByte = pBuffer[iOffset];

    if ( iFirstByte < 0x80 )
    {
        if ( IsDisplayableASCII[iFirstByte] )
        {
            return 1;
        }
        return 0;
    }

    if ( iFirstByte < 0xC2 )
    {
        return 0; // Ignore continuation bytes, they are non displayable.
    }
    if ( iFirstByte > 0xF4 )
    {
        return 0; // Invalid bytes for Utf8, https://en.wikipedia.org/wiki/UTF-8#Invalid_byte_sequences
    }
    if ( iFirstByte < 0xE0 )
    {
        // 2 byte encoding
        if ( iBufferSize < (usize) iOffset + 2 )
        {
            return 0;
        }
        if ( (pBuffer[iOffset + 1] & 0xC0) != 0x80 )
        {
            return 0;
        }

        // Now convert the character to wchar_t
        wchar_t cConvert = 0;
        cConvert |= (iFirstByte & 0x1F) << 6;
        cConvert |= (pBuffer[iOffset + 1] & 0x3F);

        // Require the character to be seen at least once in commoncrawl web scape
        if ( IsSeenCommonCrawl.find( cConvert ) == IsSeenCommonCrawl.end() )
            return 0; // Invalid unicode character

        return 2;
    }

    if ( iFirstByte < 0xF0 )
    {
        // 3 byte encoding
        if ( iBufferSize < (usize) iOffset + 3 )
        {
            return 0;
        }
        if ( (pBuffer[iOffset + 1] & 0xC0) != 0x80 )
        {
            return 0;
        }
        if ( (pBuffer[iOffset + 2] & 0xC0) != 0x80 )
        {
            return 0;
        }

        // Now convert the character to wchar_t
        wchar_t cConvert = 0;
        cConvert |= (iFirstByte & 0x0F) << 12;
        cConvert |= (pBuffer[iOffset + 1] & 0x3F) << 6;
        cConvert |= (pBuffer[iOffset + 2] & 0x3F);

        // Check special cases
        if ( iFirstByte == 0xE0 )
        {
            // Check for overlong case
            if ( cConvert < 0x800 )
            {
                return 0; // Overlong
            }
        }
        else if ( iFirstByte == 0xED )
        {
            // Check for surrogate pair case
            if ( cConvert >= 0xD800 && cConvert <= 0xDFFF )
            {
                return 0; // Surrogate pair range
            }
        }

        // Require the character to be seen at least once in commoncrawl web scape
        if ( IsSeenCommonCrawl.find( cConvert ) == IsSeenCommonCrawl.end() )
        {
            return 0; // Invalid unicode character
        }
        return 3;
    }
    // 4 byte encoding is out of scope, since it is outside the Basic Multilingual Plane.
    return 0;
}

s32 GetLanguageGroup( wchar_t c )
{
    // Returns the language group of a unicode wchar.
    // Return value of 0x0 denotes an invalid language group, and 0x1 denotes Latin.
    return BMP12BitsToGroup[c >> 4]; // Leading 12 bits identify the language group
}

#if KEN_PLATFORM_WINDOWS && !KEN_COMPILER_GCC
__declspec(safebuffers)
#endif
CExtractedString* TryExtractString( const u8* pBuffer,
                                    usize iBufferSize, isize iOffset, usize iMinChars )
{   
    usize iLocalOffset = 0;
    usize iCharCount = 0;

    // Try to parse as utf8 first
    usize UtfCharLength;
    UtfCharLength = TryUTF8CharStep( pBuffer, iBufferSize, iOffset );

    if ( UtfCharLength > 0 )
    {
        iLocalOffset = iOffset;
        while ( iLocalOffset < iBufferSize && UtfCharLength > 0 )
        {
            iLocalOffset += UtfCharLength;
            iCharCount++;

            // Try parse next character
            UtfCharLength = TryUTF8CharStep( pBuffer, iBufferSize, iLocalOffset );
        }

        if ( iCharCount >= iMinChars )
        {
            // Return the extracted string
            return new CExtractedString(
                (char*) (pBuffer + iOffset), iLocalOffset - iOffset, eStringType::UTF8, (s32) iOffset, (s32) (iLocalOffset - 1)
            );
        }

        // Not a valid utf8 string, try a unicode string parse still
    }

    // Try extract as unicode wchar
    // Unicode logic requires all characters to be in the same BMP block or in the BMP Basic Latin
    // Block. (eg any single BMP block plus latin in the same string is OK)

    // The non-basic-latin language group identified
    s32 group = -1;
    iLocalOffset = iOffset;
    iCharCount = 0;

    // Parse as unicode
    while ( iLocalOffset + 1 < iBufferSize )
    {
        wchar_t c = *(wchar_t*) (pBuffer + iLocalOffset);

        if ( c == 0 )
            break;

        if ( c < 0x100 )
        {
            // Basic Latin, require it to be displayable ascii if in this range
            if ( !IsDisplayableASCII[c] )
                break;
        }
        else
        {
            // Non-basic Latin

            // Require the character to be seen at least once in commoncrawl web scape
            if ( IsSeenCommonCrawl.find( c ) == IsSeenCommonCrawl.end() )
                break; // Invalid unicode character

            // Require it to be in the same character language group of what's already identified
            s32 group_new = GetLanguageGroup( c );

            if ( group_new == 0 )
                break; // Invalid language group

            // Check if it's the same language group as the previous characters
            if ( group == -1 )
                group = group_new;
            else if ( group_new != group )
                break; // Invalid language transition
        }

        iLocalOffset += 2;
        iCharCount++;
    }

    if ( iCharCount >= iMinChars )
    {
        // Return the extracted string
        return new CExtractedString(
            (wchar_t*) (pBuffer + iOffset), iLocalOffset - iOffset, eStringType::WideString, (s32) iOffset, (s32) (iLocalOffset - 2)
        );
    }

    return NULL; // Invalid string at this offset
}

std::tuple<std::string, std::string, std::pair<s32, s32>, bool>
TryExtractStringTuple( const u8* pBuffer, usize iBufferSize, isize iOffset,
                       usize iMinChars, bool only_interesting )
{
    // Simple wrapper to return a tuple instead
    CExtractedString* s = TryExtractString( pBuffer, iBufferSize, iOffset, iMinChars );
    if ( s != NULL )
    {
        bool is_interesting = s->IsInteresting();
        if ( !only_interesting || is_interesting )
        {
            // Create a tuple to return
            auto result = std::make_tuple(
                s->GetString(),
                s->GetTypeString(),
                std::pair<s32, s32>( s->GetOffsetStart(), s->GetOffsetEnd() ),
                is_interesting
            );

            delete s;

            return result;
        }
    }

    return std::make_tuple( "", "", std::make_pair( 0, 0 ), false );
}

std::vector<std::tuple<std::string, std::string, std::pair<s32, s32>, bool>>
ExtractAllStrings( const u8 pBuffer[], usize iBufferSize, usize iMinChars,
                   bool only_interesting )
{
    // Process the specified binary buffer and extract all strings
    isize iOffset = 0;
    std::vector<std::tuple<std::string, std::string, std::pair<s32, s32>, bool>> r_vect;
    std::vector<float> proba_interesting_vect;
    std::vector<float> proba_interesting_avg_vect;
    CExtractedString* s;

    float last_proba_interestings[WINDOW_SIZE] = { 0.0f };

    while ( iOffset + iMinChars <= iBufferSize )
    {
        // Process this offset
        s = TryExtractString( (u8*) pBuffer, iBufferSize, iOffset, iMinChars );

        if ( s )
        {
            f32 proba_interesting = s->GetProbaInteresting();
            // Add the new string
            r_vect.push_back(
                std::tuple<std::string, std::string, std::pair<s32, s32>, bool>(
                    s->GetString(),
                    s->GetTypeString(),
                    std::pair<s32, s32>( s->GetOffsetStart(), s->GetOffsetEnd() ),
                    proba_interesting > 0.5
                )
            );

            // Update the last proba_interesting array
            f32 total = 0.0;

            // Shift the moving window of interesting strings
            for ( int j = WINDOW_SIZE - 2; j >= 0; j-- )
            {
                total += last_proba_interestings[j];
                last_proba_interestings[j + 1] = last_proba_interestings[j];
            }

            // Add the new interesting string value
            last_proba_interestings[0] = proba_interesting;
            total += proba_interesting;
            f32 average = total / (f32) WINDOW_SIZE;
            proba_interesting_avg_vect.push_back( average );
            proba_interesting_vect.push_back( proba_interesting );

            // Advance by the byte-length of the string
            iOffset += (long) s->GetSizeInBytes();

            // Cleanup
            delete s;
        }
        else
        {
            // Advance the offset by 1
            iOffset += 1;
        }
    }

    // Have a pass through the strings averaging the interestingness and filtering
    std::vector<std::tuple<std::string, std::string, std::pair<s32, s32>, bool>> r_vect_filt;

    // gcc-disable: -Wsign-compare
    #if KEN_COMPILER_GCC
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wsign-compare"
    #endif

    for ( int i = 0; i < r_vect.size(); i++ )
    {
        // Get the interestingness
        f32 proba_interesting_avg = 0.0;

        // This string is interesting is the window just before or after is interesting
        proba_interesting_avg = proba_interesting_avg_vect[i];

        if ( i + WINDOW_SIZE < proba_interesting_avg_vect.size() )
        {

        #if KEN_COMPILER_GCC
            #pragma GCC diagnostic pop
        #endif

            if ( proba_interesting_avg_vect[i + WINDOW_SIZE] > proba_interesting_avg )
                proba_interesting_avg = proba_interesting_avg_vect[i + WINDOW_SIZE];
        }

        if ( !only_interesting || proba_interesting_avg >= 0.2 || proba_interesting_vect[i] >= 0.5 )
        {
            r_vect_filt.push_back(
                std::tuple<std::string, std::string, std::pair<s32, s32>, bool>(
                    std::get<0>( r_vect[i] ),
                    std::get<1>( r_vect[i] ),
                    std::get<2>( r_vect[i] ),
                    proba_interesting_avg >= 0.15 || proba_interesting_vect[i] >= 0.5
                )
            );
        }
    }

    return r_vect_filt;
}

NAMESPACE_END_KSTRINGS
