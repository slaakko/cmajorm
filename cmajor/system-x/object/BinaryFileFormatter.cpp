// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/BinaryFileFormatter.hpp>
#include <system-x/object/BinaryFile.hpp>

namespace cmsx::object {

BinaryFileFormatter::BinaryFileFormatter(cmsx::machine::Machine& machine_, BinaryFile* file_) : InstructionFormatter(machine_), file(file_), currentSection(nullptr)
{
}

void BinaryFileFormatter::Format()
{
    FormatSection(file->GetHeaderSection());
    CodeSection* codeSection = file->GetCodeSection();
    if (codeSection && codeSection->Length() > 0)
    {
        FormatSection(codeSection);
        FormatEol();
    }
    DataSection* dataSection = file->GetDataSection();
    if (dataSection && dataSection->Length() > 0)
    {
        FormatSection(dataSection);
        FormatEol();
    }
    SymbolSection* symbolSection = file->GetSymbolSection();
    if (symbolSection && symbolSection->Length() > 0) 
    {
        FormatSection(symbolSection);
        FormatEol();
    }
    LinkSection* linkSection = file->GetLinkSection();
    if (linkSection && linkSection->Length() > 0)
    {
        FormatSection(linkSection);
        FormatEol();
    }
    DebugSection* debugSection = file->GetDebugSection();
    if (debugSection && debugSection->Length() > 0)
    {
        FormatSection(debugSection);
        FormatEol();
    }
    SymbolTable& symbolTable = file->GetSymbolTable();
    FormatSymbolTable(symbolTable);
    FormatEol();
}

void BinaryFileFormatter::FormatOcta(uint64_t octa)
{
}

void BinaryFileFormatter::FormatSymbol(Symbol* symbol)
{
}

void BinaryFileFormatter::FormatSection(HeaderSection* section)
{
    currentSection = section;
}

void BinaryFileFormatter::FormatSection(CodeSection* section)
{
    currentSection = section;
    int64_t baseAddress = currentSection->BaseAddress();
    int64_t n = currentSection->Length();
    int64_t address = 0;
    while (address < n)
    {
        int8_t opc = section->GetByte(address);
        int8_t x = section->GetByte(address + 1);
        int8_t y = section->GetByte(address + 2);
        int8_t z = section->GetByte(address + 3);
        FormatInstruction(baseAddress + address, &binaryFileFormatterSetAddressMap, opc, x, y, z);
        address += 4;
    }
}

void BinaryFileFormatter::FormatSection(DataSection* section)
{
    currentSection = section;
    int64_t baseAddress = currentSection->BaseAddress();
    int64_t n = currentSection->Length();
    int64_t address = 0;
    while (address < n)
    {
        uint64_t octa = section->GetOcta(address);
        FormatCurrentAddress(baseAddress + address);
        FormatOcta(octa);
        FormatEol();
        address += 8;
    }
}

void BinaryFileFormatter::FormatSection(SymbolSection* section)
{
    currentSection = section;
    for (const auto& symbol : section->File()->GetSymbolTable().Symbols())
    {
        FormatSymbol(symbol.get());
    }
}

void BinaryFileFormatter::FormatSection(LinkSection* section)
{
    currentSection = section;
    for (const auto& linkCommand : section->LinkCommands())
    {
        FormatString(linkCommand->ToString());
        FormatEol();
    }
}

void BinaryFileFormatter::FormatSection(DebugSection* section)
{
    currentSection = section;
    for (const auto& debugRecord : section->DebugRecords())
    {
        FormatString(debugRecord->ToString());
        FormatEol();
    }
}

void BinaryFileFormatter::FormatSymbolTable(SymbolTable& symbolTable)
{
    // todo
}

} // namespace cmsx::object
