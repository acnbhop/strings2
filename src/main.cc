//===------------------------------------------------------------------------------------------===//
// main.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "kstrings/core.hh"

// All kstrings headers for the main file
#include "kstrings.hh"

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
    SetConsoleOutputCP( 65001 );
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
    return IsWow64Process( GetCurrentProcess(), &f64 ) && f64;
#else
    return FALSE;
#endif
}

bool GetPrivileges( HANDLE hProcess )
{
    HANDLE h_Token;
    DWORD dw_TokenLength;

    if ( OpenProcessToken( hProcess, TOKEN_READ | TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &h_Token ) )
    {
        TOKEN_PRIVILEGES* privilages = new TOKEN_PRIVILEGES[100];
        if ( GetTokenInformation( h_Token, TokenPrivileges, privilages, sizeof( TOKEN_PRIVILEGES ) * 100, &dw_TokenLength ) )
        {
            for ( kstrings::usize i = 0; i < privilages->PrivilegeCount; i++ )
            {
                privilages->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
            }
            if ( AdjustTokenPrivileges( h_Token, false, privilages, sizeof( TOKEN_PRIVILEGES ) * 100, NULL, NULL ) )
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

bool MatchesFilter( const std::string& szFilename, const std::string& szFilter )
{
    if ( szFilter.empty() || szFilter == "*" )
    {
        return true;
    }

    if ( szFilter.size() > 1 && szFilter[0] == '*' )
    {
        std::string szExt = szFilter.substr( 1 );
        if ( szFilename.length() >= szExt.length() )
        {
            return (0 == szFilename.compare( szFilename.length() - szExt.length(), szExt.length(), szExt ));
        }
        return false;
    }
    // Exact match
    return (szFilename == szFilter);
}

void ProcessPath( const std::filesystem::path& fsPath, const std::string& szFilter,
                  bool bRecursively, kstrings::CStringParser* pParser
)
{
    try
    {
        if ( std::filesystem::is_directory( fsPath ) )
        {
            if ( bRecursively )
            {
                for ( const auto& entry : std::filesystem::recursive_directory_iterator( fsPath ) )
                {
                    if ( entry.is_regular_file() )
                    {
                        std::string szEntryName = entry.path().filename().string();
                        if ( MatchesFilter( szEntryName, szFilter ) )
                        {
                            std::string szFullPath = entry.path().string();
                            FILE* fh = fopen( szFullPath.c_str(), "rb" );
                            if ( fh )
                            {
                                pParser->ParseStream( fh, szEntryName, szFullPath );
                                fclose( fh );
                            }
                        }
                    }
                }
            }
        }
        else if ( std::filesystem::is_regular_file( fsPath ) )
        {
            // If the user explicitly provided a file, we process it directly.
            std::string szFullPath = fsPath.string();
            std::string szFilename = fsPath.filename().string();

            FILE* fh = fopen( szFullPath.c_str(), "rb" );
            if ( fh )
            {
                pParser->ParseStream( fh, szFilename, szFullPath );
                fclose( fh );
            }
        }
    }
    catch ( const std::filesystem::filesystem_error& ex )
    {
        std::cerr << "Filesystem error: " << ex.what() << std::endl;
    }
}

int main( int argc, char* argv[] )
{
    SetConsoleUTF8();

    kstrings::sStringOptions options;
    std::string szFilterArg = "";

    bool bFlagHelp = false;
    bool bFlagDumpPid = false;
    bool bFlagDumpSystem = false;
    bool bFlagRecursive = false;

    // Check for piped input
#if KEN_PLATFORM_WINDOWS
    bool bPipedInput = !_isatty( _fileno( stdin ) );
#else
    bool bPipedInput = !isatty( fileno( stdin ) );
#endif

    if ( argc <= 1 && !bPipedInput ) bFlagHelp = true;

    for ( int i = 1; i < argc; i++ )
    {
        std::string arg = argv[i];

        if ( arg == "--help" || arg == "-help" || arg == "-h" ) bFlagHelp = true;
        else if ( arg == "-f" ) options.bPrintFilename = true;
        else if ( arg == "-F" ) options.bPrintFilepath = true;
        else if ( arg == "-r" ) bFlagRecursive = true;
        else if ( arg == "-t" ) options.bPrintStringType = true;
        else if ( arg == "-s" ) options.bPrintSpan = true;
        else if ( arg == "-e" ) options.bEscapeNewLines = true;
        else if ( arg == "-json" ) options.bPrintJson = true;
        else if ( arg == "-a" )
        {
            options.bPrintInteresting = true;
            options.bPrintNotInteresting = true;
        }
        else if ( arg == "-ni" )
        {
            options.bPrintInteresting = false;
            options.bPrintNotInteresting = true;
        }
        else if ( arg == "-utf" || arg == "-utf8" )
        {
            options.bPrintUTF8 = true;
            options.bPrintWideString = false;
        }
        else if ( arg == "-w" || arg == "-wide" )
        {
            options.bPrintUTF8 = false;
            options.bPrintWideString = true;
        }
        else if ( arg == "-pid" ) bFlagDumpPid = true;
        else if ( arg == "-system" ) bFlagDumpSystem = true;
        else if ( arg == "-l" )
        {
            if ( i + 1 < argc )
            {
                try
                {
                    options.iMinChars = std::stoi( argv[i + 1] );
                    i++;
                }
                catch ( ... ) {}
            }
        }
        else if ( arg == "-b" )
        {
            // Basic implementation of byte range args
            if ( i + 1 < argc )
            {
                // To keep it simple, we just parse the next int. 
                // A full implementation would parse "start:end"
                try
                {
                    options.iOffsetStart = std::stoull( argv[i + 1] );
                    i++;
                }
                catch ( ... ) {}
            }
        }
        else
        {
            if ( szFilterArg.empty() ) szFilterArg = arg;
            else
            {
                std::cerr << "Unknown argument: " << arg << std::endl;
                return 1;
            }
        }
    }

    if ( bFlagHelp )
    {
        printf( "kstrings extracts all unicode/ascii strings from binary data. On top of the classical strings approach, this version decodes multilingual strings (eg Chinese, Russian, etc) and uses a ML model to suppress noisy uninteresting strings.\n\n" );
        printf( "Example Usage:\n" );
        printf( "\tkstrings malware.exe\n" );
        printf( "\tkstrings *.exe > strings.txt\n" );
        printf( "\tkstrings ./files/*.exe > strings.txt\n" );
        printf( "\tkstrings -pid 419 > process_strings.txt\n" );
        printf( "\tkstrings -f -s -pid 0x1a3 > process_strings.txt\n" );
        printf( "\tkstrings -system > all_process_strings.txt\n" );
    #if KEN_PLATFORM_WINDOWS
        printf( "\ttype abcd.exe | kstrings > out.txt\n\n" );
    #else
        printf( "\tcat abcd.exe | kstrings > out.txt\n\n" );
    #endif
        printf( "\tkstrings malware.exe -json > strings.json\n" );
        printf( "Flags:\n" );
        printf( " -r\n\tRecursively process subdirectories.\n" );
        printf( " -f\n\tPrints the filename/processname for each string.\n" );
        printf( " -F\n\tPrints the full path and filename for each string.\n" );
        printf( " -s\n\tPrints the file offset or memory address span\n\tof each string.\n" );
        printf( " -t\n\tPrints the string type for each string. UTF8,\n\tor WIDE_STRING.\n" );
        printf( " -wide\n\tPrints only WIDE_STRING strings that are encoded\n\tas two bytes per character.\n" );
        printf( " -utf\n\tPrints only UTF8 encoded strings.\n" );
        printf( " -a\n\tPrints both interesting and not interesting strings.\n\tDefault only prints interesting non-junk strings.\n" );
        printf( " -ni\n\tPrints only not interesting strings. Default only\n\tprints interesting non-junk strings.\n" );
        printf( " -e\n\tEscape new line characters.\n" );
        printf( " -l [numchars]\n\tMinimum number of characters that is a valid string.\n\tDefault is 4.\n" );
        printf( " -b [start](:[end])\n\tScan only the specified byte range for strings.\n" );
        printf( " -pid [pid]\n\tThe strings from the process address space for the\n\tspecified PID will be dumped. Use a '0x' prefix to\n\tspecify a hex PID. (Windows Only)\n" );
        printf( " -system\n\tDumps strings from all accessible processes on the\n\tsystem. This takes awhile. (Windows Only)\n" );
        printf( " -json\n\tWrites output as json. Many flags are ignored in this mode.\n" );
        return 0;
    }

    kstrings::CStringParser* pParser = new kstrings::CStringParser( options );

    if ( bFlagDumpPid || bFlagDumpSystem )
    {
    #if KEN_PLATFORM_WINDOWS
            // Existing Windows Logic
        if ( IsWin64() && sizeof( void* ) == 4 )
        {
            std::cerr << "WARNING: Running 32-bit binary on 64-bit OS. Process dumping may fail." << std::endl;
        }
        GetPrivileges( GetCurrentProcess() );

        // CMemoryStrings is Windows-specific
        kstrings::CMemoryStrings* pProcess = new kstrings::CMemoryStrings( pParser );

        if ( bFlagDumpPid )
        {
            try
            {
                DWORD pid = std::stoul( szFilterArg );
                pProcess->DumpProcess( pid );
            }
            catch ( ... )
            {
                std::cerr << "Invalid PID specified." << std::endl;
            }
        }
        else if ( bFlagDumpSystem )
        {
            pProcess->DumpSystem();
        }
        delete pProcess;
    #else
        std::cerr << "Error: Process dumping (-pid / -system) is currently only supported on Windows." << std::endl;
    #endif
    }
    else if ( bPipedInput )
    {
    #if KEN_PLATFORM_WINDOWS
        _setmode( _fileno( stdin ), _O_BINARY );
    #endif
    // Use fdopen to get a FILE* from the stdin file descriptor
    #if KEN_PLATFORM_WINDOWS
        FILE* fh = _fdopen( _fileno( stdin ), "rb" );
    #else
        FILE* fh = fdopen( fileno( stdin ), "rb" );
    #endif
        if ( fh )
        {
            pParser->ParseStream( fh, "piped data", "piped data" );
            fclose( fh );
        }
        else
        {
            std::cerr << "Error reading piped input." << std::endl;
        }
    }
    else if ( !szFilterArg.empty() )
    {
        // Handle Paths vs Filters
        std::filesystem::path p( szFilterArg );

        if ( std::filesystem::exists( p ) )
        {
            // It is a real path (file or dir)
            ProcessPath( p, "", bFlagRecursive, pParser );
        }
        else
        {
            // It might be a wildcard pattern like "*.exe" or "C:/bin/*.exe"
            std::filesystem::path parent = p.parent_path();
            if ( parent.empty() ) parent = ".";

            std::string szFilePattern = p.filename().string();
            ProcessPath( parent, szFilePattern, bFlagRecursive, pParser );
        }
    }

    delete pParser;
    return 0;
}
