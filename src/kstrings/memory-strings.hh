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

__NS_BEGIN_KSTRINGS

struct MBI_BASIC_INFO
{
    u64 iBase;
    u64 iEnd;
    u32 iProtect;
    bool bValid;
    bool bExecutable;
    u64 iSize;
};

class CMemoryStrings
{
    std::vector<CModule> m_modules;
    CStringParser* m_parser;

    void _GenerateModuleList(HANDLE hSnapshot);
    bool _ProcessAllMemory(HANDLE ph, std::string process_name);
    MBI_BASIC_INFO _GetMbiInfo(unsigned __int64 address, HANDLE ph);
public:
    CMemoryStrings(CStringParser* parser);
    bool DumpProcess(DWORD pid);
    bool DumpSystem();

    ~CMemoryStrings(void);
};

__NS_END_KSTRINGS

#endif // KEN_PLATFORM_WINDOWS
