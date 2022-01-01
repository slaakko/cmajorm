// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/Interface.hpp>
#include <system-x/assembler/Assembler.hpp>
#include <system-x/assembler/AssemblyLexer.hpp>
#include <system-x/assembler/AssemblyParser.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Log.hpp>
#include <iostream>

namespace cmsx::assembler {

using namespace soulng::unicode;
using namespace soulng::util;

void Assemble(int logStreamId, const std::string& assemblyFilePath, const std::string& objectFilePath, bool verbose)
{
    if (verbose)
    {
        LogMessage(logStreamId, "> " + assemblyFilePath);
    }
    std::u32string assemblyContent = ToUtf32(ReadFile(assemblyFilePath));
    AssemblyLexer lexer(assemblyContent, assemblyFilePath, 0);
    Assembler assembler(&lexer, assemblyFilePath, objectFilePath);
    AssemblyParser::Parse(lexer, &assembler);
    assembler.Assemble();
    if (verbose)
    {
        LogMessage(logStreamId, "==> " + objectFilePath);
    }
}

} // namespace cmsx::assembler
