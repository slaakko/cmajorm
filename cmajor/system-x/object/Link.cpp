// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/Link.hpp>
#include <system-x/object/LinkTable.hpp>
#include <system-x/object/BinaryFile.hpp>
#include <system-x/object/FunctionTable.hpp>
#include <system-x/machine/Memory.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Uuid.hpp>
#include <soulng/util/Util.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <stdexcept>

namespace cmsx::object {

using namespace soulng::util;

std::string LinkCodeStr(LinkCode linkCode)
{
    switch (linkCode)
    {
        case LinkCode::once:
        {
            return "ONCE";
        }
        case LinkCode::forwardLongJump:
        {
            return "FORWARD_LONG_JUMP";
        }
        case LinkCode::forwardShortJump:
        {
            return "FORWARD_SHORT_JUMP";
        }
        case LinkCode::absoluteAddrValue:
        {
            return "ABSOLUTE_ADDR_VALUE";
        }
        case LinkCode::farOcta:
        {
            return "FAR_OCTA";
        }
        case LinkCode::clsid:
        {
            return "CLSID";
        }
        case LinkCode::end:
        {
            return "END";
        }
    }
    return std::string();
}

LinkCommand::LinkCommand(LinkCode linkCode_, uint64_t address_) : linkCode(linkCode_), address(address_), id(-1)
{
}

LinkCommand::LinkCommand(LinkCode linkCode_) : linkCode(linkCode_), address(0), id(-1)
{
}

LinkCommand::~LinkCommand()
{
}

void LinkCommand::Emit(LinkSection* linkSection)
{
    linkSection->EmitByte(static_cast<uint8_t>(linkCode));
    linkSection->EmitOcta(address);
    linkSection->EmitTetra(static_cast<uint32_t>(id));
}

void LinkCommand::Read(LinkSection* linkSection)
{
    address = linkSection->ReadOcta();
    id = static_cast<int32_t>(linkSection->ReadTetra());
}

void LinkCommand::Apply(ObjectFile* objectFile, uint64_t value)
{
}

LinkOnceCommand::LinkOnceCommand() : LinkCommand(LinkCode::once, 0)
{
}

LinkCommand* LinkOnceCommand::Clone() const
{
    LinkCommand* command = new LinkOnceCommand();
    command->SetId(Id());
    return command;
}

std::string LinkOnceCommand::ToString() const 
{
    return LinkCodeStr(Code()) + "(address=#" + ToHexString(Address()) + ", id=" + std::to_string(Id()) + ")"; 
}

LinkForwardLongJumpCommand::LinkForwardLongJumpCommand() : LinkCommand(LinkCode::forwardLongJump), symbolIndex(-1)
{
}

LinkForwardLongJumpCommand::LinkForwardLongJumpCommand(uint64_t address_, int32_t symbolIndex_)  : LinkCommand(LinkCode::forwardLongJump, address_), symbolIndex(symbolIndex_)
{
}

void LinkForwardLongJumpCommand::Emit(LinkSection* linkSection)
{
    LinkCommand::Emit(linkSection);
    linkSection->EmitTetra(static_cast<uint32_t>(symbolIndex));
}

void LinkForwardLongJumpCommand::Read(LinkSection* linkSection)
{
    LinkCommand::Read(linkSection);
    symbolIndex = static_cast<int32_t>(linkSection->ReadTetra());
}

void LinkForwardLongJumpCommand::Apply(ObjectFile* objectFile, uint64_t value)
{
    Symbol* symbol = objectFile->GetSymbolTable().GetSymbol(symbolIndex);
    if (symbol)
    {
        if (symbol->GetValue().GetFlag(ValueFlags::address))
        {
            uint64_t symbolAddress = symbol->GetValue().Val();
            uint64_t offset = (symbolAddress - Address()) >> 2;
            if (offset >= static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()) << 8)
            {
                throw std::runtime_error("error resolving long jump: jump too far: file '" + objectFile->FilePath() + "'");
            }
            objectFile->GetCodeSection()->EmitLongOffset(Address(), static_cast<uint32_t>(offset));
        }
        else
        {
            throw std::runtime_error("error resolving long jump: address value expected: file '" + objectFile->FilePath() + "'");
        }
    }
    else
    {
        throw std::runtime_error("error resolving long jump: symbol index " + std::to_string(symbolIndex) + " not found from symbol table of file '" + objectFile->FilePath() + "'");
    }
}

