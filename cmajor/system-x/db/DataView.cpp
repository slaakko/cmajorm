// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/db/DataView.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

#undef min
#undef max

namespace cmsx::db {

using namespace soulng::util;
using namespace soulng::unicode;

DataRange::DataRange() : machine(nullptr), process(nullptr)
{
}

DataRange::~DataRange()
{
}

void DataRange::SetMachine(cmsx::machine::Machine* machine_)
{
    machine = machine_;
}

void DataRange::SetProcess(cmsx::os::Process* process_)
{
    process = process_;
}

int64_t DataSegmentDataRange::Start() const
{
    return GetProcess()->DataStartAddress();
}

int64_t DataSegmentDataRange::Length() const
{
    return GetProcess()->DataLength();
}

int64_t ArgumentsDataRange::Start() const
{
    return GetProcess()->ArgumentsStartAddress();
}

int64_t ArgumentsDataRange::Length() const
{
    return GetProcess()->ArgumentsLength();
}

int64_t EnvironmentDataRange::Start() const
{
    return GetProcess()->EnvironmentStartAddress();
}

int64_t EnvironmentDataRange::Length() const
{
    return GetProcess()->EnvironmentLength();
}

int64_t HeapDataRange::Start() const
{
    return GetProcess()->HeapStartAddress();
}

int64_t HeapDataRange::Length() const
{
    return GetProcess()->HeapLength();
}

int64_t StackDataRange::Start() const
{
    return cmsx::machine::stackSegmentBaseAddress;
}

int64_t StackDataRange::Length() const
{
    return GetMachine()->Regs().Get(cmsx::machine::regSP) - Start();
}

DataRanges::DataRanges()
{
/*
    DataRange* dataRange = new DataSegmentDataRange();
    ranges.push_back(std::unique_ptr<DataRange>(dataRange));
    rangeMap["data"] = dataRange;
    DataRange* argsRange = new ArgumentsDataRange();
    ranges.push_back(std::unique_ptr<DataRange>(argsRange));
    rangeMap["args"] = argsRange;
    DataRange* envRange = new EnvironmentDataRange();
    ranges.push_back(std::unique_ptr<DataRange>(envRange));
    rangeMap["env"] = envRange;
    DataRange* heapRange = new HeapDataRange();
    ranges.push_back(std::unique_ptr<DataRange>(heapRange));
    rangeMap["heap"] = heapRange;
    DataRange* stackRange = new StackDataRange();
    ranges.push_back(std::unique_ptr<DataRange>(stackRange));
    rangeMap["stack"] = stackRange;
*/
}

DataRange* DataRanges::GetDataRange(const std::string& rangeName) const
{
    auto it = rangeMap.find(rangeName);
    if (it != rangeMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("range '" + rangeName + "' not found");
    }
}

void DataRanges::SetMachine(cmsx::machine::Machine* machine)
{
    for (auto& range : ranges)
    {
        range->SetMachine(machine);
    }
}

void DataRanges::SetProcess(cmsx::os::Process* process)
{
    for (auto& range : ranges)
    {
        range->SetProcess(process);
    }
}

std::string DefaultDataViewFontFamilyName()
{
    return "Cascadia Mono";
}

float DefaultDataViewFontSize()
{
    return 10.0f;
}

Color DefaultDataViewAddressColor()
{
    return Color(43, 145, 175);
}

Color DefaultDataViewOctaColor()
{
    return Color(128, 128, 128);
}

Color DefaultDataViewByteColor()
{
    return Color(128, 128, 128);
}

Color DefaultDataViewCharColor()
{
    return Color(214, 157, 133);
}

DataViewCreateParams::DataViewCreateParams()
{
    controlCreateParams.WindowClassName("system.x.db.DataView");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(Color::White);
    controlCreateParams.SetDock(Dock::fill);
    fontFamilyName = DefaultDataViewFontFamilyName();
    fontSize = DefaultDataViewFontSize();
    addressColor = DefaultDataViewAddressColor();
    octaColor = DefaultDataViewOctaColor();
    byteColor = DefaultDataViewByteColor();
    charColor = DefaultDataViewCharColor();
}

DataViewCreateParams& DataViewCreateParams::Defaults()
{
    return *this;
}

DataView::DataView(DataViewCreateParams& createParams, DataRange* dataRange_) : 
    Control(createParams.controlCreateParams), dataRange(dataRange_), lineHeight(0), charWidth(0), numLines(0),
    addressBrush(createParams.addressColor), octaBrush(createParams.octaColor), byteBrush(createParams.byteColor), charBrush(createParams.charColor)
{
    std::u16string fontFamilyName = ToUtf16(createParams.fontFamilyName);
    SetFont(Font(FontFamily((const WCHAR*)fontFamilyName.c_str()), createParams.fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    SetDoubleBuffered();
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    currentAddress = dataRange->Start();
}

void DataView::SetMachine(cmsx::machine::Machine* machine_)
{
    dataRange->SetMachine(machine_);
}

void DataView::SetProcess(cmsx::os::Process* process_)
{
    dataRange->SetProcess(process_);
}

void DataView::UpdateView()
{
    Invalidate();
}

void DataView::NextLine()
{
    int64_t addr = std::min(currentAddress + static_cast<int64_t>(8), dataRange->Start() + dataRange->Length());
    currentAddress = addr;
    Invalidate();
}

void DataView::PrevLine()
{
    int64_t addr = std::max(currentAddress - static_cast<int64_t>(8), dataRange->Start());
    currentAddress = addr;
    Invalidate();
}

void DataView::NextQuarter()
{
    int64_t addr = std::min(currentAddress + 4 * static_cast<int64_t>(8), dataRange->Start() + dataRange->Length());
    currentAddress = addr;
    Invalidate();
}

void DataView::PrevQuarter()
{
    int64_t addr = std::max(currentAddress - 4 * static_cast<int64_t>(8), dataRange->Start());
    currentAddress = addr;
    Invalidate();
}

void DataView::NextPage()
{
    int64_t addr = std::min(currentAddress + numLines * static_cast<int64_t>(8), dataRange->Start() + dataRange->Length());
    currentAddress = addr;
    Invalidate();
}

void DataView::PrevPage()
{
    int64_t addr = std::max(currentAddress - numLines * static_cast<int64_t>(8), dataRange->Start());
    currentAddress = addr;
    Invalidate();
}

void DataView::ToStart()
{
    currentAddress = dataRange->Start();
    Invalidate();
}

void DataView::ToEnd()
{
    currentAddress = dataRange->Start() + dataRange->Length();
    Invalidate();
}

void DataView::OnPaint(PaintEventArgs& args)
{
    try
    {
        args.graphics.Clear(BackgroundColor());
        if (!dataRange->GetMachine()) return;
        if (!dataRange->GetProcess()) return;
        Measure(args.graphics);
        Paint(args.graphics);
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, ex.what());
    }
}

void DataView::Measure(Graphics& graphics)
{
    PointF origin;
    std::string testString = "This is a test line";
    RectF r = MeasureString(graphics, testString, GetFont(), origin, stringFormat);
    lineHeight = r.Height;
    charWidth = r.Width / testString.length();
}

void DataView::Paint(Graphics& graphics)
{
    int64_t start = currentAddress;
    PointF origin;
    int64_t n = (dataRange->Start() + dataRange->Length()) - start;
    for (int64_t i = 0; i < n; i += 8)
    {
        int64_t address = start + i;
        uint64_t value = dataRange->GetMachine()->Mem().ReadOcta(address, cmsx::machine::Protection::read);
        PrintValue(origin, graphics, address, value, addressBrush, octaBrush, byteBrush, charBrush);
        origin.Y = origin.Y + lineHeight;
    }
}

char PrintChar(uint8_t x)
{
    if (x >= 32 && x < 127)
    {
        return static_cast<char>(x);
    }
    else
    {
        return '.';
    }
}

void DataView::PrintValue(const PointF& origin, Graphics& graphics, uint64_t address, uint64_t value, 
    const SolidBrush& addressBrush, const SolidBrush& octaBrush, const SolidBrush& byteBrush, const SolidBrush& charBrush)
{
    PointF p(origin);
    std::string addrText = "#" + ToHexString(address);
    DrawString(graphics, addrText, GetFont(), p, addressBrush);
    p.X = p.X + charWidth * (addrText.length() + 1);
    std::string octaText = "#" + ToHexString(value);
    DrawString(graphics, octaText, GetFont(), p, octaBrush);
    p.X = p.X + charWidth * (octaText.length() + 1);
    std::string byteText;
    uint8_t b0 = static_cast<uint8_t>(value >> 56);
    uint8_t b1 = static_cast<uint8_t>(value >> 48);
    uint8_t b2 = static_cast<uint8_t>(value >> 40);
    uint8_t b3 = static_cast<uint8_t>(value >> 32);
    uint8_t b4 = static_cast<uint8_t>(value >> 24);
    uint8_t b5 = static_cast<uint8_t>(value >> 16);
    uint8_t b6 = static_cast<uint8_t>(value >> 8);
    uint8_t b7 = static_cast<uint8_t>(value);
    byteText.append(ToHexString(b0)).append(" ");
    byteText.append(ToHexString(b1)).append(" ");
    byteText.append(ToHexString(b2)).append(" ");
    byteText.append(ToHexString(b3)).append(" ");
    byteText.append(ToHexString(b4)).append(" ");
    byteText.append(ToHexString(b5)).append(" ");
    byteText.append(ToHexString(b6)).append(" ");
    byteText.append(ToHexString(b7)).append(" ");
    DrawString(graphics, byteText, GetFont(), p, byteBrush);
    p.X = p.X + charWidth * (byteText.length() + 1);
    std::string charText;
    charText.append(1, PrintChar(b0));
    charText.append(1, PrintChar(b1));
    charText.append(1, PrintChar(b2));
    charText.append(1, PrintChar(b3));
    charText.append(1, PrintChar(b4));
    charText.append(1, PrintChar(b5));
    charText.append(1, PrintChar(b6));
    charText.append(1, PrintChar(b7));
    DrawString(graphics, charText, GetFont(), p, charBrush);
}

} // namespace cmsx::db
