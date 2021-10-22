// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/TreeView.hpp>
#include <wing/Window.hpp>
#include <soulng/util/Unicode.hpp>

#undef min
#undef max

namespace cmajor { namespace wing {

using namespace soulng::unicode;    

TreeViewNodeClickEventArgs::TreeViewNodeClickEventArgs(const Point& location_, MouseButtons buttons_, int clicks_, TreeViewNode* node_) :
    location(location_), buttons(buttons_), clicks(clicks_), node(node_)
{
}

TreeViewNodeEventArgs::TreeViewNodeEventArgs(TreeViewNode* node_) : node(node_)
{
}

TreeViewCreateParams::TreeViewCreateParams() : 
    controlCreateParams(),
    normalNodeFontFamilyName("Segoe UI"),
    normalNodeFontSize(9.0f),
    normalNodeFontStyle(FontStyle::FontStyleRegular),
    activeNodeFontFamilyName("Segoe UI"),
    activeNodeFontySize(9.0f),
    activeNodeFontStyle(FontStyle::FontStyleBold),
    stateIndicatorColor(Color::Red),
    selectedNodeColor(Color(201, 222, 245)),
    textColor(Color::Black),
    stateIndicatorPercentage(50.0f),
    addToolTip(true),
    nodeIndentPercent(200),
    nodeTextIndentPercent(100)
{
    controlCreateParams.WindowClassName("wing.TreeView");
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.WindowClassStyle(DoubleClickWindowClassStyle());
    controlCreateParams.WindowStyle(DefaultChildWindowStyle());
    controlCreateParams.BackgroundColor(Color::White);
}

TreeViewCreateParams& TreeViewCreateParams::Defaults()
{
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::Location(const Point& location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::NormalNodeFontFamilyName(const std::string& fontName)
{
    normalNodeFontFamilyName = fontName;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::NormalNodeFontSize(float fontSize)
{
    normalNodeFontSize = fontSize;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::NormalNodeFontStyle(FontStyle fontStyle)
{
    normalNodeFontStyle = fontStyle;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::ActiveNodeFontFamilyName(const std::string& fontName)
{
    activeNodeFontFamilyName = fontName;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::ActiveNodeFontSize(float fontSize)
{
    activeNodeFontySize = fontSize;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::ActiveNodeFontStyle(FontStyle fontStyle)
{
    activeNodeFontStyle = fontStyle;
    return* this;
}

TreeViewCreateParams& TreeViewCreateParams::StateIndicatorColor(const Color& color)
{
    stateIndicatorColor = color;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::SelectedNodeColor(const Color& color)
{
    selectedNodeColor = color;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::TextColor(const Color& color)
{
    textColor = color;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::StateIndicatorPercentage(float percentage)
{
    stateIndicatorPercentage = percentage;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::AddToolTip(bool addToolTip_)
{
    addToolTip = addToolTip_;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::NodeIndentPercent(float percent)
{
    nodeIndentPercent = percent;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::NodeTextIndentPercent(float percent)
{
    nodeTextIndentPercent = percent;
    return *this;
}

TreeViewCreateParams& TreeViewCreateParams::NodeImagePadding(const Padding& padding)
{
    nodeImagePadding = padding;
    return *this;
}


TreeView::TreeView(TreeViewCreateParams& createParams) : 
    Control(createParams.controlCreateParams), 
    imageList(nullptr),
    flags(),
    root(),
    selectedNode(),
    trackedNode(),
    normalNodeFontFamilyName(),
    normalNodeFontSize(),
    normalNodeFontStyle(),
    normalNodeFont(),
    activeNodeFontFamilyName(),
    activeNodeFontSize(),
    activeNodeFontStyle(),
    activeNodeFont(),
    stateIndicatorColor(createParams.stateIndicatorColor),
    selectedNodeColor(createParams.selectedNodeColor),
    textColor(createParams.textColor),
    textHeight(0),
    nodeIndentPercent(createParams.nodeIndentPercent),
    nodeTextIndentPercent(createParams.nodeTextIndentPercent),
    nodeImagePadding(createParams.nodeImagePadding),
    stateIndicatorHeight(0),
    stateIndicatorPercentage(createParams.stateIndicatorPercentage),
    stringFormat(),
    toolTipWindow()
{
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    if (createParams.addToolTip)
    {
        flags = flags | TreeViewFlags::addToolTipWindow;
    }
    SetNormalNodeFont(createParams.normalNodeFontFamilyName, createParams.normalNodeFontSize, createParams.normalNodeFontStyle);
    SetActiveNodeFont(createParams.activeNodeFontFamilyName, createParams.activeNodeFontySize, createParams.activeNodeFontStyle);
    SetMouseHoverMs(DefaultMouseHoverMs());
    SetChanged();
}

void TreeView::SetRoot(TreeViewNode* root_)
{
    if (root)
    {
        root->SetTreeView(nullptr);
    }
    root.reset(root_);
    SetContentLocation(Point(0, 0));
    if (root)
    {
        root->SetTreeView(this);
        SetTreeViewNodeChanged();
        SetTreeViewNodeStateChanged();
        SetContentChanged();
    }
    SetChanged();
    Invalidate();
}

void TreeView::EnsureNodeVisible(TreeViewNode* node)
{
    Size size = GetSize();
    Point contentLocation = ContentLocation();
    Point nodeLocation = node->Location();
    if (nodeLocation.Y >= contentLocation.Y && nodeLocation.Y < contentLocation.Y + size.Height)
    {
        return;
    }
    if (nodeLocation.Y > contentLocation.Y)
    {
        int y = nodeLocation.Y - std::max(0, static_cast<int>(size.Height - 2 * textHeight));
        Point newOrigin(0, y);
        SetContentLocation(newOrigin);
        Invalidate();
    }
    else
    {
        int y = nodeLocation.Y;
        Point newOrigin(0, y);
        SetContentLocation(newOrigin);
        Invalidate();
    }
}

bool TreeView::NodeVisible(TreeViewNode* node) const
{
    Size size = GetSize();
    Point contentLocation = ContentLocation();
    Point nodeLocation = node->Location();
    if (nodeLocation.Y >= contentLocation.Y && nodeLocation.Y < contentLocation.Y + size.Height)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<TreeViewNode*> TreeView::GetVisibleNodes() const
{
    return GetVisibleNodes(-1);
}

std::vector<TreeViewNode*> TreeView::GetVisibleNodes(int level) const
{
    std::vector<TreeViewNode*> visibleNodes;
    root->GetVisibleNodes(visibleNodes, level);
    return visibleNodes;
}

void TreeView::SetSelectedNode(TreeViewNode* node)
{
    if (selectedNode != node)
    {
        if (selectedNode)
        {
            selectedNode->ResetSelected();
        }
        selectedNode = node;
        SetTreeViewNodeStateChanged();
    }
}

void TreeView::DeselectNode()
{
    if (selectedNode)
    {
        TreeViewNode* selectedNodeParent = selectedNode->Parent();
        while (selectedNodeParent)
        {
            if (selectedNodeParent->State() == TreeViewNodeState::collapsed)
            {
                SetSelectedNode(nullptr);
                break;
            }
            selectedNodeParent = selectedNodeParent->Parent();
        }
    }
}

void TreeView::FireNodeClick(const Point& location, MouseButtons buttons, TreeViewNode* node)
{
    TreeViewNodeClickEventArgs args(location, buttons, 1, node);
    OnNodeClick(args);
}

void TreeView::FireNodeDoubleClick(const Point& location, MouseButtons buttons, TreeViewNode* node)
{
    TreeViewNodeClickEventArgs args(location, buttons, 2, node);
    OnNodeDoubleClick(args);
}

void TreeView::FireNodeEnter(TreeViewNode* node)
{
    TreeViewNodeEventArgs args(node);
    OnNodeEnter(args);
}

void TreeView::FireNodeLeave(TreeViewNode* node)
{
    TreeViewNodeEventArgs args(node);
    OnNodeLeave(args);
}

void TreeView::FireNodeSelected(TreeViewNode* node)
{
    TreeViewNodeEventArgs args(node);
    OnNodeSelected(args);
}

void TreeView::FireNodeExpanded(TreeViewNode* node)
{
    TreeViewNodeEventArgs args(node);
    OnNodeExpanded(args);
}

void TreeView::FireNodeCollapsed(TreeViewNode* node)
{
    TreeViewNodeEventArgs args(node);
    OnNodeCollapsed(args);
}

void TreeView::FireNodeHovered(TreeViewNode* node)
{
    TreeViewNodeEventArgs args(node);
    OnNodeHovered(args);
}

void TreeView::OnPaint(PaintEventArgs& args)
{
    try
    {
        if ((flags & TreeViewFlags::addToolTipWindow) != TreeViewFlags::none && !ToolTipWindowAdded())
        {
            SetToolTipWindowAdded();
            Window* window = GetWindow();
            if (window)
            {
                toolTipWindow = new ToolTip(ToolTipCreateParams().Defaults());
                window->AddChild(toolTipWindow);
            }
        }
        if (Changed())
        {
            ResetChanged();
            Measure(args.graphics);
            MakeStateIndicatorBitmaps(args.graphics);
            SetTreeViewNodeChanged();
            SetTreeViewNodeStateChanged();
        }
        if (TreeViewNodeChanged())
        {
            ResetTreeViewNodeChanged();
            if (root)
            {
                root->MeasureSize(args.graphics);
            }
        }
        if (TreeViewNodeStateChanged())
        {
            ResetTreeViewNodeStateChanged();
            if (root)
            {
                Rect r(Point(), GetSize());
                r.Width = r.Width - 1;
                r.Height = r.Height - 1;
                int idx = 0;
                Rect rect;
                root->Measure(args.graphics, Point(r.X, r.Y), 0, idx, rect);
                SetContentSize(Size(rect.Width, rect.Height));
            }
        }
        args.graphics.Clear(BackgroundColor());
        if (root)
        {
            SolidBrush selectedBrush(selectedNodeColor);
            SolidBrush textBrush(textColor);
            root->Draw(args.graphics, selectedBrush, textBrush);
        }
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void TreeView::OnSizeChanged()
{
    Control::OnSizeChanged();
    SetContentLocation(Point(0, 0));
}

void TreeView::OnNodeClick(TreeViewNodeClickEventArgs& args)
{
    nodeClick.Fire(args);
}

void TreeView::OnNodeDoubleClick(TreeViewNodeClickEventArgs& args)
{
    nodeDoubleClick.Fire(args);
}

void TreeView::OnNodeEnter(TreeViewNodeEventArgs& args)
{
    nodeEnter.Fire(args);
}

void TreeView::OnNodeLeave(TreeViewNodeEventArgs& args)
{
    nodeLeave.Fire(args);
}

void TreeView::OnNodeSelected(TreeViewNodeEventArgs& args)
{
    nodeSelected.Fire(args);
}

void TreeView::OnNodeExpanded(TreeViewNodeEventArgs& args)
{
    nodeExpanded.Fire(args);
}

void TreeView::OnNodeCollapsed(TreeViewNodeEventArgs& args)
{
    nodeCollapsed.Fire(args);
}

void TreeView::OnNodeHovered(TreeViewNodeEventArgs& args)
{
    nodeHovered.Fire(args);
}

void TreeView::OnMouseDown(MouseEventArgs& args)
{
    Control::OnMouseDown(args);
    if (root)
    {
        bool handled = false;
        root->DispatchMouseDown(args, handled);
        if (TreeViewNodeStateChanged())
        {
            DeselectNode();
            Invalidate();
        }
    }
}

void TreeView::OnMouseUp(MouseEventArgs& args)
{
    Control::OnMouseUp(args);
    if (root)
    {
        bool handled = false;
        root->DispatchMouseUp(args, handled);
    }
}

void TreeView::OnMouseDoubleClick(MouseEventArgs& args)
{
    Control::OnMouseDoubleClick(args);
    if (root)
    {
        bool handled = false;
        root->DispatchMouseDoubleClick(args, handled);
        if (TreeViewNodeStateChanged())
        {
            DeselectNode();
            Invalidate();
        }
    }
}

void TreeView::OnMouseEnter()
{
    Control::OnMouseEnter();
    trackedNode = nullptr;
}

void TreeView::OnMouseMove(MouseEventArgs& args)
{
    Control::OnMouseMove(args);
    if (root)
    {
        bool handled = false;
        root->DispatchMouseMove(args, handled);
        if (!handled)
        {
            if (trackedNode)
            {
                if (trackedNode->MouseEntered())
                {
                    trackedNode->ResetMouseEntered();
                    trackedNode->FireMouseLeave();
                }
                trackedNode = nullptr;
            }
        }
    }
}

void TreeView::OnMouseLeave()
{
    Control::OnMouseLeave();
    if (trackedNode)
    {
        if (trackedNode->MouseEntered())
        {
            trackedNode->ResetMouseEntered();
            trackedNode->FireMouseLeave();
        }
        trackedNode = nullptr;
    }
}

void TreeView::OnMouseHover(MouseEventArgs& args)
{
    Control::OnMouseHover(args);
    if (root)
    {
        bool handled = false;
        root->DispatchMouseHover(args, handled);
    }
}

void TreeView::Measure(Graphics& graphics)
{
    std::string s = "RootNode";
    RectF normalTextRect = MeasureString(graphics, s, normalNodeFont, PointF(0, 0), stringFormat);
    textHeight = normalTextRect.Height;
    RectF activeTextRect = MeasureString(graphics, s, activeNodeFont, PointF(0, 0), stringFormat);
    textHeight = std::max(textHeight, activeTextRect.Height);
    stateIndicatorHeight = stateIndicatorPercentage * textHeight / 100.0f;
    SetScrollUnits(ScrollUnits(static_cast<int>(textHeight + 0.5), static_cast<int>(textHeight + 0.5)));
}

void TreeView::MakeStateIndicatorBitmaps(Graphics& graphics)
{
    SolidBrush brush(stateIndicatorColor);
    nodeCollapsedBitmap.reset(new Bitmap(static_cast<int>(textHeight), static_cast<int>(textHeight), &graphics));
    nodeExpandedBitmap.reset(new Bitmap(static_cast<int>(textHeight), static_cast<int>(textHeight), &graphics));
    MakeStateIndicatorBitmap(graphics, brush, *nodeCollapsedBitmap, 0.0f);
    MakeStateIndicatorBitmap(graphics, brush, *nodeExpandedBitmap, 90.0f);
}

void TreeView::MakeStateIndicatorBitmap(Graphics& graphics, SolidBrush& brush, Bitmap& bm, float angle)
{
    PointF center(textHeight / 2, textHeight / 2);
    float w = static_cast<float>(std::sqrt(3.0) / 2 * stateIndicatorHeight);
    PointF pt1(center.X - w / 2, center.Y - stateIndicatorHeight / 2);
    PointF pt2(center.X - w / 2, center.Y + stateIndicatorHeight / 2);
    PointF pt3(center.X + w / 2, center.Y);
    std::vector<PointF> triangle;
    triangle.push_back(pt1);
    triangle.push_back(pt2);
    triangle.push_back(pt3);
    std::unique_ptr<Graphics> bmGraphics(Graphics::FromImage(&bm));
    bmGraphics->Clear(BackgroundColor());
    if (angle != 0)
    {
        Matrix rm;
        CheckGraphicsStatus(rm.RotateAt(angle, center));
        CheckGraphicsStatus(rm.TransformPoints(triangle.data(), 3));
    }
    CheckGraphicsStatus(bmGraphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality));
    std::vector<Point> points;
    for (int i = 0; i < triangle.size(); ++i)
    {
        points.push_back(Point(static_cast<int>(triangle[i].X), static_cast<int>(triangle[i].Y)));
    }
    CheckGraphicsStatus(bmGraphics->FillPolygon(&brush, points.data(), 3));
}

void TreeView::SetNormalNodeFont(const std::string& normalNodeFontFamilyName_, float normalNodeFontSize_, FontStyle normalNodeFontStyle_)
{
    if (normalNodeFontFamilyName_ != normalNodeFontFamilyName || normalNodeFontSize_ != normalNodeFontSize || normalNodeFontStyle_ != normalNodeFontStyle)
    {
        normalNodeFontFamilyName = normalNodeFontFamilyName_;
        normalNodeFontSize = normalNodeFontSize_;
        normalNodeFontStyle = normalNodeFontStyle_;
        std::u16string fontFamilyName = ToUtf16(normalNodeFontFamilyName);
        normalNodeFont = Font(FontFamily((const WCHAR*)fontFamilyName.c_str()), normalNodeFontSize, normalNodeFontStyle, Unit::UnitPoint);
        SetChanged();
    }
}

void TreeView::SetActiveNodeFont(const std::string& activeNodeFontFamilyName_, float activeNodeFontSize_, FontStyle activeNodeFontStyle_)
{
    if (activeNodeFontFamilyName_ != activeNodeFontFamilyName || activeNodeFontSize_ != activeNodeFontSize || activeNodeFontStyle_ != activeNodeFontStyle)
    {
        activeNodeFontFamilyName = activeNodeFontFamilyName_;
        activeNodeFontSize = activeNodeFontSize_;
        activeNodeFontStyle = activeNodeFontStyle_;
        std::u16string fontFamilyName = ToUtf16(activeNodeFontFamilyName);
        activeNodeFont = Font(FontFamily((const WCHAR*)fontFamilyName.c_str()), activeNodeFontSize, activeNodeFontStyle, Unit::UnitPoint);
        SetChanged();
    }
}

void TreeView::ShowToolTipWindow(TreeViewNode* node)
{
    if (!toolTipWindow || !ToolTipWindowAdded()) return;
    if (ToolTipWindowShown()) toolTipWindow->Hide();
    ResetToolTipWindowShown();
    Point loc = node->Location();
    Size size = node->GetSize();
    toolTipWindow->SetText(node->ToolTip());
    toolTipWindow->MeasureExtent();
    Point pt(loc.X, loc.Y + size.Height + 8);
    pt = ClientToScreen(pt);
    Window* window = GetWindow();
    if (window)
    {
        pt = window->ScreenToClient(pt);
        Size sz = toolTipWindow->GetSize();
        Point wloc = window->Location();
        Size wsz = window->GetSize();
        if (pt.Y + sz.Height > wloc.Y + wsz.Height)
        {
            pt.Y = wloc.Y + wsz.Height - sz.Height;
        }
        if (pt.X + sz.Width > wloc.X + wsz.Width)
        {
            pt.X = wloc.X + wsz.Width - sz.Width;
        }
        toolTipWindow->SetLocation(pt);
        toolTipWindow->BringToFront();
        toolTipWindow->Show();
        toolTipWindow->Invalidate();
        SetToolTipWindowShown();
    }
}

void TreeView::HideToolTipWindow()
{
    if (ToolTipWindowShown())
    {
        ResetToolTipWindowShown();
        toolTipWindow->Hide();
    }
}

TreeViewNode::TreeViewNode(const std::string& text_) : 
    text(text_), treeView(nullptr), children(this), state(TreeViewNodeState::collapsed), flags(TreeViewNodeFlags::none), data(nullptr), location(), size(), childRect(), index(-1), imageIndex(-1), expandedImageIndex(-1)
{
}

void TreeViewNode::SetText(const std::string& text_)
{
    text = text_;
    TreeView* view = GetTreeView();
    if (view)
    {
        view->SetTreeViewNodeChanged();
    }
}

TreeView* TreeViewNode::GetTreeView() const
{
    if (treeView)
    {
        return treeView;
    }
    TreeViewNode* parent = Parent();
    if (parent)
    {
        return parent->GetTreeView();
    }
    return nullptr;
}

TreeViewNode* TreeViewNode::Parent() const
{
    Container* container = GetContainer();
    if (container)
    {
        Component* parent = container->Parent();
        if (parent)
        {
            if (parent->IsTreeViewNode())
            {
                return static_cast<TreeViewNode*>(parent);
            }
        }
    }
    return nullptr;
}

void TreeViewNode::AddChild(TreeViewNode* child)
{
    children.AddChild(child);
    TreeView* view = GetTreeView();
    if (view)
    {
        view->SetTreeViewNodeChanged();
    }
}

std::unique_ptr<TreeViewNode> TreeViewNode::RemoveChild(TreeViewNode* child)
{
    std::unique_ptr<Component> removed = children.RemoveChild(child);
    TreeView* view = GetTreeView();
    if (view)
    {
        view->SetTreeViewNodeChanged();
        view->SetChanged();
        view->Invalidate();
    }
    return std::unique_ptr<TreeViewNode>(static_cast<TreeViewNode*>(removed.release()));
}

void TreeViewNode::RemoveChildren()
{
    Component* component = children.FirstChild();
    while (component)
    {
        Component* next = component->NextSibling();
        std::unique_ptr<Component> removed = children.RemoveChild(component);
        component = next;
    }
}

int TreeViewNode::Level() const
{
    TreeViewNode* parent = Parent();
    if (parent)
    {
        return parent->Level() + 1;
    }
    return 0;
}

void TreeViewNode::SetState(TreeViewNodeState state_)
{
    if (state != state_)
    {
        state = state_;
        TreeView* view = GetTreeView();
        if (view)
        {
            view->SetTreeViewNodeStateChanged();
            if (state == TreeViewNodeState::collapsed)
            {
                view->SetContentLocation(Point(0, 0));
            }
        }
    }
}

void TreeViewNode::SetActive()
{
    if (!Active())
    {
        flags = flags | TreeViewNodeFlags::active;
        TreeView* view = GetTreeView();
        if (view)
        {
            view->SetTreeViewNodeStateChanged();
        }
    }
}

void TreeViewNode::ResetActive()
{
    if (Active())
    {
        flags = flags & ~TreeViewNodeFlags::active;
        TreeView* view = GetTreeView();
        if (view)
        {
            view->SetTreeViewNodeStateChanged();
        }
    }
}

void TreeViewNode::EnsureVisible()
{
    TreeView* view = GetTreeView();
    if (view)
    {
        view->EnsureNodeVisible(this);
    }
}

void TreeViewNode::GetVisibleNodes(std::vector<TreeViewNode*>& visibleNodes, int selectLevel)
{
    if (selectLevel == -1 || Level() == selectLevel)
    {
        TreeView* view = GetTreeView();
        if (view)
        {
            if (view->NodeVisible(this))
            {
                visibleNodes.push_back(this);
            }
        }
    }
    if (state == TreeViewNodeState::expanded)
    {
        Component* child = children.FirstChild();
        while (child)
        {
            if (child->IsTreeViewNode())
            {
                TreeViewNode* node = static_cast<TreeViewNode*>(child);
                node->GetVisibleNodes(visibleNodes, selectLevel);
            }
            child = child->NextSibling();
        }
    }
}

void TreeViewNode::DispatchMouseDown(MouseEventArgs& args, bool& handled)
{
    Rect r(location, size);
    if (r.Contains(args.location))
    {
        OnMouseDown(args);
        handled = true;
    }
    else
    {
        if (childRect.Contains(args.location))
        {
            Component* child = children.FirstChild();
            while (child)
            {
                if (child->IsTreeViewNode())
                {
                    TreeViewNode* childNode = static_cast<TreeViewNode*>(child);
                    childNode->DispatchMouseDown(args, handled);
                    if (handled) return;
                }
                child = child->NextSibling();
            }
        }
    }
}

void TreeViewNode::DispatchMouseUp(MouseEventArgs& args, bool& handled)
{
    Rect r(location, size);
    if (r.Contains(args.location))
    {
        OnMouseUp(args);
        handled = true;
    }
    else
    {
        if (childRect.Contains(args.location))
        {
            Component* child = children.FirstChild();
            while (child)
            {
                if (child->IsTreeViewNode())
                {
                    TreeViewNode* childNode = static_cast<TreeViewNode*>(child);
                    childNode->DispatchMouseUp(args, handled);
                    if (handled) return;
                }
                child = child->NextSibling();
            }
        }
    }
}

void TreeViewNode::DispatchMouseDoubleClick(MouseEventArgs& args, bool& handled)
{
    Rect r(location, size);
    if (r.Contains(args.location))
    {
        OnMouseDoubleClick(args);
        handled = true;
    }
    else
    {
        if (childRect.Contains(args.location))
        {
            Component* child = children.FirstChild();
            while (child)
            {
                if (child->IsTreeViewNode())
                {
                    TreeViewNode* childNode = static_cast<TreeViewNode*>(child);
                    childNode->DispatchMouseDoubleClick(args, handled);
                    if (handled) return;
                }
                child = child->NextSibling();
            }
        }
    }
}

void TreeViewNode::DispatchMouseMove(MouseEventArgs& args, bool& handled)
{
    Rect r(location, size);
    if (r.Contains(args.location))
    {
        handled = true;
        TreeView* view = GetTreeView();
        if (view)
        {
            TreeViewNode* prevNode = view->TrackedNode();
            if (prevNode && prevNode != this)
            {
                if (prevNode->MouseEntered())
                {
                    prevNode->ResetMouseEntered();
                    prevNode->OnMouseLeave();
                }
            }
            view->SetTrackedNode(this);
            if (!MouseEntered())
            {
                SetMouseEntered();
                OnMouseEnter();
            }
        }
    }
    else
    {
        if (childRect.Contains(args.location))
        {
            Component* child = children.FirstChild();
            while (child)
            {
                if (child->IsTreeViewNode())
                {
                    TreeViewNode* childNode = static_cast<TreeViewNode*>(child);
                    childNode->DispatchMouseMove(args, handled);
                    if (handled) return;
                }
                child = child->NextSibling();
            }
        }
    }
}

void TreeViewNode::DispatchMouseHover(MouseEventArgs& args, bool& handled)
{
    Rect r(location, size);
    if (r.Contains(args.location))
    {
        handled = true;
        OnMouseHover();
    }
    else
    {
        if (childRect.Contains(args.location))
        {
            Component* child = children.FirstChild();
            while (child)
            {
                if (child->IsTreeViewNode())
                {
                    TreeViewNode* childNode = static_cast<TreeViewNode*>(child);
                    childNode->DispatchMouseHover(args, handled);
                    if (handled) return;
                }
                child = child->NextSibling();
            }
        }
    }
}

void TreeViewNode::Select()
{
    if (!Selected())
    {
        SetSelected();
        TreeView* view = GetTreeView();
        if (view)
        {
            view->SetSelectedNode(this);
            view->FireNodeSelected(this);
        }
    }
}

void TreeViewNode::Deselect()
{
    if (Selected())
    {
        ResetSelected();
        TreeView* view = GetTreeView();
        if (view)
        {
            view->SetSelectedNode(nullptr);
        }
    }
}

void TreeViewNode::Toggle()
{
    switch (state)
    {
        case TreeViewNodeState::expanded:
        {
            Collapse();
            break;
        }
        case TreeViewNodeState::collapsed:
        {
            Expand();
            break;
        }
    }
}

void TreeViewNode::Expand()
{
    if (children.IsEmpty()) return;
    Expand(false);
    TreeView* view = GetTreeView();
    if (view)
    {
        view->Invalidate();
        view->FireNodeExpanded(this);
    }
}

void TreeViewNode::ExpandAll()
{
    if (children.IsEmpty()) return;
    Expand(true);
    TreeView* view = GetTreeView();
    if (view)
    {
        view->Invalidate();
        view->FireNodeExpanded(this);
    }
}

void TreeViewNode::Expand(bool all)
{
    if (children.IsEmpty()) return;
    SetState(TreeViewNodeState::expanded);
    if (all)
    {
        Component* child = children.FirstChild();
        while (child)
        {
            if (child->IsTreeViewNode())
            {
                TreeViewNode* childNode = static_cast<TreeViewNode*>(child);
                childNode->Expand(all);
            }
            child = child->NextSibling();
        }
    }
}

void TreeViewNode::Collapse()
{
    if (children.IsEmpty()) return;
    Collapse(false);
    TreeView* view = GetTreeView();
    if (view)
    {
        view->Invalidate();
        view->FireNodeCollapsed(this);
    }
}

void TreeViewNode::CollapseAll()
{
    if (children.IsEmpty()) return;
    Collapse(true);
    TreeView* view = GetTreeView();
    if (view)
    {
        view->Invalidate();
        view->FireNodeCollapsed(this);
    }
}

void TreeViewNode::Collapse(bool all)
{
    if (children.IsEmpty()) return;
    SetState(TreeViewNodeState::collapsed);
    if (all)
    {
        Component* child = children.FirstChild();
        while (child)
        {
            if (child->IsTreeViewNode())
            {
                TreeViewNode* childNode = static_cast<TreeViewNode*>(child);
                childNode->Collapse(all);
            }
            child = child->NextSibling();
        }
    }
}

void TreeViewNode::FireMouseLeave()
{
    OnMouseLeave();
}

void TreeViewNode::MeasureSize(Graphics& graphics)
{
    TreeView* view = GetTreeView();
    RectF textRect = MeasureString(graphics, text, view->NormalNodeFont(), PointF(0, 0), view->GetStringFormat());
    SizeF imageSize(0, 0);
    if (imageIndex != -1)
    {
        Padding padding = view->NodeImagePadding();
        ImageList* imageList = view->GetImageList();
        if (imageList)
        {
            Bitmap* bitmap = imageList->GetImage(imageIndex);
            imageSize.Width = bitmap->GetWidth() + padding.Horizontal();
            imageSize.Height = bitmap->GetHeight() + padding.Vertical();
        }
    }
    size = Size(static_cast<int>(view->TextHeight() + textRect.Width + imageSize.Width), static_cast<int>(std::max(textRect.Height, imageSize.Height)));
    Component* child = children.FirstChild();
    while (child)
    {
        if (child->IsTreeViewNode())
        {
            TreeViewNode* childNode = static_cast<TreeViewNode*>(child);
            childNode->MeasureSize(graphics);
        }
        child = child->NextSibling();
    }
}

void TreeViewNode::Measure(Graphics& graphics, const Point& loc, int level, int& idx, Rect& parentRect)
{
    TreeView* view = GetTreeView();
    if (view)
    {
        //location = Point(static_cast<int>(loc.X + level * 2 * view->TextHeight()), static_cast<int>(loc.Y + idx * view->TextHeight()));
        float imageHeight = 0;
        if (imageIndex != -1)
        {
            Padding padding = view->NodeImagePadding();
            ImageList* imageList = view->GetImageList();
            if (imageList)
            {
                Bitmap* image = imageList->GetImage(imageIndex);
                imageHeight = image->GetHeight() + padding.Vertical();
            }
        }
        location = Point(static_cast<int>(loc.X + level * view->NodeIndentPercent() * view->TextHeight() / 100.0f), static_cast<int>(loc.Y + idx * std::max(view->TextHeight(), imageHeight)));
        Rect rect(location, size);
        childRect = rect;
        Rect::Union(parentRect, parentRect, childRect);
        if (state == TreeViewNodeState::expanded)
        {
            Component* child = children.FirstChild();
            while (child)
            {
                if (child->IsTreeViewNode())
                {
                    TreeViewNode* childNode = static_cast<TreeViewNode*>(child);
                    ++idx;
                    childNode->Measure(graphics, loc, level + 1, idx, childRect);
                    Rect::Union(parentRect, parentRect, childRect);
                }
                child = child->NextSibling();
            }
        }
    }
}

void TreeViewNode::Draw(Graphics& graphics, SolidBrush& selectedBrush, SolidBrush& textBrush)
{
    TreeView* view = GetTreeView();
    if (view)
    {
        bool visible = view->NodeVisible(this);
        Point loc = location;
        if (!children.IsEmpty())
        {
            if (visible)
            {
                switch (state)
                {
                    case TreeViewNodeState::expanded:
                    {
                        CheckGraphicsStatus(graphics.DrawImage(view->NodeExpandedBitmap(), PointF(loc.X, loc.Y)));
                        break;
                    }
                    case TreeViewNodeState::collapsed:
                    {
                        CheckGraphicsStatus(graphics.DrawImage(view->NodeCollapsedBitmap(), PointF(loc.X, loc.Y)));
                        break;
                    }
                }
            }
            loc.X = static_cast<int>(loc.X + view->NodeTextIndentPercent() * view->TextHeight() / 100.0f);
        }
        if (visible)
        {
            if (imageIndex != -1)
            {
                DrawImage(view, graphics, loc);
            }
        }
        if (visible)
        {
            if (Selected())
            {
                Rect rect(loc, size);
                CheckGraphicsStatus(graphics.FillRectangle(&selectedBrush, rect));
            }
            if (Active())
            {
                DrawString(graphics, text, view->ActiveNodeFont(), PointF(loc.X, loc.Y), textBrush);
            }
            else
            {
                DrawString(graphics, text, view->NormalNodeFont(), PointF(loc.X, loc.Y), textBrush);
            }
        }
        if (state == TreeViewNodeState::expanded)
        {
            Component* child = children.FirstChild();
            while (child)
            {
                if (child->IsTreeViewNode())
                {
                    TreeViewNode* childNode = static_cast<TreeViewNode*>(child);
                    childNode->Draw(graphics, selectedBrush, textBrush);
                }
                child = child->NextSibling();
            }
        }
    }
}

void TreeViewNode::SetToolTip(const std::string& toolTip_)
{
    toolTip = toolTip_;
}

void TreeViewNode::SetImageIndex(int imageIndex_)
{
    if (imageIndex != imageIndex_)
    {
        imageIndex = imageIndex_;
        TreeView* view = GetTreeView();
        if (view)
        {
            view->SetTreeViewNodeStateChanged();
            view->SetChanged();
            view->Invalidate();
        }
    }
}

void TreeViewNode::SetExpandedImageIndex(int expandedImageIndex_)
{
    expandedImageIndex = expandedImageIndex_;
    TreeView* view = GetTreeView();
    if (view)
    {
        view->SetTreeViewNodeStateChanged();
        view->SetChanged();
        view->Invalidate();
    }
}

void TreeViewNode::OnMouseDown(MouseEventArgs& args)
{
    if ((args.buttons & MouseButtons::lbutton) != MouseButtons::none)
    {
        TreeView* view = GetTreeView();
        if (view)
        {
            if (children.IsEmpty())
            {
                Select();
            }
            else
            {
                int stateSquareSideLength = static_cast<int>(view->TextHeight());
                Rect stateRect(location, Size(stateSquareSideLength, stateSquareSideLength));
                if (stateRect.Contains(args.location))
                {
                    Toggle();
                }
                else
                {
                    Select();
                }
            }
        }
    }
    else if ((args.buttons & MouseButtons::rbutton) != MouseButtons::none)
    {
        Select();
    }
}

void TreeViewNode::OnMouseUp(MouseEventArgs& args)
{
    if ((args.buttons & MouseButtons::lbutton) != MouseButtons::none)
    {
        if (Selected())
        {
            TreeView* view = GetTreeView();
            if (view)
            {
                view->FireNodeClick(args.location, args.buttons, this);
            }
        }
    }
    else if ((args.buttons & MouseButtons::rbutton) != MouseButtons::none)
    {
        if (Selected())
        { 
            TreeView* view = GetTreeView();
            if (view)
            {
                view->FireNodeClick(args.location, args.buttons, this);
            }
        }
    }
}

void TreeViewNode::OnMouseDoubleClick(MouseEventArgs& args)
{
    TreeView* view = GetTreeView();
    if (view)
    {
        view->FireNodeDoubleClick(args.location, args.buttons, this);
    }
}

void TreeViewNode::OnMouseEnter()
{
    TreeView* view = GetTreeView();
    if (view)
    {
        view->FireNodeEnter(this);
    }
}

void TreeViewNode::OnMouseLeave()
{
    TreeView* view = GetTreeView();
    if (view)
    {
        view->FireNodeLeave(this);
    }
}

void TreeViewNode::OnMouseHover()
{
    TreeView* view = GetTreeView();
    if (view)
    {
        view->FireNodeHovered(this);
    }
}

void TreeViewNode::DrawImage(TreeView* view, Graphics& graphics, Point& loc)
{
    ImageList* imageList = view->GetImageList();
    if (imageList)
    {
        int imgIndex = imageIndex;
        if (state == TreeViewNodeState::expanded)
        {
            if (expandedImageIndex != -1)
            {
                imgIndex = expandedImageIndex;
            }
        }
        Bitmap* image = imageList->GetImage(imgIndex);
        if (image)
        {
            int imageWidth = image->GetWidth();
            int imageHeight = image->GetHeight();
            Padding padding = view->NodeImagePadding();
            Rect r(loc, Size(imageWidth + padding.Horizontal(), imageHeight + padding.Vertical()));
            Gdiplus::ImageAttributes attributes;
            attributes.SetColorKey(DefaultBitmapTransparentColor(), DefaultBitmapTransparentColor());
            CheckGraphicsStatus(graphics.DrawImage(image, r, 0, 0, imageWidth + padding.Horizontal(), imageHeight + padding.Vertical(), Unit::UnitPixel, &attributes));
            loc.X = loc.X + imageWidth + padding.Horizontal();
        }
    }
}

} } // cmajor::wing
