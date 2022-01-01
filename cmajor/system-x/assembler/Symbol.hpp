// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_SYMBOL_INCLUDED
#define CMSX_ASSEMBLER_SYMBOL_INCLUDED
#include <system-x/assembler/Node.hpp>

namespace cmsx::assembler {

class CMSX_ASSEMBLER_API Symbol : public Node
{
public:
    Symbol(NodeKind kind_, const SourcePos& sourcePos_, const std::string& name_);
    const std::string& Name() const { return name; }
    void Write(CodeFormatter& formatter);
private:
    std::string name;
};

class CMSX_ASSEMBLER_API LocalSymbol : public Symbol
{
public:
    LocalSymbol(const SourcePos& sourcePos_, const std::string& name_);
    void Accept(Visitor& visitor) override;
};

class CMSX_ASSEMBLER_API GlobalSymbol : public Symbol
{
public:
    GlobalSymbol(const SourcePos& sourcePos_, const std::string& name_);
    void Accept(Visitor& visitor) override;
};

class CMSX_ASSEMBLER_API At : public Node
{
public:
    At(const SourcePos& sourcePos_);
    void Accept(Visitor& visitor) override;
    void Write(CodeFormatter& formatter);
};

CMSX_ASSEMBLER_API LocalSymbol* MakeLocalSymbol(int id);
CMSX_ASSEMBLER_API GlobalSymbol* MakeGlobalSymbol(const std::string& name);

} // namespace cmsx::assembler

#endif // CMSX_ASSEMBLER_SYMBOL_INCLUDED
