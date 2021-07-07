// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_CODE_COMPLETION_VIEW_INCLUDED
#define CMAJOR_VIEW_CODE_COMPLETION_VIEW_INCLUDED
#include <cmajor/cmview/ViewApi.hpp>
#include <cmajor/wing/BorderedControl.hpp>
#include <cmajor/wing/Container.hpp>

namespace cmajor { namespace view {

using namespace cmajor::wing;

struct CMVIEW_API CCEntry
{
    CCEntry();
    int prefixLength;
    std::string category;
    std::string completion;
    std::string help;
    std::string replacement;
    int symbolIndex;
};

CMVIEW_API std::vector<CCEntry> ParseCCList(const std::string& ccList);

CMVIEW_API Color DefaultCCListNormalBackgroundColor();
CMVIEW_API Color DefaultCCListSelectedBackgroundColor();
CMVIEW_API Color DefaultCCListNormalTextColor();
CMVIEW_API Color DefaultCCListSelectedTextColor();
CMVIEW_API std::string DefaultCCListFontFamilyName();
CMVIEW_API float DefaultCCListFontSize();

const int defaultNumberOfCCLines = 8;

struct CMVIEW_API CCListCreateParams
{
    CCListCreateParams();
    CCListCreateParams& Defaults();
    CCListCreateParams& WindowClassName(const std::string& windowClassName_);
    CCListCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    CCListCreateParams& WindowStyle(int windowStyle_);
    CCListCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    CCListCreateParams& BackgroundColor(const Color& backgroundColor_);
    CCListCreateParams& Text(const std::string& text_);
    CCListCreateParams& Location(Point location_);
    CCListCreateParams& SetSize(Size size_);
    CCListCreateParams& SetAnchors(Anchors anchors_);
    CCListCreateParams& SetDock(Dock dock_);
    CCListCreateParams& FontFamilyName(const std::string& fontFamilyName_);
    CCListCreateParams& FontSize(float fontSize_);
    CCListCreateParams& SelectedBackgroundColor(const Color& color);
    CCListCreateParams& NormalTextColor(const Color& color);
    CCListCreateParams& SelectedTextColor(const Color& color);
    ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
    Color selectedBackgroundColor;
    Color normalTextColor;
    Color selectedTextColor;
};

enum class CCListFlags : int
{
    none = 0, 
    changed = 1 << 0,
    measured = 1 << 1
};

CMVIEW_API inline CCListFlags operator|(CCListFlags left, CCListFlags right)
{
    return CCListFlags(int(left) | int(right));
}

CMVIEW_API inline CCListFlags operator&(CCListFlags left, CCListFlags right)
{
    return CCListFlags(int(left) & int(right));
}

CMVIEW_API inline CCListFlags operator~(CCListFlags flags)
{
    return CCListFlags(~int(flags));
}

class CMVIEW_API CCList : public Control
{
public:
    CCList(CCListCreateParams& createParams);
    void SetContent(const std::vector<CCEntry>& entries_);
    Size DefaultSize() const;
    bool Changed() const;
    void SetChanged();
    void ResetChanged();
    bool Measured() const;
    void SetMeasured();
    void ResetMeasured();
    void Next();
    void Prev();
    void NextPage();
    void PrevPage();
    const CCEntry& GetSelectedEntry() const;
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    void Measure(Graphics& graphics);
    int GetNumberOfVisibleLines() const;
    void SetContentLocation();
    std::vector<CCEntry> entries;
    CCListFlags flags;
    Color selectedBackgroundColor;
    Color normalTextColor;
    Color selectedTextColor;
    int topEntryIndex;
    int selectedEntryIndex;
    StringFormat stringFormat;
    SolidBrush normalBackgroundBrush;
    SolidBrush selectedBackgroundBrush;
    SolidBrush normalTextBrush;
    SolidBrush selectedTextBrush;
    float lineHeight;
    float charWidth;
};

CMVIEW_API Color DefaultCodeCompletionListViewBackgroundColor();
CMVIEW_API Color DefaultCodeCompletionListViewBorderColor();
CMVIEW_API Padding DefaultCodeCompletionListViewPadding();
CMVIEW_API BorderStyle DefaultCodeCompletionListViewBorderStyle();

struct CMVIEW_API CodeCompletionListViewCreateParams
{
    CodeCompletionListViewCreateParams();
    CodeCompletionListViewCreateParams& Defaults();
    CodeCompletionListViewCreateParams& WindowClassName(const std::string& windowClassName_);
    CodeCompletionListViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    CodeCompletionListViewCreateParams& WindowStyle(int windowStyle_);
    CodeCompletionListViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    CodeCompletionListViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    CodeCompletionListViewCreateParams& Text(const std::string& text_);
    CodeCompletionListViewCreateParams& Location(Point location_);
    CodeCompletionListViewCreateParams& SetSize(Size size_);
    CodeCompletionListViewCreateParams& SetAnchors(Anchors anchors_);
    CodeCompletionListViewCreateParams& SetDock(Dock dock_);
    CodeCompletionListViewCreateParams& BorderColor(const Color& color);
    CodeCompletionListViewCreateParams& SetPadding(const Padding& padding_);
    CodeCompletionListViewCreateParams& SetBorderStyle(BorderStyle borderStyle_);
    ControlCreateParams controlCreateParams;
    Color borderColor;
    Padding padding;
    BorderStyle borderStyle;
};

class CMVIEW_API CodeCompletionListView : public Control
{
public:
    CodeCompletionListView(CCListCreateParams& ccListCreateParams, CodeCompletionListViewCreateParams& createParams);
    void SetDefaultSize();
    void SetContent(const std::vector<CCEntry>& entries);
    void Next();
    void Prev();
    void NextPage();
    void PrevPage();
    const CCEntry& GetSelectedEntry() const;
protected:
    void OnLocationChanged() override;
    void OnSizeChanged() override;
    void OnPaint(PaintEventArgs& args) override;
private:
    void SetChildPos();
    Color borderColor;
    Padding padding;
    BorderStyle borderStyle;
    Container container;
    CCList* ccList;
    Control* child;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_CODE_COMPLETION_VIEW_INCLUDED
