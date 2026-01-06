// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/binary2strings.hh"

__NS_BEGIN_KSTRINGS

usize TryUTF8CharStep( const unsigned char* buffer, usize buffer_size, isize offset )
{
    // Returns 0 if it's not likely a valid utf8 character. For ascii range of characters it requires
    // the character to be a displayable character.
    if ( buffer_size < (usize) offset + 1 )
        return 0;

    unsigned char first_byte = buffer[offset];

    if ( first_byte < 0x80 )
    {
        if ( IsDisplayableASCII[first_byte] )
            return 1;
        return 0;
    }

    if ( first_byte < 0xC2 )
        return 0; // Ignore continuation bytes, they are non displayable.

    if ( first_byte > 0xF4 )
        return 0; // Invalid bytes for Utf8, https://en.wikipedia.org/wiki/UTF-8#Invalid_byte_sequences

    if ( first_byte < 0xE0 )
    {
        // 2 byte encoding
        if ( buffer_size < (usize) offset + 2 )
            return 0;

        if ( (buffer[offset + 1] & 0xC0) != 0x80 )
            return 0;

        // Now convert the character to wchar_t
        wchar_t c = 0;
        c |= (first_byte & 0x1F) << 6;
        c |= (buffer[offset + 1] & 0x3F);

        // Require the character to be seen at least once in commoncrawl web scape
        if ( IsSeenCommonCrawl.find( c ) == IsSeenCommonCrawl.end() )
            return 0; // Invalid unicode character

        return 2;
    }

    if ( first_byte < 0xF0 )
    {
        // 3 byte encoding
        if ( buffer_size < (usize) offset + 3 )
            return 0;

        if ( (buffer[offset + 1] & 0xC0) != 0x80 )
            return 0;

        if ( (buffer[offset + 2] & 0xC0) != 0x80 )
            return 0;

        // Now convert the character to wchar_t
        wchar_t c = 0;
        c |= (first_byte & 0x0F) << 12;
        c |= (buffer[offset + 1] & 0x3F) << 6;
        c |= (buffer[offset + 2] & 0x3F);

        // Check special cases
        if ( first_byte == 0xE0 )
        {
            // Check for overlong case
            if ( c < 0x800 )
                return 0; // Overlong
        }
        else if ( first_byte == 0xED )
        {
            // Check for surrogate pair case
            if ( c >= 0xD800 && c <= 0xDFFF )
                return 0;
        }

        // Require the character to be seen at least once in commoncrawl web scape
        if ( IsSeenCommonCrawl.find( c ) == IsSeenCommonCrawl.end() )
            return 0; // Invalid unicode character

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

// Note: Buffer overrun security checks disabled, since they added ~50% overhead.
#if KEN_PLATFORM_WINDOWS
__declspec(safebuffers)
#endif
CExtractedString* TryExtractString( const unsigned char* buffer,
                                    usize buffer_size, isize offset, usize min_chars )
{
    // Try extracting the string as either utf8 or unicode wchar format. 
    // Returns None if it's not a valid string.
    usize i;
    usize char_count;

    // Try to parse as utf8 first
    usize utf_char_len;
    utf_char_len = TryUTF8CharStep( buffer, buffer_size, offset );

    if ( utf_char_len > 0 )
    {
        i = offset;
        char_count = 0;
        while ( i < buffer_size && utf_char_len > 0 )
        {
            i += utf_char_len;
            char_count++;

            // Try parse next character
            utf_char_len = TryUTF8CharStep( buffer, buffer_size, i );
        }

        if ( char_count >= min_chars )
        {
            // Return the extracted string
            return new CExtractedString(
                (char*) (buffer + offset), i - offset, TYPE_UTF8, (s32) offset, (s32) (i - 1)
            );
        }

        // Not a valid utf8 string, try a unicode string parse still
    }

    // Try extract as unicode wchar
    // Unicode logic requires all characters to be in the same BMP block or in the BMP Basic Latin
    // Block. (eg any single BMP block plus latin in the same string is OK)

    // The non-basic-latin language group identified
    int group = -1;
    i = offset;
    char_count = 0;

    // Parse as unicode
    while ( i + 1 < buffer_size )
    {
        wchar_t c = *(wchar_t*) (buffer + i);

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

        i += 2;
        char_count++;
    }

    if ( char_count >= min_chars )
    {
        // Return the extracted string
        return new CExtractedString(
            (wchar_t*) (buffer + offset), i - offset, TYPE_WIDE_STRING, (s32) offset, (s32) (i - 2)
        );
    }

    return NULL; // Invalid string at this offset
}

std::tuple<std::string, std::string, std::pair<int, int>, bool>
TryExtractStringTuple( const unsigned char* buffer, usize buffer_size, isize offset,
                       usize min_chars, bool only_interesting )
{
    // Simple wrapper to return a tuple instead
    CExtractedString* s = TryExtractString( buffer, buffer_size, offset, min_chars );
    if ( s != NULL )
    {
        bool is_interesting = s->IsInteresting();
        if ( !only_interesting || is_interesting )
        {
            // Create a tuple to return
            auto result = std::make_tuple(
                s->GetString(),
                s->GetTypeString(),
                std::pair<int, int>( s->GetOffsetStart(), s->GetOffsetEnd() ),
                is_interesting
            );

            delete s;

            return result;
        }
    }

    return std::make_tuple( "", "", std::make_pair( 0, 0 ), false );
}


std::vector<std::tuple<std::string, std::string, std::pair<int, int>, bool>>
ExtractAllStrings( const unsigned char buffer[], usize buffer_size, usize min_chars,
                   bool only_interesting )
{
    // Process the specified binary buffer and extract all strings
    isize offset = 0;
    std::vector<std::tuple<std::string, std::string, std::pair<int, int>, bool>> r_vect;
    std::vector<float> proba_interesting_vect;
    std::vector<float> proba_interesting_avg_vect;
    CExtractedString* s;

    float last_proba_interestings[WINDOW_SIZE] = { 0.0f };

    while ( offset + min_chars <= buffer_size )
    {
        // Process this offset
        s = TryExtractString( (unsigned char*) buffer, buffer_size, offset, min_chars );

        if ( s )
        {
            f32 proba_interesting = s->GetProbaInteresting();
            // Add the new string
            r_vect.push_back(
                std::tuple<std::string, std::string, std::pair<int, int>, bool>(
                    s->GetString(),
                    s->GetTypeString(),
                    std::pair<int, int>( s->GetOffsetStart(), s->GetOffsetEnd() ),
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
            offset += (long) s->GetSizeInBytes();

            // Cleanup
            delete s;
        }
        else
        {
      // Advance the offset by 1
            offset += 1;
        }
    }

    // Have a pass through the strings averaging the interestingness and filtering
    std::vector<std::tuple<std::string, std::string, std::pair<int, int>, bool>> r_vect_filt;
    for ( int i = 0; i < r_vect.size(); i++ )
    {
        // Get the interestingness
        f32 proba_interesting_avg = 0.0;

        // This string is interesting is the window just before or after is interesting
        proba_interesting_avg = proba_interesting_avg_vect[i];

        if ( i + WINDOW_SIZE < proba_interesting_avg_vect.size() )
        {
            if ( proba_interesting_avg_vect[i + WINDOW_SIZE] > proba_interesting_avg )
                proba_interesting_avg = proba_interesting_avg_vect[i + WINDOW_SIZE];
        }

        if ( !only_interesting || proba_interesting_avg >= 0.2 || proba_interesting_vect[i] >= 0.5 )
        {
            r_vect_filt.push_back(
                std::tuple<std::string, std::string, std::pair<int, int>, bool>(
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

__NS_END_KSTRINGS