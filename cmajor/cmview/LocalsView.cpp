// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/LocalsView.hpp>
#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/LogView.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/ScrollableControl.hpp>

#undef min
#undef max

namespace cmajor { namespace view {

LocalsView::LocalsView() : Control(ControlCreateParams().WindowClassName("cmajor.view.LocalsView").WindowClassBackgroundColor(COLOR_WINDOW).BackgroundColor(Color::White).SetDock(Dock::fill)),
    localCount(-1), localCountRequested(false), childExtentRequested(false), fetchNode(nullptr), container(this), treeView(nullptr), child(nullptr)
{
    std::unique_ptr<TreeView> treeViewPtr(new TreeView(TreeViewCreateParams().Defaults()));
    treeView = treeViewPtr.get();
    treeView->SetFlag(ControlFlags::scrollSubject);
    treeView->SetDoubleBuffered();
    treeView->NodeExpanded().AddHandler(this, &LocalsView::TreeViewNodeExpanded);
    treeView->NodeHovered().AddHandler(this, &LocalsView::TreeViewNodeHover);
    treeView->ContentLocationChanged().AddHandler(this, &LocalsView::TreeViewContentLocationChanged);
    std::unique_ptr<Control> paddedTreeView(new PaddedControl(PaddedControlCreateParams(treeViewPtr.release()).Defaults()));
    std::unique_ptr<Control> scrollableTreeView(new ScrollableControl(ScrollableControlCreateParams(paddedTreeView.release()).SetDock(Dock::fill)));
    child = scrollableTreeView.get();
    container.AddChild(scrollableTreeView.release());
    Clear();
}

void LocalsView::Clear()
{
    localCount = -1;
    localCountRequested = false;
    childExtentRequested = false;
    fetchNode = nullptr;
    treeView->SetRoot(new TreeViewNode("Locals"));
    Update();
}

void LocalsView::SetLocalCount(int localCount_)
{
    LogView* logView = Application::GetLogView();
    if (localCount != localCount_)
    {
        localCount = localCount_;
        if (logView)
        {
            logView->WriteLine("setLocalCount=" + std::to_string(localCount));
        }
        childExtentRequested = false;
        fetchNode = nullptr;
        PopulateTreeViewNode(treeView->Root(), localCount);
    }
}

void LocalsView::SetFetchNode(TreeViewNode* fetchNode_)
{
    LogView* logView = Application::GetLogView();
    childExtentRequested = false;
    fetchNode = fetchNode_;
    if (logView)
    {
        logView->WriteLine("setFetchNode=" + std::to_string(fetchNode->Index()));
    }
}

const ChildExtent& LocalsView::GetChildExtent()
{
    LogView* logView = Application::GetLogView();
    childExtentRequested = false;
    childExtent = ChildExtent();
    TreeViewNode* parentNode = fetchNode;
    if (!parentNode)
    {
        parentNode = treeView->Root();
    }
    std::vector<TreeViewNode*> visibleNodes;
    parentNode->GetVisibleNodes(visibleNodes, parentNode->Level() + 1);
    if (visibleNodes.empty()) return childExtent;
    TreeViewNode* firstChild = static_cast<TreeViewNode*>(parentNode->Children().FirstChild());
    TreeViewNode* first = visibleNodes.front();
    TreeViewNode* last = visibleNodes.back();
    while (first != last && first->Data() != nullptr)
    {
        first = static_cast<TreeViewNode*>(first->NextSibling());
    }
    while (first != last && last->Data() != nullptr)
    {
        last = static_cast<TreeViewNode*>(last->PrevSibling());
    }
    int dataStart = first->Index();
    int start = first->Index() - firstChild->Index();
    int count = last->Index() - first->Index() + 1;
    if (count == 1 && first->Data() != nullptr)
    {
        return childExtent;
    }
    childExtent = ChildExtent(start, count, dataStart);
    if (logView)
    {
        logView->WriteLine("getChildExtent=" + 
            std::to_string(parentNode->Index()) + ": [" + std::to_string(start) + ":" + std::to_string(first->Index()) + ", " + std::to_string(count) + ":" + std::to_string(last->Index()) + "]");
    }
    return childExtent;
}

std::string LocalsView::FetchExpression() const
{
    if (fetchNode)
    {
        ChildResult* childResult = static_cast<ChildResult*>(fetchNode->Data());
        if (childResult)
        {
            return childResult->expr;
        }
    }
    return "@locals";
}

void LocalsView::SetChildResults(const std::vector<ChildResult>& childResults)
{
    LogView* logView = Application::GetLogView();
    if (!childExtent.IsEmpty())
    {
        childResultVec.resize(childExtent.dataStart + std::max(childExtent.count, static_cast<int>(childResults.size())));
        for (int i = 0; i < childResults.size(); ++i)
        {
            childResultVec[childExtent.dataStart + i].reset(new ChildResult(childResults[i]));
        }
        int n = childResultVec.size();
        TreeViewNode* parentNode = fetchNode;
        if (!parentNode)
        {
            parentNode = treeView->Root();
        }
        std::vector<TreeViewNode*> visibleNodes;
        parentNode->GetVisibleNodes(visibleNodes, parentNode->Level() + 1);
        if (logView && !visibleNodes.empty())
        {
            logView->WriteLine("setChildResults=" + std::to_string(visibleNodes.front()->Index()) + "-" + std::to_string(visibleNodes.back()->Index()));
        }
        for (int i = 0; i < visibleNodes.size(); ++i)
        {
            TreeViewNode* node = visibleNodes[i];
            int index = node->Index();
            if (index >= 0 && index < childResultVec.size())
            {
                ChildResult* childResult = childResultVec[index].get();
                node->SetData(childResult);
                std::string text = std::to_string(index);
                if (childResult)
                {
                    text = childResult->name;
                    if (!childResult->value.empty())
                    {
                        text.append(" = ").append(childResult->value);
                    }
                }
                node->SetText(text);
                PopulateTreeViewNode(node, n);
                if (childResult)
                {
                    n += childResult->count;
                }
            }
        }
    }
}

void LocalsView::UpdateFetchSet()
{
    std::vector<TreeViewNode*> visibleNodes = treeView->GetVisibleNodes();
    for (TreeViewNode* visibleNode : visibleNodes)
    {
        if (visibleNode != treeView->Root())
        {
            if (!visibleNode->Data())
            {
                fetchSet.insert(visibleNode->Parent());
            }
        }
    }
}

void LocalsView::Fetch()
{
    if (!fetchSet.empty())
    {
        TreeViewNode* node = *fetchSet.begin();
        fetchSet.erase(node);
        SetFetchNode(node);
    }
}

void LocalsView::Update()
{
    treeView->SetChanged();
    treeView->Invalidate();
    child->Invalidate();
    treeView->Update();
}

void LocalsView::HideToolTipWindow()
{
    treeView->HideToolTipWindow();
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

void LocalsView::PopulateTreeViewNode(TreeViewNode* node, int n)
{
    if (!node->Children().IsEmpty()) return;
    if (node == treeView->Root())
    {
        for (int i = 0; i < n; ++i)
        {
            TreeViewNode* node = new TreeViewNode("local " + std::to_string(i) + "...");
            node->SetIndex(i);
            treeView->Root()->AddChild(node);
        }
        treeView->Root()->Expand();
    }
    else
    {
        ChildResult* result = static_cast<ChildResult*>(node->Data());
        if (result)
        {
            int count = result->count;
            for (int i = 0; i < count; ++i)
            {
                int index = n + i;
                TreeViewNode* childNode = new TreeViewNode("child node " + std::to_string(i) + "...");
                childNode->SetIndex(index);
                node->AddChild(childNode);
            }
        }
    }
}

void LocalsView::OnPaint(PaintEventArgs& args)
{
    treeView->Invalidate();
    Control::OnPaint(args);
}

void LocalsView::TreeViewNodeExpanded(TreeViewNodeEventArgs& args)
{
    OnLocalNodeExpanded(args);
}

void LocalsView::TreeViewNodeHover(TreeViewNodeEventArgs& args)
{
    OnLocalNodeHovered(args);
}

void LocalsView::TreeViewContentLocationChanged()
{
    OnUpdateNeeded();
}

void LocalsView::OnLocalNodeExpanded(TreeViewNodeEventArgs& args)
{
    localNodeExpanded.Fire(args);
}

void LocalsView::OnLocalNodeHovered(TreeViewNodeEventArgs& args)
{
    localNodeHovered.Fire(args);
}

void LocalsView::OnUpdateNeeded()
{
    updateNeeded.Fire();
}

} } // namespace cmajor::view