LinkCommand* LinkForwardLongJumpCommand::Clone() const
{
    LinkCommand* command = new LinkForwardLongJumpCommand(Address(), symbolIndex);
    command->SetId(Id());
    return command;
}

std::string LinkForwardLongJumpCommand::ToString() const
{
    return LinkCodeStr(Code()) + "(address=#" + ToHexString(Address()) + ", id=" + std::to_string(Id()) + ", symbol_index=" + std::to_string(symbolIndex) + ")";
}

LinkForwardShortJumpCommand::LinkForwardShortJumpCommand() : LinkCommand(LinkCode::forwardShortJump), symbolIndex(-1)
{
}

LinkForwardShortJumpCommand::LinkForwardShortJumpCommand(uint64_t address_, int32_t symbolIndex_) : LinkCommand(LinkCode::forwardShortJump, address_), symbolIndex(symbolIndex_)
{
}

void LinkForwardShortJumpCommand::Emit(LinkSection* linkSection)
{
    LinkCommand::Emit(linkSection);
    linkSection->EmitTetra(static_cast<uint32_t>(symbolIndex));
}

void LinkForwardShortJumpCommand::Read(LinkSection* linkSection)
{
    LinkCommand::Read(linkSection);
    symbolIndex = static_cast<int32_t>(linkSection->ReadTetra());
}

void LinkForwardShortJumpCommand::Apply(ObjectFile* objectFile, uint64_t value)
{
    Symbol* symbol = objectFile->GetSymbolTable().GetSymbol(symbolIndex);
    if (symbol)
    {
        if (symbol->GetValue().GetFlag(ValueFlags::address))
        {
            uint64_t symbolAddress = symbol->GetValue().Val();
            uint64_t offset = (symbolAddress - Address()) >> 2;
            if (offset >= static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()))
            {
                throw std::runtime_error("error resolving short jump: jump too far: file '" + objectFile->FilePath() + "'");
            }
            objectFile->GetCodeSection()->EmitShortOffset(Address(), static_cast<uint16_t>(offset));
        }
        else
        {
            throw std::runtime_error("error resolving short jump: address value expected: file '" + objectFile->FilePath() + "'");
        }
    }
    else
    {
        throw std::runtime_error("error resolving short jump: symbol index " + std::to_string(symbolIndex) + " not found from symbol table of file '" + objectFile->FilePath() + "'");
    }
}
LinkCommand* LinkForwardShortJumpCommand::Clone() const
{
    LinkCommand* command = new LinkForwardShortJumpCommand(Address(), symbolIndex);
    command->SetId(Id());
    return command;
}

std::string LinkForwardShortJumpCommand::ToString() const
{
    return LinkCodeStr(Code()) + "(address=#" + ToHexString(Address()) + ", id=" + std::to_string(Id()) + ", symbol_index=" + std::to_string(symbolIndex) + ")";
}

LinkAbsoluteAddressCommand::LinkAbsoluteAddressCommand() : LinkCommand(LinkCode::absoluteAddrValue), symbolIndex(-1)
{
}

LinkAbsoluteAddressCommand::LinkAbsoluteAddressCommand(uint64_t address_, int32_t symbolIndex_) : LinkCommand(LinkCode::absoluteAddrValue, address_), symbolIndex(symbolIndex_)
{
}

void LinkAbsoluteAddressCommand::Emit(LinkSection* linkSection)
{
    LinkCommand::Emit(linkSection);
    linkSection->EmitTetra(static_cast<uint32_t>(symbolIndex));
}

void LinkAbsoluteAddressCommand::Read(LinkSection* linkSection)
{
    LinkCommand::Read(linkSection);
    symbolIndex = static_cast<int32_t>(linkSection->ReadTetra());
}

LinkCommand* LinkAbsoluteAddressCommand::Clone() const
{
    LinkCommand* command = new LinkAbsoluteAddressCommand(Address(), symbolIndex);
    command->SetId(Id());
    return command;
}

