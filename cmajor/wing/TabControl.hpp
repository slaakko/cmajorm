// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_TAB_CONTROL_INCLUDED
#define CMAJOR_WING_TAB_CONTROL_INCLUDED
#include <wing/Panel.hpp>

namespace cmajor { namespace wing {

using TabPageSelectedEvent = Event;

WING_API Color DefaultTabControlFrameColor();
WING_API Color DefaultTabControlTextColor();
WING_API Color DefaultTabControlBackgroundColor();
WING_API Color DefaultTabNormalBackgroundColor();
WING_API Color DefaultTabSelectedBackgroundColor();
WING_API Color DefaultTabControlCloseBoxSelectedColor();
WING_API int DefaultTabControlLeadingWidth();
WING_API int DefaultTabControlTopMarginHeight();
WING_API Padding DefaultTabPadding();
WING_API Padding DefaultTabCloseBoxPadding();
WING_API int DefaultTabOverlapWidth();
WING_API float DefaultTabRoundingRadius();
WING_API float DefaultTabCloseBoxPendWidth();

struct WING_API TabControlCreateParams
{
    TabControlCreateParams();
    TabControlCreateParams& Defaults();
    TabControlCreateParams& WindowClassName(const std::string& windowClassName_);
    TabControlCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    TabControlCreateParams& WindowStyle(int windowStyle_);
    TabControlCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    TabControlCreateParams& BackgroundColor(const Color& backgroundColor_);
    TabControlCreateParams& Text(const std::string& text_);
    TabControlCreateParams& Location(Point location_);
    TabControlCreateParams& SetSize(Size size_);
    TabControlCreateParams& SetAnchors(Anchors anchors_);
    TabControlCreateParams& SetDock(Dock dock_);
    TabControlCreateParams& FontFamilyName(const std::string& name);
    TabControlCreateParams& FontSize(float size);
    TabControlCreateParams& FrameColor(const Color& frameColor_);
    TabControlCreateParams& TextColor(const Color& textColor_);
    TabControlCreateParams& NormalBackgroundColor(const Color& color);
    TabControlCreateParams& SelectedBackgroundColor(const Color& color);
    TabControlCreateParams& CloseBoxSelectedColor(const Color& color);
    TabControlCreateParams& LeadingWidth(int width);
    TabControlCreateParams& TopMarginHeight(int height);
    TabControlCreateParams& TabPadding(const Padding& padding);
    TabControlCreateParams& CloseBoxPadding(const Padding& padding);
    TabControlCreateParams& TabOverlapWidth(int width);
    TabControlCreateParams& TabRoundingRadius(float radius);
    TabControlCreateParams& CloseBoxPenWidth(float width);
    ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
    Color frameColor;
    Color textColor;
    Color normalBackgroundColor;
    Color selectedBackgroundColor;
    Color closeBoxSelectedColor;
    int leadingWidth;
    int topMarginHeight;
    Padding tabPadding;
    Padding closeBoxPadding;
    int tabOverlapWidth;
    float tabRoundingRadius;
    float tabCloseBoxPenWidth;
};

enum class TabControlFlags : int
{
    none = 0, changed = 1 << 0
};

WING_API inline TabControlFlags operator&(TabControlFlags left, TabControlFlags right)
{
    return TabControlFlags(int(left) & int(right));
}

WING_API inline TabControlFlags operator|(TabControlFlags left, TabControlFlags right)
{
    return TabControlFlags(int(left) | int(right));
}

WING_API inline TabControlFlags operator~(TabControlFlags flags)
{
    return TabControlFlags(~int(flags));
}

class TabPage;

class WING_API TabControl : public Control
{
public:
    TabControl(TabControlCreateParams& createParams);
    const Container& TabPages() const { return tabPages; }
    TabPage* SelectedTabPage() const { return selectedTabPage; }
    void SetSelectedTabPage(TabPage* tabPage);
    void AddTabPage(TabPage* tabPage);
    void AddTabPage(const std::string& text, const std::string& key);
    void AddTabPage(const std::string& text);
    void CloseTabPage(TabPage* tabPage);
    void CloseAllTabPages();
    void SelectNextTabPage();
    void SelectPreviousTabPage();
    int IndexOf(TabPage* tabPage) const;
    bool IsTabControl() const override { return true; }
    TabPage* GetTabPageByKey(const std::string& key) const;
    void AddTabPageToTabPageMap(TabPage* tabPage);
    void RemoveTabPageFromTabPageMap(TabPage* tabPage);
    const StringFormat& GetStringFormat() const { return stringFormat; }
    const StringFormat& CenterFormat() const { return centerFormat; }
    const Padding& TabPadding() const { return tabPadding; }
    const Padding& TabCloseBoxPadding() const { return closeBoxPadding; }
    int OverlapWidth() const { return tabOverlapWidth; }
    int HeaderHeight() const { return headerHeight; }
    void SetHeaderHeight(int headerHeight_) { headerHeight = headerHeight_; }
    int TopMarginHeight() const { return topMarginHeight; }
    float RoundingRadius() const { return tabRoundingRadius; }
    const SolidBrush& TabNormalBackgroundBrush() const { return normalBackgroundBrush; }
    const SolidBrush& TabSelectedBackgroundBrush() const { return selectedBackgroundBrush; }
    const SolidBrush& TextBrush() const { return textBrush; }
    const Pen& FramePen() const { return framePen; }
    const Pen& CloseBoxPen() const { return closeBoxPen; }
    const SolidBrush& CloseBoxSelectedBrush() { return closeBoxSelectedBrush; }
    TabPageSelectedEvent& TabPageSelected() { return tabPageSelected; }
    bool Changed() const { return (flags & TabControlFlags::changed) != TabControlFlags::none; }
    void SetChanged() { flags = flags | TabControlFlags::changed; }
    void ResetChanged() { flags = flags & ~TabControlFlags::changed; }
    void UpdateColors() override;
    void SetTabNormalBackgroundItemName(const std::string& tabNormalBackgroundItemName_);
    void SetTabSelectedBackgroundItemName(const std::string& tabSelectedBackgroundItemName_);
    void SetCloseBoxSelectedBackgroundItemName(const std::string& closeBoxBackgroundItemName_);
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnMouseEnter() override;
    void OnMouseLeave() override;
    void OnMouseMove(MouseEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnLocationChanged() override;
    void OnSizeChanged() override;
    virtual void OnTabPageSelected();
private:
    void Measure(Graphics& graphics);
    void MeasureWidthsAndHeight(Graphics& graphics);
    void SetVisibility(Graphics& graphics);
    void CalculateMetrics(Graphics& graphics);
    void DrawTabs(Graphics& graphics);
    void SetSelectedTabPagePos();
    void DrawSelectedTabPage(const Rect& clipRect);
    void DrawFrame(Graphics& graphics);
    Container tabPages;
    TabPage* selectedTabPage;
    TabControlFlags flags;
    std::string fontFamilyName;
    float fontSize;
    Color frameColor;
    Pen framePen;
    Color textColor;
    SolidBrush textBrush;
    Color normalBackgroundColor;
    SolidBrush normalBackgroundBrush;
    Color selectedBackgroundColor;
    SolidBrush selectedBackgroundBrush;
    Color closeBoxSelectedColor;
    SolidBrush closeBoxSelectedBrush;
    int leadingWidth;
    int topMarginHeight;
    Padding tabPadding;
    Padding closeBoxPadding;
    int tabOverlapWidth;
    float tabRoundingRadius;
    float tabCloseBoxPenWidth;
    Pen closeBoxPen;
    int headerHeight;
    StringFormat stringFormat;
    StringFormat centerFormat;
    TabPage* closeStateTabPage;
    std::unordered_map<std::string, TabPage*> tabPageMap;
    TabPageSelectedEvent tabPageSelected;
    std::string tabNormalBackgroundItemName;
    std::string tabSelectedBackgroundItemName;
    std::string closeBoxBackgroundItemName;
};

enum class TabState : int
{
    normal, closeBoxSelected
};

struct WING_API Tab
{
    Tab();
    TabState state;
    bool visible;
    float textHeight;
    float textWidth;
    float closeBoxWidth;
    int height;
    int width;
    int left;
    RectF leftRoundingRect;
    RectF rightRoundingRect;
    RectF topRect;
    RectF bottomRect;
    RectF textRect;
    RectF closeBoxRect;
    Rect selectRect;
    Rect closeRect;
};

class WING_API TabPage : public Panel
{
public:
    TabPage(const std::string& text_, const std::string& key_);
    bool IsTabPage() const override { return true; }
    const std::string& Key() const { return key; }
    void SetKey(const std::string& key_);
    void Select();
    void Close();
    void SelectNextTabPage();
    void SelectPreviousTabPage();
    TabControl* GetTabControl() const;
    void MeasureWidthAndHeight(Graphics& graphics);
    int TabWidth() const { return tab.width; }
    int TabLeft() const { return tab.left; }
    void SetTabVisible(bool visible);
    void CalculateMetrics(Graphics& graphics, int& left);
    void DrawTab(Graphics& graphics);
    void SetState(TabState state_);
    TabState State() const { return tab.state; }
    const Rect& CloseRect() const { return tab.closeRect; }
    const Rect& SelectRect() const { return tab.selectRect; }
protected:
    void OnKeyDown(KeyEventArgs& args) override;
    void OnTextChanged() override;
private:
    std::string key;
    Tab tab;
};

} } // cmajor::wing

#endif // CMAJOR_WING_TAB_CONTROL_INCLUDED
