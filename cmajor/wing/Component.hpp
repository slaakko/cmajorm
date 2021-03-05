// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_COMPONENT_INCLUDED
#define CMAJOR_WING_COMPONENT_INCLUDED
#include <cmajor/wing/WingApi.hpp>
#include <stdint.h>

namespace cmajor { namespace wing {

class Container;

class WING_API Component
{
public:
    Component();
    virtual ~Component();
    virtual bool IsControl() const { return false; }
    virtual bool IsMenuItem() const { return false; }
    virtual bool IsMenuItemBase() const { return false; }
    virtual bool IsMenuControl() const { return false; }
    virtual bool IsTreeViewNode() const { return false; }
    Container* GetContainer() const { return container; }
    void SetContainer(Container* container_) { container = container_; }
    Component* NextSibling() const { return nextSibling; }
    void SetNextSibling(Component* nextSibling_) { nextSibling = nextSibling_; }
    Component* PrevSibling() const { return prevSibling; }
    void SetPrevSibling(Component* prevSibling_) { prevSibling = prevSibling_; }
    void LinkBefore(Component* component) 
    {
        component->prevSibling = prevSibling;
        component->nextSibling = this;
        prevSibling = component;
    }
    void LinkAfter(Component* component)
    {
        component->prevSibling = this;
        component->nextSibling = nextSibling;
        nextSibling = component;
    }
    void Unlink()
    {
        if (prevSibling)
        {
            prevSibling->nextSibling = nextSibling;
        }
        if (nextSibling)
        { 
            nextSibling->prevSibling = prevSibling;
        }
    }
private:
    Container* container;
    Component* nextSibling;
    Component* prevSibling;
};

} } // cmajor::wing

#endif // CMAJOR_WING_COMPONENT_INCLUDED
