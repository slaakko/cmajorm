// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_LOCALS_VIEW_INCLUDED
#define CMAJOR_VIEW_LOCALS_VIEW_INCLUDED
#include <cmajor/cmview/ViewApi.hpp>
#include <cmajor/cmmsg/CmdbMessage.hpp>
#include <cmajor/wing/Container.hpp>
#include <cmajor/wing/TreeView.hpp>
#include <set>

namespace cmajor { namespace view {

using namespace cmajor::wing;

using LocalNodeExpandedEvent = EventWithArgs<TreeViewNodeEventArgs>;
using LocalNodeHoverEvent = EventWithArgs<TreeViewNodeEventArgs>;
using LocalsViewUpdateNeededEvent = Event;

struct CMVIEW_API ChildExtent
{
    ChildExtent() : start(-1), count(-1), dataStart(-1) {}
    ChildExtent(int start_, int count_, int dataStart_) : start(start_), count(count_), dataStart(dataStart_) {}
    bool IsEmpty() const { return start == -1 && count == -1 && dataStart == -1; }
    int start;
    int count;
    int dataStart;
};

class CMVIEW_API LocalsView: public Control
{
public:
    LocalsView();
    void Clear();
    int LocalCount() const { return localCount; }
    bool LocalCountRequested() const { return localCountRequested; }
    void SetLocalCountRequested() { localCountRequested = true; }
    void SetLocalCount(int localCount_);
    TreeViewNode* FetchNode() const { return fetchNode; }
    void SetFetchNode(TreeViewNode* fetchNode_);
    const ChildExtent& GetChildExtent();
    void SetChildExtentRequested() { childExtentRequested = true; }
    void ResetChildExtentRequested() { childExtentRequested = false; }
    bool ChildExtentRequested() const { return childExtentRequested; }
    std::string FetchExpression() const;
    void SetChildResults(const std::vector<ChildResult>& childResults);
    bool IsFetchSetEmpty() const { return fetchSet.empty(); }
    void UpdateFetchSet();
    void Fetch();
    void Update();
    void HideToolTipWindow();
    LocalNodeExpandedEvent& LocalNodeExpanded() { return localNodeExpanded; }
    LocalNodeHoverEvent& LocalNodeHovered() { return localNodeHovered; }
    LocalsViewUpdateNeededEvent& UpdateNeeded() { return updateNeeded; }
protected:
    void OnLocationChanged() override;
    void OnSizeChanged() override;
    void OnPaint(PaintEventArgs& args) override;
    virtual void OnLocalNodeExpanded(TreeViewNodeEventArgs& args);
    virtual void OnLocalNodeHovered(TreeViewNodeEventArgs& args);
    virtual void OnUpdateNeeded();
private:
    void SetChildPos();
    void PopulateTreeViewNode(TreeViewNode* node, int n);
    void TreeViewNodeExpanded(TreeViewNodeEventArgs& args);
    void TreeViewNodeHover(TreeViewNodeEventArgs& args);
    void TreeViewContentLocationChanged();
    int localCount;
    bool localCountRequested;
    ChildExtent childExtent;
    bool childExtentRequested;
    TreeViewNode* fetchNode;
    std::set<TreeViewNode*> fetchSet;
    std::vector<std::unique_ptr<ChildResult>> childResultVec;
    Container container;
    TreeView* treeView;
    Control* child;
    LocalNodeExpandedEvent localNodeExpanded;
    LocalNodeHoverEvent localNodeHovered;
    LocalsViewUpdateNeededEvent updateNeeded;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_LOCALS_VIEW_INCLUDED
