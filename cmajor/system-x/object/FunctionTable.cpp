// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/FunctionTable.hpp>
#include <system-x/object/BinaryFile.hpp>
#include <system-x/object/Symbol.hpp>
#include <system-x/object/LinkTable.hpp>
#include <soulng/util/Uuid.hpp>
#include <algorithm>
#include <stack>

namespace cmsx::object {

using namespace soulng::util;

FunctionTableIndexEntry::FunctionTableIndexEntry() : functionStart(), functionLength(), entryId(), entryAddress()
{
}

FunctionTableIndexEntry::FunctionTableIndexEntry(uint64_t functionStart_, uint64_t functionLength_, uint64_t entryId_, uint64_t entryAddress_) :
    functionStart(functionStart_), functionLength(functionLength_), entryId(entryId_), entryAddress(entryAddress_)
{
}

void FunctionTableIndexEntry::Read(uint64_t rv, cmsx::machine::Memory& memory, uint64_t address)
{
    functionStart = memory.ReadOcta(rv, address, cmsx::machine::Protection::read);
    functionLength = memory.ReadOcta(rv, address + 8, cmsx::machine::Protection::read);
    entryId = memory.ReadOcta(rv, address + 16, cmsx::machine::Protection::read);
    entryAddress = memory.ReadOcta(rv, address + 24, cmsx::machine::Protection::read);
}

void FunctionTableIndexEntry::Write(Section* section)
{
    section->EmitOcta(functionStart);
    section->EmitOcta(functionLength);
    section->EmitOcta(entryId);
    section->EmitOcta(entryAddress);
}

bool operator<(const FunctionTableIndexEntry& left, const FunctionTableIndexEntry& right)
{
    return left.functionStart < right.functionStart;
}

FunctionTableIndex::FunctionTableIndex() : indexEntries()
{
}

void FunctionTableIndex::AddEntry(const FunctionTableIndexEntry& entry)
{
    indexEntries.push_back(entry);
}

void FunctionTableIndex::Sort()
{
    std::sort(indexEntries.begin(), indexEntries.end());
}

void FunctionTableIndex::Read(SymbolTable& symbolTable, uint64_t rv, cmsx::machine::Memory& memory)
{
    Symbol* symbol = symbolTable.GetSymbol("@function_table_index");
    if (symbol)
    {
        uint64_t address = symbol->Start();
        uint64_t count = symbol->GetValue().Val();
        for (uint64_t i = 0; i < count; ++i)
        {
            FunctionTableIndexEntry entry;
            entry.Read(rv, memory, address);
            indexEntries.push_back(entry);
            address = address + entry.Size();
        }
    }
    else
    {
        throw std::runtime_error("'@function_table_index' symbol not found");
    }
}

void FunctionTableIndex::Write(BinaryFile& binaryFile)
{
    binaryFile.GetDataSection()->Align(8);
    int64_t start = binaryFile.GetDataSection()->BaseAddress() + binaryFile.GetDataSection()->Address();
    for (auto& entry : indexEntries)
    {
        entry.Write(binaryFile.GetDataSection());
    }
    int64_t end = binaryFile.GetDataSection()->BaseAddress() + binaryFile.GetDataSection()->Address();
    Symbol* symbol = new Symbol(SymbolKind::global, std::string(), "@function_table_index", Value(indexEntries.size(), ValueFlags::pure));
    symbol->SetStart(start);
    symbol->SetSegment(Segment::data);
    symbol->SetLinkage(Linkage::external);
    symbol->SetLength(end - start);
    binaryFile.GetSymbolTable().AddSymbol(symbol);
}

FunctionTableIndexEntry* FunctionTableIndex::SearchEntry(uint64_t pc) 
{
    FunctionTableIndexEntry value(pc, 0, 0, 0);
    std::vector<FunctionTableIndexEntry>::iterator it = std::lower_bound(indexEntries.begin(), indexEntries.end(), value);
    if (it == indexEntries.end() && it != indexEntries.begin())
    {
        it = indexEntries.end() - 1;
    }
    while (it != indexEntries.end() && it != indexEntries.begin() && pc < it->functionStart)
    {
        --it;
    }
    if (it != indexEntries.end() && pc >= it->functionStart && pc < it->functionStart + it->functionLength)
    {
        return &(*it);
    }
    return nullptr;
}

std::string ReadString(int64_t stringAddress, uint64_t rv, cmsx::machine::Memory& memory)
{
    std::string s;
    uint64_t address = stringAddress;
    uint8_t b = memory.ReadByte(rv, address, cmsx::machine::Protection::read);
    while (b != 0)
    {
        s.append(1, static_cast<char>(b));
        ++address;
        b = memory.ReadByte(rv, address, cmsx::machine::Protection::read);
    }
    return s;
}

int64_t WriteString(Section* section, const std::string& s)
{
    int64_t address = section->BaseAddress() + section->Address();
    for (char c : s)
    {
        section->EmitByte(static_cast<uint8_t>(c));
    }
    section->EmitByte(0);
    return address;
}

StringTable::StringTable() : indexRead(false)
{
}

int32_t StringTable::AddString(const std::string& str)
{
    auto it = stringIdMap.find(str);
    if (it != stringIdMap.cend())
    {
        return it->second;
    }
    else
    {
        int32_t stringId = strings.size();
        strings.push_back(str);
        stringIdMap[str] = stringId;
        return stringId;
    }
}

void StringTable::Write(BinaryFile& binaryFile)
{
    binaryFile.GetDataSection()->Align(8);
    for (const auto& str : strings)
    {
        int64_t address = WriteString(binaryFile.GetDataSection(), str);
        stringAddresses.push_back(address);
    }
    binaryFile.GetDataSection()->Align(8);
    int64_t start = binaryFile.GetDataSection()->BaseAddress() + binaryFile.GetDataSection()->Address();
    for (uint64_t stringAddress : stringAddresses)
    {
        binaryFile.GetDataSection()->EmitOcta(stringAddress);
    }
    int64_t end = binaryFile.GetDataSection()->BaseAddress() + binaryFile.GetDataSection()->Address();
    Symbol* symbol = new Symbol(SymbolKind::global, std::string(), "@string_table", Value(stringAddresses.size(), ValueFlags::pure));
    symbol->SetStart(start);
    symbol->SetSegment(Segment::data);
    symbol->SetLinkage(Linkage::external);
    symbol->SetLength(end - start);
    binaryFile.GetSymbolTable().AddSymbol(symbol);
}

void StringTable::ReadIndex(SymbolTable& symbolTable, uint64_t rv, cmsx::machine::Memory& memory)
{
    indexRead = true;
    Symbol* symbol = symbolTable.GetSymbol("@string_table");
    if (symbol)
    {
        uint64_t address = symbol->Start();
        uint64_t count = symbol->GetValue().Val();
        for (uint64_t i = 0; i < count; ++i)
        {
            uint64_t stringAddress = memory.ReadOcta(rv, address, cmsx::machine::Protection::read);
            stringAddresses.push_back(stringAddress);
            address = address + 8;
        }
    }
    else
    {
        throw std::runtime_error("'@string_table' symbol not found");
    }
}

std::string StringTable::GetString(SymbolTable& symbolTable, int32_t stringId, uint64_t rv, cmsx::machine::Memory& memory)
{
    if (!indexRead)
    {
        ReadIndex(symbolTable, rv, memory);
    }
    auto it = stringMap.find(stringId);
    if (it != stringMap.cend())
    {
        return it->second;
    }
    if (stringId >= 0 && stringId < stringAddresses.size())
    {
        int64_t stringAddress = stringAddresses[stringId];
        std::string str = ReadString(stringAddress, rv, memory);
        stringMap[stringId] = str;
        return str;
    }
    return std::string();
}

LineNumberTableEntry::LineNumberTableEntry() : offset(), lineNumber()
{
}

LineNumberTableEntry::LineNumberTableEntry(uint32_t offset_, uint32_t lineNumber_) : offset(offset_), lineNumber(lineNumber_)
{
}

void LineNumberTableEntry::Write(Section* section)
{
    section->Align(4);
    section->EmitTetra(offset);
    section->EmitTetra(lineNumber);
}

void LineNumberTableEntry::Read(int64_t address, uint64_t rv, cmsx::machine::Memory& memory)
{
    offset = memory.ReadTetra(rv, address, cmsx::machine::Protection::read);
    lineNumber = memory.ReadTetra(rv, address + 4, cmsx::machine::Protection::read);
}

LineNumberTable::LineNumberTable()
{
}

void LineNumberTable::AddEntry(const LineNumberTableEntry& entry)
{
    entries.push_back(entry);
}

void LineNumberTable::Write(Section* section)
{
    section->Align(4);
    uint32_t entryCount = entries.size();
    section->EmitTetra(entryCount);
    for (uint32_t i = 0; i < entryCount; ++i)
    {
        entries[i].Write(section);
    }
}

int64_t LineNumberTable::Read(int64_t address, uint64_t rv, cmsx::machine::Memory& memory)
{
    uint32_t entryCount = memory.ReadTetra(rv, address, cmsx::machine::Protection::read);
    int64_t addr = address + 4;
    for (uint32_t i = 0; i < entryCount; ++i)
    {
        LineNumberTableEntry entry; 
        entry.Read(addr, rv, memory);
        AddEntry(entry);
        addr = addr + entry.Size();
    }
    return addr;
}

int32_t LineNumberTable::SearchLineNumber(uint32_t offset) const
{
    if (entries.empty())
    {
        return -1;
    }
    if (offset <= entries[0].offset)
    {
        return entries[0].lineNumber;
    }
    for (int i = 1; i < entries.size(); ++i)
    {
        if (offset >= entries[i - 1].offset && offset <= entries[i].offset)
        {
            return entries[i].lineNumber;
        }
    }
    return -1;
}

ExceptionTableRecord::ExceptionTableRecord(ExceptionTableRecordKind kind_) : kind(kind_)
{
}

ExceptionTableRecord::~ExceptionTableRecord()
{
}

void ExceptionTableRecord::Write(Section* section)
{
    section->EmitTetra(static_cast<uint32_t>(kind));
}

void ExceptionTableRecord::Read(int64_t address, uint64_t rv, cmsx::machine::Memory& memory)
{
}

void ExceptionTableRecord::Setup(ExceptionTable* exceptionTable_)
{
    table = exceptionTable_;
}

ExceptionTableRecord* MakeExceptionTableRecord(ExceptionTableRecordKind kind)
{
    switch (kind)
    {
        case ExceptionTableRecordKind::tryRecord:
        {
            return new TryRecord();
        }
        case ExceptionTableRecordKind::handlerRecord:
        {
            return new HandlerRecord();
        }
        case ExceptionTableRecordKind::cleanupRecord:
        {
            return new CleanupRecord();
        }
    }
    return nullptr;
}

ExceptionTableRecord* ReadExceptionTableRecord(int64_t address, uint64_t rv, cmsx::machine::Memory& memory)
{
    uint32_t k = memory.ReadTetra(rv, address, cmsx::machine::Protection::read);
    ExceptionTableRecord* record = MakeExceptionTableRecord(static_cast<ExceptionTableRecordKind>(k));
    record->Read(address + 4, rv, memory);
    return record;
}

TryRecord::TryRecord() : 
    ExceptionTableRecord(ExceptionTableRecordKind::tryRecord), id(-1), offset(), length(), parentTryRecordId(), parentTryRecord(nullptr)
{
}

TryRecord::TryRecord(uint32_t id_, uint32_t offset_, uint32_t parentTryRecordId_) : 
    ExceptionTableRecord(ExceptionTableRecordKind::tryRecord), id(id_), offset(offset_), length(0), parentTryRecordId(parentTryRecordId_), parentTryRecord(nullptr)
{
}

void TryRecord::Write(Section* section)
{
    ExceptionTableRecord::Write(section);
    section->EmitTetra(static_cast<uint32_t>(id));
    section->EmitTetra(offset);
    section->EmitTetra(length);
    section->EmitTetra(parentTryRecordId);
}

void TryRecord::Read(int64_t address, uint64_t rv, cmsx::machine::Memory& memory)
{
    ExceptionTableRecord::Read(address, rv, memory);
    id = memory.ReadTetra(rv, address, cmsx::machine::Protection::read);
    offset = memory.ReadTetra(rv, address + 4, cmsx::machine::Protection::read);
    length = memory.ReadTetra(rv, address + 8, cmsx::machine::Protection::read);
    parentTryRecordId = memory.ReadTetra(rv, address + 12, cmsx::machine::Protection::read);
}

void TryRecord::AddHandlerRecord(HandlerRecord* handlerRecord)
{
    handlerRecords.push_back(handlerRecord);
}

void TryRecord::AddCleanupRecord(CleanupRecord* cleanupRecord)
{
    cleanupRecords.push_back(cleanupRecord);
}

void TryRecord::AddChildRecord(TryRecord* childRecord)
{
    childRecords.push_back(childRecord);
}

TryRecord* TryRecord::SearchRecord(uint32_t offset) 
{
    for (TryRecord* childRecord : childRecords)
    {
        if (offset >= childRecord->Offset() && offset < childRecord->Offset() + childRecord->Length())
        {
            return childRecord->SearchRecord(offset);
        }
    }
    return this;
}

void TryRecord::Setup(ExceptionTable* exceptionTable)
{
    ExceptionTableRecord::Setup(exceptionTable);
    if (parentTryRecordId != -1)
    {
        parentTryRecord = exceptionTable->GetTryRecord(parentTryRecordId);
        parentTryRecord->AddChildRecord(this);
    }
}

HandlerRecord::HandlerRecord() : ExceptionTableRecord(ExceptionTableRecordKind::handlerRecord), caughtClassId(), handlerAddress(), tryRecordId(), tryRecord(nullptr)
{
}

HandlerRecord::HandlerRecord(uint64_t caughtClassId_, uint64_t handlerAddress_, uint32_t tryRecordId_) :
    ExceptionTableRecord(ExceptionTableRecordKind::handlerRecord), caughtClassId(caughtClassId_), handlerAddress(handlerAddress_), tryRecordId(tryRecordId_), tryRecord(nullptr)
{
}

void HandlerRecord::Write(Section* section)
{
    ExceptionTableRecord::Write(section);
    uint32_t caughtClassIdHi = static_cast<uint32_t>(caughtClassId >> 32);
    uint32_t caughtClassIdLo = static_cast<uint32_t>(caughtClassId);
    section->EmitTetra(caughtClassIdHi);
    section->EmitTetra(caughtClassIdLo);
    uint32_t handlerAddressHi = static_cast<uint32_t>(handlerAddress >> 32);
    uint32_t handlerAddressLo = static_cast<uint32_t>(handlerAddress);
    section->EmitTetra(handlerAddressHi);
    section->EmitTetra(handlerAddressLo);
    section->EmitTetra(tryRecordId);
}

void HandlerRecord::Read(int64_t address, uint64_t rv, cmsx::machine::Memory& memory)
{
    ExceptionTableRecord::Read(address, rv, memory);
    uint32_t caughtClassIdHi = memory.ReadTetra(rv, address, cmsx::machine::Protection::read);
    uint32_t caughtClassIdLo = memory.ReadTetra(rv, address + 4, cmsx::machine::Protection::read);
    caughtClassId = (static_cast<uint64_t>(caughtClassIdHi) << 32) | static_cast<uint64_t>(caughtClassIdLo);
    uint32_t handlerAddressHi = memory.ReadTetra(rv, address + 8, cmsx::machine::Protection::read);
    uint32_t handlerAddressLo = memory.ReadTetra(rv, address + 12, cmsx::machine::Protection::read);
    handlerAddress = (static_cast<uint64_t>(handlerAddressHi) << 32) | static_cast<uint64_t>(handlerAddressLo);
    tryRecordId = memory.ReadTetra(rv, address + 16, cmsx::machine::Protection::read);
}

void HandlerRecord::Setup(ExceptionTable* exceptionTable)
{
    ExceptionTableRecord::Setup(exceptionTable);
    if (tryRecordId != -1)
    {
        tryRecord = exceptionTable->GetTryRecord(tryRecordId);
        tryRecord->AddHandlerRecord(this);
    }
}

CleanupRecord::CleanupRecord() : ExceptionTableRecord(ExceptionTableRecordKind::cleanupRecord), offset(), length(), cleanupAddress(), tryRecordId(), tryRecord(nullptr)
{
}

CleanupRecord::CleanupRecord(uint32_t offset_, uint64_t cleanupAddress_, uint32_t tryRecordId_) : 
    ExceptionTableRecord(ExceptionTableRecordKind::cleanupRecord), offset(offset_), length(0), cleanupAddress(cleanupAddress_), tryRecordId(tryRecordId_), tryRecord(nullptr)
{
}

void CleanupRecord::Write(Section* section)
{
    ExceptionTableRecord::Write(section);
    section->EmitTetra(offset);
    section->EmitTetra(length);
    uint32_t cleanupAddressHi = static_cast<uint32_t>(cleanupAddress >> 32);
    uint32_t cleanupAddressLo = static_cast<uint32_t>(cleanupAddress);
    section->EmitTetra(cleanupAddressHi);
    section->EmitTetra(cleanupAddressLo);
    section->EmitTetra(tryRecordId);
}

void CleanupRecord::Read(int64_t address, uint64_t rv, cmsx::machine::Memory& memory)
{
    ExceptionTableRecord::Read(address, rv, memory);
    offset = memory.ReadTetra(rv, address, cmsx::machine::Protection::read);
    length = memory.ReadTetra(rv, address + 4, cmsx::machine::Protection::read);
    uint32_t cleanupAddressHi = memory.ReadTetra(rv, address + 8, cmsx::machine::Protection::read);
    uint32_t cleanupAddressLo = memory.ReadTetra(rv, address + 12, cmsx::machine::Protection::read);
    cleanupAddress = (static_cast<uint64_t>(cleanupAddressHi) << 32) | static_cast<uint64_t>(cleanupAddressLo);
    tryRecordId = memory.ReadTetra(rv, address + 16, cmsx::machine::Protection::read);
}

void CleanupRecord::Setup(ExceptionTable* exceptionTable)
{
    ExceptionTableRecord::Setup(exceptionTable);
    if (tryRecordId != -1)
    {
        tryRecord = exceptionTable->GetTryRecord(tryRecordId);
        tryRecord->AddCleanupRecord(this);
    }
}

ExceptionTable::ExceptionTable() : functionTableEntry(nullptr)
{
}

void ExceptionTable::Write(Section* section)
{
    section->Align(4);
    section->EmitTetra(static_cast<uint32_t>(records.size()));
    for (const auto& record : records)
    {
        record->Write(section);
    }
}

void ExceptionTable::Read(int64_t address, uint64_t rv, cmsx::machine::Memory& memory)
{
    uint32_t count = memory.ReadTetra(rv, address, cmsx::machine::Protection::read);
    int64_t addr = address + 4;
    for (uint32_t i = 0; i < count; ++i)
    {
        ExceptionTableRecord* record = ReadExceptionTableRecord(addr, rv, memory);
        records.push_back(std::unique_ptr<ExceptionTableRecord>(record));
        if (record->Kind() == ExceptionTableRecordKind::tryRecord)
        {
            TryRecord* tryRecord = static_cast<TryRecord*>(record);
            tryRecordMap[tryRecord->Id()] = tryRecord;
        }
        addr = addr + record->Size();
    }
    for (auto& record : records)
    {
        record->Setup(this);
    }
}

void ExceptionTable::AddRecord(ExceptionTableRecord* record)
{
    records.push_back(std::unique_ptr<ExceptionTableRecord>(record));
}

TryRecord* ExceptionTable::GetTryRecord(uint32_t id) const
{
    auto it = tryRecordMap.find(id);
    if (it != tryRecordMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

ExceptionTableRecord* ExceptionTable::SearchRecord(uint32_t offset) const
{
    for (const auto& record : records)
    {
        switch (record->Kind())
        {
            case ExceptionTableRecordKind::tryRecord:
            {
                TryRecord* tryRecord = static_cast<TryRecord*>(record.get());
                if (offset >= tryRecord->Offset() && offset < tryRecord->Offset() + tryRecord->Length())
                {
                    return tryRecord->SearchRecord(offset);
                }
                break;
            }
            case ExceptionTableRecordKind::cleanupRecord:
            {
                CleanupRecord* cleanupRecord = static_cast<CleanupRecord*>(record.get());
                if (offset >= cleanupRecord->Offset() && offset < cleanupRecord->Offset() + cleanupRecord->Length())
                {
                    return cleanupRecord;
                }
                break;
            }
        }
    }
    return nullptr;
}

FunctionTableEntry::FunctionTableEntry() : 
    functionStart(), functionLength(), id(), fullNameId(), mangledNameId(), sourceFileNameId(), fullName(), mangledName(), sourceFileName(), frameSize()
{
    exceptionTable.SetFunctionTableEntry(this);
}

int64_t FunctionTableEntry::Write(StringTable& stringTable, Section* section)
{
    section->Align(8);
    int64_t address = section->BaseAddress() + section->Address();
    section->EmitOcta(functionStart);
    section->EmitOcta(functionLength);
    section->EmitOcta(frameSize);
    section->EmitTetra(id);
    fullNameId = stringTable.AddString(fullName);
    section->EmitTetra(fullNameId);
    mangledNameId = stringTable.AddString(mangledName);
    section->EmitTetra(mangledNameId);
    sourceFileNameId = stringTable.AddString(sourceFileName);
    section->EmitTetra(sourceFileNameId);
    lineNumberTable.Write(section);
    exceptionTable.Write(section);
    return address;
}

void FunctionTableEntry::Read(StringTable& stringTable, SymbolTable& symbolTable, int64_t address, uint64_t rv, cmsx::machine::Memory& memory)
{
    functionStart = memory.ReadOcta(rv, address, cmsx::machine::Protection::read);
    functionLength = memory.ReadOcta(rv, address + 8, cmsx::machine::Protection::read);
    frameSize = memory.ReadOcta(rv, address + 16, cmsx::machine::Protection::read);
    id = memory.ReadTetra(rv, address + 24, cmsx::machine::Protection::read);
    fullNameId = memory.ReadTetra(rv, address + 28, cmsx::machine::Protection::read);
    mangledNameId = memory.ReadTetra(rv, address + 32, cmsx::machine::Protection::read);
    sourceFileNameId = memory.ReadTetra(rv, address + 36, cmsx::machine::Protection::read);
    int64_t addr = lineNumberTable.Read(address + 40, rv, memory);
    exceptionTable.Read(addr, rv, memory);
    fullName = stringTable.GetString(symbolTable, fullNameId, rv, memory);
    mangledName = stringTable.GetString(symbolTable, mangledNameId, rv, memory);
    sourceFileName = stringTable.GetString(symbolTable, sourceFileNameId, rv, memory);
}

void FunctionTableEntry::SetFullName(const std::string& fullName_)
{
    fullName = fullName_;
}

void FunctionTableEntry::SetMangledName(const std::string& mangledName_)
{
    mangledName = mangledName_;
}

void FunctionTableEntry::SetSourceFileName(const std::string& sourceFileName_)
{
    sourceFileName = sourceFileName_;
}

int32_t FunctionTableEntry::SearchLineNumber(uint64_t pc) const
{
    uint32_t offset = pc - functionStart;
    return lineNumberTable.SearchLineNumber(offset);
}

ExceptionTableRecord* FunctionTableEntry::SearchExceptionTableRecord(uint64_t pc) const
{
    uint32_t offset = pc - functionStart;
    return exceptionTable.SearchRecord(offset);
}

FunctionTable::FunctionTable() : indexRead(false), index(), stringTable()
{
}

void FunctionTable::AddEntry(FunctionTableEntry* entry)
{
    AddEntry(entry, true);
}

void FunctionTable::AddEntry(FunctionTableEntry* entry, bool setId)
{
    if (setId)
    {
        entry->SetId(entries.size());
    }
    entryMap[entry->Id()] = entry;
    entries.push_back(std::unique_ptr<FunctionTableEntry>(entry));
}

void FunctionTable::ReadIndex(SymbolTable& symbolTable, uint64_t rv, cmsx::machine::Memory& memory)
{
    indexRead = true;
    index.Read(symbolTable, rv, memory);
}

FunctionTableEntry* FunctionTable::GetEntry(uint64_t pc, SymbolTable& symbolTable, uint64_t rv, cmsx::machine::Memory& memory)
{
    if (!indexRead)
    {
        ReadIndex(symbolTable, rv, memory);
    }
    FunctionTableIndexEntry* indexEntry = index.SearchEntry(pc);
    if (indexEntry)
    {
        auto it = entryMap.find(indexEntry->entryId);
        if (it != entryMap.cend())
        {
            return it->second;
        }
        uint64_t address = indexEntry->entryAddress;
        FunctionTableEntry* entry = new FunctionTableEntry();
        entry->Read(stringTable, symbolTable, address, rv, memory);
        AddEntry(entry, false);
        return entry;
    }
    return nullptr;
}

void FunctionTable::Write(BinaryFile& binaryFile)
{
    for (const auto& entry : entries)
    {
        FunctionTableIndexEntry indexEntry;
        indexEntry.functionStart = entry->FunctionStart();
        indexEntry.functionLength = entry->FunctionLength();
        indexEntry.entryId = entry->Id();
        int64_t entryAddress = entry->Write(stringTable, binaryFile.GetDataSection());
        indexEntry.entryAddress = entryAddress;
        index.AddEntry(indexEntry);
    }
    index.Sort();
    index.Write(binaryFile);
    stringTable.Write(binaryFile);
    binaryFile.GetDataSection()->Align(8);
}

struct FunctionDebugRecordCollection
{
    FunctionDebugRecordCollection(uint32_t functionSymbolIndex_, Symbol* functionExecutableSymbol_);
    uint32_t functionSymbolIndex;
    Symbol* functionExecutableSymbol;
    std::vector<DebugRecord*> debugRecords;
};

FunctionDebugRecordCollection::FunctionDebugRecordCollection(uint32_t functionSymbolIndex_, Symbol* functionExecutableSymbol_) :
    functionSymbolIndex(functionSymbolIndex_), functionExecutableSymbol(functionExecutableSymbol_)
{
}

void ProcessFileInfoRecord(FileInfoRecord* fileInfoRecord, std::map<uint32_t, FileInfoRecord*>& fileInfoRecordMap)
{
    fileInfoRecordMap[fileInfoRecord->Id()] = fileInfoRecord;
}

void ProcessFuncInfoRecord(FuncInfoRecord* funcInfoRecord, std::map<uint32_t, FuncInfoRecord*>& funcInfoRecordMap)
{
    funcInfoRecordMap[funcInfoRecord->FunctionSymbolIndex()] = funcInfoRecord;
}

void ProcessStartFuncRecord(StartFuncRecord* startFuncRecord, ExecutableFile& executable, ObjectFile* objectFile, std::unique_ptr<FunctionDebugRecordCollection>& collection,
    std::stack<bool>& processStack, bool& process)
{
    processStack.push(process);
    Symbol* symbol = objectFile->GetSymbolTable().GetSymbol(static_cast<int>(startFuncRecord->FunctionSymbolIndex()));
    if (!symbol->IsRemoved())
    {
        process = true;
        Symbol* executableSymbol = executable.GetSymbolTable().GetSymbol(symbol->FullName());
        if (executableSymbol)
        {
            collection.reset(new FunctionDebugRecordCollection(startFuncRecord->FunctionSymbolIndex(), executableSymbol));
        }
        else
        {
            throw std::runtime_error("symbol '" + symbol->FullName() + "' not found from executable");
        }
    }
    else
    {
        process = false;
    }
}

void ProcessEndFuncRecord(EndFuncRecord* endFuncRecord, 
    std::vector<std::unique_ptr<FunctionDebugRecordCollection>>& collections, std::unique_ptr<FunctionDebugRecordCollection>& collection,
    std::stack<bool>& processStack, bool& process)
{
    if (process)
    {
        collections.push_back(std::move(collection));
    }
    process = processStack.top();
    processStack.pop();
}

void SelectLineInfoRecord(LineInfoRecord* lineInfoRecord, FunctionDebugRecordCollection* collection, bool process)
{
    if (process)
    {
        collection->debugRecords.push_back(lineInfoRecord);
    }
}

void SelectBeginTryRecord(BeginTryRecord* beginTryRecord, FunctionDebugRecordCollection* collection, bool process)
{
    if (process)
    {
        collection->debugRecords.push_back(beginTryRecord);
    }
}

void SelectEndTryRecord(EndTryRecord* endTryRecord, FunctionDebugRecordCollection* collection, bool process)
{
    if (process)
    {
        collection->debugRecords.push_back(endTryRecord);
    }
}

void SelectCatchRecord(CatchRecord* catchRecord, FunctionDebugRecordCollection* collection, bool process)
{
    if (process)
    {
        collection->debugRecords.push_back(catchRecord);
    }
}

void SelectBeginCleanupRecord(BeginCleanupRecord* beginCleanupRecord, FunctionDebugRecordCollection* collection, bool process)
{
    if (process)
    {
        collection->debugRecords.push_back(beginCleanupRecord);
    }
}

void SelectEndCleanupRecord(EndCleanupRecord* endCleanupRecord, FunctionDebugRecordCollection* collection, bool process)
{
    if (process)
    {
        collection->debugRecords.push_back(endCleanupRecord);
    }
}

void SelectDebugRecord(DebugRecord* debugRecord, ExecutableFile& executable, ObjectFile* objectFile,
    std::map<uint32_t, FileInfoRecord*>& fileInfoRecordMap, std::map<uint32_t, FuncInfoRecord*>& funcInfoRecordMap,
    std::vector<std::unique_ptr<FunctionDebugRecordCollection>>& collections, std::unique_ptr<FunctionDebugRecordCollection>& collection, 
    std::stack<bool>& processStack, bool& process)
{
    switch (debugRecord->Kind())
    {
        case DebugRecordKind::fileInfo:
        {
            ProcessFileInfoRecord(static_cast<FileInfoRecord*>(debugRecord), fileInfoRecordMap);
            break;
        }
        case DebugRecordKind::funcInfo:
        {
            ProcessFuncInfoRecord(static_cast<FuncInfoRecord*>(debugRecord), funcInfoRecordMap);
            break;
        }
        case DebugRecordKind::startFunc:
        {
            ProcessStartFuncRecord(static_cast<StartFuncRecord*>(debugRecord), executable, objectFile, collection, processStack, process);
            break;
        }
        case DebugRecordKind::endFunc:
        {
            ProcessEndFuncRecord(static_cast<EndFuncRecord*>(debugRecord), collections, collection, processStack, process);
            break;
        }
        case DebugRecordKind::lineInfo:
        {
            SelectLineInfoRecord(static_cast<LineInfoRecord*>(debugRecord), collection.get(), process);
            break;
        }
        case DebugRecordKind::beginTry:
        {
            SelectBeginTryRecord(static_cast<BeginTryRecord*>(debugRecord), collection.get(), process);
            break;
        }
        case DebugRecordKind::endTry:
        {
            SelectEndTryRecord(static_cast<EndTryRecord*>(debugRecord), collection.get(), process);
            break;
        }
        case DebugRecordKind::catch_:
        {
            SelectCatchRecord(static_cast<CatchRecord*>(debugRecord), collection.get(), process);
            break;
        }
        case DebugRecordKind::beginCleanup:
        {
            SelectBeginCleanupRecord(static_cast<BeginCleanupRecord*>(debugRecord), collection.get(), process);
            break;
        }
        case DebugRecordKind::endCleanup:
        {
            SelectEndCleanupRecord(static_cast<EndCleanupRecord*>(debugRecord), collection.get(), process);
            break;
        }
    }
}

void ProcessLineInfoRecord(LineInfoRecord* lineInfoRecord, FunctionTableEntry* functionTableEntry)
{
    functionTableEntry->GetLineNumberTable().AddEntry(LineNumberTableEntry(lineInfoRecord->Offset(), lineInfoRecord->LineNumber()));
}

void ProcessBeginTryRecord(BeginTryRecord* beginTryRecord, FunctionTableEntry* functionTableEntry, std::map<uint32_t, TryRecord*>& tryRecordMap)
{
    TryRecord* tryRecord = new TryRecord(beginTryRecord->TryBlockId(), beginTryRecord->Offset(), beginTryRecord->ParentTryBlockId());
    tryRecordMap[beginTryRecord->TryBlockId()] = tryRecord;
    functionTableEntry->GetExceptionTable().AddRecord(tryRecord);
}

void ProcessEndTryRecord(EndTryRecord* endTryRecord, std::map<uint32_t, TryRecord*>& tryRecordMap)
{
    auto it = tryRecordMap.find(endTryRecord->TryBlockId());
    if (it != tryRecordMap.cend())
    {
        TryRecord* tryRecord = it->second;
        tryRecord->SetLength(endTryRecord->Offset() - tryRecord->Offset());
    }
    else
    {
        throw std::runtime_error("try record id '" + std::to_string(endTryRecord->TryBlockId()) + "' not found");
    }
}

void ProcessCatchRecord(CatchRecord* catchRecord, FunctionTableEntry* functionTableEntry, LinkTable& linkTable, Symbol* functionExecutableSymbol, ExecutableFile& executable)
{
    boost::uuids::uuid typeId;
    IntsToUuid(catchRecord->CaughtTypeId1(), catchRecord->CaughtTypeId2(), typeId);
    uint64_t caughtClassId = linkTable.GetClassId(typeId);
    std::string handlerSymbolName = functionExecutableSymbol->FullName() + "@" + std::to_string(catchRecord->CatchBlockId());
    Symbol* handlerSymbol = executable.GetSymbolTable().GetSymbol(handlerSymbolName);
    if (handlerSymbol)
    {
        int64_t handlerAddress = handlerSymbol->Start();
        HandlerRecord* handlerRecord = new HandlerRecord(caughtClassId, handlerAddress, catchRecord->TryBlockId());
        functionTableEntry->GetExceptionTable().AddRecord(handlerRecord);
    }
    else
    {
        throw std::runtime_error("handler symbol '" + handlerSymbolName + "' not found from executable");
    }
}

void ProcessBeginCleanupRecord(BeginCleanupRecord* beginCleanupRecord, FunctionTableEntry* functionTableEntry,
    std::map<uint32_t, CleanupRecord*>& cleanupRecordMap, Symbol* functionExecutableSymbol, ExecutableFile& executable)
{
    std::string cleanupSymbolName = functionExecutableSymbol->FullName() + "@" + std::to_string(beginCleanupRecord->CleanupBlockId());
    Symbol* cleanupSymbol = executable.GetSymbolTable().GetSymbol(cleanupSymbolName);
    if (cleanupSymbol)
    {
        int64_t cleanupAddress = cleanupSymbol->Start();
        CleanupRecord* cleanupRecord = new CleanupRecord(beginCleanupRecord->Offset(), cleanupAddress, beginCleanupRecord->TryBlockId());
        cleanupRecordMap[beginCleanupRecord->CleanupBlockId()] = cleanupRecord;
        functionTableEntry->GetExceptionTable().AddRecord(cleanupRecord);
    }
    else
    {
        throw std::runtime_error("cleanup symbol '" + cleanupSymbolName + "' not found from executable");
    }
}

void ProcessEndCleanupRecord(EndCleanupRecord* endCleanupRecord, std::map<uint32_t, CleanupRecord*>& cleanupRecordMap)
{
    auto it = cleanupRecordMap.find(endCleanupRecord->CleanupBlockId());
    if (it != cleanupRecordMap.cend())
    {
        CleanupRecord* cleanupRecord = it->second;
        cleanupRecord->SetLength(endCleanupRecord->Offset() - cleanupRecord->Offset());
    }
    else
    {
        throw std::runtime_error("cleanup record id '" + std::to_string(endCleanupRecord->CleanupBlockId()) + "' not found");
    }
}

void ProcessDebugRecord(DebugRecord* debugRecord, FunctionTableEntry* functionTableEntry, 
    std::map<uint32_t, TryRecord*>& tryRecordMap, std::map<uint32_t, CleanupRecord*>& cleanupRecordMap, LinkTable& linkTable,
    Symbol* functionExecutableSymbol, ExecutableFile& executable)
{
    switch (debugRecord->Kind())
    {
        case DebugRecordKind::lineInfo:
        {
            ProcessLineInfoRecord(static_cast<LineInfoRecord*>(debugRecord), functionTableEntry);
            break;
        }
        case DebugRecordKind::beginTry:
        {
            ProcessBeginTryRecord(static_cast<BeginTryRecord*>(debugRecord), functionTableEntry, tryRecordMap);
            break;
        }
        case DebugRecordKind::endTry:
        {
            ProcessEndTryRecord(static_cast<EndTryRecord*>(debugRecord), tryRecordMap);
            break;
        }
        case DebugRecordKind::catch_:
        {
            ProcessCatchRecord(static_cast<CatchRecord*>(debugRecord), functionTableEntry, linkTable, functionExecutableSymbol, executable);
            break;
        }
        case DebugRecordKind::beginCleanup:
        {
            ProcessBeginCleanupRecord(static_cast<BeginCleanupRecord*>(debugRecord), functionTableEntry, cleanupRecordMap, functionExecutableSymbol, executable);
            break;
        }
        case DebugRecordKind::endCleanup:
        {
            ProcessEndCleanupRecord(static_cast<EndCleanupRecord*>(debugRecord), cleanupRecordMap);
            break;
        }
    }
}

void ProcessDebugRecords(FunctionTable& functionTable, LinkTable& linkTable, ExecutableFile& executable, ObjectFile* objectFile, 
    FunctionDebugRecordCollection* functionDebugRecordCollection,
    std::map<uint32_t, FileInfoRecord*>& fileInfoRecordMap, std::map<uint32_t, FuncInfoRecord*>& funcInfoRecordMap)
{
    Symbol* functionExecutableSymbol = functionDebugRecordCollection->functionExecutableSymbol;
    std::string functionFullName;
    std::string sourceFileName;
    int64_t frameSize = 8;
    auto it = funcInfoRecordMap.find(functionDebugRecordCollection->functionSymbolIndex);
    if (it != funcInfoRecordMap.cend())
    {
        FuncInfoRecord* funcInfoRecord = it->second;
        functionFullName = funcInfoRecord->FullName();
        frameSize = funcInfoRecord->FrameSize();
        uint32_t sourceFileNameId = funcInfoRecord->SourceFileNameId();
        auto it2 = fileInfoRecordMap.find(sourceFileNameId);
        if (it2 != fileInfoRecordMap.cend())
        {
            FileInfoRecord* fileInfoRecord = it2->second;
            sourceFileName = fileInfoRecord->SourceFileName();
        }
    }
    FunctionTableEntry* functionTableEntry = new FunctionTableEntry();
    functionTableEntry->SetFunctionStart(functionExecutableSymbol->Start());
    functionTableEntry->SetFunctionLength(functionExecutableSymbol->Length());
    functionTableEntry->SetFrameSize(frameSize);
    functionTableEntry->SetFullName(functionFullName);
    functionTableEntry->SetMangledName(functionExecutableSymbol->FullName());
    functionTableEntry->SetSourceFileName(sourceFileName);
    functionTable.AddEntry(functionTableEntry);
    std::map<uint32_t, TryRecord*> tryRecordMap;
    std::map<uint32_t, CleanupRecord*> cleanupRecordMap;
    for (DebugRecord* debugRecord : functionDebugRecordCollection->debugRecords)
    {
        ProcessDebugRecord(debugRecord, functionTableEntry, tryRecordMap, cleanupRecordMap, linkTable, functionExecutableSymbol, executable);
    }
}

void ProcessObjectFileDebugSection(FunctionTable& functionTable, LinkTable& linkTable, ExecutableFile& executable, ObjectFile* objectFile)
{
    std::map<uint32_t, FileInfoRecord*> fileInfoRecordMap;
    std::map<uint32_t, FuncInfoRecord*> funcInfoRecordMap;
    std::vector<std::unique_ptr<FunctionDebugRecordCollection>> collections;
    std::unique_ptr<FunctionDebugRecordCollection> collection;
    std::stack<bool> processStack;
    bool process = false;
    DebugSection* debugSection = objectFile->GetDebugSection();
    for (const auto& debugRecord : debugSection->DebugRecords())
    {
        SelectDebugRecord(debugRecord.get(), executable, objectFile, fileInfoRecordMap, funcInfoRecordMap, collections, collection, processStack, process);
    }
    for (auto& functionDebugRecordCollection : collections)
    {
        ProcessDebugRecords(functionTable, linkTable, executable, objectFile, functionDebugRecordCollection.get(), fileInfoRecordMap, funcInfoRecordMap);
    }
}

void MakeFunctionTable(const std::vector<std::unique_ptr<BinaryFile>>& binaryFiles, ExecutableFile& executable, LinkTable& linkTable)
{
    FunctionTable functionTable;
    for (const auto& binaryFile : binaryFiles)
    {
        if (binaryFile->Kind() == BinaryFileKind::objectFile)
        {
            ObjectFile* objectFile = static_cast<ObjectFile*>(binaryFile.get());
            ProcessObjectFileDebugSection(functionTable, linkTable, executable, objectFile);
        }
        else if (binaryFile->Kind() == BinaryFileKind::archiveFile)
        {
            ArchiveFile* archiveFile = static_cast<ArchiveFile*>(binaryFile.get());
            for (const auto& objectFile : archiveFile->ObjectFiles())
            {
                ProcessObjectFileDebugSection(functionTable, linkTable, executable, objectFile.get());
            }
        }
    }
    functionTable.Write(executable);
}

} // namespace cmsx::object
;