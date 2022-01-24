// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_INTRUSIVE_LIST_INCLUDED
#define SOULNG_UTIL_INTRUSIVE_LIST_INCLUDED
#include <soulng/util/UtilApi.hpp>

namespace soulng::util {

template<typename T>
class IntrusiveListNode
{
public:
    using NodePtr = IntrusiveListNode<T>*;
    IntrusiveListNode(T* ptr_) : ptr(ptr_), parent(nullptr), nextSibling(nullptr), prevSibling(nullptr), firstChild(nullptr), lastChild(nullptr) {}
    T* NextSibling() const { if (nextSibling) return nextSibling->Get(); else return nullptr; }
    void SetNextSibling(NodePtr nextSibling_) { nextSibling = nextSibling_; }
    T* PrevSibling() const { if (prevSibling) return prevSibling->Get(); else return nullptr; }
    void SetPrevSibling(NodePtr prevSibling_) { prevSibling = prevSibling_; }
    void LinkBefore(NodePtr node)
    {
        if (prevSibling)
        {
            prevSibling->nextSibling = node;
        }
        node->prevSibling = prevSibling;
        node->nextSibling = this;
        prevSibling = node;
    }
    void LinkAfter(NodePtr node)
    {
        if (nextSibling)
        {
            nextSibling->prevSibling = node;
        }
        node->prevSibling = this;
        node->nextSibling = nextSibling;
        nextSibling = node;
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
    T* Parent() const { if (parent) return parent->Get(); else return nullptr; }
    void SetParent(NodePtr parent_) { parent = parent_; }
    T* FirstChild() const { if (firstChild) return firstChild->Get(); else return nullptr; }
    T* LastChild() const { if (lastChild) return lastChild->Get(); else return nullptr; }
    void AddChild(NodePtr child)
    {
        if (lastChild)
        {
            lastChild->LinkAfter(child);
        }
        if (!firstChild)
        {
            firstChild = child;
        }
        child->SetParent(this);
    }
    NodePtr RemoveChild(NodePtr child)
    {
        child->Unlink();
        if (child == firstChild)
        {
            firstChild = child->NextSibling();
        }
        if (child == lastChild)
        {
            lastChild = child->PrevSibling();
        }
        child->SetParent(nullptr);
        child->SetNextSibling(nullptr);
        child->SetPrevSibling(nullptr);
        return child;
    }
    void InsertBefore(NodePtr child, NodePtr before)
    {
        if (!before)
        {
            AddChild(child);
        }
        else
        {
            child->SetParent(this);
            if (firstChild == before)
            {
                firstChild = child;
            }
            before->LinkBefore(child);
        }
    }
    void InsertAfter(NodePtr child, NodePtr after)
    {
        if (!after)
        {
            AddChild(child);
        }
        else
        {
            child->SetParent(this);
            after->LinkAfter(child);
            if (after == lastChild)
            {
                lastChild = child;
            }
        }
    }
    T* Get() const
    {
        return ptr;
    }
private:
    T* ptr;
    NodePtr parent;
    NodePtr nextSibling;
    NodePtr prevSibling;
    NodePtr firstChild;
    NodePtr lastChild;
};

} // namespace soulng::util

#endif // SOULNG_UTIL_INTRUSIVE_LIST_INCLUDED
