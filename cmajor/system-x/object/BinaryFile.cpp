// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/BinaryFile.hpp>
#include <system-x/machine/Memory.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/BufferedStream.hpp>
#include <soulng/util/FileStream.hpp>
#include <soulng/util/Path.hpp>
#include <stdexcept>

namespace cmsx::object {

using namespace soulng::util;

BinaryFile* ReadBinaryFileHeader(BinaryStreamReader& reader, const std::string& filePath)
{
    BinaryFile* file = nullptr;
    try
    {
        std::string s;
        int n = 7;
        for (int i = 0; i < n; ++i)
        {
            char c = reader.ReadChar();
            s.append(1, c);
        }
        if (s == "CMSXOBJ")
        {
            file = new ObjectFile(filePath);
        }
        else if (s == "CMSXLIB")
        {
            file = new ArchiveFile(filePath);
        }
        else if (s == "CMSXEXE")
        {
            file = new ExecutableFile(filePath);
        }
        else
        {
            throw std::runtime_error("unknown file type");
        }
        char c = reader.ReadChar();
        if (c != current_binary_file_version)
        {
            throw std::runtime_error("wrong binary file version '" + std::string(1, c) + "': version '" + std::string(1, current_binary_file_version) + " expected");
        }
        std::string fileName = reader.ReadUtf8String();
        file->SetFileName(fileName);
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("error reading binary file '" + filePath + "': unknown file header: " + ex.what());
    }
    return file;
}

BinaryFile* ReadBinaryFile(const std::string& filePath)
{
    FileStream fileStream(filePath, OpenMode::read | OpenMode::binary);
    BufferedStream bufferedStream(fileStream);
    BinaryStreamReader reader(bufferedStream);
    BinaryFile* file = ReadBinaryFileHeader(reader, filePath);
    file->Read(reader);
    return file;
}

Section* ReadSection(BinaryFile* file, BinaryStreamReader& reader)
{
    Section* section = nullptr;
    std::string sectionName;
    sectionName.append(1, reader.ReadChar());
    sectionName.append(1, reader.ReadChar());
    sectionName.append(1, reader.ReadChar());
    sectionName.append(1, reader.ReadChar());
    if (sectionName == "FHDR")
    {
        section = new HeaderSection(file);
    }
    else if (sectionName == "CODE")
    {
        section = new CodeSection(file);
    }
    else if (sectionName == "DATA")
    {
        section = new DataSection(file);
    }
    else if (sectionName == "SYMB")
    {
        section = new SymbolSection(file);
    }
    else if (sectionName == "LINK")
    {
        section = new LinkSection(file);
    }
    else if (sectionName == "DBUG")
    {
        section = new DebugSection(file);
    }
    else
    {
        throw std::runtime_error("error reading '" + file->FilePath() + "': unknown section name '" + sectionName + "'");
    }
    section->Read(reader);
    return section;
}

Section::Section(SectionKind kind_, BinaryFile* file_) : 
    kind(kind_), file(file_), baseAddress(0), pos(0), copyStartPos(0), copyTargetSection(nullptr), removeOffset(0), dataLength(0)
{
}

Section::~Section()
{
}

std::string Section::Name() const
{
    switch (kind)
    {
        case SectionKind::fhdr:
        {
            return "header";
        }
        case SectionKind::code:
        {
            return "code";
        }
        case SectionKind::data:
        {
            return "data";
        }
        case SectionKind::symb:
        {
            return "symbol";
        }
        case SectionKind::link:
        {
            return "link";
        }
        case SectionKind::dbug:
        {
            return "debug";
        }
        default:
        {
            return "unknown";
        }
    }
}

void Section::Write(BinaryStreamWriter& writer)
{
    switch (kind)
    {
        case SectionKind::fhdr:
        {
            writer.Write("FHDR", false);
            break;
        }
        case SectionKind::code:
        {
            writer.Write("CODE", false);
            break;
        }
        case SectionKind::data:
        {
            writer.Write("DATA", false);
            break;
        }
        case SectionKind::symb:
        {
            writer.Write("SYMB", false);
            break;
        }
        case SectionKind::link:
        {
            writer.Write("LINK", false);
            break;
        }
        case SectionKind::dbug:
        {
            writer.Write("DBUG", false);
            break;
        }
    }
    writer.Write(Length());
    writer.Write(BaseAddress());
    int64_t n = Length();
    for (int64_t i = 0; i < n; ++i)
    {
        writer.Write(data[i]);
    }
}

void Section::Read(BinaryStreamReader& reader)
{
    try
    {
        int64_t length = reader.ReadLong();
        if (length < 0)
        {
            throw std::runtime_error("invalid section length " + std::to_string(length));
        }
        baseAddress = reader.ReadLong();
        if (baseAddress < 0 || baseAddress >= cmsx::machine::poolSegmentBaseAddress)
        {
            throw std::runtime_error("invalid base address " + ToHexString(static_cast<uint64_t>(baseAddress)));
        }
        SetBaseAddress(baseAddress);
        for (int64_t i = 0; i < length; ++i)
        {
            uint8_t x = reader.ReadByte();
            data.push_back(x);
        }
        dataLength = data.size();
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("error reading '" + file->FilePath() + "': " + ex.what());
    }
}

void Section::AddSymbol(Symbol* symbol)
{
}

void Section::Finalize()
{
}

void Section::EmitByte(uint8_t x)
{
    if (pos == Length())
    {
        data.push_back(x);
        ++pos;
    }
    else if (pos < Length())
    {
        data[pos++] = x;
    }
    else
    {
        throw std::runtime_error("error emitting to file '" + file->FilePath() + "': invalid position " + ToHexString(static_cast<uint64_t>(pos)));
    }
}

void Section::EmitByte(int64_t address, uint8_t x)
{
    int64_t prevPos = pos;
    pos = address;
    EmitByte(x);
    pos = prevPos;
}

uint8_t Section::ReadByte()
{
    if (pos == Length())
    {
        throw std::runtime_error("error reading file '" + file->FilePath() + "': unexpected end of data");
    }
    return data[pos++];
}

uint8_t Section::GetByte(int64_t address) const
{
    return data[address];
}

void Section::EmitWyde(uint16_t x)
{
    uint8_t b0 = static_cast<uint8_t>(x);
    x = x >> 8;
    uint8_t b1 = static_cast<uint8_t>(x);
    EmitByte(b1);
    EmitByte(b0);
}

void Section::EmitWyde(int64_t address, uint16_t x)
{
    int64_t prevPos = pos;
    pos = address;
    EmitWyde(x);
    pos = prevPos;
}

uint16_t Section::ReadWyde()
{
    uint8_t b1 = ReadByte();
    uint8_t b0 = ReadByte();
    return (static_cast<uint16_t>(b1) << 8) | static_cast<uint16_t>(b0);
}

uint16_t Section::GetWyde(int64_t address) const
{
    uint8_t b1 = GetByte(address);
    uint8_t b0 = GetByte(address + 1);
    return (static_cast<uint16_t>(b1) << 8) | static_cast<uint16_t>(b0);
}

void Section::EmitTetra(uint32_t x)
{
    uint8_t b0 = static_cast<uint8_t>(x);
    x = x >> 8;
    uint8_t b1 = static_cast<uint8_t>(x);
    x = x >> 8;
    uint8_t b2 = static_cast<uint8_t>(x);
    x = x >> 8;
    uint8_t b3 = static_cast<uint8_t>(x);
    EmitByte(b3);
    EmitByte(b2);
    EmitByte(b1);
    EmitByte(b0);
}

void Section::EmitTetra(int64_t address, uint32_t x)
{
    int64_t prevPos = pos;
    pos = address;
    EmitTetra(x);
    pos = prevPos;
}

uint32_t Section::ReadTetra()
{
    uint8_t b3 = ReadByte();
    uint8_t b2 = ReadByte();
    uint8_t b1 = ReadByte();
    uint8_t b0 = ReadByte();
    return (static_cast<uint32_t>(b3) << 24) | (static_cast<uint32_t>(b2) << 16) | (static_cast<uint32_t>(b1) << 8) | static_cast<uint32_t>(b0);
}

uint32_t Section::GetTetra(int64_t address) const
{
    uint8_t b3 = GetByte(address);
    uint8_t b2 = GetByte(address + 1);
    uint8_t b1 = GetByte(address + 2);
    uint8_t b0 = GetByte(address + 3);
    return (static_cast<uint32_t>(b3) << 24) | (static_cast<uint32_t>(b2) << 16) | (static_cast<uint32_t>(b1) << 8) | static_cast<uint32_t>(b0);
}

void Section::EmitOcta(uint64_t x)
{
    uint8_t b0 = static_cast<uint8_t>(x);
    x = x >> 8;
    uint8_t b1 = static_cast<uint8_t>(x);
    x = x >> 8;
    uint8_t b2 = static_cast<uint8_t>(x);
    x = x >> 8;
    uint8_t b3 = static_cast<uint8_t>(x);
    x = x >> 8;
    uint8_t b4 = static_cast<uint8_t>(x);
    x = x >> 8;
    uint8_t b5 = static_cast<uint8_t>(x);
    x = x >> 8;
    uint8_t b6 = static_cast<uint8_t>(x);
    x = x >> 8;
    uint8_t b7 = static_cast<uint8_t>(x);
    EmitByte(b7);
    EmitByte(b6);
    EmitByte(b5);
    EmitByte(b4);
    EmitByte(b3);
    EmitByte(b2);
    EmitByte(b1);
    EmitByte(b0);
}

void Section::EmitOcta(int64_t address, uint64_t x)
{
    int64_t prevPos = pos;
    pos = address;
    EmitOcta(x);
    pos = prevPos;
}

uint64_t Section::ReadOcta()
{
    uint8_t b7 = ReadByte();
    uint8_t b6 = ReadByte();
    uint8_t b5 = ReadByte();
    uint8_t b4 = ReadByte();
    uint8_t b3 = ReadByte();
    uint8_t b2 = ReadByte();
    uint8_t b1 = ReadByte();
    uint8_t b0 = ReadByte();
    return
        (static_cast<uint64_t>(b7) << 56) | (static_cast<uint64_t>(b6) << 48) | (static_cast<uint64_t>(b5) << 40) | (static_cast<uint64_t>(b4) << 32) |
        (static_cast<uint64_t>(b3) << 24) | (static_cast<uint64_t>(b2) << 16) | (static_cast<uint64_t>(b1) << 8) | static_cast<uint64_t>(b0);
}

uint64_t Section::GetOcta(int64_t address) const
{
    uint8_t b7 = GetByte(address);
    uint8_t b6 = GetByte(address + 1);
    uint8_t b5 = GetByte(address + 2);
    uint8_t b4 = GetByte(address + 3);
    uint8_t b3 = GetByte(address + 4);
    uint8_t b2 = GetByte(address + 5);
    uint8_t b1 = GetByte(address + 6);
    uint8_t b0 = GetByte(address + 7);
    return 
        (static_cast<uint64_t>(b7) << 56) | (static_cast<uint64_t>(b6) << 48) | (static_cast<uint64_t>(b5) << 40) | (static_cast<uint64_t>(b4) << 32) |
        (static_cast<uint64_t>(b3) << 24) | (static_cast<uint64_t>(b2) << 16) | (static_cast<uint64_t>(b1) << 8) | static_cast<uint64_t>(b0);
}

void Section::EmitShortOffset(uint16_t offset)
{
    uint16_t ofs = offset;
    uint8_t b0 = static_cast<uint8_t>(offset);
    offset = offset >> 8;
    uint8_t b1 = static_cast<uint8_t>(offset);
    EmitByte(b1);
    EmitByte(b0);
}

void Section::EmitShortOffset(int64_t address, uint16_t offset)
{
    int64_t prevPos = pos;
    pos = address + 2;
    EmitShortOffset(offset);
    pos = prevPos;
}

void Section::EmitLongOffset(uint32_t offset)
{
    uint32_t ofs = offset;
    uint8_t b0 = static_cast<uint8_t>(offset);
    offset = offset >> 8;
    uint8_t b1 = static_cast<uint8_t>(offset);
    offset = offset >> 8;
    uint8_t b2 = static_cast<uint8_t>(offset);
    EmitByte(b2);
    EmitByte(b1);
    EmitByte(b0);
}

void Section::EmitLongOffset(int64_t address, uint32_t offset)
{
    int64_t prevPos = pos;
    pos = address + 1;
    EmitLongOffset(offset);
    pos = prevPos;
}

void Section::EmitString(const std::string& string)
{
    for (char c : string)
    {
        EmitByte(static_cast<uint8_t>(c));
    }
    EmitByte(0);
}

std::string Section::ReadString()
{
    std::string s;
    uint8_t x = ReadByte();
    while (x)
    {
        s.append(1, static_cast<char>(x));
        x = ReadByte();
    }
    return s;
}

void Section::EmitValue(const cmsx::object::Value& value)
{
    EmitByte(static_cast<uint8_t>(value.Flags()));
    EmitOcta(value.Val());
}

Value Section::ReadValue()
{
    ValueFlags flags = static_cast<ValueFlags>(ReadByte());
    uint64_t val = ReadOcta();
    return Value(val, flags);
}

void Section::Align(int64_t alignment)
{
    int64_t at = Address();
    int64_t a = at & (alignment - 1);
    if (a)
    {
        int64_t offset = alignment - a;
        for (int64_t i = 0; i < offset; ++i)
        {
            EmitByte(0);
        }
    }
}

HeaderSection::HeaderSection(BinaryFile* file_) : Section(SectionKind::fhdr, file_)
{
}

void HeaderSection::Write(BinaryStreamWriter& writer)
{
    switch (File()->Kind())
    {
        case BinaryFileKind::objectFile:
        {
            writer.Write("CMSXOBJ", false);
            break;
        }
        case BinaryFileKind::archiveFile:
        {
            writer.Write("CMSXLIB", false);
            break;
        }
        case BinaryFileKind::executableFile:
        {
            writer.Write("CMSXEXE", false);
            break;
        }
    }
    writer.Write(static_cast<uint8_t>(current_binary_file_version));
    writer.Write(Path::GetFileName(File()->FilePath()));
    writer.Write(File()->SectionCount());
    Section::Write(writer);
}

void HeaderSection::Read(BinaryStreamReader& reader)
{
    Section::Read(reader);
}

CodeSection::CodeSection(BinaryFile* file_) : Section(SectionKind::code, file_)
{
}

DataSection::DataSection(BinaryFile* file_) : Section(SectionKind::data, file_)
{
}

void DataSection::AddSymbol(Symbol* symbol)
{
    symbols.push_back(symbol);
}

void DataSection::Finalize()
{
    Align(8);
}

SymbolSection::SymbolSection(BinaryFile* file_) : Section(SectionKind::symb, file_)
{
}

void SymbolSection::Finalize()
{
    if (Address() != 0)
    {
        throw std::runtime_error("error finalizing: pos == 0 in symbol section of file '" + File()->FilePath() + " expected");
    }
    EmitSymbolTable();
}

void SymbolSection::Read(BinaryStreamReader& reader)
{
    Section::Read(reader);
    SetPos(0);
    uint32_t count = ReadTetra();
    for (uint32_t i = 0; i < count; ++i)
    {
        Symbol* symbol = ReadSymbol();
        File()->GetSymbolTable().AddSymbol(symbol, false);
    }
}

void SymbolSection::EmitSymbolTable()
{
    SymbolTable& symbolTable = File()->GetSymbolTable();
    EmitTetra(symbolTable.Symbols().size());
    for (const auto& symbol : symbolTable.Symbols())
    {
        EmitSymbol(symbol.get());
    }
    // todo: emit internal symbols;

}

void SymbolSection::EmitSymbol(Symbol* symbol)
{
    EmitByte(static_cast<uint8_t>(symbol->Kind()));
    EmitTetra(static_cast<uint32_t>(symbol->Index()));
    EmitString(symbol->LocalName());
    EmitString(symbol->FullName());
    EmitByte(static_cast<uint8_t>(symbol->GetSegment()));
    EmitByte(static_cast<uint8_t>(symbol->GetLinkage()));
    EmitValue(symbol->GetValue());
    EmitOcta(static_cast<uint64_t>(symbol->Start()));
    EmitOcta(static_cast<uint64_t>(symbol->Length()));
    EmitTetra(static_cast<uint32_t>(symbol->ParentIndex()));
    EmitByte(symbol->Alignment());
    int32_t linkCommandIdCount = symbol->LinkCommandIds().size();
    EmitTetra(static_cast<uint32_t>(linkCommandIdCount));
    for (int32_t i = 0; i < linkCommandIdCount; ++i)
    {
        int32_t linkCommandId = symbol->LinkCommandIds()[i];
        EmitTetra(static_cast<uint32_t>(linkCommandId));
    }
}

Symbol* SymbolSection::ReadSymbol()
{
    Symbol* symbol = new Symbol();
    symbol->SetKind(static_cast<SymbolKind>(ReadByte()));
    symbol->SetIndex(static_cast<int32_t>(ReadTetra()));
    symbol->SetLocalName(ReadString());
    symbol->SetFullName(ReadString());
    symbol->SetSegment(static_cast<Segment>(ReadByte()));
    symbol->SetLinkage(static_cast<Linkage>(ReadByte()));
    switch (symbol->GetSegment())
    {
        case Segment::text:
        {
            symbol->SetSection(File()->GetCodeSection());
            break;
        }
        case Segment::data:
        {
            symbol->SetSection(File()->GetDataSection());
            break;
        }
    }
    Value value = ReadValue();
    value.SetSymbol(symbol);
    symbol->SetValue(value);
    symbol->SetStart(static_cast<int64_t>(ReadOcta()));
    symbol->SetLength(static_cast<int64_t>(ReadOcta()));
    symbol->SetParentIndex(static_cast<int32_t>(ReadTetra()));
    symbol->SetAlignment(ReadByte());
    int32_t linkCommandIdCount = static_cast<int32_t>(ReadTetra());
    for (int32_t i = 0; i < linkCommandIdCount; ++i)
    {
        int32_t linkCommandId = static_cast<int32_t>(ReadTetra());
        if (linkCommandId != -1)
        {
            symbol->AddLinkCommandId(linkCommandId);
        }
    }
    return symbol;
}

LinkSection::LinkSection(BinaryFile* file_) : Section(SectionKind::link, file_)
{
}

void LinkSection::Finalize()
{
    if (Address() != 0)
    {
        throw std::runtime_error("error finalizing: pos == 0 in link section of file '" + File()->FilePath() + " expected");
    }
    EmitLinkCommands();
}

void LinkSection::EmitLinkCommands()
{
    uint32_t count = linkCommands.size();
    EmitTetra(count);
    for (const auto& linkCommand : linkCommands)
    {
        linkCommand->Emit(this);
    }
}

void LinkSection::Read(BinaryStreamReader& reader)
{
    Section::Read(reader);
    SetPos(0);
    uint32_t count = ReadTetra();
    for (uint32_t i = 0; i < count; ++i)
    {
        LinkCommand* linkCommand = ReadLinkCommand(this);
        AddLinkCommand(linkCommand, false);
        MapLinkCommand(linkCommand);
    }
}

int32_t LinkSection::AddLinkCommand(LinkCommand* linkCommand, bool setId)
{
    int32_t linkCommandId = -1;
    if (setId)
    {
        linkCommandId = linkCommands.size();
        linkCommand->SetId(linkCommandId);
    }
    linkCommands.push_back(std::unique_ptr<LinkCommand>(linkCommand));
    return linkCommandId;
}

void LinkSection::MapLinkCommand(LinkCommand* linkCommand)
{
    if (linkCommand->Id() != -1)
    {
        linkCommandMap[linkCommand->Id()] = linkCommand;
    }
}

void LinkSection::MapLinkCommands()
{
    for (auto& linkCommand : linkCommands)
    {
        MapLinkCommand(linkCommand.get());
    }
}

LinkCommand* LinkSection::GetLinkCommand(int32_t linkCommandId) const
{
    auto it = linkCommandMap.find(linkCommandId);
    if (it != linkCommandMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void LinkSection::ReplaceLinkCommmands(std::vector<std::unique_ptr<LinkCommand>>& linkCommands_)
{
    std::swap(linkCommands, linkCommands_);
}

DebugSection::DebugSection(BinaryFile* file_) : Section(SectionKind::dbug, file_)
{
}

BinaryFile::BinaryFile(const std::string& filePath_, BinaryFileKind kind_) : 
    filePath(filePath_), kind(kind_), 
    headerSection(nullptr),
    codeSection(nullptr),
    dataSection(nullptr),
    symbolSection(nullptr),
    linkSection(nullptr),
    debugSection(nullptr)
{
}

BinaryFile::~BinaryFile()
{
}

void BinaryFile::Finalize()
{
    for (const auto& section : sections)
    {
        section->Finalize();
    }
}

void BinaryFile::FinishRead()
{
}

void BinaryFile::WriteFile()
{
    FileStream fileStream(filePath, OpenMode::write | OpenMode::binary);
    BufferedStream bufferedStream(fileStream);
    BinaryStreamWriter writer(bufferedStream);
    Write(writer);
}

void BinaryFile::Write(BinaryStreamWriter& writer)
{
    for (const auto& section : sections)
    {
        section->Write(writer);
    }
}

void BinaryFile::Read(BinaryStreamReader& reader)
{
    int32_t numSections = reader.ReadInt();
    if (numSections < 0 || numSections > 6)
    {
        throw std::runtime_error("invalid number of file sections: " + std::to_string(numSections));
    }
    for (int32_t i = 0; i < numSections; ++i)
    {
        Section* section = ReadSection(this, reader);
        AddSection(section);
    }
    FinishRead();
}

void BinaryFile::SetFileName(const std::string& fileName_)
{
    fileName = fileName_;
}

void BinaryFile::AddSection(Section* section)
{
    switch (section->Kind())
    {
        case SectionKind::fhdr:
        {
            headerSection = static_cast<HeaderSection*>(section);
            break;
        }
        case SectionKind::code:
        {
            codeSection = static_cast<CodeSection*>(section);
            break;
        }
        case SectionKind::data:
        {
            dataSection = static_cast<DataSection*>(section);
            break;
        }
        case SectionKind::symb:
        {
            symbolSection = static_cast<SymbolSection*>(section);
            break;
        }
        case SectionKind::link:
        {
            linkSection = static_cast<LinkSection*>(section);
            break;
        }
        case SectionKind::dbug:
        {
            debugSection = static_cast<DebugSection*>(section);
            break;
        }
    }
    sections.push_back(std::unique_ptr<Section>(section));
}

ObjectFile::ObjectFile() : BinaryFile(std::string(), BinaryFileKind::objectFile)
{
}

ObjectFile::ObjectFile(const std::string& filePath_) : BinaryFile(filePath_, BinaryFileKind::objectFile)
{
}

void ObjectFile::CreateSections()
{
    AddSection(new HeaderSection(this));
    AddSection(new CodeSection(this));
    AddSection(new DataSection(this));
    AddSection(new SymbolSection(this));
    AddSection(new LinkSection(this));
    AddSection(new DebugSection(this));
}

void ObjectFile::FinishRead()
{
    SymbolTable& symbolTable = GetSymbolTable();
    for (const auto& symbol : symbolTable.Symbols())
    {
        symbolTable.AddSymbolToAddressMap(symbol.get(), false);
    }
}

void ObjectFile::MapLinkCommands()
{
    LinkSection* linkSection = GetLinkSection();
    if (linkSection)
    {
        linkSection->MapLinkCommands();
    }
}

ArchiveFile::ArchiveFile(const std::string& filePath_) : BinaryFile(filePath_, BinaryFileKind::archiveFile)
{
}

void ArchiveFile::CreateSections()
{
    AddSection(new HeaderSection(this));
}

void ArchiveFile::AddObjectFile(ObjectFile* objectFile)
{
    objectFiles.push_back(std::unique_ptr<ObjectFile>(objectFile));
}

void ArchiveFile::Write(BinaryStreamWriter& writer)
{
    BinaryFile::Write(writer);
    int32_t count = objectFiles.size();
    writer.Write(count);
    for (int32_t i = 0; i < count; ++i)
    {
        objectFiles[i]->Write(writer);
    }
}

void ArchiveFile::Read(BinaryStreamReader& reader)
{
    BinaryFile::Read(reader);
    int32_t count = reader.ReadInt();
    for (int32_t i = 0; i < count; ++i)
    {
        BinaryFile* file = ReadBinaryFileHeader(reader, FilePath());
        if (file->Kind() == BinaryFileKind::objectFile)
        {
            file->Read(reader);
            AddObjectFile(static_cast<ObjectFile*>(file));
        }
        else
        {
            throw std::runtime_error("error reading archive file '" + FilePath() + "': object file content expected");
        }
    }
}

ExecutableFile::ExecutableFile(const std::string& filePath_) : BinaryFile(filePath_, BinaryFileKind::executableFile)
{
}

void ExecutableFile::CreateSections()
{
    AddSection(new HeaderSection(this));
    AddSection(new CodeSection(this));
    AddSection(new DataSection(this));
    AddSection(new SymbolSection(this));
}

void ExecutableFile::FinishRead()
{
    SymbolTable& symbolTable = GetSymbolTable();
    for (const auto& symbol : symbolTable.Symbols())
    {
        symbolTable.AddSymbolToAddressMap(symbol.get(), false);
    }
}

} // namespace cmsx::object
