// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_SOLUTION_EXPLORER_INCLUDED
#define CMCODE_SOLUTION_EXPLORER_INCLUDED
#include <cmajor/wing/ContainerControl.hpp>
#include <cmajor/wing/TabControl.hpp>
#include <cmajor/wing/TreeView.hpp>
#include <cmajor/cmview/CmajorEditor.hpp>
#include <cmajor/cmview/ResourceFileEditor.hpp>
#include <cmajor/cmview/TextFileEditor.hpp>
#include <cmajor/sngcm/ast/Project.hpp>

namespace cmcode {

using namespace cmajor::wing;
using namespace cmajor::view;

class MainWindow;

Color DefaultSolutionExplorerBackgroundColor();

struct SolutionExplorerCreateParams
{
    SolutionExplorerCreateParams();
    SolutionExplorerCreateParams& Defaults();
    SolutionExplorerCreateParams& WindowClassName(const std::string& windowClassName_);
    SolutionExplorerCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    SolutionExplorerCreateParams& WindowStyle(int windowStyle_);
    SolutionExplorerCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    SolutionExplorerCreateParams& BackgroundColor(const Color& backgroundColor_);
    SolutionExplorerCreateParams& Text(const std::string& text_);
    SolutionExplorerCreateParams& Location(Point location_);
    SolutionExplorerCreateParams& SetSize(Size size_);
    SolutionExplorerCreateParams& SetAnchors(Anchors anchors_);
    SolutionExplorerCreateParams& SetDock(Dock dock_);
    ControlCreateParams controlCreateParams;
    TreeViewCreateParams treeViewCreateParams;
};

class SolutionExplorer : public ContainerControl
{
public:
    SolutionExplorer(SolutionExplorerCreateParams& createParams, MainWindow* mainWindow_);
    TreeView* GetTreeView() const { return solutionTreeView; }
    void MakeView();
    void SetRoot(TreeViewNode* solutionNode);
    void SetImageList(ImageList* imageList_);
    ImageList* GetImageList() const { return imageList; }
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    TreeViewCreateParams treeViewCreateParams;
    MainWindow* mainWindow;
    TreeView* solutionTreeView;
    Control* child;
    ImageList* imageList;
};

} // namespace cmcode

#endif // CMCODE_SOLUTION_EXPLORER_INCLUDED
