//===------------------------------------------------------------------------------------------===//
// kstrings/string-parser.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/string-parser.hh"

// kstrings headers
#include "kstrings/binary2strings.hh"

NAMESPACE_BEGIN_KSTRINGS

bool CStringParser::ParseBlock( unsigned char* buffer, u32 buffer_length, std::string name_short, std::string name_long, u64 base_address )
{
    if ( buffer != NULL && buffer_length > 0 )
    {
        // Process this buffer
        std::vector<std::tuple<std::string, std::string, std::pair<int, int>, bool>> r_vect = 
            ExtractAllStrings( buffer, buffer_length, this->m_Options.iMinChars, 
                !this->m_Options.bPrintNotInteresting );

        if ( m_Options.bPrintJson )
        {
            // Output the strings to a json file
            nlohmann::json Json;
            Json["name_short"] = name_short;
            Json["name_long"] = name_long;
            for ( int i = 0; i < r_vect.size(); i++ )
            {
                Json["strings"][i]["string"] = std::get<0>( r_vect[i] );
                Json["strings"][i]["type"] = std::get<1>( r_vect[i] );
                Json["strings"][i]["span"] = { std::get<2>( r_vect[i] ).first + base_address, 
                    std::get<2>( r_vect[i] ).second + base_address };
                Json["strings"][i]["is_interesting"] = std::get<3>( r_vect[i] );
            }
            this->m_Printer->AddJsonString( Json.dump() );
        }
        else
        {
            // Iterate through the resulting strings, printing them
            for ( int i = 0; i < r_vect.size(); i++ )
            {
                bool is_interesting = std::get<3>( r_vect[i] );
                if ( is_interesting && m_Options.bPrintInteresting ||
                     !is_interesting && m_Options.bPrintNotInteresting )
                {
                    // Add the prefixes as appropriate
                    if ( m_Options.bPrintFilepath )
                        this->m_Printer->AddString( name_long + "," );

                    if ( m_Options.bPrintFilename )
                        this->m_Printer->AddString( name_short + "," );

                    if ( m_Options.bPrintStringType )
                        this->m_Printer->AddString( std::get<1>( r_vect[i] ) + "," );

                    if ( m_Options.bPrintSpan )
                    {
                        std::stringstream span;
                        span << std::hex << "(0x" << (std::get<2>
                            ( r_vect[i] ).first + base_address) << ",0x" << (std::get<2>
                                ( r_vect[i] ).second + base_address) << "),";
                        this->m_Printer->AddString( span.str() );
                    }

                    std::string str = std::get<0>( r_vect[i] );
                    if ( m_Options.bEscapeNewLines )
                    {
                        size_t index = 0;
                        while ( true )
                        {
                            /* Locate the substring to replace. */
                            index = str.find( "\n", index );
                            if ( index == std::string::npos ) break;

                            /* Make the replacement. */
                            str.replace( index, 1, "\\n" );
                            /* Advance index forward so the next iteration 
                                doesn't pick it up as well. 
                            */
                            index += 2;
                        }

                        index = 0;
                        while ( true )
                        {
                            /* Locate the substring to replace. */
                            index = str.find( "\r", index );
                            if ( index == std::string::npos ) break;

                            /* Make the replacement. */
                            str.replace( index, 1, "\\r" );
                            /* Advance index forward so the next iteration 
                                doesn't pick it up as well. 
                            */
                            index += 2;
                        }
                    }

                    this->m_Printer->AddString( str + "\n" );
                }
            }
        }
    }
    return false;
}

CStringParser::CStringParser( sStringOptions options )
{
    m_Printer = new CPrintBuffer( 0x100000 );
    this->m_Options = options;
}

bool CStringParser::ParseStream( FILE* fh, std::string name_short, std::string name_long )
{
    if ( fh != NULL )
    {
        unsigned char* buffer;
        s32 num_read;
        s64 offset = 0;

        // Adjust the start offset if specified
        if ( m_Options.iOffsetStart > 0 )
            fseek( fh, (s32) m_Options.iOffsetStart, SEEK_SET );

        // Allocate the buffer
        buffer = new unsigned char[(usize) iBlockSize];

        do
        {
            // Read the stream in blocks of 0x50000, assuming that a string does not border the regions.
            if ( m_Options.iOffsetEnd > 0 )
            {
                num_read = (s32) fread( buffer, 1, min( (usize) iBlockSize, (usize) (m_Options.iOffsetEnd - m_Options.iOffsetStart) ), fh );
            }
            else
            {
                num_read = (s32) fread( buffer, 1, (usize) iBlockSize, fh );
            }


            if ( num_read > 0 )
            {
                // We have read in the full contents now, lets process it.
                if ( offset > 0 )
                    this->ParseBlock( buffer, num_read, name_short, name_long + ":offset=" + std::to_string( offset ), 0 );
                else
                    this->ParseBlock( buffer, num_read, name_short, name_long, 0 );

                offset += num_read;
            }

            this->m_Printer->Digest();
        }
        while ( num_read == (s32) iBlockSize );

        // Clean up
        delete[] buffer;
        return true;
    }
    else
    {
        // Failed to open file
        fprintf( stderr, "Invalid stream: %s.\n", strerror( errno ) );
        return false;
    }
}

CStringParser::~CStringParser( void )
{
    delete m_Printer;
}

NAMESPACE_END_KSTRINGS
