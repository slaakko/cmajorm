// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/Symbol.hpp>
#include <system-x/assembler/Visitor.hpp>

namespace cmsx::assembler {

Symbol::Symbol(NodeKind kind_, const SourcePos& sourcePos_, const std::string& name_) : Node(kind_, sourcePos_), name(name_)
{
}

void Symbol::Write(CodeFormatter& formatter)
{
    formatter.Write(Name());
}

LocalSymbol::LocalSymbol(const SourcePos& sourcePos_, const std::string& name_) : Symbol(NodeKind::localSymbolNode, sourcePos_, name_)
{
}

void LocalSymbol::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

GlobalSymbol::GlobalSymbol(const SourcePos& sourcePos_, const std::string& name_) : Symbol(NodeKind::globalSymbolNode, sourcePos_, name_)
{
}

void GlobalSymbol::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

At::At(const SourcePos& sourcePos_) : Node(NodeKind::atNode, sourcePos_)
{
}

void At::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void At::Write(CodeFormatter& formatter)
{
    formatter.Write("@");
}

LocalSymbol* MakeLocalSymbol(int id)
{
    return new LocalSymbol(SourcePos(), "@" + std::to_string(id));
}

GlobalSymbol* MakeGlobalSymbol(const std::string& name)
{
    return new GlobalSymbol(SourcePos(), name);
}

} // namespace cmsx::assembler
