// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_TREE_VIEW_INCLUDED
#define CMAJOR_WING_TREE_VIEW_INCLUDED
#include <cmajor/wing/ToolTip.hpp>
#include <cmajor/wing/Container.hpp>

namespace cmajor { namespace wing {

class TreeViewNode;

struct WING_API TreeViewNodeClickEventArgs
{
    TreeViewNodeClickEventArgs(const Point& location_, MouseButtons buttons_, int clicks_, TreeViewNode* node_);
    Point location;
    MouseButtons buttons;
    int clicks;
    TreeViewNode* node;
};

struct WING_API TreeViewNodeEventArgs
{
    TreeViewNodeEventArgs(TreeViewNode* node_);
    TreeViewNode* node;
};

using NodeClickEvent = EventWithArgs<TreeViewNodeClickEventArgs>;
using NodeDoubleClickEvent = EventWithArgs<TreeViewNodeClickEventArgs>;
using NodeEnterEvent = EventWithArgs<TreeViewNodeEventArgs>;
using NodeLeaveEvent = EventWithArgs<TreeViewNodeEventArgs>;
using NodeSelectedEvent = EventWithArgs<TreeViewNodeEventArgs>;
using NodeExpandedEvent = EventWithArgs<TreeViewNodeEventArgs>;
using NodeCollapsedEvent = EventWithArgs<TreeViewNodeEventArgs>;
using NodeHoveredEvent = EventWithArgs<TreeViewNodeEventArgs>;

enum class TreeViewFlags : int
{
    none = 0, changed = 1 << 0, treeViewNodeChanged = 1 << 1, treeViewNodeStateChanged = 1 << 2, toolTipWindowAdded = 1 << 3, toolTipWindowShown = 1 << 4
};

WING_API inline TreeViewFlags operator&(TreeViewFlags left, TreeViewFlags right)
{
    return TreeViewFlags(int(left) & int(right));
}

WING_API inline TreeViewFlags operator|(TreeViewFlags left, TreeViewFlags right)
{
    return TreeViewFlags(int(left) | int(right));
}

WING_API inline TreeViewFlags operator~(TreeViewFlags flags)
{
    return TreeViewFlags(~int(flags));
}

struct WING_API TreeViewCreateParams
{
    TreeViewCreateParams();
    TreeViewCreateParams& Defaults();
    TreeViewCreateParams& WindowClassName(const std::string& windowClassName_);
    TreeViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    TreeViewCreateParams& WindowStyle(int windowStyle_);
    TreeViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    TreeViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    TreeViewCreateParams& Text(const std::string& text_);
    TreeViewCreateParams& Location(const Point& location_);
    TreeViewCreateParams& SetSize(Size size_);
    TreeViewCreateParams& SetAnchors(Anchors anchors_);
    TreeViewCreateParams& SetDock(Dock dock_);
    TreeViewCreateParams& NormalNodeFontFamilyName(const std::string& fontName);
    TreeViewCreateParams& NormalNodeFontSize(float fontSize);
    TreeViewCreateParams& NormalNodeFontStyle(FontStyle fontStyle);
    TreeViewCreateParams& ActiveNodeFontFamilyName(const std::string& fontName);
    TreeViewCreateParams& ActiveNodeFontSize(float fontSize);
    TreeViewCreateParams& ActiveNodeFontStyle(FontStyle fontStyle);
    TreeViewCreateParams& StateIndicatorColor(const Color& color);
    TreeViewCreateParams& SelectedNodeColor(const Color& color);
    TreeViewCreateParams& TextColor(const Color& color);
    TreeViewCreateParams& StateIndicatorPercentage(float percentage);
    ControlCreateParams controlCreateParams;
    std::string normalNodeFontFamilyName;
    float normalNodeFontSize;
    FontStyle normalNodeFontStyle;
    std::string activeNodeFontFamilyName;
    float activeNodeFontySize;
    FontStyle activeNodeFontStyle;
    Color stateIndicatorColor;
    Color selectedNodeColor;
    Color textColor;
    float stateIndicatorPercentage;
};

class WING_API TreeView : public Control
{
public:
    TreeView(TreeViewCreateParams& createParams);
    ~TreeView();
    TreeViewNode* Root() const { return root.get(); }
    TreeViewNode* ReleaseRoot() { return root.release(); }
    void ResetRoot(TreeViewNode* root_) { root.reset(root_); }
    void SetRoot(TreeViewNode* root_);
    void EnsureNodeVisible(TreeViewNode* node);
    bool NodeVisible(TreeViewNode* node) const;
    std::vector<TreeViewNode*> GetVisibleNodes() const;
    std::vector<TreeViewNode*> GetVisibleNodes(int level) const;
    TreeViewNode* SelectedNode() const { return selectedNode; }
    void SetSelectedNode(TreeViewNode* node);
    void DeselectNode();
    TreeViewNode* TrackedNode() const { return trackedNode; }
    void SetTrackedNode(TreeViewNode* node) { trackedNode = node; }
    void SetNormalNodeFont(const std::string& normalNodeFontFamilyName_, float normalNodeFontSize_, FontStyle normalNodeFontStyle_);
    void SetActiveNodeFont(const std::string& activeNodeFontFamilyName_, float activeNodeFontSize_, FontStyle activeNodeFontStyle_);
    const Font& NormalNodeFont() const { return normalNodeFont; }
    const Font& ActiveNodeFont() const { return activeNodeFont; }
    const Color& StateIndicatorColor() const { return stateIndicatorColor; }
    const Color& SelectedNodeColor() const { return selectedNodeColor; }
    const Color& TextColor() const { return textColor; }
    void ShowToolTipWindow(TreeViewNode* node);
    void HideToolTipWindow();
    bool Changed() const { return (flags & TreeViewFlags::changed) != TreeViewFlags::none; }
    void SetChanged() { flags = flags | TreeViewFlags::changed; }
    void ResetChanged() { flags = flags & ~TreeViewFlags::changed; }
    bool TreeViewNodeChanged() const { return (flags & TreeViewFlags::treeViewNodeChanged) != TreeViewFlags::none; }
    void SetTreeViewNodeChanged() { flags = flags | TreeViewFlags::treeViewNodeChanged; }
    void ResetTreeViewNodeChanged() { flags = flags & ~TreeViewFlags::treeViewNodeChanged; }
    bool TreeViewNodeStateChanged() const { return (flags & TreeViewFlags::treeViewNodeStateChanged) != TreeViewFlags::none; }
    void SetTreeViewNodeStateChanged() { flags = flags | TreeViewFlags::treeViewNodeStateChanged; }
    void ResetTreeViewNodeStateChanged() { flags = flags & ~TreeViewFlags::treeViewNodeStateChanged; }
    bool ToolTipWindowAdded() const { return (flags & TreeViewFlags::toolTipWindowAdded) != TreeViewFlags::none; }
    void SetToolTipWindowAdded() { flags = flags | TreeViewFlags::toolTipWindowAdded; }
    void ResetToolTipWindowAdded() { flags = flags & ~TreeViewFlags::toolTipWindowAdded; }
    bool ToolTipWindowShown() const { return (flags & TreeViewFlags::toolTipWindowShown) != TreeViewFlags::none; }
    void SetToolTipWindowShown() { flags = flags | TreeViewFlags::toolTipWindowShown; }
    void ResetToolTipWindowShown() { flags = flags & ~TreeViewFlags::toolTipWindowShown; }
    float TextHeight() const { return textHeight; }
    NodeClickEvent& NodeClick() { return nodeClick; }
    NodeDoubleClickEvent& NodeDoubleClick() { return nodeDoubleClick; }
    NodeEnterEvent& NodeEnter() { return nodeEnter; }
    NodeLeaveEvent& NodeLeave() { return nodeLeave; }
    NodeSelectedEvent& NodeSelected() { return nodeSelected; }
    NodeExpandedEvent& NodeExpanded() { return  nodeExpanded; }
    NodeCollapsedEvent& NodeCollapsed() { return nodeCollapsed; }
    NodeHoveredEvent& NodeHovered() { return nodeHovered; }
    void FireNodeClick(const Point& location, MouseButtons buttons, TreeViewNode* node);
    void FireNodeDoubleClick(const Point& location, MouseButtons buttons, TreeViewNode* node);
    void FireNodeEnter(TreeViewNode* node);
    void FireNodeLeave(TreeViewNode* node);
    void FireNodeSelected(TreeViewNode* node);
    void FireNodeExpanded(TreeViewNode* node);
    void FireNodeCollapsed(TreeViewNode* node);
    void FireNodeHovered(TreeViewNode* node);
    const StringFormat& GetStringFormat() const { return stringFormat; }
    Bitmap* NodeCollapsedBitmap() const { return nodeCollapsedBitmap.get(); }
    Bitmap* NodeExpandedBitmap() const { return nodeExpandedBitmap.get(); }
protected:
    void OnPaint(PaintEventArgs& args) override;
    virtual void OnNodeClick(TreeViewNodeClickEventArgs& args);
    virtual void OnNodeDoubleClick(TreeViewNodeClickEventArgs& args);
    virtual void OnNodeEnter(TreeViewNodeEventArgs& args);
    virtual void OnNodeLeave(TreeViewNodeEventArgs& args);
    virtual void OnNodeSelected(TreeViewNodeEventArgs& args);
    virtual void OnNodeExpanded(TreeViewNodeEventArgs& args);
    virtual void OnNodeCollapsed(TreeViewNodeEventArgs& args);
    virtual void OnNodeHovered(TreeViewNodeEventArgs& args);
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseUp(MouseEventArgs& args) override;
    void OnMouseDoubleClick(MouseEventArgs& args) override;
    void OnMouseEnter() override;
    void OnMouseMove(MouseEventArgs& args) override;
    void OnMouseLeave() override;
    void OnMouseHover(MouseEventArgs& args) override;
private:
    void Measure(Graphics& graphics);
    void MakeStateIndicatorBitmaps(Graphics& graphics);
    void MakeStateIndicatorBitmap(Graphics& graphics, SolidBrush& brush, Bitmap& bm, float angle);
    TreeViewFlags flags;
    std::unique_ptr<TreeViewNode> root;
    TreeViewNode* selectedNode;
    TreeViewNode* trackedNode;
    std::string normalNodeFontFamilyName;
    float normalNodeFontSize;
    FontStyle normalNodeFontStyle;
    Font normalNodeFont;
    std::string activeNodeFontFamilyName;
    float activeNodeFontSize;
    FontStyle activeNodeFontStyle;
    Font activeNodeFont;
    Color stateIndicatorColor;
    Color selectedNodeColor;
    Color textColor;
    float textHeight;
    float stateIndicatorHeight;
    float stateIndicatorPercentage;
    std::unique_ptr<Bitmap> nodeCollapsedBitmap;
    std::unique_ptr<Bitmap> nodeExpandedBitmap;
    StringFormat stringFormat;
    ToolTip* toolTipWindow;
    NodeClickEvent nodeClick;
    NodeDoubleClickEvent nodeDoubleClick;
    NodeEnterEvent nodeEnter;
    NodeLeaveEvent nodeLeave;
    NodeSelectedEvent nodeSelected;
    NodeExpandedEvent nodeExpanded;
    NodeCollapsedEvent nodeCollapsed;
    NodeHoveredEvent nodeHovered;
};

enum class TreeViewNodeState : int
{
    collapsed, expanded
};

enum class TreeViewNodeFlags : int
{
    none = 0, mouseEntered = 1 << 0, selected = 1 << 1, active = 1 << 2
};

WING_API inline TreeViewNodeFlags operator&(TreeViewNodeFlags left, TreeViewNodeFlags right)
{
    return TreeViewNodeFlags(int(left) & int(right));
}

WING_API inline TreeViewNodeFlags operator|(TreeViewNodeFlags left, TreeViewNodeFlags right)
{
    return TreeViewNodeFlags(int(left) | int(right));
}

WING_API inline TreeViewNodeFlags operator~(TreeViewNodeFlags flags)
{
    return TreeViewNodeFlags(~int(flags));
}

class WING_API TreeViewNode : public Component
{
public:
    TreeViewNode(const std::string& text_);
    const std::string& Text() const { return text; }
    void SetText(const std::string& text_);
    bool IsTreeViewNode() const override { return true; }
    void SetTreeView(TreeView* treeView_) { treeView = treeView_; }
    TreeView* GetTreeView() const;
    TreeViewNode* Parent() const;
    const Container& Children() const { return children; }
    void AddChild(TreeViewNode* child);
    void RemoveChild(TreeViewNode* child);
    void RemoveChildren();
    int Level() const;
    TreeViewNodeState State() const { return state; }
    void SetState(TreeViewNodeState state_);
    void* Data() const { return data; }
    void SetData(void* data_) { data = data_; }
    bool MouseEntered() const { return (flags & TreeViewNodeFlags::mouseEntered) != TreeViewNodeFlags::none; }
    void SetMouseEntered() { flags = flags | TreeViewNodeFlags::mouseEntered; }
    void ResetMouseEntered() { flags = flags & ~TreeViewNodeFlags::mouseEntered; }
    bool Selected() const { return (flags & TreeViewNodeFlags::selected) != TreeViewNodeFlags::none; }
    void SetSelected() { flags = flags | TreeViewNodeFlags::selected; }
    void ResetSelected() { flags = flags & ~TreeViewNodeFlags::selected; }
    bool Active() const { return (flags & TreeViewNodeFlags::active) != TreeViewNodeFlags::none; }
    void SetActive();
    void ResetActive();
    const Point& Location() const { return location; }
    const Size& GetSize() const { return size; }
    int Index() const { return index; }
    void SetIndex(int index_) { index = index_; }
    void EnsureVisible();
    void GetVisibleNodes(std::vector<TreeViewNode*>& visibleNodes, int selectLevel);
    void DispatchMouseDown(MouseEventArgs& args, bool& handled);
    void DispatchMouseUp(MouseEventArgs& args, bool& handled);
    void DispatchMouseDoubleClick(MouseEventArgs& args, bool& handled);
    void DispatchMouseMove(MouseEventArgs& args, bool& handled);
    void DispatchMouseHover(MouseEventArgs& args, bool& handled);
    void Select();
    void Deselect();
    void Toggle();
    void Expand();
    void ExpandAll();
    void Expand(bool all);
    void Collapse();
    void CollapseAll();
    void Collapse(bool all);
    void FireMouseLeave();
    void MeasureSize(Graphics& graphics);
    void Measure(Graphics& graphics, const Point& loc, int level, int& idx, Rect& parentRect);
    void Draw(Graphics& graphics, SolidBrush& selectedBrush, SolidBrush& textBrush);
    const std::string& ToolTip() const { return toolTip; }
    void SetToolTip(const std::string& toolTip_);
protected:
    virtual void OnMouseDown(MouseEventArgs& args);
    virtual void OnMouseUp(MouseEventArgs& args);
    virtual void OnMouseDoubleClick(MouseEventArgs& args);
    virtual void OnMouseEnter();
    virtual void OnMouseLeave();
    virtual void OnMouseHover();
private:
    std::string text;
    std::string toolTip;
    TreeView* treeView;
    Container children;
    TreeViewNodeState state;
    TreeViewNodeFlags flags;
    void* data;
    Point location;
    Size size;
    Rect childRect;
    int index;
};

} } // cmajor::wing

#endif // CMAJOR_WING_TREE_VIEW_INCLUDED
