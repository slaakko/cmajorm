// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_CONSTANT_INCLUDED
#define CMSX_ASSEMBLER_CONSTANT_INCLUDED
#include <system-x/assembler/Node.hpp>

namespace cmsx::assembler {

class CMSX_ASSEMBLER_API Constant : public Node
{
public:
    Constant(NodeKind kind_, const SourcePos& sourcePos_);
};

class CMSX_ASSEMBLER_API IntegralConstant : public Constant
{
public:
    IntegralConstant(NodeKind kind_, const SourcePos& sourcePos_, uint64_t value_);
    uint64_t Value() const { return value; }
    void SetValue(uint64_t value_) { value = value_; }
private:
    uint64_t value;
};

class CMSX_ASSEMBLER_API DecimalConstant : public IntegralConstant
{
public:
    DecimalConstant(const SourcePos& sourcePos_, uint64_t value_);
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
};

class CMSX_ASSEMBLER_API HexadecimalConstant : public IntegralConstant
{
public:
    HexadecimalConstant(const SourcePos& sourcePos_, uint64_t value_);
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
};

class CMSX_ASSEMBLER_API ByteConstant : public IntegralConstant
{
public:
    ByteConstant(const SourcePos& sourcePos_, uint8_t value_);
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
};

class CMSX_ASSEMBLER_API WydeConstant : public IntegralConstant
{
public:
    WydeConstant(const SourcePos& sourcePos_, uint16_t value_);
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
};

class CMSX_ASSEMBLER_API TetraConstant : public IntegralConstant
{
public:
    TetraConstant(const SourcePos& sourcePos_, uint32_t value_);
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
};

class CMSX_ASSEMBLER_API CharacterConstant : public Constant
{
public:
    CharacterConstant(const SourcePos& sourcePos_, char32_t value_);
    char32_t Value() const { return value; }
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
private:
    char32_t value;
};

class CMSX_ASSEMBLER_API StringConstant : public Constant
{
public:
    StringConstant(const SourcePos& sourcePos_, const std::u32string& value_);
    const std::u32string& Value() const { return value; }
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
private:
    std::u32string value;
};

class CMSX_ASSEMBLER_API ClsIdConstant : public Constant
{
public:
    ClsIdConstant(const SourcePos& sourcePos_, const std::string& typeId_);
    const std::string& TypeId() const { return typeId; }
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter) override;
private:
    std::string typeId;
};

CMSX_ASSEMBLER_API Node* MakeConstantExpr(bool value);
CMSX_ASSEMBLER_API Node* MakeConstantExpr(int8_t value);
CMSX_ASSEMBLER_API Node* MakeConstantExpr(uint8_t value);
CMSX_ASSEMBLER_API Node* MakeConstantExpr(int16_t value);
CMSX_ASSEMBLER_API Node* MakeConstantExpr(uint16_t value);
CMSX_ASSEMBLER_API Node* MakeConstantExpr(int32_t value);
CMSX_ASSEMBLER_API Node* MakeConstantExpr(uint32_t value);
CMSX_ASSEMBLER_API Node* MakeConstantExpr(int64_t value);
CMSX_ASSEMBLER_API Node* MakeConstantExpr(uint64_t value);
CMSX_ASSEMBLER_API Node* MakeConstantExpr(uint64_t value, bool hex);
CMSX_ASSEMBLER_API Node* MakeConstantExpr(float value);
CMSX_ASSEMBLER_API Node* MakeConstantExpr(double value);

} // namespace cmsx::assembler

#endif // CMSX_ASSEMBLER_CONSTANT_INCLUDED
