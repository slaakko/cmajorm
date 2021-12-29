// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_VISITOR_INCLUDED
#define CMSX_ASSEMBLER_VISITOR_INCLUDED
#include <system-x/assembler/Api.hpp>

namespace cmsx::assembler {

class DecimalConstant;
class HexadecimalConstant;
class CharacterConstant;
class StringConstant;
class ClsIdConstant;
class UnaryExpression;
class BinaryExpression;
class ParenthesizedExpression;
class OpCode;
class LocalSymbol;
class GlobalSymbol;
class At;
class OperandList;
class Instruction;

class CMSX_ASSEMBLER_API Visitor
{
public:
    virtual ~Visitor();
    virtual void Visit(DecimalConstant& node) {}
    virtual void Visit(HexadecimalConstant& node) {}
    virtual void Visit(CharacterConstant& node) {}
    virtual void Visit(StringConstant& node) {}
    virtual void Visit(ClsIdConstant& node) {}
    virtual void Visit(UnaryExpression& node) {}
    virtual void Visit(BinaryExpression& node) {}
    virtual void Visit(ParenthesizedExpression& node) {}
    virtual void Visit(OpCode& node) {}
    virtual void Visit(LocalSymbol& node) {}
    virtual void Visit(GlobalSymbol& node) {}
    virtual void Visit(At& node) {}
    virtual void Visit(OperandList& node) {}
    virtual void Visit(Instruction& node) {}
};

} // namespace cmsx::assembler

#endif // CMSX_ASSEMBLER_VISITOR_INCLUDED
