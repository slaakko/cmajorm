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

class CodeCompletionListView;

class CMVIEW_API CCList : public Control
{
public:
    CCList(CCListCreateParams& createParams, CodeCompletionListView* listView_);
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
    void ShowHelp();
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
    CodeCompletionListView* listView;
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

class CodeCompletionHelpView;

class CMVIEW_API CodeCompletionListView : public Control
{
public:
    CodeCompletionListView(CCListCreateParams& ccListCreateParams, CodeCompletionListViewCreateParams& createParams, CodeCompletionHelpView* helpView_);
    void SetDefaultSize();
    void SetContent(const std::vector<CCEntry>& entries);
    void Next();
    void Prev();
    void NextPage();
    void PrevPage();
    const CCEntry& GetSelectedEntry() const;
    void SetHelpViewContentAndLocation(const std::string& help, int selectedEntryIndex, int topEntryIndex, float lineHeight) const;
protected:
    void OnLocationChanged() override;
    void OnSizeChanged() override;
    void OnVisibleChanged() override;
    void OnPaint(PaintEventArgs& args) override;
private:
    void SetChildPos();
    Color borderColor;
    Padding padding;
    BorderStyle borderStyle;
    Container container;
    CCList* ccList;
    Control* child;
    CodeCompletionHelpView* helpView;
};

enum class CCHelpFlags : int
{
    none = 0,
    changed = 1 << 0,
};

CMVIEW_API inline CCHelpFlags operator|(CCHelpFlags left, CCHelpFlags right)
{
    return CCHelpFlags(int(left) | int(right));
}

CMVIEW_API inline CCHelpFlags operator&(CCHelpFlags left, CCHelpFlags right)
{
    return CCHelpFlags(int(left) & int(right));
}

CMVIEW_API inline CCHelpFlags operator~(CCHelpFlags flags)
{
    return CCHelpFlags(~int(flags));
}

CMVIEW_API Color DefaultCCHelpBackgroundColor();
CMVIEW_API Color DefaultCCHelpTextColor();
CMVIEW_API std::string DefaultCCHelpFontFamilyName();
CMVIEW_API float DefaultCCHelpFontSize();

struct CMVIEW_API CCHelpCreateParams
{
    CCHelpCreateParams();
    CCHelpCreateParams& Defaults();
    CCHelpCreateParams& WindowClassName(const std::string& windowClassName_);
    CCHelpCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    CCHelpCreateParams& WindowStyle(int windowStyle_);
    CCHelpCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    CCHelpCreateParams& BackgroundColor(const Color& backgroundColor_);
    CCHelpCreateParams& Text(const std::string& text_);
    CCHelpCreateParams& Location(Point location_);
    CCHelpCreateParams& SetSize(Size size_);
    CCHelpCreateParams& SetAnchors(Anchors anchors_);
    CCHelpCreateParams& SetDock(Dock dock_);
    CCHelpCreateParams& FontFamilyName(const std::string& fontFamilyName_);
    CCHelpCreateParams& FontSize(float fontSize_);
    CCHelpCreateParams& TextColor(const Color& color);
    ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
    Color textColor;
};

class CMVIEW_API CCHelp : public Control
{
public:
    CCHelp(CCHelpCreateParams& createParams);
    bool Changed() const;
    void SetChanged();
    void ResetChanged();
protected:
    void OnTextChanged() override;
    void OnPaint(PaintEventArgs& args) override;
private:
    void Measure(Graphics& graphics);
    CCHelpFlags flags;
    StringFormat stringFormat;
    SolidBrush textBrush;
};

CMVIEW_API Color DefaultCodeCompletionHelpViewBackgroundColor();
CMVIEW_API Color DefaultCodeCompletionHelpViewBorderColor();
CMVIEW_API Padding DefaultCodeCompletionHelpViewPadding();
CMVIEW_API BorderStyle DefaultCodeCompletionHelpViewBorderStyle();

struct CMVIEW_API CodeCompletionHelpViewCreateParams
{
    CodeCompletionHelpViewCreateParams();
    CodeCompletionHelpViewCreateParams& Defaults();
    CodeCompletionHelpViewCreateParams& WindowClassName(const std::string& windowClassName_);
    CodeCompletionHelpViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    CodeCompletionHelpViewCreateParams& WindowStyle(int windowStyle_);
    CodeCompletionHelpViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    CodeCompletionHelpViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    CodeCompletionHelpViewCreateParams& Text(const std::string& text_);
    CodeCompletionHelpViewCreateParams& Location(Point location_);
    CodeCompletionHelpViewCreateParams& SetSize(Size size_);
    CodeCompletionHelpViewCreateParams& SetAnchors(Anchors anchors_);
    CodeCompletionHelpViewCreateParams& SetDock(Dock dock_);
    CodeCompletionHelpViewCreateParams& BorderColor(const Color& color);
    CodeCompletionHelpViewCreateParams& SetPadding(const Padding& padding_);
    CodeCompletionHelpViewCreateParams& SetBorderStyle(BorderStyle borderStyle_);
    ControlCreateParams controlCreateParams;
    Color borderColor;
    Padding padding;
    BorderStyle borderStyle;
};

class CMVIEW_API CodeCompletionHelpView : public Control
{
public:
    CodeCompletionHelpView(CCHelpCreateParams& ccHelpCreateParams, CodeCompletionHelpViewCreateParams& createParams);
    void SetContentAndLocation(const std::string& content, const Point& location);
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
    CCHelp* ccHelp;
    Control* child;
};

CMVIEW_API std::vector<std::string> ParseParamHelpList(const std::string& paramHelpList);

CMVIEW_API Color DefaultParamHelpBackgroundColor();
CMVIEW_API std::string DefaultParamHelpFontFamilyName();
CMVIEW_API float DefaultParamHelpFontSize();
CMVIEW_API Color DefaultParamHelpTextColor();

struct CMVIEW_API ParamHelpCreateParams
{
    ParamHelpCreateParams();
    ParamHelpCreateParams& Defaults();
    ParamHelpCreateParams& WindowClassName(const std::string& windowClassName_);
    ParamHelpCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ParamHelpCreateParams& WindowStyle(int windowStyle_);
    ParamHelpCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    ParamHelpCreateParams& BackgroundColor(const Color& backgroundColor_);
    ParamHelpCreateParams& Text(const std::string& text_);
    ParamHelpCreateParams& Location(Point location_);
    ParamHelpCreateParams& SetSize(Size size_);
    ParamHelpCreateParams& SetAnchors(Anchors anchors_);
    ParamHelpCreateParams& SetDock(Dock dock_);
    ParamHelpCreateParams& FontFamilyName(const std::string& fontFamilyName_);
    ParamHelpCreateParams& FontSize(float fontSize_);
    ParamHelpCreateParams& TextColor(const Color& color);
    ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
    Color textColor;
};

enum class ParamHelpFlags : int
{
    none = 0, changed = 1 << 0
};

inline CMVIEW_API ParamHelpFlags operator|(ParamHelpFlags left, ParamHelpFlags right)
{
    return ParamHelpFlags(int(left) | int(right));
}

inline CMVIEW_API ParamHelpFlags operator&(ParamHelpFlags left, ParamHelpFlags right)
{
    return ParamHelpFlags(int(left) & int(right));
}

inline CMVIEW_API ParamHelpFlags operator~(ParamHelpFlags flags)
{
    return ParamHelpFlags(~(int(flags)));
}

class CMVIEW_API ParamHelp : public Control
{
public:
    ParamHelp(ParamHelpCreateParams& createParams);
    void SetContent(const std::vector<std::string>& paramHelpList_);
    bool Changed() const;
    void SetChanged();
    void ResetChanged();
    Size DefaultSize() const;
    void Next();
    void Prev();
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    void Measure(Graphics& graphics);
    ParamHelpFlags flags;
    SolidBrush textBrush;
    StringFormat stringFormat;
    std::vector<std::string> paramHelpList;
    int index;
    float lineHeight;
    float charWidth;
};

CMVIEW_API Color DefaultCodeCompletionParamHelpViewBackgroundColor();
CMVIEW_API Color DefaultCodeCompletionParamHelpViewBorderColor();
CMVIEW_API Padding DefaultCodeCompletionParamHelpViewPadding();
CMVIEW_API BorderStyle DefaultCodeCompletionParamHelpViewBorderStyle();

struct CMVIEW_API CodeCompletionParamHelpViewCreateParams
{
    CodeCompletionParamHelpViewCreateParams();
    CodeCompletionParamHelpViewCreateParams& Defaults();
    CodeCompletionParamHelpViewCreateParams& WindowClassName(const std::string& windowClassName_);
    CodeCompletionParamHelpViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    CodeCompletionParamHelpViewCreateParams& WindowStyle(int windowStyle_);
    CodeCompletionParamHelpViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    CodeCompletionParamHelpViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    CodeCompletionParamHelpViewCreateParams& Text(const std::string& text_);
    CodeCompletionParamHelpViewCreateParams& Location(Point location_);
    CodeCompletionParamHelpViewCreateParams& SetSize(Size size_);
    CodeCompletionParamHelpViewCreateParams& SetAnchors(Anchors anchors_);
    CodeCompletionParamHelpViewCreateParams& SetDock(Dock dock_);
    CodeCompletionParamHelpViewCreateParams& BorderColor(const Color& color);
    CodeCompletionParamHelpViewCreateParams& SetPadding(const Padding& padding_);
    CodeCompletionParamHelpViewCreateParams& SetBorderStyle(BorderStyle borderStyle_);
    ControlCreateParams controlCreateParams;
    Color borderColor;
    Padding padding;
    BorderStyle borderStyle;
};

class CMVIEW_API CodeCompletionParamHelpView : public Control
{
public:
    CodeCompletionParamHelpView(ParamHelpCreateParams& paramHelpCreateParams, CodeCompletionParamHelpViewCreateParams& createParams);
    void SetContent(const std::vector<std::string>& paramHelpList);
    void Next();
    void Prev();
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
    ParamHelp* paramHelp;
    Control* child;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_CODE_COMPLETION_VIEW_INCLUDED
