//===------------------------------------------------------------------------------------------===//
// kstrings/basics.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/basics.hh"

#if !KEN_PLATFORM_WINDOWS
#include <cerrno>
#endif

NAMESPACE_BEGIN_KSTRINGS

void PrintLastError( const std::string& szMsg )
{
#if KEN_PLATFORM_WINDOWS
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

        std::cerr << szMsg << " failed with error " << dw << ": " << sysMsg << std::endl;

        LocalFree( lpMsgBuf );
    }
    else
    {
        std::cerr << szMsg << " failed with error " << dw << " (No system message found)" << std::endl;
    }
#else
    //
    // Use standard errno
    //
    // copy errno locally in case streaming to cerr resets it (unlikely but safe)
    //
    s32 err = errno;
    std::cerr << szMsg << " failed: " << std::strerror( err ) << " (" << err << ")" << std::endl;

#endif
}

NAMESPACE_END_KSTRINGS
