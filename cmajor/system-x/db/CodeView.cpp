// =================================
// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/db/CodeView.hpp>
#include <system-x/object/InstructionFormatter.hpp>
#include <system-x/object/Symbol.hpp>
#include <system-x/machine/OpCode.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

#undef min
#undef max

namespace cmsx::db {

using namespace soulng::util;
using namespace soulng::unicode;

CodeViewItem::CodeViewItem(CodeViewItemKind kind_, const std::u32string& text_) : kind(kind_), text(text_)
{
}

CodeViewLine::CodeViewLine() 
{
}

void CodeViewLine::AddItem(CodeViewItem&& item)
{
    items.emplace_back(std::move(item));
}

class CodeViewLineBuilder : public cmsx::object::InstructionFormatter
{
public:
    CodeViewLineBuilder(cmsx::machine::Machine& machine_, cmsx::object::SymbolTable& symbolTable_);
    void FormatCurrentAddress(uint64_t currentAddress) override;
    void FormatAssembledBytes(uint8_t opc, uint8_t x, uint8_t y, uint8_t z) override;
    void FormatLabel(uint64_t currentAddress) override;
    void FormatOpCode(const std::string& opCodeName) override;
    void FormatByteOperand(uint8_t operand) override;
    void FormatRegOperand(uint8_t reg) override;
    void FormatComma() override;
    void FormatColon() override;
    void FormatUndefined() override;
    void FormatSpecialReg(uint8_t reg) override;
    void FormatWydeOperand(uint16_t wyde) override;
    void FormatAddress(uint64_t address) override;
    void FormatSetAddress(uint64_t saddr) override;
    CodeViewLine GetLine() { return std::move(line); }
private:
    cmsx::object::SymbolTable& symbolTable;
    CodeViewLine line;
};

CodeViewLineBuilder::CodeViewLineBuilder(cmsx::machine::Machine& machine_, cmsx::object::SymbolTable& symbolTable_) : InstructionFormatter(machine_), symbolTable(symbolTable_)
{
}

void CodeViewLineBuilder::FormatCurrentAddress(uint64_t currentAddress)
{
    line.AddItem(CodeViewItem(CodeViewItemKind::currentAddress, ToUtf32("#" + ToHexString(currentAddress))));
    line.AddItem(CodeViewItem(CodeViewItemKind::space, ToUtf32(" ")));
}

void CodeViewLineBuilder::FormatAssembledBytes(uint8_t opc, uint8_t x, uint8_t y, uint8_t z)
{
    line.AddItem(CodeViewItem(CodeViewItemKind::assembledByte, ToUtf32(ToHexString(opc))));
    line.AddItem(CodeViewItem(CodeViewItemKind::space, ToUtf32(" ")));
    line.AddItem(CodeViewItem(CodeViewItemKind::assembledByte, ToUtf32(ToHexString(x))));
    line.AddItem(CodeViewItem(CodeViewItemKind::space, ToUtf32(" ")));
    line.AddItem(CodeViewItem(CodeViewItemKind::assembledByte, ToUtf32(ToHexString(y))));
    line.AddItem(CodeViewItem(CodeViewItemKind::space, ToUtf32(" ")));
    line.AddItem(CodeViewItem(CodeViewItemKind::assembledByte, ToUtf32(ToHexString(z))));
    line.AddItem(CodeViewItem(CodeViewItemKind::space, ToUtf32(" ")));
}

void CodeViewLineBuilder::FormatLabel(uint64_t currentAddress)
{
    cmsx::object::Symbol* symbol = symbolTable.GetSymbol(currentAddress);
    if (symbol)
    {
        if (!symbol->LocalName().empty())
        {
            line.AddItem(CodeViewItem(CodeViewItemKind::label, ToUtf32(soulng::util::Format(symbol->LocalName(), 7, FormatWidth::min))));
            line.AddItem(CodeViewItem(CodeViewItemKind::space, ToUtf32(" ")));
        }
        else
        {
            line.AddItem(CodeViewItem(CodeViewItemKind::label, ToUtf32(soulng::util::Format(symbol->FullName(), 7, FormatWidth::min))));
            line.AddItem(CodeViewItem(CodeViewItemKind::space, ToUtf32(" ")));
        }
    }
    else
    {
        line.AddItem(CodeViewItem(CodeViewItemKind::space, ToUtf32("        ")));
    }
}

void CodeViewLineBuilder::FormatOpCode(const std::string& opCodeName)
{
    line.AddItem(CodeViewItem(CodeViewItemKind::opCode, ToUtf32(soulng::util::Format(opCodeName, cmsx::machine::MaxOpCodeNameLength(), FormatWidth::min))));
    line.AddItem(CodeViewItem(CodeViewItemKind::space, ToUtf32(" ")));
}

void CodeViewLineBuilder::FormatByteOperand(uint8_t operand)
{
    line.AddItem(CodeViewItem(CodeViewItemKind::number, ToUtf32("#" + ToHexString(operand))));
}

void CodeViewLineBuilder::FormatRegOperand(uint8_t reg)
{
    cmsx::object::Symbol* registerSymbol = symbolTable.GetRegisterSymbol(reg);
    if (registerSymbol)
    {
        line.AddItem(CodeViewItem(CodeViewItemKind::reg, ToUtf32(registerSymbol->FullName())));
    }
    else
    {
        line.AddItem(CodeViewItem(CodeViewItemKind::reg, ToUtf32("$" + std::to_string(static_cast<int>(reg)))));
    }
}

void CodeViewLineBuilder::FormatComma()
{
    line.AddItem(CodeViewItem(CodeViewItemKind::punctuation, ToUtf32(",")));
}

void CodeViewLineBuilder::FormatColon()
{
    line.AddItem(CodeViewItem(CodeViewItemKind::punctuation, ToUtf32(":")));
}

void CodeViewLineBuilder::FormatUndefined()
{
    line.AddItem(CodeViewItem(CodeViewItemKind::undefined, ToUtf32("UNDEFINED")));
}

void CodeViewLineBuilder::FormatSpecialReg(uint8_t reg)
{
    cmsx::object::Symbol* registerSymbol = symbolTable.GetRegisterSymbol(reg);
    if (registerSymbol)
    {
        line.AddItem(CodeViewItem(CodeViewItemKind::reg, ToUtf32(registerSymbol->FullName())));
    }
}

void CodeViewLineBuilder::FormatWydeOperand(uint16_t wyde)
{
    line.AddItem(CodeViewItem(CodeViewItemKind::number, ToUtf32("#" + ToHexString(wyde))));
}

void CodeViewLineBuilder::FormatAddress(uint64_t address)
{
    line.AddItem(CodeViewItem(CodeViewItemKind::address, ToUtf32("#" + ToHexString(address))));
    cmsx::object::Symbol* symbol = symbolTable.GetSymbol(address);
    if (symbol)
    {
        if (!symbol->LocalName().empty())
        {
            line.AddItem(CodeViewItem(CodeViewItemKind::label, ToUtf32("[" + symbol->LocalName() + "]")));
        }
        else
        {
            line.AddItem(CodeViewItem(CodeViewItemKind::label, ToUtf32("[" + symbol->FullName() + "]")));
        }
    }
}

void CodeViewLineBuilder::FormatSetAddress(uint64_t saddr)
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

std::string DefaultCodeViewFontFamilyName()
{
    return "Cascadia Mono";
}

float DefaultCodeViewFontSize()
{
    return 10.0f;
}

Color DefaultSelectLineBackgroundColor()
{
    return Color(255, 236, 196);
}

Color DefaultPCBackgroundColor()
{
    return Color(255, 250, 240);
}

Color DefaultCurrentAddressColor()
{
    return Color(43, 145, 175);
}

Color DefaultPCColor()
{
    return Color(156, 220, 254);
}

Color DefaultAssembledByteColor()
{
    return Color(155, 155, 155);
}

Color DefaultLabelColor()
{
    return Color(78, 201, 176);
}

Color DefaultOpCodeColor()
{
    return Color(86, 156, 214);
}

Color DefaultNumberColor()
{
    return Color(128, 128, 128);
}

Color DefaultRegisterColor()
{
    return Color(214, 157, 133);
}

Color DefaultPunctuationColor()
{
    return Color::Black;
}

Color DefaultUndefinedColor()
{
    return Color::Red;
}

Color DefaultAddressColor()
{
    return Color(43, 145, 175);
}

CodeViewCreateParams::CodeViewCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("system.x.db.CodeView");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(Color::White);
    controlCreateParams.SetDock(Dock::fill);
    fontFamilyName = DefaultCodeViewFontFamilyName();
    fontSize = DefaultCodeViewFontSize();
    selectLineBackgroundColor = DefaultSelectLineBackgroundColor();
}

