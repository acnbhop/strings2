//===------------------------------------------------------------------------------------------===//
// kstrings/memory-strings.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

// Core header
#include "kstrings/core.hh"

// kstrings headers
#include "kstrings/module.hh"
#include "kstrings/string-parser.hh"
#include "kstrings/basics.hh"

#include <vector>
#include <string>
#include <sstream>

#if KEN_PLATFORM_WINDOWS

NAMESPACE_BEGIN_KSTRINGS

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
    std::vector<CModule> m_modules;     // List of modules in the process
    CStringParser* m_parser;            // String parser instance

    // Generate the module list for the process
    void _GenerateModuleList(HANDLE hSnapshot);
    // Process all memory regions in the process
    bool _ProcessAllMemory(HANDLE ph, std::string process_name);
    // Get basic MBI information for an address
    sMBIBasicInfo _GetMbiInfo(u64 address, HANDLE ph);
public:
    // Constructor
    CMemoryStrings(CStringParser* parser);
    // Dump the process memory strings
    bool DumpProcess(DWORD pid);
    // Dump the system memory strings
    bool DumpSystem();

    // Destructor
    ~CMemoryStrings(void);
};

NAMESPACE_END_KSTRINGS

#endif // KEN_PLATFORM_WINDOWS
