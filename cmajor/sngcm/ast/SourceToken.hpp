// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_SOURCE_TOKEN_INCLUDED
#define SNGCM_AST_SOURCE_TOKEN_INCLUDED
#include <sngcm/ast/AstApi.hpp>
#include <string>

namespace sngcm { namespace ast {

class SNGCM_AST_API SourceTokenFormatter
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

} } // namespace sngcm::ast

#endif // SNGCM_AST_SOURCE_TOKEN_INCLUDED
