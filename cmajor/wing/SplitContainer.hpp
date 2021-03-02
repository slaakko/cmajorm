// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_SPLIT_CONTAINER_INCLUDED
#define CMAJOR_WING_SPLIT_CONTAINER_INCLUDED
#include <cmajor/wing/ContainerControl.hpp>

namespace cmajor { namespace wing {

class SplitContainer;

enum class SplitterOrientation : int
{
    horizontal, vertical
};

WING_API Dock SplitDock(SplitterOrientation orientation);
WING_API Size SplitSize(SplitterOrientation orientation, int width);
const int defaultSplitterWidth = 5;
const int defaultSplitterDistance = 0;
WING_API Color DefaultSplitterEdgeColor();

enum class SplitterFlags : int
{
    none = 0, moveSplitter = 1 << 0
};

WING_API inline SplitterFlags operator|(SplitterFlags left, SplitterFlags right)
{
    return SplitterFlags(int(left) | int(right));
}

WING_API inline SplitterFlags operator&(SplitterFlags left, SplitterFlags right)
{
    return SplitterFlags(int(left) & int(right));
}

WING_API inline SplitterFlags operator~(SplitterFlags flags)
{
    return SplitterFlags(~int(flags));
}

struct WING_API SplitterCreateParams
{
    SplitterCreateParams(SplitterOrientation orientation_);
    SplitterCreateParams& Defaults();
    SplitterCreateParams& WindowClassName(const std::string& windowClassName_);
    SplitterCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    SplitterCreateParams& WindowStyle(int windowStyle_);
    SplitterCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    SplitterCreateParams& BackgroundColor(const Color& backgroundColor_);
    SplitterCreateParams& Text(const std::string& text_);
    SplitterCreateParams& Location(const Point& location_);
    SplitterCreateParams& SetSize(Size size_);
    SplitterCreateParams& SetAnchors(Anchors anchors_);
    SplitterCreateParams& SetDock(Dock dock_);
    SplitterCreateParams& EdgeColor(const Color& edgeColor_);
    ControlCreateParams controlCreateParams;
    SplitterOrientation orientation;
    Color edgeColor;
};

class WING_API Splitter : public Control
{
public:
    Splitter(SplitterCreateParams& createParams);
    void SetContainer(SplitContainer* container_) { container = container_; }
    SplitContainer* GetContainer() const { return container; }
    SplitterOrientation Orientation() const { return orientation; }
    const Color& EdgeColor() const { return edgeColor; }
    void SetEdgeColor(const Color& edgeColor_);
protected:
    void SetCursor() override;
    void OnPaint(PaintEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseUp(MouseEventArgs& args) override;
    void OnMouseMove(MouseEventArgs& args) override;
private:
    Cursor& horizontalSplitterCursor;
    Cursor& verticalSplitterCursor;
    SplitterFlags flags;
    SplitterOrientation orientation;
    Color edgeColor;
    std::unique_ptr<Pen> edgePen;
    int x;
    int y;
    SplitContainer* container;
};

using SplitterDistanceChangedEvent = Event;

struct WING_API SplitContainerCreateParams
{
    SplitContainerCreateParams(SplitterOrientation orientation);
    SplitContainerCreateParams(const SplitContainerCreateParams&) = delete;
    SplitContainerCreateParams& operator=(const SplitContainerCreateParams&) = delete;
    SplitContainerCreateParams& Defaults();
    SplitContainerCreateParams& SplitterDistance(int splitterDistance_);
    SplitContainerCreateParams& WindowClassName(const std::string& windowClassName_);
    SplitContainerCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    SplitContainerCreateParams& WindowStyle(int windowStyle_);
    SplitContainerCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    SplitContainerCreateParams& BackgroundColor(const Color& backgroundColor_);
    SplitContainerCreateParams& Text(const std::string& text_);
    SplitContainerCreateParams& Location(const Point& location_);
    SplitContainerCreateParams& SetSize(Size size_);
    SplitContainerCreateParams& SetAnchors(Anchors anchors_);
    SplitContainerCreateParams& SetDock(Dock dock_);
    SplitContainerCreateParams& SplitterWindowClassName(const std::string& windowClassName_);
    SplitContainerCreateParams& SplitterWindowClassStyle(uint32_t windowClassStyle_);
    SplitContainerCreateParams& SplitterWindowStyle(int windowStyle_);
    SplitContainerCreateParams& SplitterWindowClassBackgroundColor(int windowClassBackgroundColor_);
    SplitContainerCreateParams& SplitterBackgroundColor(const Color& backgroundColor_);
    SplitContainerCreateParams& SplitterText(const std::string& text_);
    SplitContainerCreateParams& SplitterLocation(const Point& location_);
    SplitContainerCreateParams& SplitterSize(const Size& size_);
    SplitContainerCreateParams& SplitterAnchors(Anchors anchors_);
    SplitContainerCreateParams& SplitterDock(Dock dock_);
    SplitContainerCreateParams& SplitterEdgeColor(const Color& edgeColor_);
    SplitContainerCreateParams& SetSplitter(Splitter* splitter_);
    SplitContainerCreateParams& Pane1WindowClassName(const std::string& windowClassName_);
    SplitContainerCreateParams& Pane1WindowClassStyle(uint32_t windowClassStyle_);
    SplitContainerCreateParams& Pane1WindowStyle(int windowStyle_);
    SplitContainerCreateParams& Pane1WindowClassBackgroundColor(int windowClassBackgroundColor_);
    SplitContainerCreateParams& Pane1BackgroundColor(const Color& backgroundColor_);
    SplitContainerCreateParams& Pane1Text(const std::string& text_);
    SplitContainerCreateParams& Pane1Location(const Point& location_);
    SplitContainerCreateParams& Pane1Size(const Size& size_);
    SplitContainerCreateParams& Pane1Anchors(Anchors anchors_);
    SplitContainerCreateParams& Pane1Dock(Dock dock_);
    SplitContainerCreateParams& SetPane1(Control* pane1_);
    SplitContainerCreateParams& Pane2WindowClassName(const std::string& windowClassName_);
    SplitContainerCreateParams& Pane2WindowClassStyle(uint32_t windowClassStyle_);
    SplitContainerCreateParams& Pane2WindowStyle(int windowStyle_);
    SplitContainerCreateParams& Pane2WindowClassBackgroundColor(int windowClassBackgroundColor_);
    SplitContainerCreateParams& Pane2BackgroundColor(const Color& backgroundColor_);
    SplitContainerCreateParams& Pane2Text(const std::string& text_);
    SplitContainerCreateParams& Pane2Location(const Point& location_);
    SplitContainerCreateParams& Pane2Size(const Size& size_);
    SplitContainerCreateParams& Pane2Anchors(Anchors anchors_);
    SplitContainerCreateParams& Pane2Dock(Dock dock_);
    SplitContainerCreateParams& SetPane2(Control* pane2_);
    ControlCreateParams controlCreateParams;
    SplitterCreateParams splitterCreateParams;
    ControlCreateParams pane1CreateParams;
    ControlCreateParams pane2CreateParams;
    std::unique_ptr<Splitter> splitter;
    std::unique_ptr<Control> pane1;
    std::unique_ptr<Control> pane2;
    int splitterDistance;
};

class WING_API SplitContainer : public ContainerControl
{
public:
    SplitContainer(SplitContainerCreateParams& createParams);
    int SplitterDistance() const { return splitterDistance; }
    void SetSplitterDistance(int splitterDistance_);
    SplitterDistanceChangedEvent& SplitterDistanceChanged() { return splitterDistanceChanged; }
    Splitter* GetSplitter() const { return splitter; }
    Control* Pane1() const { return pane1; }
    Control* Pane2() const { return pane2; }
    ContainerControl* Pane1Container() const;
    ContainerControl* Pane2Container() const;
protected:
    void OnPaint(PaintEventArgs& args) override;
    virtual void OnSplitterDistanceChanged();
private:
    Splitter* splitter;
    Control* pane1;
    Control* pane2;
    int splitterDistance;
    SplitterDistanceChangedEvent splitterDistanceChanged;
};

} } // cmajor::wing

#endif // CMAJOR_WING_SPLIT_CONTAINER_INCLUDED
