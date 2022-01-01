// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Parser.hpp>
#include <system-x/intermediate/IntermediateCodeLexer.hpp>
#include <system-x/intermediate/IntermediateCodeRules.hpp>
#include <system-x/intermediate/IntermediateCodeParser.hpp>

namespace cmsx::intermediate {

void Parse(const std::string& filePath, Context& context)
{
    context.ReadFile(filePath);
    IntermediateCodeLexer* lexer = new IntermediateCodeLexer(context.Start(), context.End(), filePath, 0);
    lexer->SetFlag(soulng::lexer::LexerFlags::farthestError);
    lexer->SetRuleNameVecPtr(GetRuleNameVecPtr());
    context.ResetLexer(lexer);
    IntermediateCodeParser::Parse(*lexer, &context);
}

} // cmsx::intermediate
