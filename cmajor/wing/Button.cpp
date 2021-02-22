#include <cmajor/wing/Button.hpp>
#include <cmajor/wing/Window.hpp>

namespace cmajor { namespace wing {

ButtonBase::ButtonBase(ControlCreateParams& createParams) : Control(createParams)
{
}

ControlCreateParams& MakeButtonCreateParams(ControlCreateParams& createParams)
{
    if (createParams.windowClassName.empty())
    {
        createParams.WindowClassName("BUTTON");
    }
    return createParams.WindowStyle(createParams.windowStyle | BS_PUSHBUTTON | BS_NOTIFY | WS_TABSTOP).WindowClassBackgroundColor(COLOR_BTNFACE).BackgroundColor(GetSystemColor(COLOR_BTNFACE));
}

Button::Button(ControlCreateParams& createParams) : ButtonBase(MakeButtonCreateParams(createParams)), isDefault(false), dialogResult(DialogResult::none)
{
    if (WindowStyle() & BS_DEFPUSHBUTTON)
    {
        SetDefault();
    }
}

void Button::SetDefault()
{
    isDefault = true;
    SetDefaultButtonStyle();
}

void Button::SetDefaultButtonStyle()
{
    SetWindowStyle(WindowStyle() | BS_DEFPUSHBUTTON);
    if (Handle())
    {
        SendMessage(Handle(), BM_SETSTYLE, WindowStyle(), 1);
    }
}

void Button::ResetDefault()
{
    isDefault = false;
    ResetDefaultButtonStyle();
}

void Button::ResetDefaultButtonStyle()
{
    SetWindowStyle(WindowStyle() & ~BS_DEFPUSHBUTTON);
    if (Handle())
    {
        SendMessage(Handle(), BM_SETSTYLE, WindowStyle(), 1);
    }
}

void Button::OnCreated()
{
    ButtonBase::OnCreated();
    Graphics graphics(Handle());
    const FontHandle& fontHandle = GetFontHandle(graphics);
    if (!fontHandle.IsNull())
    {
        SendMessage(Handle(), WM_SETFONT, reinterpret_cast<WPARAM>(fontHandle.Handle()), 0);
    }
}

void Button::OnClick()
{
    ButtonBase::OnClick();
    if (dialogResult != DialogResult::none)
    {
        Window* window = GetWindow();
        if (window)
        {
            window->SetDialogResult(dialogResult);
        }
    }
}

void Button::OnGotFocus()
{
    ButtonBase::OnGotFocus();
    if (!IsDefault())
    {
        Window* window = GetWindow();
        if (window)
        {
            Button* defaultButton = window->DefaultButton();
            if (defaultButton)
            {
                defaultButton->ResetDefaultButtonStyle();
            }
        }
        SetDefaultButtonStyle();
    }
}

void Button::OnLostFocus()
{
    ButtonBase::OnLostFocus();
    if (!IsDefault())
    {
        ResetDefaultButtonStyle();
        Window* window = GetWindow();
        if (window)
        {
            Button* defaultButton = window->DefaultButton();
            if (defaultButton)
            {
                defaultButton->SetDefaultButtonStyle();
            }
        }
    }
}

} } // cmajor::wing