void LinkAbsoluteAddressCommand::Apply(ObjectFile* objectFile, uint64_t value)
{
    uint8_t b0 = static_cast<uint8_t>(value);
    value = value >> 8;
    uint8_t b1 = static_cast<uint8_t>(value);
    value = value >> 8;
    uint8_t b2 = static_cast<uint8_t>(value);
    value = value >> 8;
    uint8_t b3 = static_cast<uint8_t>(value);
    value = value >> 8;
    uint8_t b4 = static_cast<uint8_t>(value);
    value = value >> 8;
    uint8_t b5 = static_cast<uint8_t>(value);
    value = value >> 8;
    uint8_t b6 = static_cast<uint8_t>(value);
    value = value >> 8;
    uint8_t b7 = static_cast<uint8_t>(value);
    uint16_t offset0 = (static_cast<uint16_t>(b7) << 8) | static_cast<uint16_t>(b6);
    uint16_t offset1 = (static_cast<uint16_t>(b5) << 8) | static_cast<uint16_t>(b4);
    uint16_t offset2 = (static_cast<uint16_t>(b3) << 8) | static_cast<uint16_t>(b2);
    uint16_t offset3 = (static_cast<uint16_t>(b1) << 8) | static_cast<uint16_t>(b0);
    objectFile->GetCodeSection()->EmitShortOffset(Address(), offset0);
    objectFile->GetCodeSection()->EmitShortOffset(Address() + 4, offset1);
    objectFile->GetCodeSection()->EmitShortOffset(Address() + 8, offset2);
    objectFile->GetCodeSection()->EmitShortOffset(Address() + 12, offset3);
}

std::string LinkAbsoluteAddressCommand::ToString() const
{
    return LinkCodeStr(Code()) + "(address=#" + ToHexString(Address()) + ", id=" + std::to_string(Id()) + ", symbol_index=" + std::to_string(symbolIndex) + ")";
}

LinkFarOctaCommand::LinkFarOctaCommand() : LinkCommand(LinkCode::farOcta), symbolIndex(-1)
{
}

LinkFarOctaCommand::LinkFarOctaCommand(uint64_t address_, int32_t symbolIndex_) : LinkCommand(LinkCode::farOcta, address_), symbolIndex(symbolIndex_)
{
}

void LinkFarOctaCommand::Emit(LinkSection* linkSection)
{
    LinkCommand::Emit(linkSection);
    linkSection->EmitTetra(static_cast<uint32_t>(symbolIndex));
}

void LinkFarOctaCommand::Read(LinkSection* linkSection)
{
    LinkCommand::Read(linkSection);
    symbolIndex = static_cast<int32_t>(linkSection->ReadTetra());
}

void LinkFarOctaCommand::Apply(ObjectFile* objectFile, uint64_t value)
{
    objectFile->GetDataSection()->EmitOcta(Address(), value);
}

LinkCommand* LinkFarOctaCommand::Clone() const
{
    LinkCommand* command = new LinkFarOctaCommand(Address(), symbolIndex);
    command->SetId(Id());
    return command;
}

std::string LinkFarOctaCommand::ToString() const
{
    return LinkCodeStr(Code()) + "(address=#" + ToHexString(Address()) + ", id=" + std::to_string(Id()) + ", symbol_index=" + std::to_string(symbolIndex) + ")";
}

LinkClsIdCommand::LinkClsIdCommand() : LinkCommand(LinkCode::clsid), typeId1(0), typeId2(0)
{
}

LinkClsIdCommand::LinkClsIdCommand(uint64_t address_, uint64_t typeId1_, uint64_t typeId2_) : LinkCommand(LinkCode::clsid, address_), typeId1(typeId1_), typeId2(typeId2_)
{
}

void LinkClsIdCommand::Emit(LinkSection* linkSection)
{
    LinkCommand::Emit(linkSection);
    linkSection->EmitOcta(typeId1);
    linkSection->EmitOcta(typeId2);
}

void LinkClsIdCommand::Read(LinkSection* linkSection)
{
    LinkCommand::Read(linkSection);
    typeId1 = linkSection->ReadOcta();
    typeId2 = linkSection->ReadOcta();
}

LinkCommand* LinkClsIdCommand::Clone() const
{
    LinkCommand* command = new LinkClsIdCommand(Address(), typeId1, typeId2);
    command->SetId(Id());
    return command;
}

