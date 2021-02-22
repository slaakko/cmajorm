// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_CONTAINER_CONTROL_INCLUDED
#define CMAJOR_WING_CONTAINER_CONTROL_INCLUDED
#include <cmajor/wing/Control.hpp>
#include <cmajor/wing/Container.hpp>

namespace cmajor { namespace wing {

using ControlAddedEvent = EventWithArgs<ControlEventArgs>;
using ControlRemovedEvent = EventWithArgs<ControlEventArgs>;

class WING_API ContainerControl : public Control
{
public:
    ContainerControl(ControlCreateParams& createParams);
    bool IsContainerControl() const override { return true; }
    ContainerControl* GetContainerControl() const override;
    void AddChild(Control* child);
    std::unique_ptr<Control> RemoveChild(Control* child);
    void InsertChildBefore(Control* child, Control* before);
    void InsertChildAfter(Control* child, Control* after);
    void DockChildren();
    void DockChildren(Rect& parentRect);
    ControlAddedEvent& ControlAdded() { return controlAdded; }
    ControlRemovedEvent& ControlRemoved() { return controlRemoved; }
protected:
    bool ProcessMessage(Message& msg) override;
    virtual void OnControlAdded(ControlEventArgs& args);
    virtual void OnControlRemoved(ControlEventArgs& args);
private:
    Container children;
    ControlAddedEvent controlAdded;
    ControlRemovedEvent controlRemoved;
};

} } // cmajor::wing

#endif // CMAJOR_WING_CONTAINER_CONTROL_INCLUDED
