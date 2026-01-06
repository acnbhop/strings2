//
// main.cc
//

// Core header
#include "kstrings/core.hh"

// kstrings headers
#include "kstrings/string-parser.hh"
#include "kstrings/memory-strings.hh"

// Standard headers
#include <vector>
#include <cstring>
#include <algorithm>

#if KEN_PLATFORM_WINDOWS
#include <io.h>
#include <fcntl.h>
// Define a helper to set the console output to UTF-8 on windows.
void SetConsoleUTF8()
{
    SetConsoleOutputCP(65001);
}
#else
#include <unistd.h>
// Standard terminals usually handle this for us.
void SetConsoleUTF8() {}
#endif

#if KEN_PLATFORM_WINDOWS
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

bool GetPrivileges(HANDLE hProcess)
{
    HANDLE h_Token;
    DWORD dw_TokenLength;

    if(OpenProcessToken(hProcess, TOKEN_READ | TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES , &h_Token)) 
    {
        TOKEN_PRIVILEGES* privilages = new TOKEN_PRIVILEGES[100];
        if( GetTokenInformation(h_Token, TokenPrivileges, privilages,sizeof(TOKEN_PRIVILEGES)*100,&dw_TokenLength) ) 
        {
            for ( kstrings::usize i = 0; i < privilages->PrivilegeCount; i++ ) 
            {
                privilages->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
            }
            if (AdjustTokenPrivileges( h_Token, false, privilages, sizeof(TOKEN_PRIVILEGES)*100, NULL, NULL  )) 
            {
                delete[] privilages;
                return true;
            }
        }
        delete[] privilages;
    }
    return false;
}

#endif // KEN_PLATFORM_WINDOWS

bool MatchesFilter(const string& szFilename, const string& szFilter)
{
    if (szFilter.empty() || szFilter == "*") return true;
}
