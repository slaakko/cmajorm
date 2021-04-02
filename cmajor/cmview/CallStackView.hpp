// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_CALL_STACK_VIEW_INCLUDED
#define CMAJOR_VIEW_CALL_STACK_VIEW_INCLUDED
#include <cmajor/cmview/ViewApi.hpp>
#include <cmajor/cmmsg/CmdbMessage.hpp>
#include <cmajor/wing/Container.hpp>
#include <cmajor/wing/TreeView.hpp>

namespace cmajor { namespace view {

using namespace cmajor::wing;

struct CMVIEW_API FrameSelectedEventArgs
{
    FrameSelectedEventArgs(Location* frame_);
    Location* frame;
};

using FrameSelectedEvent = EventWithArgs<FrameSelectedEventArgs>;

class CMVIEW_API CallStackView : public Control
{
public:
    CallStackView();
    void Clear();
    bool IsEmpty() const;
    int Depth() const { return depth; }
    void SetDepth(int depth_);
    const std::pair<int, int>& GetFrameRange();
    void SetFrameRange(const std::vector<::Location>& frames_);
    FrameSelectedEvent& FrameSelected() { return frameSelected; }
protected:
    void OnLocationChanged() override;
    void OnSizeChanged() override;
    void OnPaint(PaintEventArgs& args) override;
    virtual void OnFrameSelected(FrameSelectedEventArgs& args);
private:
    void SetChildPos(); 
    void PopulateTreeView();
    void UpdateNodes();
    void UpdateNode(TreeViewNode* node);
    void TreeViewNodeDoubleClick(TreeViewNodeClickEventArgs& args);
    Container container;
    TreeView* treeView;
    Control* child;
    int depth;
    std::pair<int, int> frameRange;
    std::vector<std::unique_ptr<::Location>> frames;
    FrameSelectedEvent frameSelected;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_CALL_STACK_VIEW_INCLUDED
