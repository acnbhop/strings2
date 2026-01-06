//===------------------------------------------------------------------------------------------===//
// kstrings/memory-strings.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/memory-strings.hh"

#if KEN_PLATFORM_WINDOWS

#include <iostream>

NAMESPACE_BEGIN_KSTRINGS

// Local helper for this translation unit
static bool LocalIsWin64( HANDLE process )
{
    BOOL ret_val;
    if ( IsWow64Process( process, &ret_val ) )
    {
        return ret_val == TRUE;
    }
    PrintLastError( "IsWow64Process" );
    return false;
}

CMemoryStrings::CMemoryStrings( CStringParser* parser )
{
    this->m_parser = parser;
}


CMemoryStrings::~CMemoryStrings( void )
{}

bool CMemoryStrings::DumpSystem()
{
    // Enumerate processes, and process the strings from each one
    HANDLE h_snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

    if ( h_snapshot != INVALID_HANDLE_VALUE )
    {
        // Handle the first process
        PROCESSENTRY32 tmp_p;
        tmp_p.dwSize = sizeof( PROCESSENTRY32 );

        if ( Process32First( h_snapshot, &tmp_p ) )
        {
            DumpProcess( tmp_p.th32ProcessID );

            while ( Process32Next( h_snapshot, &tmp_p ) )
            {
                DumpProcess( tmp_p.th32ProcessID );
            }
        }

        // Cleanup the handle
        CloseHandle( h_snapshot );
        return true;
    }
    return false;
}

bool CMemoryStrings::DumpProcess( DWORD pid )
{
    // Open the process
    HANDLE ph = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid );
    if ( ph != NULL )
    {
        // Assign the process name
        char process_name[0x100] = { 0 };
        GetModuleBaseNameA( ph, 0, process_name, 0x100 );

        // Generate memory region list
        HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, pid );
        if ( hSnapshot != INVALID_HANDLE_VALUE )
        {
            this->_GenerateModuleList( hSnapshot );
            CloseHandle( hSnapshot );

            // Walk through the process regions, extracting the strings
            bool result = this->_ProcessAllMemory( ph, process_name );
            CloseHandle( ph ); // Don't forget to close the process handle!

            return result;
        }
        else
        {
            fprintf( stderr, "Failed to gather module information for process 0x%x (%lu). ", pid, pid );
            PrintLastError( "DumpProcess (CreateToolhelp32Snapshot)" );
        }
        CloseHandle( ph );
    }
    else
    {
        fprintf( stderr, "Failed to open process 0x%x (%lu). ", pid, pid );
        PrintLastError( "DumpProcess (OpenProcess)" );
    }
    return false;
}

sMBIBasicInfo CMemoryStrings::_GetMbiInfo( u64 address, HANDLE ph )
{
    _MEMORY_BASIC_INFORMATION64 mbi;
    sMBIBasicInfo result;
    result.iBase = 0;
    result.iEnd = 0;
    result.iProtect = 0;
    result.bValid = false;
    result.bExecutable = false;
    result.iSize = 0;

    // Load this heap information
    SIZE_T blockSize = VirtualQueryEx( ph, (LPCVOID) address, (PMEMORY_BASIC_INFORMATION) &mbi, sizeof( _MEMORY_BASIC_INFORMATION64 ) );

    if ( blockSize == sizeof( _MEMORY_BASIC_INFORMATION64 ) )
    {
        result.iBase = mbi.BaseAddress;
        result.iEnd = mbi.BaseAddress + mbi.RegionSize;
        result.iProtect = mbi.Protect;
        result.bValid = (mbi.State != MEM_FREE) && !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD));
        result.bExecutable = (mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) > 0;
        result.iSize = mbi.RegionSize;
    }
    else
    {
        // Fallback for 32-bit queries if needed, or partial reads
        _MEMORY_BASIC_INFORMATION32 mbi32;
        if ( VirtualQueryEx( ph, (LPCVOID) address, (PMEMORY_BASIC_INFORMATION) &mbi32, sizeof( _MEMORY_BASIC_INFORMATION32 ) ) == sizeof( _MEMORY_BASIC_INFORMATION32 ) )
        {
            result.iBase = mbi32.BaseAddress;
            result.iEnd = (u64) mbi32.BaseAddress + (u64) mbi32.RegionSize;
            result.iProtect = mbi32.Protect;
            result.bValid = (mbi32.State != MEM_FREE) && !(mbi32.Protect & (PAGE_NOACCESS | PAGE_GUARD));
            result.bExecutable = (mbi32.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) > 0;
            result.iSize = mbi32.RegionSize;
        }
    }

    return result;
}

bool CMemoryStrings::_ProcessAllMemory( HANDLE ph, std::string process_name )
{
    // Set the max address of the target process. Assume it is a 64 bit process.
    u64 max_address = 0xffffffffffffffff;

    // Walk the process heaps
    u64 address = 0;

    while ( address < max_address )
    {
        // Load this region information
        sMBIBasicInfo mbi_info = _GetMbiInfo( address, ph );

        if ( mbi_info.iEnd <= address ) // prevent infinite loop if size is 0 or wrap around
            break;

        u64 next_address = mbi_info.iEnd;

        if ( mbi_info.bValid && mbi_info.iSize > 0 )
        {
            // Read in the region
            unsigned char* buffer = new unsigned char[mbi_info.iSize];
            if ( buffer != NULL )
            {
                SIZE_T num_read = 0;
                BOOL result = ReadProcessMemory( ph, (LPCVOID) mbi_info.iBase, buffer, mbi_info.iSize, &num_read );

                if ( result && num_read > 0 )
                {
                    // Load the module name if applicable
                    std::string module_name_short = "region";
                    std::string module_name_long = "region";

                    for ( const auto& mod : m_modules )
                    {
                        if ( mod.Contains( (u64) mbi_info.iBase ) )
                        {
                            module_name_short = mod.GetFilename();
                            module_name_long = mod.GetFilepath();
                            break;
                        }
                    }

                    // Print the strings from this region
                    std::stringstream long_name;
                    long_name << process_name << ":" << module_name_long << "@0x" << std::hex << mbi_info.iBase;
                    std::stringstream short_name;
                    short_name << process_name << ":" << module_name_short << "@0x" << std::hex << mbi_info.iBase;

                    m_parser->ParseBlock( buffer, (u32) num_read, short_name.str(), long_name.str(), mbi_info.iBase );
                }
                else
                {
                    // Optional: Verbose logging for unreadable regions
                    // fprintf(stderr,"Failed read region 0x%llx: %s\n", mbi_info.iBase, strerror(errno));
                }

                // Cleanup
                delete[] buffer;
            }
            else
            {
                fprintf( stderr, "Failed to allocate space of %llu for reading in a region.\n", mbi_info.iSize );
            }
        }

        address = next_address;
    }

    return true;
}

void CMemoryStrings::_GenerateModuleList( HANDLE hSnapshot )
{
    MODULEENTRY32W tmp_m;
    tmp_m.dwSize = sizeof( MODULEENTRY32W );

    if ( Module32FirstW( hSnapshot, &tmp_m ) )
    {
        m_modules.push_back( CModule( tmp_m ) );

        while ( Module32NextW( hSnapshot, &tmp_m ) )
        {
            m_modules.push_back( CModule( tmp_m ) );
        }
    }
}

NAMESPACE_END_KSTRINGS

#endif // KEN_PLATFORM_WINDOWS
