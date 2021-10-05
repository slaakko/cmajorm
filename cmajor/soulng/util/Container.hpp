// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_CONTAINER_INCLUDED
#define SOULNG_UTIL_CONTAINER_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <memory>

namespace soulng { namespace util {

class Component;

class UTIL_API Container
{
public:
    Container(Component* parent_);
    ~Container();
    bool IsEmpty() const { return firstChild == nullptr; }
    Component* Parent() const { return parent; }
    Component* FirstChild() const { return firstChild; }
    Component* LastChild() const { return lastChild; }
    void AddChild(Component* child);
    std::unique_ptr<Component> RemoveChild(Component* child);
    void InsertBefore(Component* child, Component* before);
    void InsertAfter(Component* child, Component* after);
private:
    Component* parent;
    Component* firstChild;
    Component* lastChild;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_CONTAINER_INCLUDED

