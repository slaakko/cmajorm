// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/ErrorView.hpp>
#include <cmajor/wing/TextView.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/ScrollableControl.hpp>
#include <wing/Theme.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace view {

using namespace soulng::unicode;

std::vector<std::u32string> GetErrorLines(const std::u32string& errorLinesStr)
{
    std::vector<std::u32string> errorLines = SplitTextIntoLines(errorLinesStr);
    return errorLines;
}

ErrorViewCreateParams::ErrorViewCreateParams() : controlCreateParams(), treeViewCreateParams()
{
    controlCreateParams.WindowClassName("cmajor.view.ErrorView");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(GetColor("error.view.background"));
    controlCreateParams.SetDock(Dock::fill);
    treeViewCreateParams.BackgroundColor(GetColor("error.view.background")).TextColor(GetColor("error.view.text")).
        SelectedNodeColor(GetColor("error.view.node.selected")).StateIndicatorColor(GetColor("error.view.state.indicator"));
    treeViewCreateParams.NormalNodeFontFamilyName("Consolas");
}

ErrorViewCreateParams& ErrorViewCreateParams::Defaults()
{
    return *this;
}

ErrorViewCreateParams& ErrorViewCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

ErrorViewCreateParams& ErrorViewCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

ErrorViewCreateParams& ErrorViewCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

ErrorViewCreateParams& ErrorViewCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

ErrorViewCreateParams& ErrorViewCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

ErrorViewCreateParams& ErrorViewCreateParams::Text(const std::string& text_) 
{
    controlCreateParams.Text(text_);
    return *this;
}

ErrorViewCreateParams& ErrorViewCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

ErrorViewCreateParams& ErrorViewCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

ErrorViewCreateParams& ErrorViewCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

ErrorViewCreateParams& ErrorViewCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

ErrorView::ErrorView(ErrorViewCreateParams& createParams) : Control(createParams.controlCreateParams), container(this), treeView(nullptr), child(nullptr)
{
    std::unique_ptr<TreeView> treeViewPtr(new TreeView(createParams.treeViewCreateParams));
    treeView = treeViewPtr.get();
    treeView->SetBackgroundItemName("error.view.background");
    treeView->SetTextItemName("error.view.text");
    treeView->SetStateIndicatorItemName("error.view.state.indicator");
    treeView->SetNodeSelectedItemName("error.view.node.selected");
    treeView->SetFlag(ControlFlags::scrollSubject);
    treeView->SetDoubleBuffered();
    treeView->NodeDoubleClick().AddHandler(this, &ErrorView::TreeViewNodeDoubleClick);
    std::unique_ptr<Control> paddedTreeView(new PaddedControl(PaddedControlCreateParams(treeViewPtr.release()).Defaults()));
    std::unique_ptr<Control> scollableTreeView(new ScrollableControl(ScrollableControlCreateParams(paddedTreeView.release()).SetDock(Dock::fill)));
    child = scollableTreeView.get();
    container.AddChild(scollableTreeView.release());
    Clear();
}

void ErrorView::Clear()
{
    treeView->SetRoot(new TreeViewNode("Errors"));
    child->Invalidate();
}

void ErrorView::SetErrors(const std::vector<CompileError>&& errors_)
{
    errors = errors_;
    Clear();
    for (CompileError& error : errors)
    {
        std::vector<std::u32string> lines = GetErrorLines(ToUtf32(error.message));
        std::string firstLine;
        if (!lines.empty())
        {
            firstLine = ToUtf8(lines.front());
        }
        TreeViewNode* node = new TreeViewNode(firstLine);
        if (!error.project.empty())
        {
            TreeViewNode* projectNode = new TreeViewNode("Module: " + error.project);
            node->AddChild(projectNode);
        }
        if (!error.file.empty())
        {
            TreeViewNode* file = new TreeViewNode("File: " + error.file);
            node->AddChild(file);
            TreeViewNode* line = new TreeViewNode("Line: " + std::to_string(error.line));
            node->AddChild(line);
        }
        if (lines.size() > 1)
        {
            TreeViewNode* code = new TreeViewNode("Code:");
            for (int i = 1; i < lines.size(); ++i)
            {
                TreeViewNode* codeLineNode = new TreeViewNode(ToUtf8(lines[i]));
                code->AddChild(codeLineNode);
            }
            node->AddChild(code);
        }
        node->SetData(&error);
        treeView->Root()->AddChild(node);
    }
    Invalidate();
}

void ErrorView::TreeViewNodeDoubleClick(TreeViewNodeClickEventArgs& args)
{
    CompileError* error = static_cast<CompileError*>(args.node->Data());
    OnViewError(error);
}

void ErrorView::OnViewError(CompileError* error)
{
    if (!error || error->file.empty()) return;
    ViewErrorArgs args(error);
    viewError.Fire(args);
}

void ErrorView::OnLocationChanged()
{
    Control::OnLocationChanged();
    SetChildPos();
}

void ErrorView::OnSizeChanged()
{
    Control::OnSizeChanged();
    SetChildPos();
}

void ErrorView::SetChildPos()
{
    Point loc;
    Size size = GetSize();
    child->SetLocation(loc);
    child->SetSize(size);
}

void ErrorView::OnPaint(PaintEventArgs& args)
{
    treeView->Root()->ExpandAll();
    treeView->Invalidate();
    child->Invalidate();
}

} } // namespace cmajor::view
