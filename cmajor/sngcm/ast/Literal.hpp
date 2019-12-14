// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_LITERAL_INCLUDED
#define SNGCM_AST_LITERAL_INCLUDED
#include <sngcm/ast/Node.hpp>
#include <sngcm/ast/NodeList.hpp>

namespace sngcm { namespace ast {

class SNGCM_AST_API LiteralNode : public Node
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

SNGCM_AST_API LiteralNode* CreateIntegerLiteralNode(const Span& span, uint64_t value, bool unsignedSuffix);
SNGCM_AST_API LiteralNode* CreateFloatingLiteralNode(const Span& span, double value, bool float_);
SNGCM_AST_API LiteralNode* CreateCharacterLiteralNode(const Span& span, char32_t value, int chrLitPrefix);
SNGCM_AST_API LiteralNode* CreateStringLiteralNode(const Span& span, const std::u32string& value, int strLitPrefix);

class SNGCM_AST_API BooleanLiteralNode : public LiteralNode
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

class SNGCM_AST_API SByteLiteralNode : public LiteralNode
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

class SNGCM_AST_API ByteLiteralNode : public LiteralNode
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

class SNGCM_AST_API ShortLiteralNode : public LiteralNode
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

class SNGCM_AST_API UShortLiteralNode : public LiteralNode
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

class SNGCM_AST_API IntLiteralNode : public LiteralNode
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

class SNGCM_AST_API UIntLiteralNode : public LiteralNode
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

class SNGCM_AST_API LongLiteralNode : public LiteralNode
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

class SNGCM_AST_API ULongLiteralNode : public LiteralNode
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

class SNGCM_AST_API FloatLiteralNode : public LiteralNode
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

class SNGCM_AST_API DoubleLiteralNode : public LiteralNode
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

class SNGCM_AST_API CharLiteralNode : public LiteralNode
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

class SNGCM_AST_API WCharLiteralNode : public LiteralNode
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

class SNGCM_AST_API UCharLiteralNode : public LiteralNode
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

class SNGCM_AST_API StringLiteralNode : public LiteralNode
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

class SNGCM_AST_API WStringLiteralNode : public LiteralNode
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

class SNGCM_AST_API UStringLiteralNode : public LiteralNode
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

class SNGCM_AST_API NullLiteralNode : public LiteralNode
{
public:
    NullLiteralNode(const Span& span_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    std::string ToString() const override { return "null"; }
};

class SNGCM_AST_API ArrayLiteralNode : public LiteralNode
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

class SNGCM_AST_API StructuredLiteralNode : public LiteralNode
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

class SNGCM_AST_API UuidLiteralNode : public LiteralNode
{
public:
    UuidLiteralNode(const Span& span_);
    UuidLiteralNode(const Span& span_, const boost::uuids::uuid& uuid_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    std::string ToString() const override { return "uuid"; }
    const boost::uuids::uuid& GetUuid() const { return uuid; }
private:
    boost::uuids::uuid uuid;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_LITERAL_INCLUDED