CodeViewCreateParams& CodeViewCreateParams::Defaults()
{
    return *this;
}

CodeView::CodeView(CodeViewCreateParams& createParams) : 
    Control(createParams.controlCreateParams), machine(nullptr), process(nullptr), lineHeight(0), charWidth(0), viewHeight(0), numLines(0), currentAddress(0)
{
    //SetMinWidth(16);
    //SetMinHeight(16);
    std::u16string fontFamilyName = ToUtf16(createParams.fontFamilyName);
    SetFont(Font(FontFamily((const WCHAR*)fontFamilyName.c_str()), createParams.fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    SetDoubleBuffered();
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    colorMap[CodeViewItemKind::selection_background] = createParams.selectLineBackgroundColor;
    colorMap[CodeViewItemKind::pc_background] = DefaultPCBackgroundColor();
    colorMap[CodeViewItemKind::space] = DefaultPunctuationColor();
    colorMap[CodeViewItemKind::currentAddress] = DefaultCurrentAddressColor();
    colorMap[CodeViewItemKind::assembledByte] = DefaultAssembledByteColor();
    colorMap[CodeViewItemKind::label] = DefaultLabelColor();
    colorMap[CodeViewItemKind::opCode] = DefaultOpCodeColor();
    colorMap[CodeViewItemKind::number] = DefaultNumberColor();
    colorMap[CodeViewItemKind::reg] = DefaultRegisterColor();
    colorMap[CodeViewItemKind::punctuation] = DefaultPunctuationColor();
    colorMap[CodeViewItemKind::undefined] = DefaultUndefinedColor();
    colorMap[CodeViewItemKind::address] = DefaultAddressColor();
}

void CodeView::Reset()
{
    lineMap.clear();
}

void CodeView::SetMachine(cmsx::machine::Machine* machine_)
{
    machine = machine_;
}

void CodeView::SetProcess(cmsx::os::Process* process_)
{
    process = process_;
}

void CodeView::UpdateView()
{
    if (machine)
    {
        currentAddress = machine->Regs().GetPC();
    }
    Invalidate();
}

void CodeView::NextLine()
{
    int64_t addr = std::min(currentAddress + static_cast<int64_t>(4), process->CodeStartAddress() + process->CodeLength());
    currentAddress = addr;
    Invalidate();
}

void CodeView::PrevLine()
{
    int64_t addr = std::max(currentAddress - static_cast<int64_t>(4), process->CodeStartAddress());
    currentAddress = addr;
    Invalidate();
}

void CodeView::NextQuarter()
{
    int64_t addr = std::min(currentAddress + static_cast<int64_t>(4) * 4, process->CodeStartAddress() + process->CodeLength());
    currentAddress = addr;
    Invalidate();
}

void CodeView::PrevQuarter()
{
    int64_t addr = std::max(currentAddress - static_cast<int64_t>(4) * 4, process->CodeStartAddress());
    currentAddress = addr;
    Invalidate();
}

void CodeView::NextPage()
{
    int64_t addr = std::min(currentAddress + static_cast<int64_t>(4) * numLines, process->CodeStartAddress() + process->CodeLength());
    currentAddress = addr;
    Invalidate();
}

void CodeView::PrevPage()
{
    int64_t addr = std::max(currentAddress - static_cast<int64_t>(4) * numLines, process->CodeStartAddress());
    currentAddress = addr;
    Invalidate();
}

void CodeView::ToStart()
{
    currentAddress = process->CodeStartAddress();
    Invalidate();
}

void CodeView::ToEnd()
{
    currentAddress = process->CodeStartAddress() + process->CodeLength();
    Invalidate();
}

void CodeView::OnPaint(PaintEventArgs& args)
{
    try
    {
        args.graphics.Clear(BackgroundColor());
        if (!machine) return;
        if (!process) return;
        if (!process->GetSymbolTable()) return;
        Measure(args.graphics);
        if (numLines > 0)
        {
            int64_t startAddress = process->CodeStartAddress();
            int64_t endAddress = std::min(currentAddress + static_cast<int64_t>(4) * (numLines / 2), process->CodeStartAddress() + process->CodeLength());
            int64_t instructionCount = (endAddress - startAddress) >> 2;
            FetchAddressRange(startAddress, instructionCount);
            PaintLines(args.graphics, currentAddress);
        }
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, ex.what());
    }
}

void CodeView::Measure(Graphics& graphics)
{
    PointF origin;
    std::string testString = "This is a test line";
    RectF r = MeasureString(graphics, testString, GetFont(), origin, stringFormat);
    lineHeight = r.Height;
    charWidth = r.Width / testString.length();
    Size size = GetSize();
    viewHeight = size.Height;
    numLines = (viewHeight / lineHeight) + 1;
}

void CodeView::FetchAddressRange(int64_t startAddress, int64_t instructionCount)
{
    cmsx::machine::Memory& mem = machine->Mem();
    int64_t address = startAddress;
    CodeViewLineBuilder lineBuilder(*machine, *process->GetSymbolTable());
    while (instructionCount > 0)
    {
        if (address >= process->CodeStartAddress() && address < process->CodeStartAddress() + process->CodeLength())
        {
            if (lineMap.find(address) == lineMap.cend())
            {
                uint8_t opc = mem.ReadByte(static_cast<uint64_t>(address), cmsx::machine::Protection::execute);
                uint8_t x = mem.ReadByte(static_cast<uint64_t>(address + 1), cmsx::machine::Protection::execute);
                uint8_t y = mem.ReadByte(static_cast<uint64_t>(address + 2), cmsx::machine::Protection::execute);
                uint8_t z = mem.ReadByte(static_cast<uint64_t>(address + 3), cmsx::machine::Protection::execute);
                lineBuilder.FormatInstruction(address, &setAddressMap, opc, x, y, z);
                CodeViewLine line = lineBuilder.GetLine();
                lineMap[address] = line;
            }
        }
        --instructionCount;
        address += 4;
    }
}

void CodeView::PaintLines(Graphics& graphics, int64_t curAddr)
{
    int64_t pc = machine->Regs().GetPC();
    float center = viewHeight / 2.0f;
    float y = center - (numLines / 2.0f) * lineHeight;
    PointF origin(0, y);
    int64_t address = curAddr - static_cast<int64_t>(4) * (numLines / 2);
    for (int i = 0; i < numLines; ++i)
    {
        CodeViewLine line;
        CodeViewLine* linePtr = &line;
        auto it = lineMap.find(address);
        if (it != lineMap.cend())
        {
            linePtr = &it->second;
        }
        PaintLine(graphics, origin, *linePtr, address, curAddr, pc);
        origin.Y = origin.Y + lineHeight;
        address += 4;
    }
}

void CodeView::PaintLine(Graphics& graphics, const PointF& origin, const CodeViewLine& line, int64_t address, int64_t curAddr, int64_t pc)
{
    if (address == curAddr)
    {
        Size size = GetSize();
        Rect rect(origin.X, origin.Y, size.Width, lineHeight);
        graphics.FillRectangle(&GetItemBrush(CodeViewItemKind::selection_background), rect);
    }
    else if (address == pc)
    {
        Size size = GetSize();
        Rect rect(origin.X, origin.Y, size.Width, lineHeight);
        graphics.FillRectangle(&GetItemBrush(CodeViewItemKind::pc_background), rect);
    }
    PointF pos = origin;
    for (const CodeViewItem& item : line.Items())
    {
        std::string itemText = ToUtf8(item.text);
        DrawString(graphics, itemText, GetFont(), pos, GetItemBrush(item.kind));
        pos.X = pos.X + charWidth * item.text.length();
    }
}

Brush& CodeView::GetItemBrush(CodeViewItemKind itemKind)
{
    auto brushIt = brushMap.find(itemKind);
    if (brushIt != brushMap.cend())
    {
        return *brushIt->second;
    }
    auto colorIt = colorMap.find(itemKind);
    if (colorIt != colorMap.cend())
    {
        Color color = colorIt->second;
        SolidBrush* brush = new SolidBrush(color);
        brushes.push_back(std::unique_ptr<SolidBrush>(brush));
        brushMap[itemKind] = brush;
        return *brush;
    }
    else
    {
        throw std::runtime_error("color for item kind " + std::to_string(static_cast<int>(itemKind)) + " not found");
    }
}

} // namespace cmsx::db
