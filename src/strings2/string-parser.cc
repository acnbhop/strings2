//===------------------------------------------------------------------------------------------===//
// strings2/string-parser.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "strings2/core.hh"

// File header
#include "strings2/string-parser.hh"

// kstrings headers
#include "strings2/binary2strings.hh"

NAMESPACE_BEGIN_STRINGS2

bool CStringParser::ParseBlock(u8* pBuffer, u32 iBufferLength, std::string szNameShort,
                               std::string szNameLong, u64 iBaseAddress)
{
    if (pBuffer != NULL && iBufferLength > 0)
    {
        // Process this buffer
        std::vector<std::tuple<std::string, std::string, std::pair<int, int>, bool>> r_vect =
            ExtractAllStrings(pBuffer, iBufferLength, this->m_Options.iMinChars,
                              !this->m_Options.bPrintNotInteresting);

        if (m_Options.bPrintJson)
        {
            // Output the strings to a json file
            nlohmann::json Json;
            Json["name_short"] = szNameShort;
            Json["name_long"] = szNameLong;

            // gcc-disable: -Wsign-compare
            #if AAO_COMPILER_GCC || KEN_COMPILER_APPLE_CLANG
            #   if KEN_COMPILER_APPLE_CLANG
            #       pragma clang diagnostic push
            #       pragma clang diagnostic ignored "-Wsign-compare"
            #   else
            #       pragma GCC diagnostic push
            #       pragma GCC diagnostic ignored "-Wsign-compare"
            #   endif
            #endif

            for (usize i = 0; i < r_vect.size(); i++)
            {
                Json["strings"][i]["string"] = std::get<0>(r_vect[i]);
                Json["strings"][i]["type"] = std::get<1>(r_vect[i]);
                Json["strings"][i]["span"] = {std::get<2>(r_vect[i]).first + iBaseAddress,
                                              std::get<2>(r_vect[i]).second + iBaseAddress};
                Json["strings"][i]["is_interesting"] = std::get<3>(r_vect[i]);
            }
            this->m_Printer->AddJsonString(Json.dump());
        }
        else
        {
            // Iterate through the resulting strings, printing them
            for (int i = 0; i < r_vect.size(); i++)
            {

                #if AAO_COMPILER_GCC || KEN_COMPILER_APPLE_CLANG
                #   if KEN_COMPILER_APPLE_CLANG
                #       pragma clang diagnostic pop
                #   else
                #       pragma GCC diagnostic pop
                #   endif
                #endif

                #if AAO_COMPILER_GCC || KEN_COMPILER_APPLE_CLANG
                #   if KEN_COMPILER_APPLE_CLANG
                #       pragma clang diagnostic push
                #       pragma clang diagnostic ignored "-Wlogical-op-parentheses"
                #   else
                #       pragma GCC diagnostic push
                #       pragma GCC diagnostic ignored "-Wparentheses"
                #   endif
                #endif

                bool is_interesting = std::get<3>(r_vect[i]);
                if (is_interesting && m_Options.bPrintInteresting ||
                    !is_interesting && m_Options.bPrintNotInteresting)
                {

                #if AAO_COMPILER_GCC || KEN_COMPILER_APPLE_CLANG
                #   if KEN_COMPILER_APPLE_CLANG
                #       pragma clang diagnostic pop
                #   else
                #       pragma GCC diagnostic pop
                #   endif
                #endif

                    // Add the prefixes as appropriate
                    if (m_Options.bPrintFilepath)
                        this->m_Printer->AddString(szNameLong + ",");

                    if (m_Options.bPrintFilename)
                        this->m_Printer->AddString(szNameShort + ",");

                    if (m_Options.bPrintStringType)
                        this->m_Printer->AddString(std::get<1>(r_vect[i]) + ",");

                    if (m_Options.bPrintSpan)
                    {
                        std::stringstream span;
                        span << std::hex << "(0x" << (std::get<2>(r_vect[i]).first + iBaseAddress)
                             << ",0x" << (std::get<2>(r_vect[i]).second + iBaseAddress) << "),";
                        this->m_Printer->AddString(span.str());
                    }

                    std::string str = std::get<0>(r_vect[i]);
                    if (m_Options.bEscapeNewLines)
                    {
                        size_t index = 0;
                        while (true)
                        {
                            /* Locate the substring to replace. */
                            index = str.find("\n", index);
                            if (index == std::string::npos)
                                break;

                            /* Make the replacement. */
                            str.replace(index, 1, "\\n");
                            /* Advance index forward so the next iteration
                                doesn't pick it up as well.
                            */
                            index += 2;
                        }

                        index = 0;
                        while (true)
                        {
                            /* Locate the substring to replace. */
                            index = str.find("\r", index);
                            if (index == std::string::npos)
                                break;

                            /* Make the replacement. */
                            str.replace(index, 1, "\\r");
                            /* Advance index forward so the next iteration
                                doesn't pick it up as well.
                            */
                            index += 2;
                        }
                    }

                    this->m_Printer->AddString(str + "\n");
                }
            }
        }
    }
    return false;
}

CStringParser::CStringParser(sStringOptions options)
{
    m_Printer = new CPrintBuffer(0x100000);
    this->m_Options = options;
}

bool CStringParser::ParseStream(FILE* pFileHandle, std::string szNameShort, std::string szNameLong)
{
    if (pFileHandle != NULL)
    {
        u8* pBuffer;
        s32 iNumRead;
        s64 iOffset = 0;

        // Adjust the start offset if specified
        if (m_Options.iOffsetStart > 0)
            fseek(pFileHandle, (s32)m_Options.iOffsetStart, SEEK_SET);

        // Allocate the buffer
        pBuffer = new u8[(usize)iBlockSize];

        do
        {
            // Read the stream in blocks of 0x50000, assuming that a string does not border the
            // regions.
            if (m_Options.iOffsetEnd > 0)
            {
                iNumRead =
                    (s32)fread(pBuffer, 1,
                               strings2::min((usize)iBlockSize, (usize)(m_Options.iOffsetEnd -
                                                                        m_Options.iOffsetStart)),
                               pFileHandle);
            }
            else
            {
                iNumRead = (s32)fread(pBuffer, 1, (usize)iBlockSize, pFileHandle);
            }

            if (iNumRead > 0)
            {
                // We have read in the full contents now, lets process it.
                if (iOffset > 0)
                    this->ParseBlock(pBuffer, iNumRead, szNameShort,
                                     szNameLong + ":offset=" + std::to_string(iOffset), 0);
                else
                    this->ParseBlock(pBuffer, iNumRead, szNameShort, szNameLong, 0);

                iOffset += iNumRead;
            }

            this->m_Printer->Digest();
        } while (iNumRead == (s32)iBlockSize);

        // Clean up
        delete[] pBuffer;
        return true;
    }
    else
    {
        // Failed to open file
        fprintf(stderr, "Invalid stream: %s.\n", strerror(errno));
        return false;
    }
}

CStringParser::~CStringParser(void) { delete m_Printer; }

NAMESPACE_END_STRINGS2
