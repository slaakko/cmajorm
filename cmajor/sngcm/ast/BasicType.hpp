// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_BASIC_TYPE_INCLUDED
#define SNGCM_AST_BASIC_TYPE_INCLUDED
#include <sngcm/ast/Node.hpp>

namespace sngcm { namespace ast {

class SNGCM_AST_API BoolNode : public Node
{
public:
    BoolNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "bool";  }
};

class SNGCM_AST_API SByteNode : public Node
{
public:
    SByteNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "sbyte"; }
};

class SNGCM_AST_API ByteNode : public Node
{
public:
    ByteNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "byte"; }
};

class SNGCM_AST_API ShortNode : public Node
{
public:
    ShortNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "short"; }
};

class SNGCM_AST_API UShortNode : public Node
{
public:
    UShortNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "ushort"; }
};

class SNGCM_AST_API IntNode : public Node
{
public:
    IntNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "int"; }
};

class SNGCM_AST_API UIntNode : public Node
{
public:
    UIntNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "uint"; }
};

class SNGCM_AST_API LongNode : public Node
{
public:
    LongNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "long"; }
};

class SNGCM_AST_API ULongNode : public Node
{
public:
    ULongNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "ulong"; }
};

class SNGCM_AST_API FloatNode : public Node
{
public:
    FloatNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "float"; }
};

class SNGCM_AST_API DoubleNode : public Node
{
public:
    DoubleNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "double"; }
};

class SNGCM_AST_API CharNode : public Node
{
public:
    CharNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "char"; }
};

class SNGCM_AST_API WCharNode : public Node
{
public:
    WCharNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "wchar"; }
};

class SNGCM_AST_API UCharNode : public Node
{
public:
    UCharNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "uchar"; }
};

class SNGCM_AST_API VoidNode : public Node
{
public:
    VoidNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "void"; }
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_BASIC_TYPE_INCLUDED
