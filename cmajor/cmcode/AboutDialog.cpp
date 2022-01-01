// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/AboutDialog.hpp>
#include <cmajor/cmcode/MainWindow.hpp>
#include <cmajor/wing/Label.hpp>
#include <cmajor/wing/Metrics.hpp>

namespace cmcode {

using namespace cmajor::wing;

AboutDialog::AboutDialog() : 
    Window(WindowCreateParams().WindowClassName("cmcode.AboutDialog").Location(DefaultLocation()).WindowStyle(DialogWindowStyle()).Text("About").
    WindowClassBackgroundColor(DefaultControlWindowClassBackgroundColor()).BackgroundColor(DefaultControlBackgroundColor()).
    SetSize(Size(ScreenMetrics::Get().MMToHorizontalPixels(100), ScreenMetrics::Get().MMToVerticalPixels(80))))
{
    SetCaretDisabled();
    Size s = GetSize();
    Size defaultControlSpacing = ScreenMetrics::Get().DefaultControlSpacing();
    Size defaultButtonSize = ScreenMetrics::Get().DefaultButtonSize();
    Size defaultLabelSize = ScreenMetrics::Get().DefaultLabelSize();

    std::u16string fontFamilyName = u"Segoe UI";
    Font font(FontFamily((const WCHAR*)fontFamilyName.c_str()), 14.0f, FontStyle::FontStyleRegular, Unit::UnitPoint);
    std::unique_ptr<Label> label(new Label(LabelCreateParams().Text("Cmajor Code version " + std::string(cmajorCodeVersion) + "\nCopyright (c) 2021 Seppo Laakko").Location(Point(16, 16)).SetSize(defaultLabelSize).
        SetAnchors(Anchors::left | Anchors::top)));
    label->SetFont(std::move(font));
    AddChild(label.release());

    int x = s.Width - defaultButtonSize.Width - defaultControlSpacing.Width;
    int y = s.Height - defaultButtonSize.Height - defaultControlSpacing.Height;

    std::unique_ptr<Button> okButtonPtr(new Button(ControlCreateParams().Location(Point(x, y)).SetSize(defaultButtonSize).Text("OK").SetAnchors(Anchors::right | Anchors::bottom)));
    okButtonPtr->SetDialogResult(DialogResult::ok);
    okButtonPtr->SetDefault();
    SetDefaultButton(okButtonPtr.get());
    AddChild(okButtonPtr.release());
}

} // namespace cmcode