void LinkClsIdCommand::Apply(ObjectFile* objectFile, uint64_t value)
{
    objectFile->GetDataSection()->EmitOcta(Address(), value);
}

std::string LinkClsIdCommand::ToString() const
{
    boost::uuids::uuid id;
    IntsToUuid(typeId1, typeId2, id);
    std::string s = boost::uuids::to_string(id);
    return LinkCodeStr(Code()) + "(address=#" + ToHexString(Address()) + ", id=" + std::to_string(Id()) + ", type_id=" + s + ")";
}

LinkEndCommamnd::LinkEndCommamnd() : LinkCommand(LinkCode::end, 0)
{
}

LinkCommand* LinkEndCommamnd::Clone() const
{
    LinkCommand* command = new LinkEndCommamnd();
    command->SetId(Id());
    return command;
}

std::string LinkEndCommamnd::ToString() const
{
    return LinkCodeStr(Code()) + "(address=#" + ToHexString(Address()) + ", id=" + std::to_string(Id()) + ")";
}

LinkCommand* MakeLinkCommand(LinkCode linkCode)
{
    switch (linkCode)
    {
        case LinkCode::once: return new LinkOnceCommand();
        case LinkCode::forwardLongJump: return new LinkForwardLongJumpCommand();
        case LinkCode::forwardShortJump: return new LinkForwardShortJumpCommand();
        case LinkCode::absoluteAddrValue: return new LinkAbsoluteAddressCommand();
        case LinkCode::farOcta: return new LinkFarOctaCommand();
        case LinkCode::clsid: return new LinkClsIdCommand();
        case LinkCode::end: return new LinkEndCommamnd();
        default:
        {
            return nullptr;
        }
    }
}

LinkCommand* ReadLinkCommand(LinkSection* linkSection)
{
    LinkCode linkCode = static_cast<LinkCode>(linkSection->ReadByte());
    LinkCommand* linkCommand = MakeLinkCommand(linkCode);
    if (linkCommand)
    {
        linkCommand->Read(linkSection);
    }
    return linkCommand;
}

void LinkInternal(ObjectFile* objectFile)
{
    std::vector<std::unique_ptr<LinkCommand>> externalCommands;
    LinkSection* linkSection = objectFile->GetLinkSection();
    for (const auto& linkCommand : linkSection->LinkCommands())
    {
        switch (linkCommand->Code())
        {
            case LinkCode::forwardLongJump:
            {
                linkCommand->Apply(objectFile, 0);
                break;
            }
            case LinkCode::forwardShortJump:
            {
                linkCommand->Apply(objectFile, 0);
                break;
            }
            default:
            {
                externalCommands.push_back(std::unique_ptr<LinkCommand>(linkCommand->Clone()));
                break;
            }
        }
    }
    linkSection->ReplaceLinkCommmands(externalCommands);
}

