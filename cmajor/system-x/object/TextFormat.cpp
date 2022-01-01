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
    void FormatCurrentAddress(uint64_t currentAddress) override;
    void FormatAssembledBytes(uint8_t opc, uint8_t x, uint8_t y, uint8_t z) override;
    void FormatLabel(uint64_t currentAddress) override;
    void FormatOpCode(const std::string& opCodeName) override;
    void FormatByteOperand(uint8_t operand) override;
    void FormatRegOperand(uint8_t reg) override;
    void FormatComma() override;
    void FormatColon() override;
    void FormatEol() override;
    void FormatUndefined() override;
    void FormatSpecialReg(uint8_t reg) override;
    void FormatWydeOperand(uint16_t wyde) override;
    void FormatAddress(uint64_t address) override;
    void FormatSetAddress(uint64_t saddr) override;
private:
    std::ofstream file;
    CodeFormatter formatter;
};

TextFormatter::TextFormatter(cmsx::machine::Machine& machine, BinaryFile* file_) : BinaryFileFormatter(machine, file_), file(file_->FilePath() + ".txt"), formatter(file)
{
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
    Symbol* registerSymbol = File()->GetSymbolTable().GetRegisterSymbol(reg);
    if (registerSymbol)
    {
        formatter.Write(registerSymbol->FullName());
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

void WriteBinaryFileAsText(const std::string& binaryFileName, cmsx::machine::Machine& machine)
{
    std::unique_ptr<BinaryFile> binaryFile(ReadBinaryFile(binaryFileName));
    TextFormatter formatter(machine, binaryFile.get());
    formatter.Format();
}

} // namespace cmsx::object
