// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_CLASS_INCLUDED
#define CMAJOR_AST_CLASS_INCLUDED
#include <cmajor/ast/Function.hpp>
#include <cmajor/ast/Attribute.hpp>

namespace cmajor { namespace ast {

class AST_API ClassNode : public Node
{
public:
    ClassNode(const Span& span_);
    ClassNode(const Span& span_, Specifiers specifiers_, IdentifierNode* id_, Attributes* attributes_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddTemplateParameter(TemplateParameterNode* templateParameter) override;
    Specifiers GetSpecifiers() const { return specifiers; }
    IdentifierNode* Id() const { return id.get(); }
    void AddBaseClassOrInterface(Node* baseClassOrInterface);
    const WhereConstraintNode* WhereConstraint() const { return constraint.get(); }
    WhereConstraintNode* WhereConstraint() { return constraint.get(); }
    void SetConstraint(WhereConstraintNode* whereConstraint);
    void AddMember(Node* member);
    const NodeList<TemplateParameterNode>& TemplateParameters() const { return templateParameters; }
    const NodeList<Node>& BaseClassOrInterfaces() const { return baseClassOrInterfaces; }
    const NodeList<Node>& Members() const { return members; }
    Attributes* GetAttributes() const { return attributes.get(); }
    void SetSpecifierSpan(const Span& specifierSpan_) { specifierSpan = specifierSpan_; }
    const Span& SpecifierSpan() const { return specifierSpan; }
    void SetClassSpan(const Span& classSpan_) { classSpan = classSpan_; }
    const Span& ClassSpan() const { return classSpan; }
    void SetBeginBraceSpan(const Span& beginBraceSpan_) { beginBraceSpan = beginBraceSpan_; }
    const Span& BeginBraceSpan() const { return beginBraceSpan; }
    void SetEndBraceSpan(const Span& endBraceSpan_) { endBraceSpan = endBraceSpan_; }
    const Span& EndBraceSpan() const { return endBraceSpan; }
private:
    Specifiers specifiers;
    Span specifierSpan;
    Span classSpan;
    Span beginBraceSpan;
    Span endBraceSpan;
    std::unique_ptr<IdentifierNode> id;
    NodeList<TemplateParameterNode> templateParameters;
    NodeList<Node> baseClassOrInterfaces;
    std::unique_ptr<WhereConstraintNode> constraint;
    NodeList<Node> members;
    std::unique_ptr<Attributes> attributes;
};

class AST_API InitializerNode : public Node
{
public:
    InitializerNode(NodeType nodeType_, const Span& span_);
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddArgument(Node* argument) override;
    const NodeList<Node>& Arguments() const { return arguments; }
private:
    NodeList<Node> arguments;
};

class AST_API ThisInitializerNode : public InitializerNode
{
public:
    ThisInitializerNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
};

class AST_API BaseInitializerNode : public InitializerNode
{
public:
    BaseInitializerNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
};

class AST_API MemberInitializerNode : public InitializerNode
{
public:
    MemberInitializerNode(const Span& span_);
    MemberInitializerNode(const Span& span_, IdentifierNode* memberId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const IdentifierNode* MemberId() const { return memberId.get(); }
    IdentifierNode* MemberId() { return memberId.get(); }
private:
    std::unique_ptr<IdentifierNode> memberId;
};

class AST_API StaticConstructorNode : public FunctionNode
{
public:
    StaticConstructorNode(const Span& span_);
    StaticConstructorNode(const Span& span_, Specifiers specifiers_, Attributes* attributes_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddInitializer(InitializerNode* initializer);
    const NodeList<InitializerNode>& Initializers() const { return initializers; }
    void SetClassId(IdentifierNode* classId_) { classId.reset(classId_); }
    IdentifierNode* ClassId() { return classId.get(); }
private:
    NodeList<InitializerNode> initializers;
    std::unique_ptr<IdentifierNode> classId;
};

class AST_API ConstructorNode : public FunctionNode
{
public:
    ConstructorNode(const Span& span_);
    ConstructorNode(const Span& span_, Specifiers specifiers_, Attributes* attributes_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void AddInitializer(InitializerNode* initializer);
    const NodeList<InitializerNode>& Initializers() const { return initializers; }
    void SetClassId(IdentifierNode* classId_) { classId.reset(classId_); }
    IdentifierNode* ClassId() { return classId.get(); }
private:
    NodeList<InitializerNode> initializers;
    std::unique_ptr<IdentifierNode> classId;
};

class AST_API DestructorNode : public FunctionNode
{
public:
    DestructorNode(const Span& span_);
    DestructorNode(const Span& span_, Specifiers specifiers_, Attributes* attributes_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void SetClassId(IdentifierNode* classId_) { classId.reset(classId_); }
    IdentifierNode* ClassId() { return classId.get(); }
private:
    std::unique_ptr<IdentifierNode> classId;
};

class AST_API MemberFunctionNode : public FunctionNode
{
public:
    MemberFunctionNode(const Span& span_);
    MemberFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, const std::u32string& groupId_, Attributes* attributes_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsConst() const { return (GetSpecifiers() & Specifiers::const_) != Specifiers::none; }
    void SetConst() { SetSpecifiers(GetSpecifiers() | Specifiers::const_); }
};

class AST_API ConversionFunctionNode : public FunctionNode
{
public:
    ConversionFunctionNode(const Span& span_);
    ConversionFunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, Attributes* attributes_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsConst() const { return (GetSpecifiers() & Specifiers::const_) != Specifiers::none; }
    void SetConst() { SetSpecifiers(GetSpecifiers() | Specifiers::const_); }
};

class AST_API MemberVariableNode : public Node
{
public:
    MemberVariableNode(const Span& span_);
    MemberVariableNode(const Span& span_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_, Attributes* attributes_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    Specifiers GetSpecifiers() const { return specifiers; }
    Node* TypeExpr() const { return typeExpr.get(); }
    IdentifierNode* Id() const { return id.get(); }
    Attributes* GetAttributes() const { return attributes.get(); }
    void SetSpecifierSpan(const Span& specifierSpan_) { specifierSpan = specifierSpan_; }
    const Span& SpecifierSpan() const { return specifierSpan; }
private:
    Specifiers specifiers;
    Span specifierSpan;
    std::unique_ptr<Node> typeExpr;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Attributes> attributes;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_CLASS_INCLUDED