void ProcessUsedSymbol(Symbol* symbol, LinkSection* linkSection, SymbolTable* symbolTable, LinkTable& linkTable)
{
    if (symbol->IsUsed()) return;
    symbol->SetUsed();
    for (int32_t linkCommandId : symbol->LinkCommandIds())
    {
        int32_t symbolIndex = -1;
        LinkCommand* linkCommand = linkSection->GetLinkCommand(linkCommandId);
        if (linkCommand)
        {
            switch (linkCommand->Code())
            {
                case LinkCode::absoluteAddrValue:
                {
                    LinkAbsoluteAddressCommand* command = static_cast<LinkAbsoluteAddressCommand*>(linkCommand);
                    symbolIndex = command->SymbolIndex();
                    if (symbolIndex == -1)
                    {
                        throw std::runtime_error("error processing used symbol '" + symbol->FullName() + "': symbol index of link command " + std::to_string(linkCommandId) +
                            " not set in link section of file '" + linkSection->File()->FileName() + "'");
                    }
                    break;
                }
                case LinkCode::farOcta:
                {
                    LinkFarOctaCommand* command = static_cast<LinkFarOctaCommand*>(linkCommand);
                    symbolIndex = command->SymbolIndex();
                    if (symbolIndex == -1)
                    {
                        throw std::runtime_error("error processing used symbol '" + symbol->FullName() + "': symbol index of link command " + std::to_string(linkCommandId) +
                            " not set in link section of file '" + linkSection->File()->FileName() + "'");
                    }
                    break;
                }
            }
        }
        else
        {
            throw std::runtime_error("error processing used symbol '" + symbol->FullName() + "': link command " + std::to_string(linkCommandId) + 
                " not found from link section of file '" + linkSection->File()->FileName() + "'");
        }
        if (symbolIndex != -1)
        {
            Symbol* symbol = symbolTable->GetSymbol(symbolIndex);
            if (symbol)
            {
                linkTable.AddUsedSymbolName(symbol->FullName());
                const std::vector<UnprocessedSymbol>* unprocessedSymbolsPtr = linkTable.GetUnprocessedSymbols(symbol->FullName());
                if (unprocessedSymbolsPtr)
                {
                    std::vector<UnprocessedSymbol> unprocessedSymbols = *unprocessedSymbolsPtr;
                    linkTable.ClearUnprocessedSymbols(symbol->FullName());
                    for (const auto& unprocessedSymbol : unprocessedSymbols)
                    {
                        ProcessUsedSymbol(unprocessedSymbol.GetSymbol(), unprocessedSymbol.GetLinkSection(), unprocessedSymbol.GetSymbolTable(), linkTable);
                    }
                }
            }
            else
            {
                throw std::runtime_error("error processing used symbol: symbol index " + std::to_string(symbolIndex) + " not found from symbol table of file '" + 
                    linkSection->File()->FileName() + "'");
            }
        }
    }
}

void MarkUsedSymbols(LinkTable& linkTable, ObjectFile* objectFile)
{
    SymbolTable& symbolTable = objectFile->GetSymbolTable();
    for (const auto& symbol : symbolTable.Symbols())
    {
        if (symbol->IsGlobalSymbol())
        {
            if (linkTable.IsUsedSymbolName(symbol->FullName()))
            {
                ProcessUsedSymbol(symbol.get(), objectFile->GetLinkSection(), &symbolTable, linkTable);
            }
            else
            {
                linkTable.AddUnprocessedSymbol(symbol.get(), objectFile->GetLinkSection(), &symbolTable);
            }
        }
    }
}

void MarkUsedSymbols(LinkTable& linkTable, const std::vector<std::unique_ptr<BinaryFile>>& binaryFiles)
{
    linkTable.AddUsedSymbolName("Main");
    for (const auto& binaryFile : binaryFiles)
    {
        switch (binaryFile->Kind())
        {
            case BinaryFileKind::objectFile:
            {
                ObjectFile* objectFile = static_cast<ObjectFile*>(binaryFile.get());
                MarkUsedSymbols(linkTable, objectFile);
                break;
            }
            case BinaryFileKind::archiveFile:
            {
                ArchiveFile* archiveFile = static_cast<ArchiveFile*>(binaryFile.get());
                for (const auto& objectFile : archiveFile->ObjectFiles())
                {
                    MarkUsedSymbols(linkTable, objectFile.get());
                }
                break;
            }
            default:
            {
                throw std::runtime_error("error linking: object or archive file expected");
            }
        }
    }
}

void ProcessInternalSymbol(Symbol* symbol, Symbol* parentSymbolClone, SymbolTable& objectFileSymbolTable, SymbolTable& executableSymbolTable)
{
    bool process = parentSymbolClone != nullptr;
    if (process)
    {
        if (symbol->ParentIndex() != -1)
        {
            Symbol* parentSymbol = objectFileSymbolTable.GetSymbol(symbol->ParentIndex());
            if (parentSymbol)
            {
                if ((parentSymbol->GetLinkage() == Linkage::remove) || !parentSymbol->IsUsed())
                {
                    process = false;
                }
                if (parentSymbol->FullName() != parentSymbolClone->FullName())
                {
                    process = false;
                }
            }
        }
    }
    if (process)
    {
        symbol->SetStart(symbol->GetSection()->BaseAddress() + symbol->GetValue().Val() - symbol->GetSection()->RemoveOffset());
        uint64_t value = symbol->Start();
        value = Align(value, symbol->Alignment());
        symbol->SetStart(value);
        Symbol* clone = symbol->Clone();
        clone->SetParentIndex(parentSymbolClone->Index());
        executableSymbolTable.AddSymbol(clone);
    }
}

