// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/TabControl.hpp>
#include <wing/Theme.hpp>
#include <soulng/util/Unicode.hpp>

#undef min
#undef max

namespace cmajor { namespace wing {

using namespace soulng::unicode;

Color DefaultTabControlFrameColor()
{
    return Color(204, 206, 219);
}

Color DefaultTabControlTextColor()
{
    return Color::Black;
}

Color DefaultTabControlBackgroundColor()
{
    return DefaultControlBackgroundColor();
}

Color DefaultTabNormalBackgroundColor()
{
    return Color::White;
}

Color DefaultTabSelectedBackgroundColor()
{
    return Color(255, 255, 215);
}

Color DefaultTabControlCloseBoxSelectedColor()
{
    return Color(222, 238, 245);
}

int DefaultTabControlLeadingWidth()
{
    return 0;
}

int DefaultTabControlTopMarginHeight()
{
    return 4;
}

Padding DefaultTabPadding()
{
    return Padding(8, 2, 8, 2);
}

Padding DefaultTabCloseBoxPadding()
{
    return Padding(8, 6, 8, 0);
}

int DefaultTabOverlapWidth()
{
    return 2;
}

float DefaultTabRoundingRadius()
{
    return 8;
}

float DefaultTabCloseBoxPendWidth()
{
    return 1.0f;
}

TabControlCreateParams::TabControlCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("wing.TabControl");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(DefaultTabControlBackgroundColor());
    fontFamilyName = "Segoe UI";
    fontSize = 9.0f;
    frameColor = DefaultTabControlFrameColor();
    textColor = DefaultTabControlTextColor();
    normalBackgroundColor = DefaultTabNormalBackgroundColor();
    selectedBackgroundColor = DefaultTabSelectedBackgroundColor();
    closeBoxSelectedColor = DefaultTabControlCloseBoxSelectedColor();
    leadingWidth = DefaultTabControlLeadingWidth();
    topMarginHeight = DefaultTabControlTopMarginHeight();
    tabPadding = DefaultTabPadding();
    closeBoxPadding = DefaultTabCloseBoxPadding();
    tabOverlapWidth = DefaultTabOverlapWidth();
    tabRoundingRadius = DefaultTabRoundingRadius();
    tabCloseBoxPenWidth = DefaultTabCloseBoxPendWidth();
}

TabControlCreateParams& TabControlCreateParams::Defaults()
{
    return *this;
}

TabControlCreateParams& TabControlCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

TabControlCreateParams& TabControlCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

TabControlCreateParams& TabControlCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

TabControlCreateParams& TabControlCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

TabControlCreateParams& TabControlCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

TabControlCreateParams& TabControlCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

TabControlCreateParams& TabControlCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

TabControlCreateParams& TabControlCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

TabControlCreateParams& TabControlCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

TabControlCreateParams& TabControlCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

TabControlCreateParams& TabControlCreateParams::FontFamilyName(const std::string& name)
{
    fontFamilyName = name;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::FontSize(float size)
{
    fontSize = size;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::FrameColor(const Color& frameColor_)
{
    frameColor = frameColor_;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::TextColor(const Color& textColor_)
{
    textColor = textColor_;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::NormalBackgroundColor(const Color& color)
{
    normalBackgroundColor = color;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::SelectedBackgroundColor(const Color& color)
{
    selectedBackgroundColor = color;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::CloseBoxSelectedColor(const Color& color)
{
    closeBoxSelectedColor = color;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::LeadingWidth(int width)
{
    leadingWidth = width;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::TopMarginHeight(int height)
{
    topMarginHeight = height;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::TabPadding(const Padding& padding)
{
    tabPadding = padding;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::CloseBoxPadding(const Padding& padding)
{
    closeBoxPadding = padding;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::TabOverlapWidth(int width)
{
    tabOverlapWidth = width;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::TabRoundingRadius(float radius)
{
    tabRoundingRadius = radius;
    return *this;
}

TabControlCreateParams& TabControlCreateParams::CloseBoxPenWidth(float width)
{
    tabCloseBoxPenWidth = width;
    return *this;
}

TabControl::TabControl(TabControlCreateParams& createParams) :
    Control(createParams.controlCreateParams),
    tabPages(this),
    selectedTabPage(nullptr),
    fontFamilyName(createParams.fontFamilyName),
    fontSize(createParams.fontSize),
    frameColor(createParams.frameColor),
    framePen(frameColor),
    textColor(createParams.textColor),
    textBrush(textColor),
    normalBackgroundColor(createParams.normalBackgroundColor),
    normalBackgroundBrush(normalBackgroundColor),
    selectedBackgroundColor(createParams.selectedBackgroundColor),
    selectedBackgroundBrush(selectedBackgroundColor),
    closeBoxSelectedColor(createParams.closeBoxSelectedColor),
    closeBoxSelectedBrush(closeBoxSelectedColor),
    leadingWidth(createParams.leadingWidth),
    topMarginHeight(createParams.topMarginHeight),
    tabPadding(createParams.tabPadding),
    closeBoxPadding(createParams.closeBoxPadding),
    tabOverlapWidth(createParams.tabOverlapWidth),
    tabRoundingRadius(createParams.tabRoundingRadius),
    tabCloseBoxPenWidth(createParams.tabCloseBoxPenWidth),
    closeBoxPen(textColor, tabCloseBoxPenWidth),
    headerHeight(0),
    stringFormat(),
    centerFormat(),
    closeStateTabPage(nullptr)
{
    std::u16string familyName = ToUtf16(fontFamilyName);
    SetFont(Font(FontFamily((const WCHAR*)familyName.c_str()), fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    SetChanged();
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    centerFormat.SetAlignment(StringAlignment::StringAlignmentCenter);
    centerFormat.SetLineAlignment(StringAlignment::StringAlignmentCenter);
}

void TabControl::SetSelectedTabPage(TabPage* tabPage)
{
    if (selectedTabPage != tabPage)
    {
        if (selectedTabPage)
        {
            selectedTabPage->Hide();
        }
        selectedTabPage = tabPage;
        if (selectedTabPage)
        {
            selectedTabPage->Show();
            Control* control = selectedTabPage->GetFirstEnabledTabStopControl();
            if (control)
            {
                control->SetFocus();
            }
            OnTabPageSelected();
        }
        Invalidate();
    }
}

void TabControl::AddTabPage(TabPage* tabPage)
{
    AddTabPageToTabPageMap(tabPage);
    tabPages.AddChild(tabPage);
    SetSelectedTabPage(tabPage);
    SetChanged();
}

void TabControl::AddTabPage(const std::string& text, const std::string& key)
{
    AddTabPage(new TabPage(text, key));
}

void TabControl::AddTabPage(const std::string& text)
{
    AddTabPage(text, std::string());
}

void TabControl::CloseTabPage(TabPage* tabPage)
{
    tabPage->Hide();
    RemoveTabPageFromTabPageMap(tabPage);
    if (tabPage == selectedTabPage)
    {
        if (selectedTabPage->NextSibling())
        {
            SetSelectedTabPage(static_cast<TabPage*>(selectedTabPage->NextSibling()));
        }
        else if (selectedTabPage->PrevSibling())
        {
            SetSelectedTabPage(static_cast<TabPage*>(selectedTabPage->PrevSibling()));
        }
        else
        {
            SetSelectedTabPage(nullptr);
        }
    }
    std::unique_ptr<Component> component = tabPages.RemoveChild(tabPage);
    if (component->IsTabPage())
    {
        TabPage* tb = static_cast<TabPage*>(component.get());
        ControlEventArgs args(tb);
        OnControlRemoved(args);
    }
    SetChanged();
    Invalidate();
}

void TabControl::CloseAllTabPages()
{
    if (selectedTabPage)
    {
        selectedTabPage->Hide();
        selectedTabPage = nullptr;
    }
    Component* component = tabPages.FirstChild();
    while (component)
    {
        Component* next = component->NextSibling();
        std::unique_ptr<Component> comp = tabPages.RemoveChild(component);
        if (comp->IsTabPage())
        {
            TabPage* tabPage = static_cast<TabPage*>(comp.get());
            RemoveTabPageFromTabPageMap(tabPage);
            ControlEventArgs args(tabPage);
            OnControlRemoved(args);
        }
        component = next;
    }
    SetChanged();
    Invalidate();
}

void TabControl::SelectNextTabPage()
{
    if (!selectedTabPage) return;
    Component* next = selectedTabPage->NextSibling();
    while (next)
    {
        if (next->IsTabPage())
        {
            TabPage* nextTabPage = static_cast<TabPage*>(next);
            SetSelectedTabPage(nextTabPage);
            return;
        }
        next = next->NextSibling();
    }
    Component* first = tabPages.FirstChild();
    while (first)
    {
        if (first->IsTabPage())
        {
            TabPage* firstTabPage = static_cast<TabPage*>(first);
            SetSelectedTabPage(firstTabPage);
            return;
        }
        first = first->NextSibling();
    }
}

void TabControl::SelectPreviousTabPage()
{
    if (!selectedTabPage) return;
    Component* prev = selectedTabPage->PrevSibling();
    while (prev)
    {
        if (prev->IsTabPage())
        {
            TabPage* prevTabPage = static_cast<TabPage*>(prev);
            SetSelectedTabPage(prevTabPage);
            return;
        }
        prev = prev->PrevSibling();
    }
    Component* last = tabPages.LastChild();
    while (last)
    {
        if (last->IsTabPage())
        {
            TabPage* lastTabPage = static_cast<TabPage*>(last);
            SetSelectedTabPage(lastTabPage);
            return;
        }
        last = last->PrevSibling();
    }
}

int TabControl::IndexOf(TabPage* tabPage) const
{
    int index = 0;
    Component* component = tabPages.FirstChild();
    while (component)
    {
        if (component == tabPage)
        {
            return index;
        }
        component = component->NextSibling();
        ++index;
    }
    return -1;
}

TabPage* TabControl::GetTabPageByKey(const std::string& key) const
{
    auto it = tabPageMap.find(key);
    if (it != tabPageMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void TabControl::AddTabPageToTabPageMap(TabPage* tabPage)
{
    if (!tabPage->Key().empty())
    {
        tabPageMap[tabPage->Key()] = tabPage;
    }
}

void TabControl::RemoveTabPageFromTabPageMap(TabPage* tabPage)
{
    if (!tabPage->Key().empty())
    {
        tabPageMap.erase(tabPage->Key());
    }
}

void TabControl::UpdateColors()
{
    Control::UpdateColors();
    normalBackgroundBrush.SetColor(GetColor(tabNormalBackgroundItemName)); 
    selectedBackgroundBrush.SetColor(GetColor(tabSelectedBackgroundItemName));
    textBrush.SetColor(GetColor(TextItemName()));
    framePen.SetColor(GetColor(FrameItemName()));
    closeBoxPen.SetColor(GetColor(TextItemName()));
    closeBoxSelectedBrush.SetColor(GetColor(closeBoxBackgroundItemName));
}

void TabControl::SetTabNormalBackgroundItemName(const std::string& tabNormalBackgroundItemName_)
{
    tabNormalBackgroundItemName = tabNormalBackgroundItemName_;
}

void TabControl::SetTabSelectedBackgroundItemName(const std::string& tabSelectedBackgroundItemName_)
{
    tabSelectedBackgroundItemName = tabSelectedBackgroundItemName_;
}

void TabControl::SetCloseBoxSelectedBackgroundItemName(const std::string& closeBoxBackgroundItemName_)
{
    closeBoxBackgroundItemName = closeBoxBackgroundItemName_;
}

void TabControl::OnPaint(PaintEventArgs& args)
{
    try
    {
        Gdiplus::SmoothingMode prevSmoothingMode = args.graphics.GetSmoothingMode();
        CheckGraphicsStatus(args.graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality));
        if (Changed())
        {
            ResetChanged();
            Measure(args.graphics);
        }
        args.graphics.Clear(BackgroundColor());
        DrawTabs(args.graphics);
        DrawSelectedTabPage(args.clipRect);
        DrawFrame(args.graphics);
        CheckGraphicsStatus(args.graphics.SetSmoothingMode(prevSmoothingMode));
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void TabControl::OnMouseEnter()
{
    Control::OnMouseEnter();
    closeStateTabPage = nullptr;
}

void TabControl::OnMouseLeave()
{
    Control::OnMouseLeave();
    if (closeStateTabPage)
    {
        closeStateTabPage->SetState(TabState::normal);
        Invalidate(closeStateTabPage->CloseRect());
        closeStateTabPage = nullptr;
    }
}

void TabControl::OnMouseMove(MouseEventArgs& args)
{
    Control::OnMouseMove(args);
    Component* component = tabPages.FirstChild();
    while (component)
    {
        if (component->IsTabPage())
        {
            TabPage* tabPage = static_cast<TabPage*>(component);
            if (tabPage->CloseRect().Contains(args.location))
            {
                if (tabPage->State() == TabState::normal)
                {
                    tabPage->SetState(TabState::closeBoxSelected);
                    closeStateTabPage = tabPage;
                    Invalidate(tabPage->CloseRect());
                    return;
                }
            }
            else if (closeStateTabPage)
            {
                if (tabPage->State() == TabState::closeBoxSelected)
                {
                    closeStateTabPage->SetState(TabState::normal);
                    Invalidate(closeStateTabPage->CloseRect());
                    closeStateTabPage = nullptr;
                    return;
                }
            }
        }
        component = component->NextSibling();
    }
}

void TabControl::OnMouseDown(MouseEventArgs& args)
{
    Control::OnMouseDown(args);
    Component* component = tabPages.FirstChild();
    while (component)
    {
        if (component->IsTabPage())
        {
            TabPage* tabPage = static_cast<TabPage*>(component);
            if (tabPage->SelectRect().Contains(args.location))
            {
                tabPage->Select();
                return;
            }
            else if (tabPage->CloseRect().Contains(args.location))
            {
                tabPage->Close();
                return;
            }
        }
        component = component->NextSibling();
    }
}

void TabControl::OnLocationChanged()
{
    Control::OnLocationChanged();
    SetSelectedTabPagePos();
}

void TabControl::OnSizeChanged()
{
    Control::OnSizeChanged();
    SetSelectedTabPagePos();
}

void TabControl::OnTabPageSelected()
{
    tabPageSelected.Fire();
}

void TabControl::Measure(Graphics& graphics)
{
    headerHeight = 0;
    MeasureWidthsAndHeight(graphics);
    SetVisibility(graphics);
    CalculateMetrics(graphics);
}

void TabControl::MeasureWidthsAndHeight(Graphics& graphics)
{
    Component* component = tabPages.FirstChild();
    while (component)
    {
        if (component->IsTabPage())
        {
            TabPage* tabPage = static_cast<TabPage*>(component);
            tabPage->MeasureWidthAndHeight(graphics);
        }
        component = component->NextSibling();
    }
}

void TabControl::SetVisibility(Graphics& graphics)
{
    Component* component = tabPages.FirstChild();
    if (component)
    {
        if (component->IsTabPage())
        {
            TabPage* firstVisibleTabPage = static_cast<TabPage*>(component);
            int width = GetSize().Width;
            int sum = leadingWidth;
            bool selectedPassed = false;
            while (component)
            {
                if (component->IsTabPage())
                {
                    TabPage* tabPage = static_cast<TabPage*>(component);
                    int w = tabPage->TabWidth();
                    sum = sum + w;
                    if (tabPage == selectedTabPage)
                    {
                        if (sum < width)
                        {
                            firstVisibleTabPage->SetTabVisible(true);
                            while (firstVisibleTabPage != selectedTabPage)
                            {
                                firstVisibleTabPage = static_cast<TabPage*>(firstVisibleTabPage->NextSibling());
                                firstVisibleTabPage->SetTabVisible(true);
                            }
                            selectedPassed = true;
                        }
                        else
                        {
                            sum = sum - firstVisibleTabPage->TabWidth();
                            firstVisibleTabPage->SetTabVisible(false);
                            component = firstVisibleTabPage;
                        }
                    }
                    else if (selectedPassed)
                    {
                        if (sum < width)
                        {
                            tabPage->SetTabVisible(true);
                        }
                        else
                        {
                            tabPage->SetTabVisible(false);
                        }
                    }
                    sum = sum - tabOverlapWidth;
                }
                component = component->NextSibling();
            }
        }
    }
}

void TabControl::CalculateMetrics(Graphics& graphics)
{
    Component* component = tabPages.FirstChild();
    int left = leadingWidth;
    while (component)
    {
        if (component->IsTabPage())
        {
            TabPage* tabPage = static_cast<TabPage*>(component);
            tabPage->CalculateMetrics(graphics, left);
        }
        component = component->NextSibling();
    }
}

void TabControl::DrawTabs(Graphics& graphics)
{
    Component* component = tabPages.FirstChild();
    int left = leadingWidth;
    while (component)
    {
        if (component->IsTabPage())
        {
            TabPage* tabPage = static_cast<TabPage*>(component);
            if (tabPage != selectedTabPage)
            {
                tabPage->DrawTab(graphics);
            }
        }
        component = component->NextSibling();
    }
    if (selectedTabPage)
    {
        selectedTabPage->DrawTab(graphics);
    }
}

void TabControl::SetSelectedTabPagePos()
{
    if (!selectedTabPage) return;
    Point loc(1, headerHeight + 1);
    selectedTabPage->SetLocation(loc);
    Size size = GetSize();
    size.Width = size.Width - 2;
    size.Height = size.Height - headerHeight - 2;
    selectedTabPage->SetSize(size);
    selectedTabPage->DockChildren();
}

void TabControl::DrawSelectedTabPage(const Rect& clipRect)
{
    if (!selectedTabPage) return;
    SetSelectedTabPagePos();
    Rect r(selectedTabPage->Location(), selectedTabPage->GetSize());
    if (!r.IntersectsWith(clipRect)) return;
    selectedTabPage->Invalidate();
}

void TabControl::DrawFrame(Graphics& graphics)
{
    if (!selectedTabPage) return;
    Size size = GetSize();
    CheckGraphicsStatus(graphics.DrawLine(&framePen,
        PointF(0, headerHeight),
        PointF(selectedTabPage->TabLeft(), headerHeight)));
    CheckGraphicsStatus(graphics.DrawLine(&framePen,
        PointF(selectedTabPage->TabLeft() + selectedTabPage->TabWidth(), headerHeight),
        PointF(size.Width - 1, headerHeight)));
    CheckGraphicsStatus(graphics.DrawLine(&framePen,
        PointF(size.Width - 1, headerHeight),
        PointF(size.Width - 1, size.Height - 1)));
    CheckGraphicsStatus(graphics.DrawLine(&framePen,
        PointF(size.Width - 1, size.Height - 1),
        PointF(0, size.Height - 1)));
    CheckGraphicsStatus(graphics.DrawLine(&framePen,
        PointF(0, size.Height - 1),
        PointF(0, headerHeight)));
}

Tab::Tab() : state(TabState::normal), visible(false), textHeight(0), textWidth(0), closeBoxWidth(0), height(0), width(0), left(0),
    leftRoundingRect(), rightRoundingRect(), topRect(), bottomRect(), textRect(), closeBoxRect(), selectRect(), closeRect()
{
}

TabPage::TabPage(const std::string& text_, const std::string& key_) : 
    Panel(ControlCreateParams().WindowClassName("wing.TabPage").WindowClassBackgroundColor(COLOR_WINDOW).BackgroundColor(Color::White).Text(text_)), key(key_)
{
}

void TabPage::OnKeyDown(KeyEventArgs& args)
{
    Control::OnKeyDown(args);
    if (!args.handled)
    {
        switch (args.key)
        {
            case Keys::controlModifier | Keys::f4:
            {
                Close();
                args.handled = true;
                break;
            }
            case Keys::controlModifier | Keys::tab:
            {
                SelectNextTabPage();
                args.handled = true;
                break;
            }
            case Keys::controlModifier | Keys::shiftModifier | Keys::tab:
            {
                SelectPreviousTabPage();
                args.handled = true;
                break;
            }
        }
    }
}

void TabPage::OnTextChanged()
{
    Panel::OnTextChanged();
    tab.width = 0;
    TabControl* tabControl = GetTabControl();
    if (tabControl)
    {
        tabControl->SetChanged();
        tabControl->Invalidate();
    }
}

void TabPage::SetKey(const std::string& key_)
{
    TabControl* tabControl = GetTabControl();
    if (tabControl)
    {
        tabControl->RemoveTabPageFromTabPageMap(this);
    }
    key = key_;
    if (tabControl)
    {
        tabControl->AddTabPageToTabPageMap(this);
    }
}

void TabPage::Select()
{
    TabControl* tabControl = GetTabControl();
    if (tabControl)
    {
        tabControl->SetSelectedTabPage(this);
    }
}

void TabPage::Close()
{
    TabControl* tabControl = GetTabControl();
    if (tabControl)
    {
        tabControl->CloseTabPage(this);
    }
}

void TabPage::SelectNextTabPage()
{
    TabControl* tabControl = GetTabControl();
    if (tabControl)
    {
        tabControl->SelectNextTabPage();
    }
}

void TabPage::SelectPreviousTabPage()
{
    TabControl* tabControl = GetTabControl();
    if (tabControl)
    {
        tabControl->SelectPreviousTabPage();
    }
}

TabControl* TabPage::GetTabControl() const
{
    Control* parentControl = ParentControl();
    if (parentControl && parentControl->IsTabControl())
    {
        return static_cast<TabControl*>(parentControl);
    }
    return nullptr;
}

void TabPage::MeasureWidthAndHeight(Graphics& graphics)
{
    TabControl* tabControl = GetTabControl();
    if (tab.width == 0)
    {
        RectF textRect = MeasureString(graphics, Text(), tabControl->GetFont(), PointF(), tabControl->GetStringFormat());
        RectF closeRect = MeasureString(graphics, "x", tabControl->GetFont(), PointF(), tabControl->GetStringFormat());
        tab.textHeight = textRect.Height;
        tab.textWidth = textRect.Width;
        tab.closeBoxWidth = closeRect.Width;
        tab.height = static_cast<int>(tabControl->TabPadding().Vertical() + tabControl->TabCloseBoxPadding().Vertical() + tab.textHeight);
        tab.width = static_cast<int>(tabControl->TabPadding().Horizontal() + tab.textWidth + tabControl->OverlapWidth() +
            tabControl->TabCloseBoxPadding().Horizontal() + tab.closeBoxWidth);
    }
    tabControl->SetHeaderHeight(std::max(tabControl->HeaderHeight(), tab.height + tabControl->TopMarginHeight()));
}

void TabPage::SetTabVisible(bool visible)
{
    tab.visible = visible;
}

void TabPage::CalculateMetrics(Graphics& graphics, int& left)
{
    if (!tab.visible) return;
    TabControl* tabControl = GetTabControl();
    if (tabControl)
    {
        float roundingRadius = tabControl->RoundingRadius();
        int topMarginHeight = tabControl->TopMarginHeight();
        tab.left = left;
        tab.leftRoundingRect = RectF(PointF(), SizeF(2 * roundingRadius, 2 * roundingRadius));
        tab.leftRoundingRect.Offset(left, topMarginHeight);
        tab.rightRoundingRect = RectF(PointF(), SizeF(2 * roundingRadius, 2 * roundingRadius));
        tab.rightRoundingRect.Offset(left + tab.width - 2 * roundingRadius, topMarginHeight);
        tab.topRect = RectF(PointF(), SizeF(tab.width - 2 * roundingRadius, roundingRadius));
        tab.topRect.Offset(left + roundingRadius, topMarginHeight);
        tab.bottomRect = RectF(PointF(), SizeF(tab.width, tab.height - roundingRadius + 1));
        tab.bottomRect.Offset(left, topMarginHeight + roundingRadius);
        tab.textRect = RectF(PointF(), SizeF(tab.width - (tabControl->TabCloseBoxPadding().Horizontal() + tab.closeBoxWidth), tab.height));
        tab.textRect.Offset(left, topMarginHeight);
        tab.selectRect = Rect(Point(tab.left, topMarginHeight),
            Size(static_cast<int>(tab.width - (tab.closeBoxWidth + tabControl->TabCloseBoxPadding().right)), tab.height));
        tab.closeBoxRect = RectF(
            PointF(tab.left + tab.selectRect.Width,
                topMarginHeight + tabControl->TabPadding().top + tabControl->TabCloseBoxPadding().top),
            SizeF(tab.closeBoxWidth, tab.closeBoxWidth));
        tab.closeRect = Rect(Point(static_cast<int>(tab.closeBoxRect.X), static_cast<int>(tab.closeBoxRect.Y)),
            Size(static_cast<int>(tab.closeBoxRect.Width), static_cast<int>(tab.closeBoxRect.Height)));
        tab.closeRect.Inflate(3, 3);
        tab.closeBoxRect.Inflate(-1, -1);
        left = left + tab.width - tabControl->OverlapWidth();
    }
}

void TabPage::DrawTab(Graphics& graphics)
{
    if (!tab.visible) return;
    TabControl* tabControl = GetTabControl();
    const SolidBrush* backgroundBrush = nullptr;
    if (this == tabControl->SelectedTabPage())
    {
        const SolidBrush& brush = tabControl->TabSelectedBackgroundBrush();
        backgroundBrush = &brush;
    }
    else
    {
        const SolidBrush& brush = tabControl->TabNormalBackgroundBrush();
        backgroundBrush = &brush;
    }
    const Pen& framePen = tabControl->FramePen();
    const Pen& closeBoxPen = tabControl->CloseBoxPen();
    int topMarginHeight = tabControl->TopMarginHeight();
    float roundingRadius = tabControl->RoundingRadius();
    CheckGraphicsStatus(graphics.FillEllipse(backgroundBrush, tab.leftRoundingRect));
    CheckGraphicsStatus(graphics.FillEllipse(backgroundBrush, tab.rightRoundingRect));
    CheckGraphicsStatus(graphics.FillRectangle(backgroundBrush, tab.topRect));
    CheckGraphicsStatus(graphics.FillRectangle(backgroundBrush, tab.bottomRect));
    CheckGraphicsStatus(graphics.DrawLine(&framePen, PointF(tab.left, tab.bottomRect.Y + tab.bottomRect.Height), PointF(tab.left, tab.bottomRect.Y)));
    CheckGraphicsStatus(graphics.DrawArc(&framePen, tab.leftRoundingRect, -180.0f, 90.0f));
    DrawString(graphics, Text(), tabControl->GetFont(), tab.textRect, tabControl->CenterFormat(), tabControl->TextBrush());
    CheckGraphicsStatus(graphics.DrawLine(&framePen,
        PointF(tab.leftRoundingRect.X + roundingRadius, topMarginHeight),
        PointF(tab.rightRoundingRect.X + roundingRadius, topMarginHeight)));
    CheckGraphicsStatus(graphics.DrawArc(&framePen, tab.rightRoundingRect, -90.0f, 90.0f));
    CheckGraphicsStatus(graphics.DrawLine(&framePen,
        PointF(tab.left + tab.width, tab.bottomRect.Y),
        PointF(tab.left + tab.width, tab.bottomRect.Y + tab.bottomRect.Height - 1)));
    if (tab.state == TabState::normal)
    {
        RectF r = tab.closeBoxRect;
        r.Inflate(3, 3);
        CheckGraphicsStatus(graphics.FillRectangle(backgroundBrush, r));
    }
    else if (tab.state == TabState::closeBoxSelected)
    {
        RectF r = tab.closeBoxRect;
        r.Inflate(3, 3);
        const SolidBrush& selectedBrush = tabControl->CloseBoxSelectedBrush();
        CheckGraphicsStatus(graphics.FillRectangle(&selectedBrush, r));
    }
    CheckGraphicsStatus(graphics.DrawLine(&closeBoxPen,
        PointF(tab.closeBoxRect.X, tab.closeBoxRect.Y), PointF(tab.closeBoxRect.X + tab.closeBoxRect.Width, tab.closeBoxRect.Y + tab.closeBoxRect.Height)));
    CheckGraphicsStatus(graphics.DrawLine(&closeBoxPen,
        PointF(tab.closeBoxRect.X, tab.closeBoxRect.Y+ tab.closeBoxRect.Height),
        PointF(tab.closeBoxRect.X + tab.closeBoxRect.Width, tab.closeBoxRect.Y)));
}

void TabPage::SetState(TabState state_)
{
    tab.state = state_;
}

} } // cmajor::wing
