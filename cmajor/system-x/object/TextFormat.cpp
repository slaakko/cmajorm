// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/TextFormat.hpp>
#include <system-x/object/BinaryFile.hpp>
#include <system-x/object/BinaryFileFormatter.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <memory>
#include <fstream>

namespace cmsx::object {

using namespace soulng::util;

class TextFormatter : public BinaryFileFormatter
{
public:
    TextFormatter(cmsx::machine::Machine& machine, BinaryFile* file_);
    const std::string& TextFilePath() const { return textFilePath; }
    void FormatSection(CodeSection* section) override;
    void FormatSection(DataSection* section) override;
    void FormatSection(SymbolSection* section) override;
    void FormatSection(LinkSection* section) override;
    void FormatSection(DebugSection* section) override;
    void FormatSection(ResourceSection* section) override;
    void FormatCurrentAddress(uint64_t currentAddress) override;
    void FormatAssembledBytes(uint8_t opc, uint8_t x, uint8_t y, uint8_t z) override;
    void FormatLabel(uint64_t currentAddress) override;
    void FormatOpCode(const std::string& opCodeName) override;
    void FormatByteOperand(uint8_t operand) override;
    void FormatRegOperand(uint8_t reg) override;
    void FormatComma() override;
    void FormatColon() override;
    void FormatSpace() override;
    void FormatChar(uint8_t x) override;
    void FormatString(const std::string& s) override;
    void FormatEol() override;
    void FormatUndefined() override;
    void FormatSpecialReg(uint8_t reg) override;
    void FormatWydeOperand(uint16_t wyde) override;
    void FormatAddress(uint64_t address) override;
    void FormatSetAddress(uint64_t saddr) override;
    void FormatTrapName(uint8_t trap) override;
    void FormatOcta(uint64_t octa) override;
    void FormatSymbol(Symbol* symbol) override;
private:
    std::string textFilePath;
    std::ofstream file;
    CodeFormatter formatter;
};

TextFormatter::TextFormatter(cmsx::machine::Machine& machine, BinaryFile* file_) : 
    BinaryFileFormatter(machine, file_), textFilePath(file_->FilePath() + ".txt"), file(textFilePath), formatter(file)
{
}

void TextFormatter::FormatSection(CodeSection* section)
{
    if (section && section->Length() > 0)
    {
        formatter.WriteLine("CODE SECTION");
        formatter.WriteLine();
        formatter.WriteLine("BASE ADDRESS=#" + ToHexString(static_cast<uint64_t>(section->BaseAddress())));
        formatter.WriteLine();
        BinaryFileFormatter::FormatSection(section);
    }
}

void TextFormatter::FormatSection(DataSection* section)
{
    if (section && section->Length() > 0)
    {
        formatter.WriteLine("DATA SECTION");
        formatter.WriteLine();
        formatter.WriteLine("BASE ADDRESS=#" + ToHexString(static_cast<uint64_t>(section->BaseAddress())));
        formatter.WriteLine();
        BinaryFileFormatter::FormatSection(section);
    }
}

void TextFormatter::FormatSection(SymbolSection* section)
{
    if (section && section->Length() > 0)
    {
        formatter.WriteLine("SYMBOL SECTION");
        formatter.WriteLine();
        BinaryFileFormatter::FormatSection(section);
    }
}

void TextFormatter::FormatSection(LinkSection* section)
{
    if (section && section->Length() > 0)
    {
        formatter.WriteLine("LINK SECTION");
        formatter.WriteLine();
        BinaryFileFormatter::FormatSection(section);
    }
}

void TextFormatter::FormatSection(DebugSection* section)
{
    if (section && section->Length() > 0)
    {
        formatter.WriteLine("DEBUG SECTION");
        formatter.WriteLine();
        BinaryFileFormatter::FormatSection(section);
    }
}

void TextFormatter::FormatSection(ResourceSection* section)
{
    if (section && section->Length() > 0)
    {
        formatter.WriteLine("RESOURCE SECTION");
        formatter.WriteLine();
        BinaryFileFormatter::FormatSection(section);
    }
}

void TextFormatter::FormatCurrentAddress(uint64_t currentAddress)
{
    formatter.Write("#" + ToHexString(currentAddress) + " ");
}

void TextFormatter::FormatAssembledBytes(uint8_t opc, uint8_t x, uint8_t y, uint8_t z)
{
    formatter.Write(ToHexString(opc) + " " + ToHexString(x) + " " + ToHexString(y) + " " + ToHexString(z) + " ");
}

void TextFormatter::FormatLabel(uint64_t currentAddress)
{
    Symbol* symbol = File()->GetSymbolTable().GetSymbol(currentAddress);
    if (symbol)
    {
        if (!symbol->LocalName().empty()) 
        {
            formatter.Write(soulng::util::Format(symbol->LocalName(), 7, FormatWidth::min) + " ");
        }
        else
        {
            formatter.Write(soulng::util::Format(symbol->FullName(), 7, FormatWidth::min) + " ");
        }
    }
    else
    {
        formatter.Write("        ");
    }
}

void TextFormatter::FormatOpCode(const std::string& opCodeName)
{
    formatter.Write(soulng::util::Format(opCodeName, MaxOpCodeNameLength(), FormatWidth::min) + " ");
}

void TextFormatter::FormatByteOperand(uint8_t operand)
{
    formatter.Write("#" + ToHexString(operand));
}

void TextFormatter::FormatRegOperand(uint8_t reg)
{
    Symbol* registerSymbol = File()->GetSymbolTable().GetRegisterSymbol(reg);
    if (registerSymbol)
    {
        formatter.Write(registerSymbol->FullName());
    }
    else
    {
        formatter.Write("$" + std::to_string(static_cast<int>(reg)));
    }
}

void TextFormatter::FormatComma()
{
    formatter.Write(",");
}

void TextFormatter::FormatColon()
{
    formatter.Write(":");
}

void TextFormatter::FormatSpace()
{
    formatter.Write(" ");
}

void TextFormatter::FormatChar(uint8_t x)
{
    if (x >= 32 && x < 127)
    {
        formatter.Write(std::string(1, static_cast<char>(x)));
    }
    else
    {
        formatter.Write(".");
    }
}

void TextFormatter::FormatString(const std::string& s)
{
    formatter.Write(s);
}

void TextFormatter::FormatEol()
{
    formatter.WriteLine();
}

void TextFormatter::FormatUndefined()
{
    formatter.Write("UNDEFINED");
}

void TextFormatter::FormatSpecialReg(uint8_t reg)
{
    Symbol* specialRegisterSymbol = File()->GetSymbolTable().GetSpecialRegisterSymbol(reg);
    if (specialRegisterSymbol)
    {
        formatter.Write(specialRegisterSymbol->FullName());
    }
}

void TextFormatter::FormatWydeOperand(uint16_t wyde)
{
    formatter.Write("#" + ToHexString(wyde));
}

void TextFormatter::FormatAddress(uint64_t address)
{
    formatter.Write("#" + ToHexString(address));
    Symbol* symbol = File()->GetSymbolTable().GetSymbol(address);
    if (symbol)
    {
        if (!symbol->LocalName().empty())
        {
            formatter.Write("[" + symbol->LocalName() + "]");
        }
        else
        {
            formatter.Write("[" + symbol->FullName() + "]");
        }
    }
}

void TextFormatter::FormatSetAddress(uint64_t saddr)
{
    if (saddr == 0xFFFFFFFFFFFFFFFF)
    {
        FormatUndefined();
    }
    else
    {
        FormatAddress(saddr);
    }
}

void TextFormatter::FormatTrapName(uint8_t trap)
{
    Symbol* symbol = File()->GetSymbolTable().GetTrapSymbol(trap);
    if (symbol)
    {
        FormatColon();
        formatter.Write("[" + symbol->FullName() + "]");
    }
}

void TextFormatter::FormatOcta(uint64_t octa)
{
    formatter.Write("#" + ToHexString(octa));
    uint8_t b0 = static_cast<uint8_t>(octa >> 56);
    uint8_t b1 = static_cast<uint8_t>(octa >> 48);
    uint8_t b2 = static_cast<uint8_t>(octa >> 40);
    uint8_t b3 = static_cast<uint8_t>(octa >> 32);
    uint8_t b4 = static_cast<uint8_t>(octa >> 24);
    uint8_t b5 = static_cast<uint8_t>(octa >> 16);
    uint8_t b6 = static_cast<uint8_t>(octa >> 8);
    uint8_t b7 = static_cast<uint8_t>(octa);
    FormatSpace();
    FormatByteOperand(b0);
    FormatSpace();
    FormatByteOperand(b1);
    FormatSpace();
    FormatByteOperand(b2);
    FormatSpace();
    FormatByteOperand(b3);
    FormatSpace();
    FormatByteOperand(b4);
    FormatSpace();
    FormatByteOperand(b5);
    FormatSpace();
    FormatByteOperand(b6);
    FormatSpace();
    FormatByteOperand(b7);
    FormatSpace();
    FormatChar(b0);
    FormatChar(b1);
    FormatChar(b2);
    FormatChar(b3);
    FormatChar(b4);
    FormatChar(b5);
    FormatChar(b6);
    FormatChar(b7);
}

void TextFormatter::FormatSymbol(Symbol* symbol) 
{
    symbol->Print(formatter);
}

TrapAdderFunc trapAdder = nullptr;

void SetTrapAdderFunc(TrapAdderFunc trapAdderFunc)
{
    trapAdder = trapAdderFunc;
}

void AddTrapsToSymbolTable(SymbolTable& symbolTable)
{
    if (trapAdder)
    {
        trapAdder(symbolTable);
    }
}

void WriteBinaryFileAsText(const std::string& binaryFileName, cmsx::machine::Machine& machine, std::string& textFileName)
{
    std::unique_ptr<BinaryFile> binaryFile(ReadBinaryFile(binaryFileName));
    SymbolTable& symbolTable = binaryFile->GetSymbolTable();
    AddTrapsToSymbolTable(symbolTable);
    TextFormatter formatter(machine, binaryFile.get());
    textFileName = formatter.TextFilePath();
    formatter.Format();
}

} // namespace cmsx::object
