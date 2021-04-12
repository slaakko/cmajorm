// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/PortMapView.hpp>
#include <cmajor/cmsvc/Message.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/ScrollableControl.hpp>

namespace cmajor { namespace view {

using namespace cmajor::service;

PortMapView::PortMapView() : Control(ControlCreateParams().WindowClassName("cmajor.view.PortMapView").WindowClassBackgroundColor(COLOR_WINDOW).BackgroundColor(Color::White).SetDock(Dock::fill)),
    container(this), treeView(nullptr), child(nullptr)
{
    std::unique_ptr<TreeView> treeViewPtr(new TreeView(TreeViewCreateParams().AddToolTip(false)));
    treeView = treeViewPtr.get();
    treeView->SetDoubleBuffered();
    std::unique_ptr<Control> paddedTreeView(new PaddedControl(PaddedControlCreateParams(treeViewPtr.release()).Defaults()));
    std::unique_ptr<Control> scrollableTreeView(new ScrollableControl(ScrollableControlCreateParams(paddedTreeView.release()).SetDock(Dock::fill)));
    child = scrollableTreeView.get();
    container.AddChild(scrollableTreeView.release());
    Clear();
}

void PortMapView::Clear()
{
    treeView->SetRoot(new TreeViewNode("Port Map Entries"));
    child->Invalidate();
}

void PortMapView::OnPaint(PaintEventArgs& args)
{
    treeView->Root()->ExpandAll();
    treeView->Invalidate();
    child->Invalidate();
    Control::OnPaint(args);
}

void PortMapView::Update(const std::vector<PortLease>& portLeases)
{
    Clear();
    for (const PortLease& portLease : portLeases)
    {
        TreeViewNode* node = new TreeViewNode("Port " + std::to_string(portLease.portNumber));
        treeView->Root()->AddChild(node);
        TreeViewNode* processNode = new TreeViewNode("Process: " + portLease.processName);
        node->AddChild(processNode);
        TreeViewNode* pidNode = new TreeViewNode("PID: " + std::to_string(portLease.pid));
        node->AddChild(pidNode);
        TreeViewNode* startTimeNode = new TreeViewNode("Lease start time: " + portLease.leaseStartTime.ToString());
        node->AddChild(startTimeNode);
        if (portLease.leaseRenewalTime != datetime())
        {
            TreeViewNode* renewalTimeNode = new TreeViewNode("Lease renewal time: " + portLease.leaseRenewalTime.ToString());
            node->AddChild(renewalTimeNode);
        }
        TreeViewNode* stateNode = new TreeViewNode("Lease state: " + portLease.leaseState);
        node->AddChild(stateNode);
    }
    Invalidate();
    PutOutputServiceMessage(std::to_string(portLeases.size()) + " ports in the port map");
}

void PortMapView::OnLocationChanged()
{
    Control::OnLocationChanged();
    SetChildPos();
}

void PortMapView::OnSizeChanged()
{
    Control::OnSizeChanged();
    SetChildPos();
}

void PortMapView::SetChildPos()
{
    child->SetLocation(Point());
    child->SetSize(GetSize());
}

} } // namespace cmajor::view
