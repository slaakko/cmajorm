// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_DB_REGISTER_VIEW_INCLUDED
#define CMSX_DB_REGISTER_VIEW_INCLUDED
#include <system-x/db/DebugView.hpp>
#include <wing/Control.hpp>

namespace cmsx::db {

using namespace cmajor::wing;

std::string DefaultRegisterViewFontFamilyName();
float DefaultRegisterViewFontSize();
Color DefaultRegNameColor();
Color DefaultRegValueColor();

struct RegisterViewCreateParams
{
    RegisterViewCreateParams();
    RegisterViewCreateParams& Defaults();
    ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
    Color regNameColor;
    Color regValueColor;
};

class RegisterView : public Control, public DebugView
{
public:
    RegisterView(RegisterViewCreateParams& createParams);
    void SetMachine(cmsx::machine::Machine* machine_) override;
    void SetProcess(cmsx::os::Process* process_) override;
    void UpdateView() override;
    void NextLine() override;
    void PrevLine() override;
    void NextQuarter() override;
    void PrevQuarter() override;
    void NextPage() override;
    void PrevPage() override;
    void ToStart() override;
    void ToEnd() override;
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    void Measure(Graphics& graphics);
    void Paint(Graphics& graphics);
    void PrintReg(const PointF& origin, Graphics& graphics, const std::string& regName, const std::string& regValue);
    cmsx::machine::Machine* machine;
    cmajor::wing::StringFormat stringFormat;
    float lineHeight;
    float charWidth;
    SolidBrush regNameBrush;
    SolidBrush regValueBrush;
};

} // namespace cmsx::db

#endif // CMSX_DB_REGISTER_VIEW_INCLUDED
