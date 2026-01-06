// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/basics.hh"

#if !KEN_PLATFORM_WINDOWS
#include <cerrno>
#endif

__NS_BEGIN_KSTRINGS

void PrintLastError( const std::string& msg )
{
#if KEN_PLATFORM_WINDOWS
    // --- Windows Implementation ---

    // Retrieve the system error message for the last-error code
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();

    // Use FormatMessageA (ANSI) to play nice with std::string and UTF-8
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        (LPSTR) &lpMsgBuf,
        0, NULL );

    if ( lpMsgBuf )
    {
        // Strip newlines that FormatMessage adds
        std::string sysMsg = (char*) lpMsgBuf;
        if ( !sysMsg.empty() && sysMsg.back() == '\n' ) sysMsg.pop_back();
        if ( !sysMsg.empty() && sysMsg.back() == '\r' ) sysMsg.pop_back();

        std::cerr << msg << " failed with error " << dw << ": " << sysMsg << std::endl;

        LocalFree( lpMsgBuf );
    }
    else
    {
        std::cerr << msg << " failed with error " << dw << " (No system message found)" << std::endl;
    }

#else
    // --- Linux / macOS / BSD Implementation ---

    // Use standard errno
    // copy errno locally in case streaming to cerr resets it (unlikely but safe)
    int err = errno;
    std::cerr << msg << " failed: " << std::strerror( err ) << " (" << err << ")" << std::endl;

#endif
}

__NS_END_KSTRINGS
