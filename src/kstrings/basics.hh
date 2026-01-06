//===------------------------------------------------------------------------------------------===//
// kstrings/basics.hh
//===------------------------------------------------------------------------------------------===//
#pragma once

// Core header
#include "kstrings/core.hh"

// Standard headers
#include <string>

NAMESPACE_BEGIN_KSTRINGS

// Print the last error message from the system
void PrintLastError(const std::string& szMsg);

NAMESPACE_END_KSTRINGS
