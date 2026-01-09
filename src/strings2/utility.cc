//===------------------------------------------------------------------------------------------===//
// strings2/utility.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "strings2/core.hh"

// File header
#include "strings2/utility.hh"

#if AAO_PLATFORM_WINDOWS
// Check if we are running under WOW64
BOOL IsWin64()
{
#if defined(_WIN64)
    return TRUE;
#elif defined(_WIN32)
    BOOL f64 = FALSE;
    return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
    return FALSE;
#endif
}

NAMESPACE_BEGIN_STRINGS2

// Get Windows privileges for the current process
bool GetPrivileges(HANDLE hProcess)
{
    HANDLE p_hToken;
    DWORD iTokenLength = 0;

    if (OpenProcessToken(hProcess,                                           // Process handle
                         TOKEN_READ | TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, // Desired access
                         &p_hToken                                           // Token handle
                         ))
    {
        TOKEN_PRIVILEGES* privilages = new TOKEN_PRIVILEGES[100];
        if (GetTokenInformation(p_hToken,                       // Token handle
                                TokenPrivileges,                // Token information type
                                privilages,                     // Buffer for token information
                                sizeof(TOKEN_PRIVILEGES) * 100, // Size of buffer
                                &iTokenLength // Size needed if buffer is too small
                                ))
        {
            for (usize i = 0; i < privilages->PrivilegeCount; i++)
            {
                privilages->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
            }
            if (AdjustTokenPrivileges(p_hToken,                       // Token handle
                                      false,                          // Disable all privileges
                                      privilages,                     // New state
                                      sizeof(TOKEN_PRIVILEGES) * 100, // Size of buffer
                                      NULL,                           // Previous state
                                      NULL)) // Return size of previous state
            {
                delete[] privilages;
                return true;
            }
        }
        delete[] privilages;
    }
    return false;
}

#endif // AAO_PLATFORM_WINDOWS

bool MatchesFilter(const std::string& szFilename, const std::string& szFilter)
{
    if (szFilter.empty() || szFilter == "*")
    {
        return true;
    }

    if (szFilter.size() > 1 && szFilter[0] == '*')
    {
        std::string szExt = szFilter.substr(1);
        if (szFilename.length() >= szExt.length())
        {
            return (0 == szFilename.compare(
                             szFilename.length() - szExt.length(), // start position to compare
                             szExt.length(),                       // length to compare
                             szExt                                 // the extension to match
                             ));
        }
        return false;
    }
    // Exact match
    return (szFilename == szFilter);
}

void ProcessPath(const std::filesystem::path& fsPath, const std::string& szFilter,
                 bool bRecursively, CStringParser* pParser)
{
    try
    {
        if (std::filesystem::is_directory(fsPath))
        {
            if (bRecursively)
            {
                for (const auto& entry : std::filesystem::recursive_directory_iterator(fsPath))
                {
                    if (entry.is_regular_file())
                    {
                        std::string szEntryName = entry.path().filename().string();
                        if (MatchesFilter(szEntryName, szFilter))
                        {
                            std::string szFullPath = entry.path().string();
                            FILE* fh = fopen(szFullPath.c_str(), "rb");
                            if (fh)
                            {
                                pParser->ParseStream(fh, szEntryName, szFullPath);
                                fclose(fh);
                            }
                        }
                    }
                }
            }
        }
        else if (std::filesystem::is_regular_file(fsPath))
        {
            // If the user explicitly provided a file, we process it directly.
            std::string szFullPath = fsPath.string();
            std::string szFilename = fsPath.filename().string();

            FILE* fh = fopen(szFullPath.c_str(), "rb");
            if (fh)
            {
                pParser->ParseStream(fh, szFilename, szFullPath);
                fclose(fh);
            }
        }
    }
    catch (const std::filesystem::filesystem_error& ex)
    {
        std::cerr << "Filesystem error: " << ex.what() << std::endl;
    }
}

NAMESPACE_END_STRINGS2