void ProcessExternalSymbol(LinkTable& linkTable, Symbol* symbol, Symbol*& parentSymbolClone, SymbolTable& executableSymbolTable, std::vector<Symbol*>& linkSymbols)
{
    if (symbol->GetSection()->IsDataSection())
    {
        int x = 0;
    }
    symbol->SetStart(symbol->GetSection()->BaseAddress() + symbol->GetValue().Val() - symbol->GetSection()->RemoveOffset());
    uint64_t value = symbol->Start();
    uint64_t alignedValue = Align(value, symbol->Alignment());
    symbol->SetStart(alignedValue);
    Symbol* clone = symbol->Clone();
    clone->SetLinkage(Linkage::external);
    parentSymbolClone = clone;
    executableSymbolTable.AddSymbol(clone);
    linkTable.ExecuteLinkCommands(symbol->FullName(), symbol->Start());
    linkTable.AddCopyRange(CopyRange(symbol->GetSection(), symbol->GetSection()->CopyTargetSection(), symbol->GetValue().Val(), symbol->Length(), symbol->Alignment()));
    linkSymbols.push_back(symbol);
}

void RemoveSymbol(Symbol* symbol)
{
    if (symbol->GetSection()->IsDataSection())
    {
        int x = 0;
    }
    symbol->SetLinkage(Linkage::remove);
    uint64_t length = symbol->Length();
    if (symbol->GetSegment() == Segment::data)
    {
        length = Align(length, 8);
    }
    symbol->GetSection()->SetRemoveOffset(symbol->GetSection()->RemoveOffset() + length);
    int64_t newLength = symbol->GetSection()->DataLength() - length;
    symbol->GetSection()->SetDataLength(newLength);
}

void ProcessSymbol(LinkTable& linkTable, Symbol* symbol, SymbolTable& objectFileSymbolTable, SymbolTable& executableSymbolTable, ExecutableFile* executable, 
    std::vector<Symbol*>& linkSymbols, Symbol*& parentSymbolClone)
{
    if (symbol->IsInternalSymbol())
    {
        ProcessInternalSymbol(symbol, parentSymbolClone, objectFileSymbolTable, executableSymbolTable);
    }
    else if (symbol->IsExternalOrUndefinedSymbol())
    {
        parentSymbolClone = nullptr;
        if (symbol->IsDefinitionSymbol())
        {
            Symbol* prevSymbol = executableSymbolTable.GetSymbol(symbol->FullName());
            if (prevSymbol)
            {
                throw std::runtime_error("duplicate external symbol '" + symbol->FullName() + "'. Original defined in " + prevSymbol->GetSection()->Name() +
                    " section of file " + prevSymbol->GetSection()->File()->FileName() + ". Duplicate detected in " + symbol->GetSection()->Name() +
                    " section of file " + symbol->GetSection()->File()->FileName() + ".");
            }
            else if (symbol->IsUsed())
            {
                ProcessExternalSymbol(linkTable, symbol, parentSymbolClone, executableSymbolTable, linkSymbols);
            }
            else
            {
                RemoveSymbol(symbol);
            }
        }
        else if (symbol->IsUndefinedSymbol())
        {
            Symbol* prevSymbol = executableSymbolTable.GetSymbol(symbol->FullName());
            if (prevSymbol)
            {
                symbol->SetStart(prevSymbol->Start());
                symbol->SetLinkage(Linkage::external);
            }
        }
    }
    else if (symbol->IsLinkOnceSymbol())
    {
        parentSymbolClone = nullptr;
        Symbol* prevSymbol = executableSymbolTable.GetSymbol(symbol->FullName());
        if (!prevSymbol)
        {
            if (symbol->IsUsed())
            {
                ProcessExternalSymbol(linkTable, symbol, parentSymbolClone, executableSymbolTable, linkSymbols);
            }
            else
            {
                RemoveSymbol(symbol);
            }
        }
        else
        {
            RemoveSymbol(symbol);
        }
    }
}

