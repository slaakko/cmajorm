// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/LinkTable.hpp>
#include <system-x/object/Symbol.hpp>
#include <system-x/object/Link.hpp>
#include <system-x/object/BinaryFile.hpp>
#include <soulng/util/Log.hpp>
#include <stdexcept>

namespace cmsx::object {

using namespace soulng::util;

UnprocessedLinkCommand::UnprocessedLinkCommand(ObjectFile* objectFile_, LinkCommand* linkCommand_) : objectFile(objectFile_), linkCommand(linkCommand_)
{
}

UnprocessedSymbol::UnprocessedSymbol(Symbol* symbol_, LinkSection* linkSection_, SymbolTable* symbolTable_) : symbol(symbol_), linkSection(linkSection_), symbolTable(symbolTable_)
{
}

LinkTable::LinkTable()
{
}

void LinkTable::AddUsedSymbolName(const std::string& symbolName)
{
    usedSymbolNames.insert(symbolName);
}

bool LinkTable::IsUsedSymbolName(const std::string& symbolName) const
{
    return usedSymbolNames.find(symbolName) != usedSymbolNames.cend();
}

void LinkTable::AddUnprocessedSymbol(Symbol* symbol, LinkSection* linkSection, SymbolTable* symbolTable)
{
    unprocessedSymbolMap[symbol->FullName()].push_back(UnprocessedSymbol(symbol, linkSection, symbolTable));
}

const std::vector<UnprocessedSymbol>* LinkTable::GetUnprocessedSymbols(const std::string& symbolName) const
{
    auto it = unprocessedSymbolMap.find(symbolName);
    if (it != unprocessedSymbolMap.cend())
    {
        return &it->second;
    }
    else
    {
        return nullptr;
    }
}

void LinkTable::ClearUnprocessedSymbols(const std::string& symbolName)
{
    unprocessedSymbolMap.erase(symbolName);
}

void LinkTable::ClearUnprocessedSymbols()
{
    unprocessedSymbolMap.clear();
}

void LinkTable::ExecuteLinkCommands(const std::string& symbolName, int64_t start)
{
    auto it = unprocessedLinkCommandMap.find(symbolName);
    if (it != unprocessedLinkCommandMap.cend())
    {
        const std::vector<UnprocessedLinkCommand>& unprocessedLinkCommands = it->second;
        for (auto& unprocessedLinkCommand : unprocessedLinkCommands)
        {
            unprocessedLinkCommand.GetLinkCommand()->Apply(unprocessedLinkCommand.GetObjectFile(), start);
        }
        unprocessedLinkCommandMap.erase(symbolName);
    }
}

void LinkTable::AddCopyRange(const CopyRange& copyRange)
{
    if (copyRange.Length() == 0) return;
    if (!copyRanges.empty())
    {
        CopyRange& prev = copyRanges.back();
        if (prev.FromSection() == copyRange.FromSection() && prev.ToSection() == copyRange.ToSection() && prev.StartPos() + prev.Length() == copyRange.StartPos())
        {
            prev.SetLength(prev.Length() + copyRange.Length());
            return;
        }
    }
    copyRanges.push_back(copyRange);
}

uint64_t LinkTable::GetClassId(const boost::uuids::uuid& typeId) const
{
    // todo
    return 0;
}

void LinkTable::AddLinkCommand(const std::string& symbolName, ObjectFile* objectFile, LinkCommand* linkCommand)
{
    std::vector<UnprocessedLinkCommand>& linkCommands = unprocessedLinkCommandMap[symbolName];
    linkCommands.push_back(UnprocessedLinkCommand(objectFile, linkCommand));
}

void LinkTable::CheckUnresolvedSymbols(int logStreamId)
{
    bool unresolvedSymbols = false;
    for (const auto& p : unprocessedLinkCommandMap)
    {
        const std::string& symbolName = p.first;
        for (const auto& unprocessedLinkCommand : p.second)
        {
            ObjectFile* objectFile = unprocessedLinkCommand.GetObjectFile();
            LogMessage(logStreamId, "unresolved extern symbol '" + symbolName + "' detected in object file '" + objectFile->FileName() + "'");
            unresolvedSymbols = true;
        }
    }
    if (unresolvedSymbols)
    {
        throw std::runtime_error("unresolved symbols detected");
    }
}

} // namespace cmsx::object
