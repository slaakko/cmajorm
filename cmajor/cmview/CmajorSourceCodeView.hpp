// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_CMAJOR_SOURCE_CODE_VIEW_INCLUDED
#define CMAJOR_VIEW_CMAJOR_SOURCE_CODE_VIEW_INCLUDED
#include <cmajor/cmview/ViewApi.hpp>
#include <cmajor/wing/SourceCodeView.hpp>
#include <sngcm/cmlexer/CmajorLexer.hpp>

#undef TRUE
#undef FALSE
#undef INTERFACE
#undef VOID
#undef CDECL
#undef WINAPI
#undef CONST
#undef THIS
#undef DELETE

namespace cmajor { namespace view {

using namespace cmajor::wing;

struct CMVIEW_API ExpressionHoverEventArgs
{
    std::string expression;
    Point screenLoc;
};

using ExpressionHoverEvent = EventWithArgs<ExpressionHoverEventArgs>;
using ExitExpressionHoverEvent = Event;

class DebugStrip;
class CmajorEditor;

struct CMVIEW_API CmajorSourceCodeViewCreateParams
{
    CmajorSourceCodeViewCreateParams();
    CmajorSourceCodeViewCreateParams& Defaults();
    CmajorSourceCodeViewCreateParams& WindowClassName(const std::string& windowClassName_);
    CmajorSourceCodeViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    CmajorSourceCodeViewCreateParams& WindowStyle(int windowStyle_);
    CmajorSourceCodeViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    CmajorSourceCodeViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    CmajorSourceCodeViewCreateParams& Text(const std::string& text_);
    CmajorSourceCodeViewCreateParams& Location(const Point& location_);
    CmajorSourceCodeViewCreateParams& SetSize(const Size& size_);
    CmajorSourceCodeViewCreateParams& SetAnchors(Anchors anchors_);
    CmajorSourceCodeViewCreateParams& SetDock(Dock dock_);
    CmajorSourceCodeViewCreateParams& TextColor(const Color& textColor_);
    CmajorSourceCodeViewCreateParams& SelectionBackgroundColor(const Color& selectionBackgroundColor_);
    CmajorSourceCodeViewCreateParams& FontFamilyName(const std::string& fontFamilyName_);
    CmajorSourceCodeViewCreateParams& FontSize(float fontSize_);
    CmajorSourceCodeViewCreateParams& SetSourceCodeTokenStyle(SourceCodeTokenKind tokenKind, TokenStyle tokenStyle);
    CmajorSourceCodeViewCreateParams& DebugLocationBackgroundColor(const Color& debugLocationBackgroundColor_);
    CmajorSourceCodeViewCreateParams& DebugLocationTextColor(const Color& debugLocationTextColor_);
    CmajorSourceCodeViewCreateParams& DebugLocationPadding(const Padding& debugLocationPadding_);
    SourceCodeViewCreateParams sourceCodeViewCreateParams;
    Color debugLocationBackgroundColor;
    Color debugLocationTextColor;
    Padding debugLocationPadding;
};

class CMVIEW_API CmajorSourceCodeView : public SourceCodeView
{
public:
    CmajorSourceCodeView(CmajorSourceCodeViewCreateParams& createParams);
    DebugStrip* GetDebugStrip() const { return debugStrip; }
    void SetDebugStrip(DebugStrip* debugStrip_) { debugStrip = debugStrip_; }
    CmajorEditor* Editor() const { return editor; }
    void SetEditor(CmajorEditor* editor_) { editor = editor_; }
    void ToggleBreakpoint();
    ExpressionHoverEvent& ExpressionHover() { return expressionHover; }
protected:
    TokenLine TokenizeLine(const std::u32string& line, int lineNumber, int startState) override;
    SourceCodeTokenKind GetTokenKind(const Token& token) const override;
    void DrawHilites(Graphics& graphics, int lineIndex, const PointF& origin) override;
    void OnMouseHover(MouseEventArgs& args) override;
    virtual void OnExpressionHover(ExpressionHoverEventArgs& args);
private:
    void DrawDebugLocationHilite(Graphics& graphics, const SourceSpan& debugLocation, const PointF& origin);
    CmajorLexer lexer;
    Color debugLocationBackgroundColor;
    Color debugLocationTextColor;
    Padding debugLocationPadding;
    DebugStrip* debugStrip;
    CmajorEditor* editor;
    ExpressionHoverEvent expressionHover;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_CMAJOR_SOURCE_CODE_VIEW_INCLUDED
