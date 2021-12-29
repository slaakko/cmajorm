// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_SYMBOL_INCLUDED
#define CMSX_OBJECT_SYMBOL_INCLUDED
#include <system-x/object/Api.hpp>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

namespace cmsx::object {

const uint64_t undefinedValue = static_cast<uint64_t>(-1);

class Section;

enum class Segment : uint8_t
{
    text = 0, data = 1, pool = 2, stack = 3, unknown = 0xFF
};

enum class Linkage : uint8_t
{
    internal = 0, external = 1, once = 2, remove = 3, undefined = 0xFF
};

enum class ValueFlags : uint8_t
{
    none = 0,
    undefined = 1 << 0,
    definition = 1 << 1,
    reg = 1 << 2,
    pure = 1 << 3,
    address = 1 << 4,
    function = 1 << 5,
    structure = 1 << 6,
    used = 1 << 7
};

CMSX_OBJECT_API inline ValueFlags operator|(ValueFlags left, ValueFlags right)
{
    return ValueFlags(uint8_t(left) | uint8_t(right));
}

CMSX_OBJECT_API inline ValueFlags operator&(ValueFlags left, ValueFlags right)
{
    return ValueFlags(uint8_t(left) & uint8_t(right));
}

CMSX_OBJECT_API inline ValueFlags operator~(ValueFlags flags)
{
    return ValueFlags(~uint8_t(flags));
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
    bool IsSymbolValue() const { return symbol != nullptr; }
    uint64_t Val() const { return val; }
    void SetVal(uint64_t val_) { val = val_; }
    Symbol* GetSymbol() const { return symbol; }
    void SetSymbol(Symbol* symbol_) { symbol = symbol_; }
private:
    ValueFlags flags;
    uint64_t val;
    Symbol* symbol;
};

enum class SymbolKind : uint8_t
{
    none, local, global
};

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
    void AddSymbol(Symbol* symbol);
    void AddSymbol(Symbol* symbol, bool setIndex);
    void AddSymbolToAddressMap(Symbol* symbol, bool setStart);
    const std::vector<std::unique_ptr<Symbol>>& Symbols() const { return symbols; }
private:
    std::vector<std::unique_ptr<Symbol>> symbols;
    std::map<std::string, Symbol*> table;
    std::map<uint8_t, Symbol*> registerMap;
    std::map<uint64_t, Symbol*> valueMap;
    std::map<uint64_t, Symbol*> addressMap;
};

} // namespace cmsx::object

#endif // CMSX_OBJECT_SYMBOL_INCLUDED
