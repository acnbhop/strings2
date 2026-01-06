//===------------------------------------------------------------------------------------------===//
// kstrings/module.cc
//===------------------------------------------------------------------------------------------===//

// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/module.hh"

#if KEN_PLATFORM_WINDOWS

#include <filesystem>

NAMESPACE_BEGIN_KSTRINGS

bool CModule::Contains(u64 iAddress) const
{
    // Check if this module contains the specified address
    // Convert pointers to u64 for safe comparison
    u64 iBase = (u64)m_ModuleDetails.modBaseAddr;
    u64 iSize = (u64)m_ModuleDetails.modBaseSize;

    return (iAddress >= iBase) && (iAddress < iBase + iSize);
}

std::string CModule::GetFilepath() const
{
    // Use std::filesystem for robust Wide -> UTF8 string conversion
    // This avoids std::codecvt deprecation warnings
    try
    {
        return std::filesystem::path(m_ModuleDetails.szExePath).string();
    }
    catch (...)
    {
        return "";
    }
}

std::string CModule::GetFilename() const
{
    try
    {
        return std::filesystem::path(m_ModuleDetails.szModule).string();
    }
    catch (...)
    {
        return "";
    }
}

bool CModule::operator==(const CModule& other) const
{
    return this->m_ModuleDetails.hModule == other.m_ModuleDetails.hModule;
}

CModule::CModule(MODULEENTRY32W details) { m_ModuleDetails = details; }

CModule::~CModule(void) {}

NAMESPACE_END_KSTRINGS

#endif // KEN_PLATFORM_WINDOWS
