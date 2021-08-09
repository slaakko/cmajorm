// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_STATUS_BAR_INCLUDED
#define CMAJOR_WING_STATUS_BAR_INCLUDED
#include <wing/Control.hpp>
#include <wing/Container.hpp>

namespace cmajor { namespace wing {

WING_API Color DefaultTextColor();
WING_API Color DefaultTopLineColor();
WING_API Color DefaultSunkenBorderOuterTopLeftColor();
WING_API Color DefaultSunkenBorderInnerTopLeftColor();
WING_API Color DefaultSunkenBorderOuterRightBottomColor();
WING_API Color DefaultSunkenBorderInnerRightBottomColor();
WING_API Color DefaultRaisedBorderOuterTopLeftColor();
WING_API Color DefaultRaisedBorderOuterRightBottomColor();
WING_API Color DefaultRaisedBorderInnerTopLeftColor();
WING_API Color DefaultRaisedBorderInnerRightBottomColor();

enum class StatusBarFlags : int
{
    none = 0, changed = 1 << 0
};

WING_API inline StatusBarFlags operator|(StatusBarFlags left, StatusBarFlags right)
{
    return StatusBarFlags(int(left) | int(right));
}

WING_API inline StatusBarFlags operator&(StatusBarFlags left, StatusBarFlags right)
{
    return StatusBarFlags(int(left) & int(right));
}

WING_API inline StatusBarFlags operator~(StatusBarFlags flags)
{
    return StatusBarFlags(~int(flags));
}

const int initialHeight = 20;
const int topLineWidth = 1;

struct WING_API StatusBarCreateParams
{
    StatusBarCreateParams();
    StatusBarCreateParams& Defaults();
    StatusBarCreateParams& WindowClassName(const std::string& windowClassName_);
    StatusBarCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    StatusBarCreateParams& WindowStyle(int windowStyle_);
    StatusBarCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    StatusBarCreateParams& BackgroundColor(const Color& backgroundColor_);
    StatusBarCreateParams& Text(const std::string& text_);
    StatusBarCreateParams& Location(Point location_);
    StatusBarCreateParams& SetSize(Size size_);
    StatusBarCreateParams& SetAnchors(Anchors anchors_);
    StatusBarCreateParams& SetDock(Dock dock_);
    StatusBarCreateParams& TopLineColor(const Color& color);
    StatusBarCreateParams& SunkenBorderOuterTopLeftColor(const Color& color);
    StatusBarCreateParams& SunkenBorderInnerTopLeftColor(const Color& color);
    StatusBarCreateParams& SunkenBorderOuterRightBottomColor(const Color& color);
    StatusBarCreateParams& SunkenBorderInnerRightBottomColor(const Color& color);
    StatusBarCreateParams& RaisedBorderOuterTopLeftColor(const Color& color);
    StatusBarCreateParams& RaisedBorderOuterRightBottomColor(const Color& color);
    StatusBarCreateParams& RaisedBorderInnerTopLeftColor(const Color& color);
    StatusBarCreateParams& RaisedBorderInnerRightBottomColor(const Color& color);
    StatusBarCreateParams& FontFamilyName(const std::string& fontFamilyName_);
    StatusBarCreateParams& FontSize(float fontSize_);
    StatusBarCreateParams& SetPadding(const Padding& padding_);
    StatusBarCreateParams& StatusBarItemPadding(const Padding& itemPadding);
    StatusBarCreateParams& HorizontalItemSpacing(int spacing);
    ControlCreateParams controlCreateParams;
    Color textColor;
    Color topLineColor;
    Color sunkenBorderOuterTopLeftColor;
    Color sunkenBorderInnerTopLeftColor;
    Color sunkenBorderOuterRightBottomColor;
    Color sunkenBorderInnerRightBottomColor;
    Color raisedBorderOuterTopLeftColor;
    Color raisedBorderOuterRightBottomColor;
    Color raisedBorderInnerTopLeftColor;
    Color raisedBorderInnerRightBottomColor;
    std::string fontFamilyName;
    float fontSize;
    Padding padding;
    Padding statusBarItemPadding;
    int horizontalItemSpacing;
};

class StatusBar;

class WING_API StatusBarPens
{
public:
    StatusBarPens(StatusBar* statusBar_);
    StatusBarPens(const StatusBarPens&) = delete;
    StatusBarPens(StatusBarPens&&) = delete;
    StatusBarPens& operator=(const StatusBarPens&) = delete;
    StatusBarPens& operator=(StatusBarPens&&) = delete;
    Pen* TopLinePen(); 
    Pen* SunkenBorderOuterTopLeftPen();
    Pen* SunkenBorderInnerTopLeftPen();
    Pen* SunkenBorderOuterRightBottomPen();
    Pen* SunkenBorderInnerRightBottomPen();
    Pen* RaisedBorderOuterTopLeftPen();
    Pen* RaisedBorderOuterRightBottomPen();
    Pen* RaisedBorderInnerTopLeftPen();
    Pen* RaisedBorderInnerRightBottomPen();
private:
    StatusBar* statusBar;
    Pen* topLinePen;
    Pen* sunkenBorderOuterTopLeftPen;
    Pen* sunkenBorderInnerTopLeftPen;
    Pen* sunkenBorderOuterRightBottomPen;
    Pen* sunkenBorderInnerRightBottomPen;
    Pen* raisedBorderOuterTopLeftPen;
    Pen* raisedBorderOuterRightBottomPen;
    Pen* raisedBorderInnerTopLeftPen;
    Pen* raisedBorderInnerRightBottomPen;
    std::vector<std::unique_ptr<Pen>> pens;
};

class StatusBarItem;

class WING_API StatusBar : public Control
{
public:
    StatusBar(StatusBarCreateParams& createParams);
    void AddItem(StatusBarItem* item);
    bool IsStatusBar() const override { return true; }
    const Color& TopLineColor() const { return topLineColor; }
    const Color& SunkenBorderOuterTopLeftColor() const { return sunkenBorderOuterTopLeftColor; }
    const Color& SunkenBorderInnerTopLeftColor() const { return sunkenBorderInnerTopLeftColor; }
    const Color& SunkenBorderOuterRightBottomColor() const { return sunkenBorderOuterRightBottomColor; }
    const Color& SunkenBorderInnerRightBottomColor() const { return sunkenBorderInnerRightBottomColor; }
    const Color& RaisedBorderOuterTopLeftColor() const { return raisedBorderOuterTopLeftColor; }
    const Color& RaisedBorderOuterRightBottomColor() const { return raisedBorderOuterRightBottomColor; }
    const Color& RaisedBorderInnerTopLeftColor() const { return raisedBorderInnerTopLeftColor; }
    const Color& RaisedBorderInnerRightBottomColor() const { return raisedBorderInnerRightBottomColor; }
    StatusBarPens& Pens() { return pens; }
    const SolidBrush& TextBrush() { return textBrush; }
    const Padding& GetPadding() const { return padding; }
    const Padding& StatusBarItemPadding() const { return itemPadding; }
    int HorizontalItemSpacing() const { return horizontalItemSpacing; }
    const StringFormat& GetStringFormat() const { return stringFormat; }
    const StringFormat& CenterFormat() const { return centerFormat; }
    float CharWidth() const { return charWidth; }
    float CharHeight() const { return charHeight; }
    bool Changed() const { return (flags & StatusBarFlags::changed) != StatusBarFlags::none; }
    void SetChanged() { flags = flags | StatusBarFlags::changed; }
    void ResetChanged() { flags = flags & ~StatusBarFlags::changed; }
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    void Measure(Graphics& graphics);
    void SizeItems(Graphics& graphics);
    void LocateItems();
    void DrawTopLine(Graphics& graphics);
    void DrawItems(Graphics& graphics);
    Container items;
    StatusBarFlags flags;
    Color textColor;
    Color topLineColor;
    Color sunkenBorderOuterTopLeftColor;
    Color sunkenBorderInnerTopLeftColor;
    Color sunkenBorderOuterRightBottomColor;
    Color sunkenBorderInnerRightBottomColor;
    Color raisedBorderOuterTopLeftColor;
    Color raisedBorderOuterRightBottomColor;
    Color raisedBorderInnerTopLeftColor;
    Color raisedBorderInnerRightBottomColor;
    StatusBarPens pens;
    SolidBrush textBrush;
    Padding padding;
    Padding itemPadding;
    int horizontalItemSpacing;
    StringFormat stringFormat;
    StringFormat centerFormat;
    float charWidth;
    float charHeight;
};

class WING_API StatusBarItem : public Component
{
public:
    StatusBarItem();
    virtual ~StatusBarItem();
    bool IsStatusBarItem() const override { return true; }
    virtual bool IsEmpty() const { return false; }
    virtual void Measure(Graphics& graphics);
    virtual void Draw(Graphics& graphics);
    StatusBar* GetStatusBar() const;
    const Point& Location() const { return location; }
    void SetLocation(const Point& location_);
    const Size& GetSize() const { return size; }
    void SetSize(const Size& size_);
private:
    Point location;
    Size size;
};

enum class StatusBarItemBorderStyle : int
{
    flat = 0, sunken = 1, raised = 2
};

struct WING_API StatusBarTextItemCreateParams
{
    StatusBarTextItemCreateParams();
    StatusBarTextItemCreateParams& Defaults();
    StatusBarTextItemCreateParams& BorderStyle(StatusBarItemBorderStyle borderStyle_);
    StatusBarTextItemCreateParams& MaxTextLength(int length);
    StatusBarTextItemCreateParams& Text(const std::string& text_);
    StatusBarItemBorderStyle borderStyle;
    int maxTextLength;
    std::string text;
};

class WING_API StatusBarTextItem : public StatusBarItem
{
public:
    StatusBarTextItem(StatusBarTextItemCreateParams& createParams);
    StatusBarItemBorderStyle BorderStyle() const { return borderStyle; }
    int BorderWidth() const { return 2; }
    const std::string& Text() const { return text; }
    void SetText(const std::string& text_);
    bool IsEmpty() const override;
    void Measure(Graphics& graphics) override;
    void Draw(Graphics& graphics) override;
    virtual void DrawBorder(Graphics& graphics);
    virtual void DrawBorder(Graphics& graphics, Pen* outerTopLeftPen, Pen* innerTopLeftPen, Pen* outerRightBottomPen, Pen* innerRightBottomPen);
private:
    void Invalidate();
    StatusBarItemBorderStyle borderStyle;
    int maxTextLength;
    std::string text;
};

class WING_API StatusBarSpringItem : public StatusBarItem
{
public:
    StatusBarSpringItem();
    bool IsStatusBarSpringItem() const override { return true; }
};

} } // cmajor::wing

#endif // CMAJOR_WING_STATUS_BAR_INCLUDED
