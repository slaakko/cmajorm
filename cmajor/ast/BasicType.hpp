// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_BASIC_TYPE_INCLUDED
#define CMAJOR_AST_BASIC_TYPE_INCLUDED
#include <cmajor/ast/Node.hpp>

namespace cmajor { namespace ast {

class BoolNode : public Node
{
public:
    BoolNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "bool";  }
};

class SByteNode : public Node
{
public:
    SByteNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "sbyte"; }
};

class ByteNode : public Node
{
public:
    ByteNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "byte"; }
};

class ShortNode : public Node
{
public:
    ShortNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "short"; }
};

class UShortNode : public Node
{
public:
    UShortNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "ushort"; }
};

class IntNode : public Node
{
public:
    IntNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "int"; }
};

class UIntNode : public Node
{
public:
    UIntNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "uint"; }
};

class LongNode : public Node
{
public:
    LongNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "long"; }
};

class ULongNode : public Node
{
public:
    ULongNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    bool IsUnsignedTypeNode() const override { return true; }
    std::string ToString() const override { return "ulong"; }
};

class FloatNode : public Node
{
public:
    FloatNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "float"; }
};

class DoubleNode : public Node
{
public:
    DoubleNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "double"; }
};

class CharNode : public Node
{
public:
    CharNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "char"; }
};

class WCharNode : public Node
{
public:
    WCharNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "wchar"; }
};

class UCharNode : public Node
{
public:
    UCharNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "uchar"; }
};

class VoidNode : public Node
{
public:
    VoidNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "void"; }
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_BASIC_TYPE_INCLUDED
