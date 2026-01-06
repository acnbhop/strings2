//===------------------------------------------------------------------------------------------===//
// kstrings/utility.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

// Core header
#include "kstrings/core.hh"

// kstrings headers
#include "kstrings/string-parser.hh"

#if KEN_PLATFORM_WINDOWS
// Check if we are running under WOW64
BOOL IsWin64();
// Get Windows privileges for the current process
bool GetPrivileges( HANDLE hProcess );
#endif

// Check if a filename matches a given filter (supports wildcard * at start)
bool MatchesFilter( const std::string& szFilename, const std::string& szFilter );

// Process a filesystem path (file or directory) for string extraction
void ProcessPath( const std::filesystem::path& fsPath, const std::string& szFilter,
                  bool bRecursively, kstrings::CStringParser* pParser );
