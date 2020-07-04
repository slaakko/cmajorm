// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Literal.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/AstWriter.hpp>
#include <sngcm/ast/AstReader.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <limits>

namespace sngcm { namespace ast {

using namespace soulng::unicode;

LiteralNode::LiteralNode(NodeType nodeType_, const Span& span_) : Node(nodeType_, span_)
{
}

void LiteralNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.GetBinaryWriter().Write(text);
}

void LiteralNode::Read(AstReader& reader)
{
    Node::Read(reader);
    text = reader.GetBinaryReader().ReadUtf32String();
}

void LiteralNode::SetText(const std::u32string& text_)
{
    text = text_;
}

LiteralNode* CreateIntegerLiteralNode(const Span& span, uint64_t value, bool unsignedSuffix)
{
    if (unsignedSuffix)
    {
        if (value <= std::numeric_limits<uint8_t>::max()) return new ByteLiteralNode(span, static_cast<uint8_t>(value));
        if (value <= std::numeric_limits<uint16_t>::max()) return new UShortLiteralNode(span, static_cast<uint16_t>(value));
        if (value <= std::numeric_limits<uint32_t>::max()) return new UIntLiteralNode(span, static_cast<uint32_t>(value));
        return new ULongLiteralNode(span, value);
    }
    else
    {
        if (value <= std::numeric_limits<int8_t>::max()) return new SByteLiteralNode(span, static_cast<int8_t>(value));
        if (value <= std::numeric_limits<uint8_t>::max()) return new ByteLiteralNode(span, static_cast<uint8_t>(value));
        if (value <= std::numeric_limits<int16_t>::max()) return new ShortLiteralNode(span, static_cast<int16_t>(value));
        if (value <= std::numeric_limits<uint16_t>::max()) return new UShortLiteralNode(span, static_cast<uint16_t>(value));
        if (value <= std::numeric_limits<int32_t>::max()) return new IntLiteralNode(span, static_cast<int32_t>(value));
        if (value <= std::numeric_limits<uint32_t>::max()) return new UIntLiteralNode(span, static_cast<uint32_t>(value));
#pragma warning(disable : 4018)
        if (value <= std::numeric_limits<int64_t>::max()) return new LongLiteralNode(span, static_cast<int64_t>(value));
#pragma warning(default : 4018)
        return new ULongLiteralNode(span, value);
    }
}

LiteralNode* CreateFloatingLiteralNode(const Span& span, double value, bool float_)
{
    if (float_)
    {
        return new FloatLiteralNode(span, static_cast<float>(value));
    }
    else
    {
        return new DoubleLiteralNode(span, value);
    }
}

SNGCM_AST_API LiteralNode* CreateCharacterLiteralNode(const Span& span, char32_t value, int chrLitPrefix)
{
    switch (chrLitPrefix)
    {
        case 0:
        {
            return new CharLiteralNode(span, static_cast<char>(value));
        }
        case 1:
        {
            return new WCharLiteralNode(span, static_cast<char16_t>(value));
        }
        case 2:
        {
            return new UCharLiteralNode(span, value);
        }
    }
    return nullptr;
}

SNGCM_AST_API LiteralNode* CreateStringLiteralNode(const Span& span, const std::u32string& value, int strLitPrefix)
{
    switch (strLitPrefix)
    {
        case 0:
        {
            return new StringLiteralNode(span, ToUtf8(value));
        }
        case 1:
        {
            return new WStringLiteralNode(span, ToUtf16(value));
        }
        case 2:
        {
            return new UStringLiteralNode(span, value);
        }
    }
    return nullptr;
}

BooleanLiteralNode::BooleanLiteralNode(const Span& span_) : LiteralNode(NodeType::booleanLiteralNode, span_), value(false)
{
}

BooleanLiteralNode::BooleanLiteralNode(const Span& span_, bool value_) : LiteralNode(NodeType::booleanLiteralNode, span_), value(value_)
{
}

Node* BooleanLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new BooleanLiteralNode(GetSpan(), value);
}

void BooleanLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BooleanLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void BooleanLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadBool();
}

std::string BooleanLiteralNode::ToString() const
{
    if (value) return "true"; else return "false";
}

SByteLiteralNode::SByteLiteralNode(const Span& span_) : LiteralNode(NodeType::sbyteLiteralNode, span_), value(0)
{
}

SByteLiteralNode::SByteLiteralNode(const Span& span_, int8_t value_) : LiteralNode(NodeType::sbyteLiteralNode, span_), value(value_)
{
}

Node* SByteLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new SByteLiteralNode(GetSpan(), value);
}

void SByteLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void SByteLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void SByteLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadSByte();
}

std::string SByteLiteralNode::ToString() const
{
    return std::to_string(value);
}

ByteLiteralNode::ByteLiteralNode(const Span& span_) : LiteralNode(NodeType::byteLiteralNode, span_), value(0u)
{
}

ByteLiteralNode::ByteLiteralNode(const Span& span_, uint8_t value_) : LiteralNode(NodeType::byteLiteralNode, span_), value(value_)
{
}

Node* ByteLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new ByteLiteralNode(GetSpan(), value);
}

void ByteLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ByteLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void ByteLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadByte();
}

std::string ByteLiteralNode::ToString() const
{
    return std::to_string(value) + "u";
}

ShortLiteralNode::ShortLiteralNode(const Span& span_) : LiteralNode(NodeType::shortLiteralNode, span_), value(0)
{
}

ShortLiteralNode::ShortLiteralNode(const Span& span_, int16_t value_) : LiteralNode(NodeType::shortLiteralNode, span_), value(value_)
{
}

Node* ShortLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new ShortLiteralNode(GetSpan(), value);
}

void ShortLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ShortLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void ShortLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadShort();
}

std::string ShortLiteralNode::ToString() const
{
    return std::to_string(value);
}

UShortLiteralNode::UShortLiteralNode(const Span& span_) : LiteralNode(NodeType::ushortLiteralNode, span_), value(0u)
{
}

UShortLiteralNode::UShortLiteralNode(const Span& span_, uint16_t value_) : LiteralNode(NodeType::ushortLiteralNode, span_), value(value_)
{
}

Node* UShortLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new UShortLiteralNode(GetSpan(), value);
}

void UShortLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void UShortLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void UShortLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadUShort();
}

std::string UShortLiteralNode::ToString() const
{
    return std::to_string(value) + "u";
}

IntLiteralNode::IntLiteralNode(const Span& span_) : LiteralNode(NodeType::intLiteralNode, span_), value(0)
{
}

IntLiteralNode::IntLiteralNode(const Span& span_, int32_t value_) : LiteralNode(NodeType::intLiteralNode, span_), value(value_)
{
}

Node* IntLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new IntLiteralNode(GetSpan(), value);
}

void IntLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void IntLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void IntLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadInt();
}

std::string IntLiteralNode::ToString() const
{
    return std::to_string(value);
}

UIntLiteralNode::UIntLiteralNode(const Span& span_) : LiteralNode(NodeType::uintLiteralNode, span_), value(0u)
{
}

UIntLiteralNode::UIntLiteralNode(const Span& span_, uint32_t value_) : LiteralNode(NodeType::uintLiteralNode, span_), value(value_)
{
}

Node* UIntLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new UIntLiteralNode(GetSpan(), value);
}

void UIntLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void UIntLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void UIntLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadUInt();
}

std::string UIntLiteralNode::ToString() const
{
    return std::to_string(value) + "u";
}

LongLiteralNode::LongLiteralNode(const Span& span_) : LiteralNode(NodeType::longLiteralNode, span_), value(0)
{
}

LongLiteralNode::LongLiteralNode(const Span& span_, int64_t value_) : LiteralNode(NodeType::longLiteralNode, span_), value(value_)
{
}

Node* LongLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new LongLiteralNode(GetSpan(), value);
}

void LongLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void LongLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void LongLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadLong();
}

