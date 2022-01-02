// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_NODE_INCLUDED
#define CMSX_ASSEMBLER_NODE_INCLUDED
#include <system-x/assembler/Api.hpp>
#include <soulng/lexer/SourcePos.hpp>
#include <soulng/util/CodeFormatter.hpp>

namespace cmsx::assembler {

using soulng::lexer::SourcePos;
using soulng::util::CodeFormatter;

class Visitor;

enum class NodeKind : int
{
    decimalConstantNode, hexConstantNode, byteConstantNode, wydeConstantNode, tetraConstantNode, characterConstantNode, stringConstantNode, clsIdConstantNode,
    localSymbolNode, globalSymbolNode, atNode, 
    unaryExprNode, binaryExprNode, parenExprNode, opCodeNode, operandListNode, instructionNode
};

class CMSX_ASSEMBLER_API Node
{
public:
    Node(NodeKind kind_, const SourcePos& sourcePos_);
    virtual ~Node();
    NodeKind Kind() const { return kind; }
    const SourcePos& GetSourcePos() const { return sourcePos; }
    virtual void Accept(Visitor& visitor) = 0;
    virtual void Write(CodeFormatter& formatter);
private:
    NodeKind kind;
    SourcePos sourcePos;
};

} // namespace cmsx::assembler

#endif // CMSX_ASSEMBLER_NODE_INCLUDED
