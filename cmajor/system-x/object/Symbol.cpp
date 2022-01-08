// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/Symbol.hpp>
#include <system-x/object/BinaryFile.hpp>
#include <system-x/machine/Registers.hpp>
#include <soulng/util/TextUtils.hpp>

namespace cmsx::object {

std::string SegmentStr(Segment segment)
{
    switch (segment)
    {
        case Segment::text: return "text";
        case Segment::data: return "data";
        case Segment::pool: return "pool";
        case Segment::stack: return "stack";
        case Segment::unknown: return "unknown";
    }
    return std::string();
}

std::string LinkageStr(Linkage linkage)
{
    switch (linkage)
    {
        case Linkage::internal: return "internal";
        case Linkage::external: return "external";
        case Linkage::once: return "once";
        case Linkage::remove: return "remove";
        case Linkage::undefined: return "undefined";
    }
    return std::string();
}

std::string ValueFlagStr(ValueFlags flags)
{
    std::string s;
    if (flags == ValueFlags::none)
    {
        s.append("none");
    }
    else
    {
        if ((flags & ValueFlags::undefined) != ValueFlags::none)
        {
            if (!s.empty())
            {
                s.append(" | ");
            }
            s.append("undefined");
        }
        if ((flags & ValueFlags::definition) != ValueFlags::none)
        {
            if (!s.empty())
            {
                s.append(" | ");
            }
            s.append("definition");
        }
        if ((flags & ValueFlags::reg) != ValueFlags::none)
        {
            if (!s.empty())
            {
                s.append(" | ");
            }
            s.append("reg");
        }
        if ((flags & ValueFlags::pure) != ValueFlags::none)
        {
            if (!s.empty())
            {
                s.append(" | ");
            }
            s.append("pure");
        }
        if ((flags & ValueFlags::address) != ValueFlags::none)
        {
            if (!s.empty())
            {
                s.append(" | ");
            }
            s.append("address");
        }
        if ((flags & ValueFlags::function) != ValueFlags::none)
        {
            if (!s.empty())
            {
                s.append(" | ");
            }
            s.append("function");
        }
        if ((flags & ValueFlags::structure) != ValueFlags::none)
        {
            if (!s.empty())
            {
                s.append(" | ");
            }
            s.append("structure");
        }
        if ((flags & ValueFlags::typeIdIndex) != ValueFlags::none)
        {
            if (!s.empty())
            {
                s.append(" | ");
            }
            s.append("type_id_index");
        }
        if ((flags & ValueFlags::used) != ValueFlags::none)
        {
            if (!s.empty())
            {
                s.append(" | ");
            }
            s.append("used");
        }
    }
    return s;
}

Value::Value() : flags(ValueFlags::undefined), val(undefinedValue), symbol(nullptr)
{
}

Value::Value(uint64_t val_) : flags(ValueFlags::pure), val(val_), symbol(nullptr)
{
}

Value::Value(uint64_t val_, ValueFlags flags_) : flags(flags_), val(val_), symbol(nullptr)
{
}

Value::Value(uint64_t val_, ValueFlags flags_, Symbol* symbol_) : flags(flags_), val(val_), symbol(symbol_)
{
}

Value::Value(Symbol* symbol_) : flags(ValueFlags::undefined), val(undefinedValue), symbol(symbol_)
{
}

std::string Value::ToString() const
{
    std::string symbolName = "none";
    if (symbol)
    {
        symbolName = "'" + symbol->FullName() + "'";
    }
    return "(flags=" + ValueFlagStr(flags) + ", val=#" + ToHexString(val) + ", symbol=" + symbolName + ")";
}

std::string SymbolKindStr(SymbolKind symbolKind)
{
    switch (symbolKind)
    {
        case SymbolKind::none: return "none";
        case SymbolKind::local: return "local";
        case SymbolKind::global: return "global";
    }
    return std::string();
}

Symbol::Symbol() : 
    kind(SymbolKind::none), 
    index(-1), 
    segment(Segment::unknown), 
    linkage(Linkage::internal), 
    section(nullptr),
    start(-1),
    length(-1), 
    parentIndex(-1), 
    alignment(1)
{
}

Symbol::Symbol(SymbolKind kind_, const std::string& localName_, const std::string& fullName_) : 
    kind(kind_), 
    index(-1),
    localName(localName_),
    fullName(fullName_),
    segment(Segment::unknown),
    linkage(Linkage::internal),
    section(nullptr),
    start(-1),
    length(-1),
    parentIndex(-1),
    alignment(1)
{
}

Symbol::Symbol(SymbolKind kind_, const std::string& localName_, const std::string& fullName_, const Value& value_) : 
    kind(kind_),
    index(-1),
    localName(localName_),
    fullName(fullName_),
    segment(Segment::unknown),
    linkage(Linkage::internal),
    section(nullptr),
    value(value_),
    start(-1),
    length(-1),
    parentIndex(-1),
    alignment(1)
{
    value.SetSymbol(this);
}

bool Symbol::IsGlobalSymbol() const
{
    if (kind == SymbolKind::global && 
        value.GetFlag(ValueFlags::definition) && 
        value.GetFlag(ValueFlags::address) && 
        (linkage == Linkage::external || linkage == Linkage::once) && 
        (segment == Segment::text || segment == Segment::data))
    { 
        return true;
    }
    else
    {
        return false;
    }
}

bool Symbol::IsInternalSymbol() const
{
    if (linkage == Linkage::internal && value.GetFlag(ValueFlags::address) && (segment == Segment::text || segment == Segment::data))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Symbol::IsExternalOrUndefinedSymbol() const
{
    if (linkage == Linkage::external || linkage == Linkage::undefined)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Symbol::IsDefinitionSymbol() const
{
    if (value.GetFlag(ValueFlags::definition) && value.GetFlag(ValueFlags::address) && (segment == Segment::text || segment == Segment::data))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Symbol::IsUndefinedSymbol() const
{
    if (value.Flags() == ValueFlags::undefined)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Symbol::IsLinkOnceSymbol() const
{
    if (linkage == Linkage::once && value.GetFlag(ValueFlags::definition) && value.GetFlag(ValueFlags::address) && (segment == Segment::text || segment == Segment::data))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Symbol::AddLinkCommandId(int32_t linkCommandId)
{
    if (std::find(linkCommandIds.cbegin(), linkCommandIds.cend(), linkCommandId) == linkCommandIds.cend())
    {
        linkCommandIds.push_back(linkCommandId);
    }
}

void Symbol::SetLinkCommandIds(const std::vector<int32_t>& linkCommandIds_)
{
    linkCommandIds = linkCommandIds_;
}

Symbol* Symbol::Clone() const
{
    Symbol* clone = new Symbol();
    clone->SetKind(kind);
    clone->SetLocalName(localName);
    clone->SetFullName(fullName);
    clone->SetSegment(segment);
    clone->SetLinkage(linkage);
    clone->SetSection(section);
    clone->SetValue(value);
    clone->SetStart(start);
    clone->SetLength(length);
    clone->SetAlignment(alignment);
    return clone;
}

void Symbol::SetLocalName(const std::string& localName_)
{
    localName = localName_;
}

void Symbol::SetFullName(const std::string& fullName_)
{
    fullName = fullName_;
}

void Symbol::SetValue(const Value& value_)
{
    value = value_;
}

void Symbol::Print(CodeFormatter& formatter)
{
    formatter.WriteLine(SymbolKindStr(kind) + " symbol index " + std::to_string(index) + ":");
    formatter.IncIndent();
    formatter.WriteLine("local name=" + localName);
    formatter.WriteLine("full name=" + fullName);
    formatter.WriteLine("segment=" + SegmentStr(segment));
    formatter.WriteLine("linkage=" + LinkageStr(linkage));
    if (section)
    {
        formatter.WriteLine("section=" + section->Name());
    }
    formatter.WriteLine("value=" + value.ToString());
    formatter.WriteLine("start=#" + ToHexString(static_cast<uint64_t>(start)));
    formatter.WriteLine("length=#" + ToHexString(static_cast<uint64_t>(length)));
    formatter.WriteLine("parentIndex=" + std::to_string(parentIndex));
    formatter.WriteLine("alignment=" + std::to_string(static_cast<int>(alignment)));
    std::string linkCommandIdStr;
    bool first = true;
    for (int linkCommandId : linkCommandIds)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            linkCommandIdStr.append(", ");
        }
        linkCommandIdStr.append(std::to_string(linkCommandId));
    }
    formatter.WriteLine("link command ids: (" + linkCommandIdStr + ")");
    formatter.DecIndent();
}

SymbolTable::SymbolTable()
{
}

void SymbolTable::InstallDefaultSymbols()
{
    AddSymbol(new Symbol(SymbolKind::global, "", "ax", Value(cmsx::machine::regAX, ValueFlags::reg)));
    AddSymbol(new Symbol(SymbolKind::global, "", "bx", Value(cmsx::machine::regBX, ValueFlags::reg)));
    AddSymbol(new Symbol(SymbolKind::global, "", "cx", Value(cmsx::machine::regCX, ValueFlags::reg)));
    AddSymbol(new Symbol(SymbolKind::global, "", "dx", Value(cmsx::machine::regDX, ValueFlags::reg)));
    AddSymbol(new Symbol(SymbolKind::global, "", "ex", Value(cmsx::machine::regEX, ValueFlags::reg)));
    AddSymbol(new Symbol(SymbolKind::global, "", "sp", Value(cmsx::machine::regSP, ValueFlags::reg)));
    AddSymbol(new Symbol(SymbolKind::global, "", "fp", Value(cmsx::machine::regFP, ValueFlags::reg)));
    AddSymbol(new Symbol(SymbolKind::global, "", "ix", Value(cmsx::machine::regIX, ValueFlags::reg)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rA", Value(cmsx::machine::rA, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rB", Value(cmsx::machine::rB, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rC", Value(cmsx::machine::rC, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rD", Value(cmsx::machine::rD, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rE", Value(cmsx::machine::rE, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rF", Value(cmsx::machine::rF, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rG", Value(cmsx::machine::rG, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rH", Value(cmsx::machine::rH, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rI", Value(cmsx::machine::rI, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rJ", Value(cmsx::machine::rJ, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rK", Value(cmsx::machine::rK, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rL", Value(cmsx::machine::rL, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rM", Value(cmsx::machine::rM, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rN", Value(cmsx::machine::rN, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rO", Value(cmsx::machine::rO, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rP", Value(cmsx::machine::rP, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rQ", Value(cmsx::machine::rQ, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rR", Value(cmsx::machine::rR, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rS", Value(cmsx::machine::rS, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rT", Value(cmsx::machine::rT, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rU", Value(cmsx::machine::rU, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rV", Value(cmsx::machine::rV, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rW", Value(cmsx::machine::rW, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rX", Value(cmsx::machine::rX, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rY", Value(cmsx::machine::rY, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rZ", Value(cmsx::machine::rZ, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rBB", Value(cmsx::machine::rBB, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rTT", Value(cmsx::machine::rTT, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rWW", Value(cmsx::machine::rWW, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rXX", Value(cmsx::machine::rXX, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rYY", Value(cmsx::machine::rYY, ValueFlags::pure)));
    AddSymbol(new Symbol(SymbolKind::global, "", "rZZ", Value(cmsx::machine::rZZ, ValueFlags::pure)));
}

Symbol* SymbolTable::GetSymbol(int index) const
{
    return symbols[index].get();
}

Symbol* SymbolTable::GetSymbol(const std::string& name) const
{
    auto it = table.find(name);
    if (it != table.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

Symbol* SymbolTable::GetSymbol(uint64_t address) const
{
    auto it = addressMap.find(address);
    if (it != addressMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

Symbol* SymbolTable::GetRegisterSymbol(uint8_t reg) const
{
    auto it = registerMap.find(reg);
    if (it != registerMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

Symbol* SymbolTable::GetTrapSymbol(uint64_t trap) const
{
    auto it = trapMap.find(trap);
    if (it != trapMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void SymbolTable::AddSymbol(Symbol* symbol)
{
    AddSymbol(symbol, true);
}

void SymbolTable::AddSymbol(Symbol* symbol, bool setIndex)
{
    if (setIndex)
    {
        symbol->SetIndex(symbols.size());
    }
    symbols.push_back(std::unique_ptr<Symbol>(symbol));
    table[symbol->FullName()] = symbol;
    if (symbol->GetValue().GetFlag(ValueFlags::reg))
    {
        registerMap[static_cast<uint8_t>(symbol->GetValue().Val())] = symbol;
    }
    if (symbol->GetSegment() == cmsx::object::Segment::text && !symbol->GetValue().GetFlag(ValueFlags::undefined))
    {
        valueMap[symbol->GetValue().Val()] = symbol;
    }
}

void SymbolTable::AddTrapSymbol(Symbol* trapSymbol)
{
    trapMap[trapSymbol->GetValue().Val()] = trapSymbol;
}

void SymbolTable::AddSymbolToAddressMap(Symbol* symbol, bool setStart)
{
    if (symbol->GetValue().GetFlag(ValueFlags::address) && symbol->GetValue().Val() != undefinedValue)
    {
        if (setStart)
        {
            symbol->SetStart(symbol->GetSection()->BaseAddress() + symbol->GetValue().Val());
        }
        addressMap[symbol->Start()] = symbol;
    }
}

} // namespace cmsx::object