std::string LongLiteralNode::ToString() const
{
    return std::to_string(value);
}

ULongLiteralNode::ULongLiteralNode(const Span& span_) : LiteralNode(NodeType::ulongLiteralNode, span_), value(0u)
{
}

ULongLiteralNode::ULongLiteralNode(const Span& span_, uint64_t value_) : LiteralNode(NodeType::ulongLiteralNode, span_), value(value_)
{
}

Node* ULongLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new ULongLiteralNode(GetSpan(), value);
}

void ULongLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ULongLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void ULongLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadULong();
}

std::string ULongLiteralNode::ToString() const
{
    return std::to_string(value) + "u";
}

FloatLiteralNode::FloatLiteralNode(const Span& span_) : LiteralNode(NodeType::floatLiteralNode, span_), value(0)
{
}

FloatLiteralNode::FloatLiteralNode(const Span& span_, float value_) : LiteralNode(NodeType::floatLiteralNode, span_), value(value_)
{
}

Node* FloatLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new FloatLiteralNode(GetSpan(), value);
}

void FloatLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void FloatLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void FloatLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadFloat();
}

std::string FloatLiteralNode::ToString() const
{
    return std::to_string(value) + "f";
}

DoubleLiteralNode::DoubleLiteralNode(const Span& span_) : LiteralNode(NodeType::doubleLiteralNode, span_), value(0)
{
}

DoubleLiteralNode::DoubleLiteralNode(const Span& span_, double value_) : LiteralNode(NodeType::doubleLiteralNode, span_), value(value_)
{
}

Node* DoubleLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new DoubleLiteralNode(GetSpan(), value);
}

void DoubleLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void DoubleLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void DoubleLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadDouble();
}

std::string DoubleLiteralNode::ToString() const
{
    return std::to_string(value);
}

CharLiteralNode::CharLiteralNode(const Span& span_) : LiteralNode(NodeType::charLiteralNode, span_), value('\0')
{
}

CharLiteralNode::CharLiteralNode(const Span& span_, char value_) : LiteralNode(NodeType::charLiteralNode, span_), value(value_)
{
}

Node* CharLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new CharLiteralNode(GetSpan(), value);
}

void CharLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void CharLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void CharLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadChar();
}

std::string CharLiteralNode::ToString() const
{
    return "'" + CharStr(value) + "'";
}

WCharLiteralNode::WCharLiteralNode(const Span& span_) : LiteralNode(NodeType::wcharLiteralNode, span_), value('\0')
{
}

WCharLiteralNode::WCharLiteralNode(const Span& span_, char16_t value_) : LiteralNode(NodeType::wcharLiteralNode, span_), value(value_)
{
}

Node* WCharLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new WCharLiteralNode(GetSpan(), value);
}

void WCharLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void WCharLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void WCharLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadWChar();
}

std::string WCharLiteralNode::ToString() const
{
    return "w'" + ToUtf8(CharStr(char32_t(value))) + "'";
}

UCharLiteralNode::UCharLiteralNode(const Span& span_) : LiteralNode(NodeType::ucharLiteralNode, span_), value('\0')
{
}

UCharLiteralNode::UCharLiteralNode(const Span& span_, char32_t value_) : LiteralNode(NodeType::ucharLiteralNode, span_), value(value_)
{
}

Node* UCharLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new UCharLiteralNode(GetSpan(), value);
}

void UCharLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void UCharLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void UCharLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadUChar();
}

std::string UCharLiteralNode::ToString() const
{
    return "u'" + ToUtf8(CharStr(value)) + "'";
}

StringLiteralNode::StringLiteralNode(const Span& span_) : LiteralNode(NodeType::stringLiteralNode, span_), value()
{
}

StringLiteralNode::StringLiteralNode(const Span& span_, const std::string& value_) : LiteralNode(NodeType::stringLiteralNode, span_), value(value_)
{
}

Node* StringLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new StringLiteralNode(GetSpan(), value);
}

void StringLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void StringLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void StringLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadUtf8String();
}

