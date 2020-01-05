// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_TYPE_EXPR_INCLUDED
#define SNGCM_AST_TYPE_EXPR_INCLUDED
#include <sngcm/ast/Node.hpp>

namespace sngcm { namespace ast {

class SNGCM_AST_API ConstNode : public Node
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

class SNGCM_AST_API LValueRefNode : public Node
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

class SNGCM_AST_API RValueRefNode : public Node
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

class SNGCM_AST_API PointerNode : public Node
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

class SNGCM_AST_API ArrayNode : public Node
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

} } // namespace sngcm::ast

#endif // SNGCM_AST_TYPE_EXPR_INCLUDED
