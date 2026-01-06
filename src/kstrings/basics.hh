#pragma once

// Core header
#include "kstrings/core.hh"

// Standard headers
#include <string>

__NS_BEGIN_KSTRINGS

// Windows: Prints GetLastError() and FormatMessage
// Linux/Mac: Prints errno and strerror()
void PrintLastError( const std::string& msg );

__NS_END_KSTRINGS