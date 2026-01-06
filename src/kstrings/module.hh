//===------------------------------------------------------------------------------------------===//
// kstrings/module.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

// Core header
#include "kstrings/core.hh"

#if KEN_PLATFORM_WINDOWS

__NS_BEGIN_KSTRINGS

class CModule
{
    // Detailed module information
    MODULEENTRY32W m_ModuleDetails;
public:
    // Takes u64 to match the address type used in memory-strings.cc
    bool Contains(u64 address) const;
    
    // Gets the module filepath
    std::string GetFilepath() const;
    // Gets the module filename
    std::string GetFilename() const;

    // Constructor
    CModule(MODULEENTRY32W details);
    // Destructor
    ~CModule(void);
    
    bool operator== (const CModule &other) const;
};

__NS_END_KSTRINGS

#endif // KEN_PLATFORM_WINDOWS
