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
    FormatSection(file->GetCodeSection());
    FormatSection(file->GetDataSection());
    FormatSection(file->GetSymbolSection());
    FormatSection(file->GetLinkSection());
    FormatSection(file->GetDebugSection());
    FormatSymbolTable(file->GetSymbolTable());
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
}

void BinaryFileFormatter::FormatSection(SymbolSection* section)
{
    currentSection = section;
}

void BinaryFileFormatter::FormatSection(LinkSection* section)
{
    currentSection = section;
}

void BinaryFileFormatter::FormatSection(DebugSection* section)
{
    currentSection = section;
}

void BinaryFileFormatter::FormatSymbolTable(SymbolTable& symbolTable)
{
    // todo
}

} // namespace cmsx::object
