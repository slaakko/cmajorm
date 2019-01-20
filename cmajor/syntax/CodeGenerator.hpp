// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYNTAX_CODEGENERATOR_INCLUDED
#define CMAJOR_SYNTAX_CODEGENERATOR_INCLUDED

#include <cmajor/syntax/ParserFileContent.hpp>

namespace cmajor { namespace syntax {

void SetForceCodeGen(bool force);
void GenerateCode(const std::vector<std::unique_ptr<ParserFileContent>>& parserFiles);

} } // namespace cmajor::syntax

#endif // CMAJOR_SYNTAX_CODEGENERATOR_INCLUDED