std::string StringLiteralNode::ToString() const
{
    return "\"" + StringStr(value) + "\"";
}

WStringLiteralNode::WStringLiteralNode(const Span& span_) : LiteralNode(NodeType::wstringLiteralNode, span_), value()
{
}

WStringLiteralNode::WStringLiteralNode(const Span& span_, const std::u16string& value_) : LiteralNode(NodeType::wstringLiteralNode, span_), value(value_)
{
}

Node* WStringLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new WStringLiteralNode(GetSpan(), value);
}

void WStringLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void WStringLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void WStringLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadUtf16String();
}

std::string WStringLiteralNode::ToString() const
{
    return "\"" + StringStr(ToUtf8(value)) + "\"";
}

UStringLiteralNode::UStringLiteralNode(const Span& span_) : LiteralNode(NodeType::ustringLiteralNode, span_), value()
{
}

UStringLiteralNode::UStringLiteralNode(const Span& span_, const std::u32string& value_) : LiteralNode(NodeType::ustringLiteralNode, span_), value(value_)
{
}

Node* UStringLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new UStringLiteralNode(GetSpan(), value);
}

void UStringLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void UStringLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(value);
}

void UStringLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    value = reader.GetBinaryReader().ReadUtf32String();
}

std::string UStringLiteralNode::ToString() const
{
    return "\"" + StringStr(ToUtf8(value)) + "\"";
}

NullLiteralNode::NullLiteralNode(const Span& span_) : LiteralNode(NodeType::nullLiteralNode, span_)
{
}

Node* NullLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new NullLiteralNode(GetSpan());
}

void NullLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ArrayLiteralNode::ArrayLiteralNode(const Span& span_) : LiteralNode(NodeType::arrayLiteralNode, span_)
{
}

Node* ArrayLiteralNode::Clone(CloneContext& cloneContext) const
{
    ArrayLiteralNode* clone = new ArrayLiteralNode(GetSpan());
    int n = values.Count();
    for (int i = 0; i < n; ++i)
    {
        clone->AddValue(values[i]->Clone(cloneContext));
    }
    return clone;
}

void ArrayLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ArrayLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    values.Write(writer);
}

void ArrayLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    values.Read(reader);
}

void ArrayLiteralNode::AddValue(Node* value)
{
    value->SetParent(this);
    values.Add(value);
}

StructuredLiteralNode::StructuredLiteralNode(const Span& span_) : LiteralNode(NodeType::structuredLiteralNode, span_)
{
}

Node* StructuredLiteralNode::Clone(CloneContext& cloneContext) const
{
    StructuredLiteralNode* clone = new StructuredLiteralNode(GetSpan());
    int n = members.Count();
    for (int i = 0; i < n; ++i)
    {
        clone->AddMember(members[i]->Clone(cloneContext));
    }
    return clone;
}

void StructuredLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void StructuredLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    members.Write(writer);
}

void StructuredLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    members.Read(reader);
}

void StructuredLiteralNode::AddMember(Node* member)
{
    member->SetParent(this);
    members.Add(member);
}

UuidLiteralNode::UuidLiteralNode(const Span& span_) : LiteralNode(NodeType::uuidLiteralNode, span_), uuid(boost::uuids::nil_uuid())
{
}

UuidLiteralNode::UuidLiteralNode(const Span& span_, const boost::uuids::uuid& uuid_): LiteralNode(NodeType::uuidLiteralNode, span_), uuid(uuid_)
{
}

Node* UuidLiteralNode::Clone(CloneContext& cloneContext) const
{
    return new UuidLiteralNode(GetSpan(), uuid);
}

void UuidLiteralNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void UuidLiteralNode::Write(AstWriter& writer)
{
    LiteralNode::Write(writer);
    writer.GetBinaryWriter().Write(uuid);
}

void UuidLiteralNode::Read(AstReader& reader)
{
    LiteralNode::Read(reader);
    reader.GetBinaryReader().ReadUuid(uuid);
}

} } // namespace sngcm::ast
