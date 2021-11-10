// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/StatusBar.hpp>
#include <wing/Theme.hpp>
#include <soulng/util/Unicode.hpp>

#undef max
#undef min

namespace cmajor { namespace wing {

using namespace soulng::unicode;

Color DefaultTextColor()
{
    return Color::Black;
}

Color DefaultTopLineColor()
{
    return Color(215, 215, 215);
}

Color DefaultSunkenBorderOuterTopLeftColor()
{
    return Color(160, 160, 160);
}

Color DefaultSunkenBorderInnerTopLeftColor()
{
    return Color(105, 105, 105);
}

Color DefaultSunkenBorderOuterRightBottomColor()
{
    return Color::White;
}

Color DefaultSunkenBorderInnerRightBottomColor()
{
    return Color(227, 227, 227);
}

Color DefaultRaisedBorderOuterTopLeftColor()
{
    return Color(227, 227, 227);
}

Color DefaultRaisedBorderOuterRightBottomColor()
{
    return Color(105, 105, 105);
}

Color DefaultRaisedBorderInnerTopLeftColor()
{
    return Color::White;
}

Color DefaultRaisedBorderInnerRightBottomColor()
{
    return Color(160, 160, 160);
}

StatusBarCreateParams::StatusBarCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("wing.StatusBar");
    controlCreateParams.SetDock(Dock::bottom);
    textColor = DefaultTextColor();
    topLineColor = DefaultTopLineColor();
    sunkenBorderOuterTopLeftColor = DefaultSunkenBorderOuterTopLeftColor();
    sunkenBorderInnerTopLeftColor = DefaultSunkenBorderInnerTopLeftColor();
    sunkenBorderOuterRightBottomColor = DefaultSunkenBorderOuterRightBottomColor();
    sunkenBorderInnerRightBottomColor = DefaultSunkenBorderInnerRightBottomColor();
    raisedBorderOuterTopLeftColor = DefaultRaisedBorderOuterTopLeftColor();
    raisedBorderOuterRightBottomColor = DefaultRaisedBorderOuterRightBottomColor();
    raisedBorderInnerTopLeftColor = DefaultRaisedBorderInnerTopLeftColor();
    raisedBorderInnerRightBottomColor = DefaultRaisedBorderInnerRightBottomColor();
    fontFamilyName = "Segoe UI";
    fontSize = 9.0f;
    padding = Padding(4, 4, 4, 4);
    statusBarItemPadding = Padding(2, 2, 2, 2);
    horizontalItemSpacing = 4;
}

StatusBarCreateParams& StatusBarCreateParams::Defaults()
{
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::TextColor(const Color& textColor_)
{
    textColor = textColor_;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::Text(const std::string& text_) 
{
    controlCreateParams.Text(text_);
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::TopLineColor(const Color& color)
{
    topLineColor = color;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::SunkenBorderOuterTopLeftColor(const Color& color)
{
    sunkenBorderOuterTopLeftColor = color;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::SunkenBorderInnerTopLeftColor(const Color& color)
{
    sunkenBorderInnerTopLeftColor = color;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::SunkenBorderOuterRightBottomColor(const Color& color)
{
    sunkenBorderOuterRightBottomColor = color;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::SunkenBorderInnerRightBottomColor(const Color& color)
{
    sunkenBorderInnerRightBottomColor = color;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::RaisedBorderOuterTopLeftColor(const Color& color)
{
    raisedBorderOuterTopLeftColor = color;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::RaisedBorderOuterRightBottomColor(const Color& color)
{
    raisedBorderOuterRightBottomColor = color;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::RaisedBorderInnerTopLeftColor(const Color& color)
{
    raisedBorderInnerTopLeftColor = color;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::RaisedBorderInnerRightBottomColor(const Color& color)
{
    raisedBorderInnerRightBottomColor = color;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::FontFamilyName(const std::string& fontFamilyName_)
{
    fontFamilyName = fontFamilyName_;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::FontSize(float fontSize_)
{
    fontSize = fontSize_;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::SetPadding(const Padding& padding_)
{
    padding = padding_;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::StatusBarItemPadding(const Padding& itemPadding)
{
    statusBarItemPadding = itemPadding;
    return *this;
}

StatusBarCreateParams& StatusBarCreateParams::HorizontalItemSpacing(int spacing)
{
    horizontalItemSpacing = spacing;
    return *this;
}

StatusBarPens::StatusBarPens(StatusBar* statusBar_) : 
    statusBar(statusBar_),
    topLinePen(nullptr),
    sunkenBorderOuterTopLeftPen(nullptr),
    sunkenBorderInnerTopLeftPen(nullptr),
    sunkenBorderOuterRightBottomPen(nullptr),
    sunkenBorderInnerRightBottomPen(nullptr),
    raisedBorderOuterTopLeftPen(nullptr),
    raisedBorderOuterRightBottomPen(nullptr),
    raisedBorderInnerTopLeftPen(nullptr),
    raisedBorderInnerRightBottomPen(nullptr)
{
}

Pen* StatusBarPens::TopLinePen()
{
    if (topLinePen) return topLinePen;
    topLinePen = new Pen(statusBar->TopLineColor());
    pens.push_back(std::unique_ptr<Pen>(topLinePen));
    return topLinePen;
}

Pen* StatusBarPens::SunkenBorderOuterTopLeftPen()
{
    if (sunkenBorderOuterTopLeftPen) return sunkenBorderOuterTopLeftPen;
    sunkenBorderOuterTopLeftPen = new Pen(statusBar->SunkenBorderOuterTopLeftColor());
    pens.push_back(std::unique_ptr<Pen>(sunkenBorderOuterTopLeftPen));
    return sunkenBorderOuterTopLeftPen;
}

Pen* StatusBarPens::SunkenBorderInnerTopLeftPen()
{
    if (sunkenBorderInnerTopLeftPen) return sunkenBorderInnerTopLeftPen;
    sunkenBorderInnerTopLeftPen = new Pen(statusBar->SunkenBorderInnerTopLeftColor());
    pens.push_back(std::unique_ptr<Pen>(sunkenBorderInnerTopLeftPen));
    return sunkenBorderInnerTopLeftPen;
}

Pen* StatusBarPens::SunkenBorderOuterRightBottomPen()
{
    if (sunkenBorderOuterRightBottomPen) return sunkenBorderOuterRightBottomPen;
    sunkenBorderOuterRightBottomPen = new Pen(statusBar->SunkenBorderOuterRightBottomColor());
    pens.push_back(std::unique_ptr<Pen>(sunkenBorderOuterRightBottomPen));
    return sunkenBorderOuterRightBottomPen;
}

Pen* StatusBarPens::SunkenBorderInnerRightBottomPen()
{
    if (sunkenBorderInnerRightBottomPen) return sunkenBorderInnerRightBottomPen;
    sunkenBorderInnerRightBottomPen = new Pen(statusBar->SunkenBorderInnerRightBottomColor());
    pens.push_back(std::unique_ptr<Pen>(sunkenBorderInnerRightBottomPen));
    return sunkenBorderInnerRightBottomPen;
}

Pen* StatusBarPens::RaisedBorderOuterTopLeftPen()
{
    if (raisedBorderOuterTopLeftPen) return raisedBorderOuterTopLeftPen;
    raisedBorderOuterTopLeftPen = new Pen(statusBar->RaisedBorderOuterTopLeftColor());
    pens.push_back(std::unique_ptr<Pen>(raisedBorderOuterTopLeftPen));
    return raisedBorderOuterTopLeftPen;
}

Pen* StatusBarPens::RaisedBorderOuterRightBottomPen()
{
    if (raisedBorderOuterRightBottomPen) return raisedBorderOuterRightBottomPen;
    raisedBorderOuterRightBottomPen = new Pen(statusBar->RaisedBorderOuterRightBottomColor());
    pens.push_back(std::unique_ptr<Pen>(raisedBorderOuterRightBottomPen));
    return raisedBorderOuterRightBottomPen;
}

Pen* StatusBarPens::RaisedBorderInnerTopLeftPen()
{
    if (raisedBorderInnerTopLeftPen) return raisedBorderInnerTopLeftPen;
    raisedBorderInnerTopLeftPen = new Pen(statusBar->RaisedBorderInnerTopLeftColor());
    pens.push_back(std::unique_ptr<Pen>(raisedBorderInnerTopLeftPen));
    return raisedBorderInnerTopLeftPen;
}

Pen* StatusBarPens::RaisedBorderInnerRightBottomPen()
{
    if (raisedBorderInnerRightBottomPen) return raisedBorderInnerRightBottomPen;
    raisedBorderInnerRightBottomPen = new Pen(statusBar->RaisedBorderInnerRightBottomColor());
    pens.push_back(std::unique_ptr<Pen>(raisedBorderInnerRightBottomPen));
    return raisedBorderInnerRightBottomPen;
}

StatusBar::StatusBar(StatusBarCreateParams& createParams) : 
    Control(createParams.controlCreateParams), 
    items(this),
    flags(StatusBarFlags::none), 
    pens(new StatusBarPens(this)), 
    textBrush(createParams.textColor), 
    padding(createParams.padding),
    itemPadding(createParams.statusBarItemPadding),
    horizontalItemSpacing(createParams.horizontalItemSpacing),
    stringFormat(), 
    centerFormat(),
    charWidth(0),
    charHeight(0)
{
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    centerFormat.SetAlignment(StringAlignment::StringAlignmentCenter);
    centerFormat.SetLineAlignment(StringAlignment::StringAlignmentCenter);
    std::u16string familyName = ToUtf16(createParams.fontFamilyName);
    SetFont(Font(FontFamily((const WCHAR*)familyName.c_str()), createParams.fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    SetDoubleBuffered();
    textColor = createParams.textColor;
    topLineColor = createParams.topLineColor;
    sunkenBorderOuterTopLeftColor = createParams.sunkenBorderOuterTopLeftColor;
    sunkenBorderInnerTopLeftColor = createParams.sunkenBorderInnerTopLeftColor;
    sunkenBorderOuterRightBottomColor = createParams.sunkenBorderOuterRightBottomColor;
    sunkenBorderInnerRightBottomColor = createParams.sunkenBorderInnerRightBottomColor;
    raisedBorderOuterTopLeftColor = createParams.raisedBorderOuterTopLeftColor;
    raisedBorderOuterRightBottomColor = createParams.raisedBorderOuterRightBottomColor;
    raisedBorderInnerTopLeftColor = createParams.raisedBorderInnerTopLeftColor;
    raisedBorderInnerRightBottomColor = createParams.raisedBorderInnerRightBottomColor;
    SetSize(Size(0, initialHeight));
    SetChanged();
}

void StatusBar::AddItem(StatusBarItem* item)
{
    items.AddChild(item);
    SetChanged();
    Invalidate();
}

void StatusBar::SetTopLineItemName(const std::string& topLineItemName_)
{
    topLineItemName = topLineItemName_;
}

void StatusBar::SetSunkenBorderOuterTopLeftItemName(const std::string& sunkenBorderOuterTopLeftItemName_)
{
    sunkenBorderOuterTopLeftItemName = sunkenBorderOuterTopLeftItemName_;
}

void StatusBar::SetSunkenBorderInnerTopLeftItemName(const std::string& sunkenBorderInnerTopLeftItemName_)
{
    sunkenBorderInnerTopLeftItemName = sunkenBorderInnerTopLeftItemName_;
}

void StatusBar::SetSunkenBorderOuterRightBottomItemName(const std::string& sunkenBorderOuterRightBottomItemName_)
{
    sunkenBorderOuterRightBottomItemName = sunkenBorderOuterRightBottomItemName_;
}

void StatusBar::SetSunkenBorderInnerRightBottomItemName(const std::string& sunkenBorderInnerRightBottomItemName_)
{
    sunkenBorderInnerRightBottomItemName = sunkenBorderInnerRightBottomItemName_;
}

void StatusBar::SetRaisedBorderOuterTopLeftItemName(const std::string& raisedBorderOuterTopLeftItemName_)
{
    raisedBorderOuterTopLeftItemName = raisedBorderOuterTopLeftItemName_;
}

void StatusBar::SetRaisedBorderOuterRightBottomItemName(const std::string& raisedBorderOuterRightBottomItemName_)
{
    raisedBorderOuterRightBottomItemName = raisedBorderOuterRightBottomItemName_;
}

void StatusBar::SetRaisedBorderInnerTopLeftItemName(const std::string& raisedBorderInnerTopLeftItemName_)
{
    raisedBorderInnerTopLeftItemName = raisedBorderInnerTopLeftItemName_;
}

void StatusBar::SetRaisedBorderInnerRightBottomItemName(const std::string& raisedBorderInnerRightBottomItemName_)
{
    raisedBorderInnerRightBottomItemName = raisedBorderInnerRightBottomItemName_;
}

void StatusBar::UpdateColors()
{
    Control::UpdateColors();
    textColor = GetColor(TextItemName());
    topLineColor = GetColor(topLineItemName);
    sunkenBorderOuterTopLeftColor = GetColor(sunkenBorderOuterTopLeftItemName);
    sunkenBorderInnerTopLeftColor = GetColor(sunkenBorderInnerTopLeftItemName);
    sunkenBorderOuterRightBottomColor = GetColor(sunkenBorderOuterRightBottomItemName);
    sunkenBorderInnerRightBottomColor = GetColor(sunkenBorderInnerRightBottomItemName);
    raisedBorderOuterTopLeftColor = GetColor(raisedBorderOuterTopLeftItemName);
    raisedBorderOuterRightBottomColor = GetColor(raisedBorderOuterRightBottomItemName);
    raisedBorderInnerTopLeftColor = GetColor(raisedBorderInnerTopLeftItemName);
    raisedBorderInnerRightBottomColor = GetColor(raisedBorderInnerRightBottomItemName);
    pens.reset(new StatusBarPens(this));
}

void StatusBar::OnPaint(PaintEventArgs& args)
{
    if (Changed())
    {
        ResetChanged();
        Measure(args.graphics);;
    }
    args.graphics.Clear(BackgroundColor());
    DrawTopLine(args.graphics);
    DrawItems(args.graphics);
    Control::OnPaint(args);
}

void StatusBar::Measure(Graphics& graphics)
{
    const Font& font = GetFont();
    RectF r = MeasureString(graphics, "M", font, PointF(0, 0), stringFormat);
    charWidth = r.Width;
    charHeight = r.Height;
    SizeItems(graphics);
    LocateItems();
}

void StatusBar::SizeItems(Graphics& graphics)
{
    Size size = GetSize();
    size.Height = 0;
    Component* child = items.FirstChild();
    while (child)
    {
        if (child->IsStatusBarItem())
        {
            StatusBarItem* item = static_cast<StatusBarItem*>(child);
            if (!item->IsEmpty())
            {
                item->Measure(graphics);
                size.Height = std::max(size.Height, item->GetSize().Height);
            }
        }
        child = child->NextSibling();
    }
    if (size.Height != 0)
    {
        size.Height = size.Height + padding.Vertical();
        SetSize(size);
        DockWindow();
    }
}

void StatusBar::LocateItems()
{
    Component* child = items.FirstChild();
    bool hasSpringItem = false;
    Point loc(padding.left, padding.top);
    bool first = true;
    while (child)
    {
        if (child->IsStatusBarItem())
        {
            StatusBarItem* item = static_cast<StatusBarItem*>(child);
            if (!item->IsEmpty())
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    loc.X = loc.X + horizontalItemSpacing;
                }
                if (item->IsStatusBarSpringItem())
                {
                    hasSpringItem = true;
                    break;
                }
                else
                {
                    item->SetLocation(loc);
                    loc.X = loc.X + item->GetSize().Width;
                }
            }
        }
        child = child->NextSibling();
    }
    if (hasSpringItem)
    {
        Point loc(GetSize().Width - padding.right, padding.top);
        bool first = true;
        child = items.LastChild();
        while (child)
        {
            if (child->IsStatusBarItem())
            {
                StatusBarItem* item = static_cast<StatusBarItem*>(child);
                if (!item->IsEmpty())
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        loc.X = loc.X - horizontalItemSpacing;
                    }
                    if (child->IsStatusBarSpringItem())
                    {
                        break;
                    }
                    else
                    {
                        loc.X = loc.X - item->GetSize().Width;
                        item->SetLocation(loc);
                    }
                }
            }
            child = child->PrevSibling();
        }
    }
}

void StatusBar::DrawTopLine(Graphics& graphics)
{
    Size size = GetSize();
    Point start;
    Point end(size.Width - 1, 0);
    CheckGraphicsStatus(graphics.DrawLine(pens->TopLinePen(), start, end));
}

void StatusBar::DrawItems(Graphics& graphics)
{
    Component* child = items.FirstChild();
    while (child)
    {
        if (child->IsStatusBarItem())
        {
            StatusBarItem* item = static_cast<StatusBarItem*>(child);
            if (!item->IsEmpty())
            {
                item->Draw(graphics);
            }
        }
        child = child->NextSibling();
    }
}

StatusBarItem::StatusBarItem() : Component()
{
}

StatusBarItem::~StatusBarItem()
{
}

void StatusBarItem::SetLocation(const Point& location_)
{
    location = location_;
}

void StatusBarItem::SetSize(const Size& size_)
{
    size = size_;
}

void StatusBarItem::Measure(Graphics& graphics)
{
}

void StatusBarItem::Draw(Graphics& graphics)
{
}

StatusBar* StatusBarItem::GetStatusBar() const
{
    Container* container = GetContainer();
    if (container)
    {
        Component* parent = container->Parent();
        if (parent)
        {
            if (parent->IsStatusBar())
            {
                return static_cast<StatusBar*>(parent);
            }
        }
    }
    return nullptr;
}

StatusBarTextItemCreateParams::StatusBarTextItemCreateParams() : borderStyle(StatusBarItemBorderStyle::flat), maxTextLength(0), text()
{
}

StatusBarTextItemCreateParams& StatusBarTextItemCreateParams::Defaults()
{
    return* this;
}

StatusBarTextItemCreateParams& StatusBarTextItemCreateParams::BorderStyle(StatusBarItemBorderStyle borderStyle_)
{
    borderStyle = borderStyle_;
    return *this;
}

StatusBarTextItemCreateParams& StatusBarTextItemCreateParams::MaxTextLength(int length)
{
    maxTextLength = length;
    return *this;
}

StatusBarTextItemCreateParams& StatusBarTextItemCreateParams::Text(const std::string& text_)
{
    text = text_;
    return *this;
}

StatusBarTextItem::StatusBarTextItem(StatusBarTextItemCreateParams& createParams) : 
    StatusBarItem(), 
    borderStyle(createParams.borderStyle), 
    maxTextLength(createParams.maxTextLength),
    text(createParams.text)
{
}

void StatusBarTextItem::SetText(const std::string& text_)
{
    if (text != text_)
    {
        text = text_;
        if (maxTextLength == 0 || text.length() > maxTextLength)
        {
            StatusBar* statusBar = GetStatusBar();
            if (statusBar)
            {
                statusBar->SetChanged();
            }
        }
        Invalidate();
    }
}

void StatusBarTextItem::Invalidate()
{
    StatusBar* statusBar = GetStatusBar();
    if (statusBar)
    {
        statusBar->Invalidate();
    }
}

bool StatusBarTextItem::IsEmpty() const
{
    if (maxTextLength == 0 && text.empty()) return true;
    return false;
}

void StatusBarTextItem::Measure(Graphics& graphics)
{
    RectF rect;
    float textHeight = 0;
    float textWidth = 0;
    Padding itemPadding;
    StatusBar* statusBar = GetStatusBar();
    if (statusBar)
    {
        itemPadding = statusBar->StatusBarItemPadding();
        if (maxTextLength > 0 && text.length() <= maxTextLength)
        {
            textHeight = statusBar->CharHeight();
            textWidth = maxTextLength * statusBar->CharWidth();
        }
        else
        {
            const Font& font = statusBar->GetFont();
            RectF r = MeasureString(graphics, text, font, PointF(0, 0), statusBar->GetStringFormat());
            textHeight = r.Height;
            textWidth = r.Width;
        }
    }
    rect.Width = textWidth;
    rect.Height = textHeight;
    rect.Width = rect.Width + 2 * BorderWidth() + itemPadding.Horizontal();
    rect.Height = rect.Height + 2 * BorderWidth() + itemPadding.Vertical();
    SetSize(Size(static_cast<int>(rect.Width), static_cast<int>(rect.Height)));
}

void StatusBarTextItem::Draw(Graphics& graphics)
{
    StatusBar* statusBar = GetStatusBar();
    if (!statusBar) return;
    DrawBorder(graphics);
    Point loc = Location();
    Size sz = GetSize();
    RectF rect(loc.X, loc.Y, sz.Width, sz.Height);
    const Font& font = statusBar->GetFont();
    DrawString(graphics, text, font, rect, statusBar->CenterFormat(), statusBar->TextBrush());
}

void StatusBarTextItem::DrawBorder(Graphics& graphics)
{
    switch (borderStyle)
    {
        case StatusBarItemBorderStyle::raised:
        {
            StatusBar* statusBar = GetStatusBar();
            if (statusBar)
            {
                DrawBorder(graphics,
                    statusBar->Pens().RaisedBorderOuterTopLeftPen(),
                    statusBar->Pens().RaisedBorderInnerTopLeftPen(),
                    statusBar->Pens().RaisedBorderOuterRightBottomPen(),
                    statusBar->Pens().RaisedBorderInnerRightBottomPen());
            }
        }
        case StatusBarItemBorderStyle::sunken:
        {
            StatusBar* statusBar = GetStatusBar();
            if (statusBar)
            {
                DrawBorder(graphics,
                    statusBar->Pens().SunkenBorderOuterTopLeftPen(),
                    statusBar->Pens().SunkenBorderInnerTopLeftPen(),
                    statusBar->Pens().SunkenBorderOuterRightBottomPen(),
                    statusBar->Pens().SunkenBorderInnerRightBottomPen());
            }
        }
    }
}

void StatusBarTextItem::DrawBorder(Graphics& graphics, Pen* outerTopLeftPen, Pen* innerTopLeftPen, Pen* outerRightBottomPen, Pen* innerRightBottomPen)
{
    Point location = Location();
    Size size = GetSize();
    CheckGraphicsStatus(graphics.DrawLine(outerTopLeftPen, location, Point(location.X + size.Width - 2, location.Y)));
    CheckGraphicsStatus(graphics.DrawLine(outerTopLeftPen, location, Point(location.X, location.Y + size.Height - 2)));
    CheckGraphicsStatus(graphics.DrawLine(outerRightBottomPen, Point(location.X + size.Width - 1, location.Y), Point(location.X + size.Width - 1, location.Y + size.Height - 1)));
    CheckGraphicsStatus(graphics.DrawLine(outerRightBottomPen, Point(location.X, location.Y + size.Height - 1), Point(location.X + size.Width - 1, location.Y + size.Height - 1)));
    CheckGraphicsStatus(graphics.DrawLine(innerTopLeftPen, Point(location.X + 1, location.Y + 1), Point(location.X + size.Width - 3, location.Y + 1)));
    CheckGraphicsStatus(graphics.DrawLine(innerTopLeftPen, Point(location.X + 1, location.Y + 1), Point(location.X + 1, location.Y + size.Height - 3)));
    CheckGraphicsStatus(graphics.DrawLine(innerRightBottomPen, Point(location.X + size.Width - 2, location.Y + 1), Point(location.X + size.Width - 2, location.Y + size.Height - 2)));
    CheckGraphicsStatus(graphics.DrawLine(innerRightBottomPen, Point(location.X + 1, location.Y + size.Height - 2), Point(location.X + size.Width - 2, location.Y + size.Height - 2)));
}

StatusBarSpringItem::StatusBarSpringItem() : StatusBarItem()
{
}

} } // cmajor::wing
