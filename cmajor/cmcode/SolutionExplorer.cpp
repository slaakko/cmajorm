// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcode/SolutionExplorer.hpp>
#include <cmajor/cmcode/Solution.hpp>
#include <cmajor/cmcode/MainWindow.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/ScrollableControl.hpp>
#include <cmajor/wing/Theme.hpp>
#include <soulng/util/Path.hpp>

namespace cmcode {

using namespace soulng::util;

Color DefaultSolutionExplorerBackgroundColor()
{
    return Color::White;
}

Padding SolutionExplorerNodeImagePadding()
{
    return Padding(2, 2, 2, 2);
}

SolutionExplorerCreateParams::SolutionExplorerCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassName("cmajor.cmcode.solution_explorer");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(DefaultSolutionExplorerBackgroundColor());
    controlCreateParams.WindowStyle(DefaultChildWindowStyle());
    treeViewCreateParams = TreeViewCreateParams().StateIndicatorColor(GetColor("solution.explorer.state.indicator")).SelectedNodeColor(GetColor("solution.explorer.node.selected")).
        BackgroundColor(GetColor("solution.explorer.background")).TextColor(GetColor("solution.explorer.text")).
        NodeIndentPercent(100).NodeImagePadding(SolutionExplorerNodeImagePadding());
}

SolutionExplorerCreateParams& SolutionExplorerCreateParams::Defaults()
{
    return *this;
}

SolutionExplorerCreateParams& SolutionExplorerCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

SolutionExplorerCreateParams& SolutionExplorerCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

SolutionExplorerCreateParams& SolutionExplorerCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

SolutionExplorerCreateParams& SolutionExplorerCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

SolutionExplorerCreateParams& SolutionExplorerCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

SolutionExplorerCreateParams& SolutionExplorerCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

SolutionExplorerCreateParams& SolutionExplorerCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

SolutionExplorerCreateParams& SolutionExplorerCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

SolutionExplorerCreateParams& SolutionExplorerCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

SolutionExplorerCreateParams& SolutionExplorerCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

SolutionExplorer::SolutionExplorer(SolutionExplorerCreateParams& createParams, MainWindow* mainWindow_) : 
    ContainerControl(createParams.controlCreateParams), mainWindow(mainWindow_), solutionTreeView(nullptr), child(nullptr), imageList(nullptr), treeViewCreateParams(createParams.treeViewCreateParams)
{
    MakeView();
}

void SolutionExplorer::SetRoot(TreeViewNode* solutionNode)
{
    MakeView();
    solutionTreeView->SetImageList(imageList);
    solutionTreeView->SetRoot(solutionNode);
}

void SolutionExplorer::SetImageList(ImageList* imageList_)
{
    imageList = imageList_;
}

void SolutionExplorer::OnPaint(PaintEventArgs& args)
{
    try
    {
        if (solutionTreeView)
        {
            solutionTreeView->Invalidate();
        }
        if (child)
        {
            child->Invalidate();
        }
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void SolutionExplorer::MakeView()
{
    if (child)
    {
        RemoveChild(child);
        child = nullptr;
        solutionTreeView = nullptr;
    }
    std::unique_ptr<TreeView> solutionTreeViewPtr(new TreeView(treeViewCreateParams));
    solutionTreeView = solutionTreeViewPtr.get();
    solutionTreeView->SetFlag(ControlFlags::scrollSubject);
    solutionTreeView->NodeDoubleClick().AddHandler(mainWindow, &MainWindow::TreeViewNodeDoubleClick);
    solutionTreeView->NodeClick().AddHandler(mainWindow, &MainWindow::TreeViewNodeClick);
    solutionTreeView->NodeExpanded().AddHandler(mainWindow, &MainWindow::TreeViewNodeExpanded);
    solutionTreeView->NodeCollapsed().AddHandler(mainWindow, &MainWindow::TreeViewNodeCollapsed);
    solutionTreeView->SetDoubleBuffered();
    solutionTreeView->SetBackgroundItemName("solution.explorer.background");
    solutionTreeView->SetTextItemName("solution.explorer.text");
    solutionTreeView->SetStateIndicatorItemName("solution.explorer.state.indicator");
    solutionTreeView->SetNodeSelectedItemName("solution.explorer.node.selected");
    std::unique_ptr<PaddedControl> paddedTreeViewPtr(new PaddedControl(PaddedControlCreateParams(solutionTreeViewPtr.release()).Defaults()));
    std::unique_ptr<ScrollableControl> scrollableTreeViewPtr(new ScrollableControl(ScrollableControlCreateParams(paddedTreeViewPtr.release()).SetDock(Dock::fill)));
    child = scrollableTreeViewPtr.get();
    AddChild(scrollableTreeViewPtr.release());
}

} // namespace cmcode