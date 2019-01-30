// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_BASIC_TYPE_INCLUDED
#define CMAJOR_AST_BASIC_TYPE_INCLUDED
#include <cmajor/ast/Node.hpp>

namespace cmajor { namespace ast {

class AST_API BoolNode : public Node
{
public:
    BoolNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "bool";  }
};

class AST_API SByteNode : public Node
{
public:
    SByteNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "sbyte"; }
};

class AST_API ByteNode : public Node
{
public:
    ByteNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "byte"; }
};

class AST_API ShortNode : public Node
{
public:
    ShortNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "short"; }
};

class AST_API UShortNode : public Node
{
public:
    UShortNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "ushort"; }
};

class AST_API IntNode : public Node
{
public:
    IntNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "int"; }
};

class AST_API UIntNode : public Node
{
public:
    UIntNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "uint"; }
};

class AST_API LongNode : public Node
{
public:
    LongNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "long"; }
};

class AST_API ULongNode : public Node
{
public:
    ULongNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "ulong"; }
};

class AST_API FloatNode : public Node
{
public:
    FloatNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "float"; }
};

class AST_API DoubleNode : public Node
{
public:
    DoubleNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "double"; }
};

class AST_API CharNode : public Node
{
public:
    CharNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "char"; }
};

class AST_API WCharNode : public Node
{
public:
    WCharNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "wchar"; }
};

class AST_API UCharNode : public Node
{
public:
    UCharNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "uchar"; }
};

class AST_API VoidNode : public Node
{
public:
    VoidNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "void"; }
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_BASIC_TYPE_INCLUDED
