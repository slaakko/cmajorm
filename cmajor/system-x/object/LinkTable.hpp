// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_OBJECT_LINK_TABLE_INCLUDED
#define CMSX_OBJECT_LINK_TABLE_INCLUDED
#include <system-x/object/CopyRange.hpp>
#include <boost/uuid/uuid.hpp>
#include <memory>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace cmsx::object {

class Symbol;
class LinkSection;
class SymbolTable;
class LinkCommand;
class ObjectFile;

class CMSX_OBJECT_API UnprocessedLinkCommand
{
public:
    UnprocessedLinkCommand(ObjectFile* objectFile_, LinkCommand* linkCommand_);
    ObjectFile* GetObjectFile() const { return objectFile; }
    LinkCommand* GetLinkCommand() const { return linkCommand; }
private:
    ObjectFile* objectFile;
    LinkCommand* linkCommand;
};

class CMSX_OBJECT_API UnprocessedSymbol
{
public:
    UnprocessedSymbol(Symbol* symbol_, LinkSection* linkSection_, SymbolTable* symbolTable_);
    Symbol* GetSymbol() const { return symbol;  }
    LinkSection* GetLinkSection() const { return linkSection; }
    SymbolTable* GetSymbolTable() const { return symbolTable; }
private:
    Symbol* symbol;
    LinkSection* linkSection;
    SymbolTable* symbolTable;
};

class CMSX_OBJECT_API LinkTable
{
public:
    LinkTable();
    LinkTable(const LinkTable&) = delete;
    LinkTable& operator=(const LinkTable&) = delete;
    void AddUsedSymbolName(const std::string& symbolName);
    bool IsUsedSymbolName(const std::string& symbolName) const;
    void AddUnprocessedSymbol(Symbol* symbol, LinkSection* linkSection, SymbolTable* symbolTable);
    const std::vector<UnprocessedSymbol>* GetUnprocessedSymbols(const std::string& symbolName) const;
    void ClearUnprocessedSymbols(const std::string& symbolName);
    void ClearUnprocessedSymbols();
    void ExecuteLinkCommands(const std::string& symbolName, int64_t start);
    void AddCopyRange(const CopyRange& copyRange);
    uint64_t GetClassId(const boost::uuids::uuid& typeId) const;
    void AddLinkCommand(const std::string& symbolName, ObjectFile* objectFile, LinkCommand* linkCommand);
    void CheckUnresolvedSymbols(int logStreamId);
    const std::vector<CopyRange>& CopyRanges() const { return copyRanges; }
private:
    std::set<std::string> usedSymbolNames;
    std::map<std::string, std::vector<UnprocessedSymbol>> unprocessedSymbolMap;
    std::vector<CopyRange> copyRanges;
    std::map<std::string, std::vector<UnprocessedLinkCommand>> unprocessedLinkCommandMap;
};

} // namespace cmsx::object

#endif // CMSX_OBJECT_LINK_TABLE_INCLUDED
