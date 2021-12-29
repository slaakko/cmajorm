// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_INTERFACE_INCLUDED
#define CMSX_ASSEMBLER_INTERFACE_INCLUDED
#include <system-x/assembler/Api.hpp>
#include <string>

namespace cmsx::assembler {

CMSX_ASSEMBLER_API void Assemble(int logStreamId, const std::string& assemblyFilePath, const std::string& objectFilePath, bool verbose);

} // namespace cmsx::assembler

#endif // CMSX_ASSEMBLER_INTERFACE_INCLUDED
