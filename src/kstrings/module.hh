#pragma once

// Core header
#include "kstrings/core.hh"

#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

__NS_BEGIN_KSTRINGS

class CModule
{
    MODULEENTRY32W m_ModuleDetails;
public:
    bool Contains(PVOID64 address);
    std::string GetFilepath();
    std::string GetFilename();

    CModule(MODULEENTRY32W details);
    ~CModule(void);
    bool operator== (const CModule &other) const;
};

__NS_END_KSTRINGS
