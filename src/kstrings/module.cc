// Core header
#include "kstrings/core.hh"

// File header
#include "kstrings/module.hh"

__NS_BEGIN_KSTRINGS

bool CModule::Contains(PVOID64 address)
{
    // Check if this module contains the specified address
    return (BYTE*) address >= m_ModuleDetails.modBaseAddr && (BYTE*) address < m_ModuleDetails.modBaseAddr + m_ModuleDetails.modBaseSize;
}

std::string CModule::GetFilepath()
{
    std::wstring ws = m_ModuleDetails.szExePath;

    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.to_bytes(ws);
}

std::string CModule::GetFilename()
{
    std::wstring ws = m_ModuleDetails.szModule;

    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.to_bytes(ws);
}

bool CModule::operator== (const CModule &other) const
{
    return this->m_ModuleDetails.hModule == other.m_ModuleDetails.hModule;
}

CModule::CModule(MODULEENTRY32W details)
{
    m_ModuleDetails = details;
}

CModule::~CModule(void)
{
}

__NS_END_KSTRINGS
