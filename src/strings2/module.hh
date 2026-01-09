//===------------------------------------------------------------------------------------------===//
// strings2/module.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

// Core header
#include "strings2/core.hh"

#if AAO_PLATFORM_WINDOWS

NAMESPACE_BEGIN_STRINGS2

class CModule
{
    // Detailed module information
    MODULEENTRY32W m_ModuleDetails;

public:
    // Takes u64 to match the address type used in memory-strings.cc
    bool Contains(u64 iAddress) const;

    // Gets the module filepath
    std::string GetFilepath() const;
    // Gets the module filename
    std::string GetFilename() const;

    // Constructor
    CModule(MODULEENTRY32W Details);
    // Destructor
    ~CModule(void);

    bool operator==(const CModule& other) const;
};

NAMESPACE_END_STRINGS2

#endif // AAO_PLATFORM_WINDOWS
