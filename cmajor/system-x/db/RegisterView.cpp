// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/db/RegisterView.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmsx::db {

using namespace soulng::util;
using namespace soulng::unicode;

std::string DefaultRegisterViewFontFamilyName()
{
    return "Cascadia Mono";
}

float DefaultRegisterViewFontSize()
{
    return 10.0f;
}

Color DefaultRegNameColor()
{
    return Color(78, 201, 176);
}

Color DefaultRegValueColor()
{
    return Color(155, 155, 155);
}

RegisterViewCreateParams::RegisterViewCreateParams()
{
    controlCreateParams.WindowClassName("system.x.db.RegisterView");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(Color::White);
    controlCreateParams.SetDock(Dock::fill);
    fontFamilyName = DefaultRegisterViewFontFamilyName();
    fontSize = DefaultRegisterViewFontSize();
    regNameColor = DefaultRegNameColor();
    regValueColor = DefaultRegValueColor();
}

RegisterViewCreateParams& RegisterViewCreateParams::Defaults()
{
    return *this;
}

RegisterView::RegisterView(RegisterViewCreateParams& createParams) : 
    Control(createParams.controlCreateParams), machine(nullptr), lineHeight(0), charWidth(0), regNameBrush(createParams.regNameColor), regValueBrush(createParams.regValueColor)
{
    std::u16string fontFamilyName = ToUtf16(createParams.fontFamilyName);
    SetFont(Font(FontFamily((const WCHAR*)fontFamilyName.c_str()), createParams.fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    SetDoubleBuffered();
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
}

void RegisterView::SetMachine(cmsx::machine::Machine* machine_)
{
    machine = machine_;
}

void RegisterView::SetProcess(cmsx::os::Process* process_)
{
}

void RegisterView::UpdateView()
{
    Invalidate();
}

void RegisterView::NextLine()
{
}

void RegisterView::PrevLine()
{
}

void RegisterView::NextQuarter()
{
}
void RegisterView::PrevQuarter()
{
}

void RegisterView::NextPage()
{
}

void RegisterView::PrevPage()
{
}

void RegisterView::ToStart()
{
}

void RegisterView::ToEnd()
{
}

void RegisterView::OnPaint(PaintEventArgs& args)
{
    try
    {
        args.graphics.Clear(BackgroundColor());
        if (!machine) return;
        Measure(args.graphics);
        Paint(args.graphics);
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(nullptr, ex.what());
    }
}

void RegisterView::Measure(Graphics& graphics)
{
    PointF origin;
    std::string testString = "This is a test line";
    RectF r = MeasureString(graphics, testString, GetFont(), origin, stringFormat);
    lineHeight = r.Height;
    charWidth = r.Width / testString.length();
}

void RegisterView::Paint(Graphics& graphics)
{
    PointF origin;
    float col1x = 21 * charWidth;
    float col2x = 2 * 21 * charWidth;
    PrintReg(origin, graphics, "ax", "#" + ToHexString(machine->Regs().Get(cmsx::machine::regAX)));
    PrintReg(PointF(origin.X + col1x, origin.Y), graphics, "ex", "#" + ToHexString(machine->Regs().Get(cmsx::machine::regEX)));
    PrintReg(PointF(origin.X + col2x, origin.Y), graphics, "$0", "#" + ToHexString(machine->Regs().Get(0)));
    origin.Y = origin.Y + lineHeight;
    PrintReg(origin, graphics, "bx", "#" + ToHexString(machine->Regs().Get(cmsx::machine::regBX)));
    PrintReg(PointF(origin.X + col1x, origin.Y), graphics, "ix", "#" + ToHexString(machine->Regs().Get(cmsx::machine::regIX)));
    PrintReg(PointF(origin.X + col2x, origin.Y), graphics, "$1", "#" + ToHexString(machine->Regs().Get(1)));
    origin.Y = origin.Y + lineHeight;
    PrintReg(origin, graphics, "cx", "#" + ToHexString(machine->Regs().Get(cmsx::machine::regCX)));
    PrintReg(PointF(origin.X + col1x, origin.Y), graphics, "fp", "#" + ToHexString(machine->Regs().Get(cmsx::machine::regFP)));
    PrintReg(PointF(origin.X + col2x, origin.Y), graphics, "$2", "#" + ToHexString(machine->Regs().Get(2)));
    origin.Y = origin.Y + lineHeight;
    PrintReg(origin, graphics, "dx", "#" + ToHexString(machine->Regs().Get(cmsx::machine::regDX)));
    PrintReg(PointF(origin.X + col1x, origin.Y), graphics, "sp", "#" + ToHexString(machine->Regs().Get(cmsx::machine::regSP)));
    PrintReg(PointF(origin.X + col2x, origin.Y), graphics, "$3", "#" + ToHexString(machine->Regs().Get(3)));
}

void RegisterView::PrintReg(const PointF& origin, Graphics& graphics, const std::string& regName, const std::string& regValue)
{
    DrawString(graphics, regName, GetFont(), origin, regNameBrush);
    DrawString(graphics, regValue, GetFont(), PointF(origin.X + charWidth * 3, origin.Y), regValueBrush);
}

} // namespace cmsx::db
