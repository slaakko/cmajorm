// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/CallStackView.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/ScrollableControl.hpp>

#undef min
#undef max

namespace cmajor { namespace view {

FrameSelectedEventArgs::FrameSelectedEventArgs(Location* frame_) : frame(frame_)
{
}

CallStackView::CallStackView() : 
    Control(ControlCreateParams().WindowClassName("cmajor.view.CallStackView").WindowClassBackgroundColor(COLOR_WINDOW).BackgroundColor(Color::White).SetDock(Dock::fill)),
    container(this), treeView(nullptr), child(nullptr), depth(-1)
{
    std::unique_ptr<TreeView> treeViewPtr(new TreeView(TreeViewCreateParams().Defaults()));
    treeView = treeViewPtr.get();
    treeView->SetFlag(ControlFlags::scrollSubject);
    treeView->SetDoubleBuffered();
    treeView->NodeDoubleClick().AddHandler(this, &CallStackView::TreeViewNodeDoubleClick);
    std::unique_ptr<Control> paddedTreeView(new PaddedControl(PaddedControlCreateParams(treeViewPtr.release()).Defaults()));
    std::unique_ptr<Control> scrollableTreeView(new ScrollableControl(ScrollableControlCreateParams(paddedTreeView.release()).SetDock(Dock::fill)));
    child = scrollableTreeView.get();
    container.AddChild(scrollableTreeView.release());
    Clear();
}

void CallStackView::Clear()
{
    treeView->SetRoot(new TreeViewNode("Frames"));
    depth = -1;
    frames.clear();
    frameRange = std::make_pair(-1, -1);
    child->Invalidate();
}

bool CallStackView::IsEmpty() const
{
    return depth == -1 || frameRange.first == frameRange.second;
}

void CallStackView::SetDepth(int depth_)
{
    if (depth != depth_)
    {
        depth = depth_;
        PopulateTreeView();
    }
}

void CallStackView::PopulateTreeView()
{
    for (int i = 0; i < depth; ++i)
    {
        std::string nodeText = std::to_string(i);
        TreeViewNode* node = new TreeViewNode(nodeText);
        node->SetIndex(i);
        treeView->Root()->AddChild(node);
    }
    treeView->Root()->Expand();
    treeView->Invalidate();
}

const std::pair<int, int>& CallStackView::GetFrameRange() 
{
    std::vector<TreeViewNode*> visibleNodes = treeView->GetVisibleNodes(1);
    if (visibleNodes.empty())
    {
        frameRange = std::make_pair(-1, -1);
        return frameRange;
    }
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
    int lowFrame = first->Index();
    int highFrame = last->Index();
    frameRange = std::make_pair(lowFrame, highFrame);
    return frameRange;
}

void CallStackView::SetFrameRange(const std::vector<::Location>& frames_)
{
    if (frameRange.first == frameRange.second) return;
    frames.resize(std::max(frameRange.second + 1, static_cast<int>(frameRange.first + frames_.size()))); 
    for (int i = 0; i < frames_.size(); ++i)
    {
        frames[frameRange.first + i] = std::unique_ptr<::Location>(new ::Location(frames_[i]));
    }
    UpdateNodes();
    treeView->SetChanged();
    treeView->Invalidate();
}

void CallStackView::UpdateNodes()
{
    std::vector<TreeViewNode*> visibleNodes = treeView->GetVisibleNodes(1);
    if (visibleNodes.empty()) return;
    TreeViewNode* first = visibleNodes.front();
    TreeViewNode* last = visibleNodes.back();
    int firstIndex = first->Index();
    int lastIndex = last->Index();
    TreeViewNode* node = first;
    for (int i = firstIndex; i <= lastIndex; ++i)
    {
        if (node && i >= 0 && i < frames.size())
        {
            node->SetData(frames[i].get());
            UpdateNode(node);
            node = static_cast<TreeViewNode*>(node->NextSibling());
        }
    }
}

void CallStackView::UpdateNode(TreeViewNode* node)
{
    ::Location* location = static_cast<::Location*>(node->Data());
    if (location)
    {
        std::string func = location->func;
        if (func.empty())
        {
            func = "<external>";
        }
        std::string nodeText = std::to_string(node->Index());
        nodeText.append(" - ").append(func);
        node->SetText(nodeText);
        node->RemoveChildren();
        if (!location->func.empty())
        {
            TreeViewNode* fileNode = new TreeViewNode(location->file);
            node->AddChild(fileNode);
            std::string lineText;
            lineText.append("Line ").append(std::to_string(location->line));
            TreeViewNode* lineNode = new TreeViewNode(lineText);
            node->AddChild(lineNode);
        }
    }
}

void CallStackView::TreeViewNodeDoubleClick(TreeViewNodeClickEventArgs& args)
{
    TreeViewNode* node = args.node;
    if (node->Data())
    {
        ::Location* frame = static_cast<::Location*>(node->Data());
        FrameSelectedEventArgs args(frame);
        OnFrameSelected(args);
    }
}

void CallStackView::OnFrameSelected(FrameSelectedEventArgs& args)
{
    frameSelected.Fire(args);
}

void CallStackView::OnLocationChanged()
{
    Control::OnLocationChanged();
    SetChildPos();
}

void CallStackView::OnSizeChanged()
{
    Control::OnSizeChanged();
    SetChildPos();
}

void CallStackView::SetChildPos()
{
    child->SetLocation(Point());
    child->SetSize(GetSize());
}

void CallStackView::OnPaint(PaintEventArgs& args)
{
    treeView->Invalidate();
    Control::OnPaint(args);
}

} } // namespace cmajor::view
