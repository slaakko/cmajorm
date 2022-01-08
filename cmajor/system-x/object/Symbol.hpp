// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_SYMBOL_INCLUDED
#define CMSX_OBJECT_SYMBOL_INCLUDED
#include <system-x/object/Api.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <boost/uuid/uuid.hpp>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

namespace cmsx::object {

using namespace soulng::util;

const uint64_t undefinedValue = static_cast<uint64_t>(-1);

class Section;

enum class Segment : uint8_t
{
    text = 0, data = 1, pool = 2, stack = 3, unknown = 0xFF
};

CMSX_OBJECT_API std::string SegmentStr(Segment segment);

enum class Linkage : uint8_t
{
    internal = 0, external = 1, once = 2, remove = 3, undefined = 0xFF
};

CMSX_OBJECT_API std::string LinkageStr(Linkage linkage);

enum class ValueFlags : uint16_t
{
    none = 0,
    undefined = 1 << 0,
    definition = 1 << 1,
    reg = 1 << 2,
    pure = 1 << 3,
    address = 1 << 4,
    function = 1 << 5,
    structure = 1 << 6,
    typeIdIndex = 1 << 7,
    used = 1 << 8
};

CMSX_OBJECT_API std::string ValueFlagStr(ValueFlags flags);

CMSX_OBJECT_API inline ValueFlags operator|(ValueFlags left, ValueFlags right)
{
    return ValueFlags(uint16_t(left) | uint16_t(right));
}

CMSX_OBJECT_API inline ValueFlags operator&(ValueFlags left, ValueFlags right)
{
    return ValueFlags(uint16_t(left) & uint16_t(right));
}

CMSX_OBJECT_API inline ValueFlags operator~(ValueFlags flags)
{
    return ValueFlags(~uint16_t(flags));
}

class Symbol;

class CMSX_OBJECT_API Value
{
public:
    Value();
    Value(uint64_t val_);
    Value(uint64_t val_, ValueFlags flags_);
    Value(uint64_t val_, ValueFlags flags_, Symbol* symbol_);
    Value(Symbol* symbol_);
    ValueFlags Flags() const { return flags; }
    bool GetFlag(ValueFlags flag) const { return (flags & flag) != ValueFlags::none; }
    void SetFlag(ValueFlags flag) { flags = flags | flag; }
    void ResetFlag(ValueFlags flag) { flags = flags & ~flag; }
    bool IsRegValue() const { return flags == ValueFlags::reg; }
    bool IsPureValue() const { return flags == ValueFlags::pure; }
    bool IsTypeIdIndex() const { return flags == ValueFlags::typeIdIndex; }
    bool IsSymbolValue() const { return symbol != nullptr; }
    uint64_t Val() const { return val; }
    void SetVal(uint64_t val_) { val = val_; }
    Symbol* GetSymbol() const { return symbol; }
    void SetSymbol(Symbol* symbol_) { symbol = symbol_; }
    std::string ToString() const;
private:
    ValueFlags flags;
    uint64_t val;
    Symbol* symbol;
};

enum class SymbolKind : uint8_t
{
    none, local, global
};

CMSX_OBJECT_API std::string SymbolKindStr(SymbolKind symbolKind);

class CMSX_OBJECT_API Symbol
{
public:
    Symbol();
    Symbol(SymbolKind kind_, const std::string& localName_, const std::string& fullName_);
    Symbol(SymbolKind kind_, const std::string& localName_, const std::string& fullName_, const Value& value_);
    bool IsGlobalSymbol() const;
    bool IsInternalSymbol() const;
    bool IsExternalOrUndefinedSymbol() const;
    bool IsDefinitionSymbol() const;
    bool IsUndefinedSymbol() const;
    bool IsLinkOnceSymbol() const;
    bool IsUsed() const { return value.GetFlag(ValueFlags::used); }
    void SetUsed() { value.SetFlag(ValueFlags::used); }
    SymbolKind Kind() const { return kind; }
    void SetKind(SymbolKind kind_) { kind = kind_; }
    int Index() const { return index; }
    void SetIndex(int index_) { index = index_; }
    const std::string& LocalName() const { return localName; }
    void SetLocalName(const std::string& localName_);
    const std::string& FullName() const { return fullName; }
    void SetFullName(const std::string& fullName_);
    Segment GetSegment() const { return segment; }
    void SetSegment(Segment segment_) { segment = segment_;  }
    Linkage GetLinkage() const { return linkage; }
    void SetLinkage(Linkage linkage_) { linkage = linkage_; }
    Section* GetSection() const { return section; }
    void SetSection(Section* section_) { section = section_; }
    const Value& GetValue() const { return value; }
    void SetValue(const Value& value_);
    int64_t Start() const { return start; }
    void SetStart(int64_t start_) { start = start_; }
    int64_t Length() const { return length; }
    void SetLength(int64_t length_) { length = length_; }
    void SetParentIndex(int parentIndex_) { parentIndex = parentIndex_; }
    int ParentIndex() const { return parentIndex; }
    void SetAlignment(uint8_t alignment_) { alignment = alignment_; }
    uint8_t Alignment() const { return alignment; }
    void AddLinkCommandId(int32_t linkCommandId);
    const std::vector<int32_t>& LinkCommandIds() const { return linkCommandIds; }
    void SetLinkCommandIds(const std::vector<int32_t>& linkCommandIds_);
    Symbol* Clone() const;
    void Print(CodeFormatter& formatter);
private:
    SymbolKind kind;
    int index;
    std::string localName;
    std::string fullName;
    Segment segment;
    Linkage linkage;
    Section* section;
    Value value;
    int64_t start;
    int64_t length;
    int parentIndex;
    uint8_t alignment;
    std::vector<int32_t> linkCommandIds;
};

class CMSX_OBJECT_API SymbolTable
{
public:
    SymbolTable();
    SymbolTable(const SymbolTable&) = delete;
    SymbolTable& operator=(const SymbolTable&) = delete;
    void InstallDefaultSymbols();
    Symbol* GetSymbol(int index) const;
    Symbol* GetSymbol(const std::string& name) const;
    Symbol* GetSymbol(uint64_t address) const;
    Symbol* GetRegisterSymbol(uint8_t reg) const;
    Symbol* GetTrapSymbol(uint64_t trap) const;
    void AddSymbol(Symbol* symbol);
    void AddSymbol(Symbol* symbol, bool setIndex);
    void AddSymbolToAddressMap(Symbol* symbol, bool setStart);
    void AddTrapSymbol(Symbol* trapSymbol);
    const std::vector<std::unique_ptr<Symbol>>& Symbols() const { return symbols; }
private:
    std::vector<std::unique_ptr<Symbol>> symbols;
    std::map<std::string, Symbol*> table;
    std::map<uint8_t, Symbol*> registerMap;
    std::map<uint64_t, Symbol*> trapMap;
    std::map<uint64_t, Symbol*> valueMap;
    std::map<uint64_t, Symbol*> addressMap;
};

} // namespace cmsx::object

#endif // CMSX_OBJECT_SYMBOL_INCLUDED
