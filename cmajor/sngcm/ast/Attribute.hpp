// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_ATTRIBUTE_INCLUDED
#define SNGCM_AST_ATTRIBUTE_INCLUDED
#include <sngcm/ast/Node.hpp>

namespace sngcm { namespace ast {

class SNGCM_AST_API AttributeNode : public Node
{
public:
    AttributeNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    AttributeNode(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& name_, const std::u32string& value_);
    AttributeNode(const AttributeNode&) = delete;
    AttributeNode& operator=(const AttributeNode&) = delete;
    const std::u32string& Name() const { return name; }
    const std::u32string& Value() const { return value; }
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
private:
    std::u32string name;
    std::u32string value;
};

class SNGCM_AST_API AttributesNode : public Node
{
public:
    AttributesNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    AttributesNode(const AttributesNode&) = delete;
    AttributesNode& operator=(const AttributesNode&) = delete;
    const std::vector<std::unique_ptr<AttributeNode>>& GetAttributes() const { return attributes; }
    void AddAttribute(const Span& span, const boost::uuids::uuid& moduleId, const std::u32string& name);
    void AddAttribute(const Span& span, const boost::uuids::uuid& moduleId, const std::u32string& name, const std::u32string& value);
    AttributeNode* GetAttribute(const std::u32string& name) const;
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
private:
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::map<std::u32string, AttributeNode*> attributeMap;
    void AddAttribute(AttributeNode* attribute);
};

class AttributeNotUniqueException : public std::runtime_error
{
public:
    AttributeNotUniqueException(const std::string& message_, const Span& span_, const boost::uuids::uuid& moduleId_, 
        const Span& prevSpan_, const boost::uuids::uuid& prevModuleId_);
    const Span& GetSpan() const { return span; }
    const boost::uuids::uuid& ModuleId() const { return moduleId; }
    const Span& PrevSpan() const { return prevSpan; }
    const boost::uuids::uuid& PrevModuleId() const { return prevModuleId; }
private:
    Span span;
    boost::uuids::uuid moduleId;
    Span prevSpan;
    boost::uuids::uuid prevModuleId;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_ATTRIBUTE_INCLUDED

