// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_ATTRIBUTE_INCLUDED
#define SNGCM_AST_ATTRIBUTE_INCLUDED
#include <sngcm/ast/AstApi.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <soulng/lexer/Span.hpp>
#include <stdexcept>
#include <memory>
#include <string>
#include <map>
#include <vector>

namespace sngcm { namespace ast {

using soulng::lexer::Span;
class AstWriter;
class AstReader;

class SNGCM_AST_API Attribute
{
public:
    Attribute();
    Attribute(const Span& span_, const std::u32string& name_, const std::u32string& value_);
    Attribute(const Attribute&) = delete;
    Attribute& operator=(const Attribute&) = delete;
    const Span& GetSpan() const { return span; }
    const std::u32string& Name() const { return name; }
    const std::u32string& Value() const { return value; }
    void Accept(Visitor& visitor);
    void Write(AstWriter& writer);
    void Read(AstReader& reader);
private:
    Span span;
    std::u32string name;
    std::u32string value;
};

class SNGCM_AST_API Attributes
{
public:
    Attributes();
    Attributes(const Attributes&) = delete;
    Attributes& operator=(const Attributes&) = delete;
    const std::vector<std::unique_ptr<Attribute>>& GetAttributes() const { return attributes; }
    void AddAttribute(const Span& span, const std::u32string& name);
    void AddAttribute(const Span& span, const std::u32string& name, const std::u32string& value);
    Attribute* GetAttribute(const std::u32string& name) const;
    Attributes* Clone() const;
    void Accept(Visitor& visitor);
    void Write(AstWriter& writer);
    void Read(AstReader& reader);
private:
    std::vector<std::unique_ptr<Attribute>> attributes;
    std::map<std::u32string, Attribute*> attributeMap;
    void AddAttribute(Attribute* attribute);
};

class AttributeNotUniqueException : public std::runtime_error
{
public:
    AttributeNotUniqueException(const std::string& message_, const Span& span_, const Span& prevSpan_);
    const Span& GetSpan() const { return span; }
    const Span& PrevSpan() const { return prevSpan; }
private:
    Span span;
    Span prevSpan;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_ATTRIBUTE_INCLUDED

