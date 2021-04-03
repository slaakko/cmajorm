// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/LocalsView.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/ScrollableControl.hpp>

#undef min
#undef max

namespace cmajor { namespace view {

LocalsView::LocalsView() : Control(ControlCreateParams().WindowClassName("cmajor.view.LocalsView").WindowClassBackgroundColor(COLOR_WINDOW).BackgroundColor(Color::White).SetDock(Dock::fill)),
    container(this), treeView(nullptr), child(nullptr)
{
    std::unique_ptr<TreeView> treeViewPtr(new TreeView(TreeViewCreateParams().Defaults()));
    treeView = treeViewPtr.get();
    treeView->SetDoubleBuffered();
    std::unique_ptr<Control> paddedTreeView(new PaddedControl(PaddedControlCreateParams(treeViewPtr.release()).Defaults()));
    std::unique_ptr<Control> scrollableTreeView(new ScrollableControl(ScrollableControlCreateParams(paddedTreeView.release()).SetDock(Dock::fill)));
    child = scrollableTreeView.get();
    container.AddChild(scrollableTreeView.release());
    Clear();
}

void LocalsView::Clear()
{
    treeView->SetRoot(new TreeViewNode("Locals"));
    child->Invalidate();
}

void LocalsView::OnLocationChanged()
{
    Control::OnLocationChanged();
    SetChildPos();
}

void LocalsView::OnSizeChanged()
{
    Control::OnSizeChanged();
    SetChildPos();
}

void LocalsView::SetChildPos()
{
    child->SetLocation(Point());
    child->SetSize(GetSize());
}

void LocalsView::OnPaint(PaintEventArgs& args)
{
    treeView->Invalidate();
    Control::OnPaint(args);
}

} } // namespace cmajor::view
