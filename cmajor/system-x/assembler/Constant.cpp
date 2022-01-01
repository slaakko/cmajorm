// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/Constant.hpp>
#include <system-x/assembler/Visitor.hpp>
#include <system-x/assembler/Expression.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmsx::assembler {

using namespace soulng::util;
using namespace soulng::unicode;

Constant::Constant(NodeKind kind_, const SourcePos& sourcePos_) : Node(kind_, sourcePos_)
{
}

IntegralConstant::IntegralConstant(NodeKind kind_, const SourcePos& sourcePos_, uint64_t value_) : Constant(kind_, sourcePos_), value(value_)
{
}

DecimalConstant::DecimalConstant(const SourcePos& sourcePos_, uint64_t value_) : IntegralConstant(NodeKind::decimalConstantNode, sourcePos_, value_)
{
}

void DecimalConstant::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void DecimalConstant::Write(CodeFormatter& formatter)
{
    formatter.Write(std::to_string(Value()));
}

HexadecimalConstant::HexadecimalConstant(const SourcePos& sourcePos_, uint64_t value_) : IntegralConstant(NodeKind::hexConstantNode, sourcePos_, value_)
{
}

void HexadecimalConstant::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void HexadecimalConstant::Write(CodeFormatter& formatter)
{
    formatter.Write("#" + ToHexString(Value()));
}

CharacterConstant::CharacterConstant(const SourcePos& sourcePos_, char32_t value_) : Constant(NodeKind::characterConstantNode, sourcePos_), value(value_)
{
}

void CharacterConstant::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void CharacterConstant::Write(CodeFormatter& formatter)
{
    std::u32string c(1, value);
    formatter.Write("'" + ToUtf8(c) + "'");
}

StringConstant::StringConstant(const SourcePos& sourcePos_, const std::u32string& value_) : Constant(NodeKind::stringConstantNode, sourcePos_), value(value_)
{
}

void StringConstant::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void StringConstant::Write(CodeFormatter& formatter)
{
    formatter.Write("\"" + ToUtf8(value) + "\"");
}

ClsIdConstant::ClsIdConstant(const SourcePos& sourcePos_, const std::string& typeId_) : Constant(NodeKind::clsIdConstantNode, sourcePos_), typeId(typeId_)
{
}

void ClsIdConstant::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ClsIdConstant::Write(CodeFormatter& formatter)
{
    formatter.Write("$CLSID(" + typeId + ")");
}

Node* MakeConstantExpr(bool value)
{
    if (value)
    {
        return new DecimalConstant(SourcePos(), static_cast<uint64_t>(1));
    }
    else
    {
        return new DecimalConstant(SourcePos(), static_cast<uint64_t>(0));
    }
}

Node* MakeConstantExpr(int8_t value)
{
    return MakeConstantExpr(static_cast<int64_t>(value));
}

Node* MakeConstantExpr(uint8_t value)
{
    return new DecimalConstant(SourcePos(), value);
}

Node* MakeConstantExpr(int16_t value)
{
    return MakeConstantExpr(static_cast<int64_t>(value));
}

Node* MakeConstantExpr(uint16_t value)
{
    return new DecimalConstant(SourcePos(), value);
}

Node* MakeConstantExpr(int32_t value)
{
    return MakeConstantExpr(static_cast<int64_t>(value));
}

Node* MakeConstantExpr(uint32_t value)
{
    return new DecimalConstant(SourcePos(), value);
}

Node* MakeConstantExpr(int64_t value)
{
    if (value >= 0)
    {
        return new DecimalConstant(SourcePos(), static_cast<uint64_t>(value));
    }
    else
    {
        return new UnaryExpression(SourcePos(), Operator::unaryMinus, new DecimalConstant(SourcePos(), static_cast<uint64_t>(-value)));
    }
}

Node* MakeConstantExpr(uint64_t value)
{
    return new DecimalConstant(SourcePos(), value);
}

Node* MakeConstantExpr(uint64_t value, bool hex)
{
    if (hex)
    {
        return new HexadecimalConstant(SourcePos(), value);
    }
    else
    {
        return new DecimalConstant(SourcePos(), value);
    }
}

Node* MakeConstantExpr(float value)
{
    double v = value;
    return MakeConstantExpr(v);
}

Node* MakeConstantExpr(double value)
{
    uint64_t x = *static_cast<uint64_t*>(static_cast<void*>(&value));
    return MakeConstantExpr(x, true);
}

} // namespace cmsx::assembler
