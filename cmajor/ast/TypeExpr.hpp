// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_TYPE_EXPR_INCLUDED
#define CMAJOR_AST_TYPE_EXPR_INCLUDED
#include <cmajor/ast/Node.hpp>

namespace cmajor { namespace ast {

class ConstNode : public Node
{
public:
    ConstNode(const Span& span_);
    ConstNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    const Node* Subject() const { return subject.get(); }
    Node* Subject() { return subject.get(); }
private:
    std::unique_ptr<Node> subject;
};

class LValueRefNode : public Node
{
public:
    LValueRefNode(const Span& span_);
    LValueRefNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    const Node* Subject() const { return subject.get(); }
    Node* Subject() { return subject.get(); }
private:
    std::unique_ptr<Node> subject;
};

class RValueRefNode : public Node
{
public:
    RValueRefNode(const Span& span_);
    RValueRefNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    const Node* Subject() const { return subject.get(); }
    Node* Subject() { return subject.get(); }
private:
    std::unique_ptr<Node> subject;
};

class PointerNode : public Node
{
public:
    PointerNode(const Span& span_);
    PointerNode(const Span& span_, Node* subject_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    const Node* Subject() const { return subject.get(); }
    Node* Subject() { return subject.get(); }
private:
    std::unique_ptr<Node> subject;
};

class ArrayNode : public Node
{
public:
    ArrayNode(const Span& span_);
    ArrayNode(const Span& span_, Node* subject_, Node* size_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    const Node* Subject() const { return subject.get(); }
    Node* Subject() { return subject.get(); }
    const Node* Size() const { return size.get(); }
    Node* Size() { return size.get(); }
private:
    std::unique_ptr<Node> subject;
    std::unique_ptr<Node> size;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_TYPE_EXPR_INCLUDED