void ProcessLinkCommands(LinkTable& linkTable, Symbol* linkSymbol, ObjectFile* objectFile, ExecutableFile* executable)
{
    LinkSection* linkSection = objectFile->GetLinkSection();
    for (int32_t linkCommandId : linkSymbol->LinkCommandIds())
    {
        LinkCommand* linkCommand = linkSection->GetLinkCommand(linkCommandId);
        if (!linkCommand)
        {
            throw std::runtime_error("link command " + std::to_string(linkCommandId) + " not found from link section of object file '" + objectFile->FileName() + "'");
        }
        Symbol* symbol = nullptr;
        bool processLinkCommand = false;
        switch (linkCommand->Code())
        {
            case LinkCode::absoluteAddrValue:
            {
                LinkAbsoluteAddressCommand* command = static_cast<LinkAbsoluteAddressCommand*>(linkCommand);
                symbol = objectFile->GetSymbolTable().GetSymbol(command->SymbolIndex());
                if (!symbol)
                {
                    throw std::runtime_error("symbol " + std::to_string(command->SymbolIndex()) + " not found from symbol table of object file '" + objectFile->FileName() + "'");
                }
                if (symbol->IsUndefinedSymbol() || symbol->GetLinkage() == Linkage::remove)
                {
                    Symbol* executableSymbol = executable->GetSymbolTable().GetSymbol(symbol->FullName());
                    if (executableSymbol)
                    {
                        symbol = executableSymbol;
                    }
                }
                processLinkCommand = true;
                break;
            }
            case LinkCode::farOcta:
            {
                LinkFarOctaCommand* command = static_cast<LinkFarOctaCommand*>(linkCommand);
                symbol = objectFile->GetSymbolTable().GetSymbol(command->SymbolIndex());
                if (!symbol)
                {
                    throw std::runtime_error("symbol " + std::to_string(command->SymbolIndex()) + " not found from symbol table of object file '" + objectFile->FileName() + "'");
                }
                if (symbol->IsUndefinedSymbol() || symbol->GetLinkage() == Linkage::remove)
                {
                    Symbol* executableSymbol = executable->GetSymbolTable().GetSymbol(symbol->FullName());
                    if (executableSymbol)
                    {
                        symbol = executableSymbol;
                    }
                }
                processLinkCommand = true;
                break;
            }
            case LinkCode::clsid:
            {
                LinkClsIdCommand* command = static_cast<LinkClsIdCommand*>(linkCommand);
                boost::uuids::uuid typeId;
                IntsToUuid(command->TypeId1(), command->TypeId2(), typeId);
                uint64_t value = linkTable.GetClassId(typeId);
                command->Apply(objectFile, value);
                symbol = nullptr;
                processLinkCommand = false;
                break;
            }
            default:
            {
                symbol = nullptr;
                processLinkCommand = false;
                break;
            }
        }
        if (symbol && processLinkCommand)
        {
            if (symbol->GetValue().GetFlag(ValueFlags::address))
            {
                linkCommand->Apply(objectFile, symbol->Start());
            }
            else if (symbol->IsUndefinedSymbol())
            {
                linkTable.AddLinkCommand(symbol->FullName(), objectFile, linkCommand);
            }
        }
    }
}

void ProcessSymbols(LinkTable& linkTable, ObjectFile* objectFile, ExecutableFile* executable)
{
    SymbolTable& executableSymbolTable = executable->GetSymbolTable();
    SymbolTable& objectFileSymbolTable = objectFile->GetSymbolTable();
    Section* codeSection = objectFile->GetCodeSection();
    codeSection->SetCopyTargetSection(executable->GetCodeSection());
    codeSection->SetRemoveOffset(0);
    Section* dataSection = objectFile->GetDataSection();
    dataSection->SetCopyTargetSection(executable->GetDataSection());
    dataSection->SetRemoveOffset(0);
    std::vector<Symbol*> linkSymbols;
    Symbol* parentSymbolClone = nullptr;
    for (auto& symbol : objectFileSymbolTable.Symbols())
    {
        ProcessSymbol(linkTable, symbol.get(), objectFileSymbolTable, executableSymbolTable, executable, linkSymbols, parentSymbolClone);
    }
    for (Symbol* linkSymbol : linkSymbols)
    {
        ProcessLinkCommands(linkTable, linkSymbol, objectFile, executable);
    }
}

