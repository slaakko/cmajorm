// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_LITERAL_INCLUDED
#define CMAJOR_AST_LITERAL_INCLUDED
#include <cmajor/ast/Node.hpp>
#include <cmajor/ast/NodeList.hpp>

namespace cmajor { namespace ast {

class LiteralNode : public Node
{
public:
    LiteralNode(NodeType nodeType_, const Span& span_);
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void SetText(const std::u32string& text_);
    const std::u32string& Text() const { return text; }
private:
    std::u32string text;
};

LiteralNode* CreateIntegerLiteralNode(const Span& span, uint64_t value, bool unsignedSuffix);
LiteralNode* CreateFloatingLiteralNode(const Span& span, double value, bool float_);

class BooleanLiteralNode : public LiteralNode
{
public:
    BooleanLiteralNode(const Span& span_);
    BooleanLiteralNode(const Span& span_, bool value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    bool Value() const { return value; }
private:
    bool value;
};

class SByteLiteralNode : public LiteralNode
{
public:
    SByteLiteralNode(const Span& span_);
    SByteLiteralNode(const Span& span_, int8_t value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    int8_t Value() const { return value; }
private:
    int8_t value;
};

class ByteLiteralNode : public LiteralNode
{
public:
    ByteLiteralNode(const Span& span_);
    ByteLiteralNode(const Span& span_, uint8_t value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    uint8_t Value() const { return value; }
private:
    uint8_t value;
};

class ShortLiteralNode : public LiteralNode
{
public:
    ShortLiteralNode(const Span& span_);
    ShortLiteralNode(const Span& span_, int16_t value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    int16_t Value() const { return value; }
private:
    int16_t value;
};

class UShortLiteralNode : public LiteralNode
{
public:
    UShortLiteralNode(const Span& span_);
    UShortLiteralNode(const Span& span_, uint16_t value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    uint16_t Value() const { return value; }
private:
    uint16_t value;
};

class IntLiteralNode : public LiteralNode
{
public:
    IntLiteralNode(const Span& span_);
    IntLiteralNode(const Span& span_, int32_t value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    int32_t Value() const { return value; }
private:
    int32_t value;
};

class UIntLiteralNode : public LiteralNode
{
public:
    UIntLiteralNode(const Span& span_);
    UIntLiteralNode(const Span& span_, uint32_t value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    uint32_t Value() const { return value; }
private:
    uint32_t value;
};

class LongLiteralNode : public LiteralNode
{
public:
    LongLiteralNode(const Span& span_);
    LongLiteralNode(const Span& span_, int64_t value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    int64_t Value() const { return value; }
private:
    int64_t value;
};

class ULongLiteralNode : public LiteralNode
{
public:
    ULongLiteralNode(const Span& span_);
    ULongLiteralNode(const Span& span_, uint64_t value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    uint64_t Value() const { return value; }
private:
    uint64_t value;
};

class FloatLiteralNode : public LiteralNode
{
public:
    FloatLiteralNode(const Span& span_);
    FloatLiteralNode(const Span& span_, float value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    float Value() const { return value; }
private:
    float value;
};

class DoubleLiteralNode : public LiteralNode
{
public:
    DoubleLiteralNode(const Span& span_);
    DoubleLiteralNode(const Span& span_, double value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    double Value() const { return value; }
private:
    double value;
};

class CharLiteralNode : public LiteralNode
{
public:
    CharLiteralNode(const Span& span_);
    CharLiteralNode(const Span& span_, char value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    char Value() const { return value; }
private:
    char value;
};

class WCharLiteralNode : public LiteralNode
{
public:
    WCharLiteralNode(const Span& span_);
    WCharLiteralNode(const Span& span_, char16_t value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    char16_t Value() const { return value; }
private:
    char16_t value;
};

class UCharLiteralNode : public LiteralNode
{
public:
    UCharLiteralNode(const Span& span_);
    UCharLiteralNode(const Span& span_, char32_t value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    char32_t Value() const { return value; }
private:
    char32_t value;
};

class StringLiteralNode : public LiteralNode
{
public:
    StringLiteralNode(const Span& span_);
    StringLiteralNode(const Span& span_, const std::string& value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    const std::string& Value() const { return value; }
private:
    std::string value;
};

class WStringLiteralNode : public LiteralNode
{
public:
    WStringLiteralNode(const Span& span_);
    WStringLiteralNode(const Span& span_, const std::u16string& value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    const std::u16string& Value() const { return value; }
private:
    std::u16string value;
};

class UStringLiteralNode : public LiteralNode
{
public:
    UStringLiteralNode(const Span& span_);
    UStringLiteralNode(const Span& span_, const std::u32string& value_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override;
    const std::u32string& Value() const { return value; }
private:
    std::u32string value;
};

class NullLiteralNode : public LiteralNode
{
public:
    NullLiteralNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "null"; }
};

class ArrayLiteralNode : public LiteralNode
{
public:
    ArrayLiteralNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override { return "array"; }
    void AddValue(Node* value);
    const NodeList<Node>& Values() const { return values; }
    NodeList<Node>& Values() { return values; }
private:
    NodeList<Node> values;
};

class StructuredLiteralNode : public LiteralNode
{
public:
    StructuredLiteralNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override { return "structure"; }
    void AddMember(Node* member);
    const NodeList<Node>& Members() const { return members; }
    NodeList<Node>& Members() { return members; }
private:
    NodeList<Node> members;
};

class UuidLiteralNode : public LiteralNode
{
public:
    UuidLiteralNode(const Span& span_);
    UuidLiteralNode(const Span& span_, const boost::uuids::uuid& uuid_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override { return "uuid"; }
    const boost::uuids::uuid& Uuid() const { return uuid; }
private:
    boost::uuids::uuid uuid;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_LITERAL_INCLUDED
