#pragma once

// Core header
#include "kstrings/core.hh"

#if KEN_PLATFORM_WINDOWS

__NS_BEGIN_KSTRINGS

class CModule
{
    MODULEENTRY32W m_ModuleDetails;
public:
    // Takes u64 to match the address type used in memory-strings.cc
    bool Contains(u64 address) const;
    
    std::string GetFilepath() const;
    std::string GetFilename() const;

    CModule(MODULEENTRY32W details);
    ~CModule(void);
    
    bool operator== (const CModule &other) const;
};

__NS_END_KSTRINGS

#endif // KEN_PLATFORM_WINDOWS
