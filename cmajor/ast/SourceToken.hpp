// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_SOURCE_TOKEN_INCLUDED
#define CMAJOR_AST_SOURCE_TOKEN_INCLUDED
#include <cmajor/ast/AstApi.hpp>
#include <string>

namespace cmajor { namespace ast {

class AST_API SourceTokenFormatter
{
public:
    virtual ~SourceTokenFormatter();
    virtual void BeginFormat() {}
    virtual void EndFormat() {}
    virtual void Keyword(const std::u32string& keyword) {}
    virtual void Identifier(const std::u32string& identifier) {}
    virtual void Number(const std::u32string& number) {}
    virtual void Char(const std::u32string& char_) {}
    virtual void String(const std::u32string& string) {}
    virtual void Spaces(const std::u32string& spaces) {}
    virtual void Comment(const std::u32string& comment) {}
    virtual void NewLine(const std::u32string& newline) {}
    virtual void Other(const std::u32string& other) {}
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_SOURCE_TOKEN_INCLUDED
