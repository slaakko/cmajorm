// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_ENUMERATION_INCLUDED
#define SNGCM_AST_ENUMERATION_INCLUDED
#include <sngcm/ast/Function.hpp>
#include <sngcm/ast/NodeList.hpp>
#include <sngcm/ast/Specifier.hpp>

namespace sngcm { namespace ast {

class IdentifierNode;
class EnumConstantNode;

class SNGCM_AST_API EnumTypeNode : public Node
{
public:
    EnumTypeNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    EnumTypeNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, IdentifierNode* id_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddConstant(EnumConstantNode* constant);
    EnumConstantNode* GetLastConstant() const;
    Specifiers GetSpecifiers() const override { return specifiers; }
    IdentifierNode* Id() const { return id.get(); }
    void SetUnderlyingType(Node* underlyingType_);
    Node* GetUnderlyingType() const { return underlyingType.get(); }
    const NodeList<EnumConstantNode>& Constants() const { return constants; }
    void SetBeginBraceSpan(const Span& beginBraceSpan_) { beginBraceSpan = beginBraceSpan_; }
    const Span& BeginBraceSpan() const { return beginBraceSpan; }
    void SetEndBraceSpan(const Span& endBraceSpan_) { endBraceSpan = endBraceSpan_; }
    const Span& EndBraceSpan() const { return endBraceSpan; }
private:
    Specifiers specifiers;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> underlyingType;
    NodeList<EnumConstantNode> constants;
    Span beginBraceSpan;
    Span endBraceSpan;
};

class SNGCM_AST_API EnumConstantNode : public Node
{
public:
    EnumConstantNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    EnumConstantNode(const Span& span_, const boost::uuids::uuid& moduleId_, IdentifierNode* id_, Node* value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    Node* GetValue() const { return value.get(); }
    IdentifierNode* Id() const { return id.get(); }
    void SetHasValue() { hasValue = true; }
    bool HasValue() const { return hasValue; }
    void SetStrValue(const std::u32string& strValue_) { strValue = strValue_; }
    const std::u32string& StrValue() const { return strValue; }
private:
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> value;
    bool hasValue;
    std::u32string strValue;
};

SNGCM_AST_API Node* MakeNextEnumConstantValue(const Span& span, const boost::uuids::uuid& moduleId_, EnumTypeNode* enumType);

} } // namespace sngcm::ast

#endif // SNGCM_AST_ENUMERATION_INCLUDED
