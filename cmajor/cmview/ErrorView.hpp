// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_ERROR_VIEW_INCLUDED
#define CMAJOR_VIEW_ERROR_VIEW_INCLUDED
#include <cmajor/cmview/ViewApi.hpp>
#include <cmajor/cmmsg/BuildServerMessage.hpp>
#include <cmajor/wing/TreeView.hpp>

namespace cmajor { namespace view {

using namespace cmajor::wing;

struct CMVIEW_API ViewErrorArgs
{
    ViewErrorArgs(CompileError* error_) : error(error_) {}
    CompileError* error;
};

using ViewErrorEvent = EventWithArgs<ViewErrorArgs>;

struct CMVIEW_API ErrorViewCreateParams
{
    ErrorViewCreateParams();
    ErrorViewCreateParams& Defaults();
    ErrorViewCreateParams& WindowClassName(const std::string& windowClassName_);
    ErrorViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ErrorViewCreateParams& WindowStyle(int windowStyle_);
    ErrorViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    ErrorViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    ErrorViewCreateParams& Text(const std::string& text_);
    ErrorViewCreateParams& Location(Point location_);
    ErrorViewCreateParams& SetSize(Size size_);
    ErrorViewCreateParams& SetAnchors(Anchors anchors_);
    ErrorViewCreateParams& SetDock(Dock dock_);
    ControlCreateParams controlCreateParams;
    TreeViewCreateParams treeViewCreateParams;
};

class CMVIEW_API ErrorView : public Control
{
public:
    ErrorView(ErrorViewCreateParams& createParams);
    void Clear();
    void SetErrors(const std::vector<CompileError>&& errors_);
    ViewErrorEvent& ViewError() { return viewError; }
protected:
    virtual void OnViewError(CompileError* error);
    void OnLocationChanged() override;
    void OnSizeChanged() override;
    void OnPaint(PaintEventArgs& args) override;
private:
    void TreeViewNodeDoubleClick(TreeViewNodeClickEventArgs& args);
    void SetChildPos();
    Container container;
    TreeView* treeView;
    Control* child;
    std::vector<CompileError> errors;
    ViewErrorEvent viewError;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_ERROR_VIEW_INCLUDED
