//===------------------------------------------------------------------------------------------===//
// strings2/memory-strings.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

// Core header
#include "strings2/core.hh"

// kstrings headers
#include "strings2/basics.hh"
#include "strings2/module.hh"
#include "strings2/string-parser.hh"

#include <sstream>
#include <string>
#include <vector>

#if AAO_PLATFORM_WINDOWS

NAMESPACE_BEGIN_STRINGS2

// Structure that holds basic MBI information
struct sMBIBasicInfo
{
    u64 iBase;
    u64 iEnd;
    u32 iProtect;
    bool bValid;
    bool bExecutable;
    u64 iSize;
};

// Memory strings dumper class
class CMemoryStrings
{
public:
    // Constructor
    CMemoryStrings(CStringParser* pParser);
    // Dump the process memory strings
    bool DumpProcess(DWORD iPID);
    // Dump the system memory strings
    bool DumpSystem();

    // Destructor
    ~CMemoryStrings(void);
private:
    std::vector<CModule> m_Modules; // List of modules in the process
    CStringParser* m_Parser;        // String parser instance

    // Generate the module list for the process
    void _GenerateModuleList(HANDLE pSnapshot);
    // Process all memory regions in the process
    bool _ProcessAllMemory(HANDLE pHandle, std::string szProcessName);
    // Get basic MBI information for an address
    sMBIBasicInfo _GetMbiInfo(u64 iAddress, HANDLE pHandle);
};

NAMESPACE_END_STRINGS2

#endif // AAO_PLATFORM_WINDOWS
