// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_COMPONENT_INCLUDED
#define SOULNG_UTIL_COMPONENT_INCLUDED
#include <soulng/util/UtilApi.hpp>

namespace soulng { namespace util {

class Container;

class UTIL_API Component
{
public:
    Component();
    virtual ~Component();
    Container* GetContainer() const { return container; }
    void SetContainer(Container* container_) { container = container_; }
    Component* NextSibling() const { return nextSibling; }
    void SetNextSibling(Component* nextSibling_) { nextSibling = nextSibling_; }
    Component* PrevSibling() const { return prevSibling; }
    void SetPrevSibling(Component* prevSibling_) { prevSibling = prevSibling_; }
    void LinkBefore(Component* component)
    {
        if (prevSibling)
        {
            prevSibling->nextSibling = component;
        }
        component->prevSibling = prevSibling;
        component->nextSibling = this;
        prevSibling = component;
    }
    void LinkAfter(Component* component)
    {
        if (nextSibling)
        {
            nextSibling->prevSibling = component;
        }
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

} } // namespace soulng::util

#endif // SOULNG_UTIL_COMPONENT_INCLUDED