void LinkObjectFile(LinkTable& linkTable, ObjectFile* objectFile, Section*& prevCodeSection, Section*& prevDataSection, ExecutableFile* executable)
{
    Section* codeSection = objectFile->GetCodeSection();
    if (!prevCodeSection)
    {
        CodeSection* executableCodeSection = executable->GetCodeSection();
        codeSection->SetBaseAddress(cmsx::machine::textSegmentBaseAddress + 4096);
        executableCodeSection->SetBaseAddress(codeSection->BaseAddress());
    }
    else
    {
        codeSection->SetBaseAddress(prevCodeSection->BaseAddress() + prevCodeSection->DataLength());
    }
    prevCodeSection = codeSection;
    Section* dataSection = objectFile->GetDataSection();
    if (!prevDataSection)
    {
        DataSection* executableDataSection = executable->GetDataSection();
        dataSection->SetBaseAddress(cmsx::machine::dataSegmentBaseAddress);
        executableDataSection->SetBaseAddress(dataSection->BaseAddress());
    }
    else
    {
        dataSection->SetBaseAddress(prevDataSection->BaseAddress() + prevDataSection->DataLength());
    }
    prevDataSection = dataSection;
    ProcessSymbols(linkTable, objectFile, executable);
}

void LinkObjectFiles(LinkTable& linkTable, const std::vector<std::unique_ptr<BinaryFile>>& binaryFiles, ExecutableFile* executable)
{
    Section* prevCodeSection = nullptr;
    Section* prevDataSection = nullptr;
    for (const auto& binaryFile : binaryFiles)
    {
        switch (binaryFile->Kind())
        {
            case BinaryFileKind::objectFile:
            {
                ObjectFile* objectFile = static_cast<ObjectFile*>(binaryFile.get());
                LinkObjectFile(linkTable, objectFile, prevCodeSection, prevDataSection, executable);
                break;
            }
            case BinaryFileKind::archiveFile:
            {
                ArchiveFile* archiveFile = static_cast<ArchiveFile*>(binaryFile.get());
                for (const auto& objectFile : archiveFile->ObjectFiles())
                {
                    LinkObjectFile(linkTable, objectFile.get(), prevCodeSection, prevDataSection, executable);
                }
                break;
            }
            default:
            {
                throw std::runtime_error("error linking: object or archive file expected");
            }
        }
    }
}

void CopyRanges(const std::vector<CopyRange>& copyRanges)
{
    for (const auto& copyRange : copyRanges)
    {
        int64_t length = copyRange.Length();
        if (length > 0)
        {
            Section* fromSection = copyRange.FromSection();
            Section* toSection = copyRange.ToSection();
            int64_t startPos = copyRange.StartPos();
            int8_t alignment = copyRange.Alignment();
            toSection->Align(alignment);
            for (int64_t i = 0; i < length; ++i)
            {
                uint8_t x = fromSection->GetByte(startPos + i);
                toSection->EmitByte(x);
            }
        }
    }
}

void Link(int logStreamId, const std::string& executableFilePath, const std::vector<std::string>& binaryFileNames, const std::string& clsIdFileName, bool verbose)
{
    LinkTable linkTable(clsIdFileName);
    std::unique_ptr<ExecutableFile> executable(new ExecutableFile(executableFilePath));
    executable->CreateSections();
    executable->GetSymbolTable().InstallDefaultSymbols();
    std::vector<std::unique_ptr<BinaryFile>> binaryFiles;
    for (const auto& binaryFileName : binaryFileNames)
    {
        if (verbose)
        {
            LogMessage(logStreamId, "> " + binaryFileName);
        }
        std::unique_ptr<BinaryFile> binaryFile(ReadBinaryFile(binaryFileName));
        binaryFiles.push_back(std::move(binaryFile));
    }
    MarkUsedSymbols(linkTable, binaryFiles);
    linkTable.ClearUnprocessedSymbols();
    LinkObjectFiles(linkTable, binaryFiles, executable.get());
    linkTable.CheckUnresolvedSymbols(logStreamId);
    CopyRanges(linkTable.CopyRanges());
    MakeFunctionTable(binaryFiles, *executable, linkTable);
    ProcessResources(binaryFiles, *executable);
    executable->Finalize();
    executable->WriteFile();
    if (verbose)
    {
        LogMessage(logStreamId, "==> " + executableFilePath);
    }
}

} // namespace cmsx::object
