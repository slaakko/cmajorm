// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_CONTAINER_INCLUDED
#define CMAJOR_DEBUG_CONTAINER_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <soulng/util/Json.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdint.h>

namespace cmajor { namespace debug {

using namespace soulng::util;

enum class ContainerClassTemplateKind : int8_t
{
    notContainerClassTemplate = 0,
    forwardList = 1,
    hashMap = 2,
    hashSet = 3,
    linkedList = 4,
    list = 5,
    map = 6,
    queue = 7,
    set = 8,
    stack = 9
};

std::string ContainerName(ContainerClassTemplateKind containerKind);

class Container;
class Debugger;
class DIType;

class Pointer
{
public:
    Pointer(Container& container_);
    Pointer(Container& container_, const std::string& expression_);
    bool IsNull();
    const std::string& Expression() const { return expression; }
    uint64_t Address();
    DIType* Type();
    std::unique_ptr<JsonValue> Value();
    std::unique_ptr<JsonValue> Deref();
    Pointer* PtrMember(const std::string& name);
    std::unique_ptr<JsonValue> ValueMember(const std::string& name);
private:
    Container& container;
    std::string expression;
    bool hasAddress;
    uint64_t address;
};

class Iterator
{
public:
    Iterator(Container& container_, int64_t index_);
    virtual ~Iterator();
    Container& GetContainer() { return container; }
    int64_t Index() const { return index; }
    bool IsEnd() const { return index == -1; }
    bool Equals(Iterator* that) const { return index == that->index; }
    virtual std::unique_ptr<JsonValue> Current() = 0;
    virtual Iterator* Next() = 0;
private:
    Container& container;
    int64_t index;
};

class ForwardListIterator : public Iterator
{
public:
    ForwardListIterator(Container& container, int64_t index, Pointer* nodePtr_);
    std::unique_ptr<JsonValue> Current() override;
    Iterator* Next() override;
private:
    Pointer* nodePtr;
};

class LinkedListIterator : public Iterator
{
public:
    LinkedListIterator(Container& container, int64_t index, Pointer* nodePtr_);
    std::unique_ptr<JsonValue> Current() override;
    Iterator* Next() override;
private:
    Pointer* nodePtr;
};

class HashtableIterator : public Iterator
{
public:
    HashtableIterator(Container& container, int64_t index, Pointer* bucketPtr_, int64_t bucketIndex_, int64_t bucketCount_, const std::string& bucketsExpr_);
    std::unique_ptr<JsonValue> Current() override;
    Iterator* Next() override;
private:
    Pointer* bucketPtr;
    int64_t bucketIndex;
    int64_t bucketCount;
    std::string bucketsExpr;
};

class TreeIterator : public Iterator
{
public:
    TreeIterator(Container& container, int64_t index, Pointer* nodePtr_);
    std::unique_ptr<JsonValue> Current() override;
    Iterator* Next() override;
private:
    Pointer* nodePtr;
};

class ListIterator : public Iterator
{
public:
    ListIterator(Container& container, int64_t index, Pointer* itemsPtr_);
    std::unique_ptr<JsonValue> Current() override;
    Iterator* Next() override;
private:
    Pointer* itemsPtr;
};

uint64_t GetContainerAddress(Debugger& debugger, ContainerClassTemplateKind containerKind, const std::string& containerVarExpr);
Container* CreateContainer(Debugger& debugger, ContainerClassTemplateKind containerKind, uint64_t address);

class Container
{
public:
    Container(ContainerClassTemplateKind kind_, Debugger& debugger_, uint64_t address_);
    virtual ~Container();
    virtual void ClearBrowsingData();
    virtual int64_t Count(const std::string& expr);
    virtual Iterator* Begin(const std::string& containerVarExpr) = 0;
    virtual Iterator* End(const std::string& containerVarExpr) = 0;
    virtual std::unique_ptr<JsonValue> Subscript(const std::string& containerVarExpr, int64_t index) = 0;
    std::unique_ptr<JsonValue> Range(const std::string& containerVarExpr, int64_t rangeStart, int64_t rangeEnd);
    ContainerClassTemplateKind GetKind() const { return kind; }
    uint64_t Address() const { return address; }
    Pointer* GetPointer(uint64_t address) const;
    void AddPointer(Pointer* ptr);
    Debugger& GetDebugger() { return debugger; }
    Iterator* GetIterator(int64_t index) const;
    void AddIterator(Iterator* iterator);
    int64_t GetCount() { return count; }
    void SetCount(int64_t count_) { count = count_; }
    DIType* ValueNodePtrType() const { return valueNodePtrType; }
    void SetValueNodePtrType(DIType* type) { valueNodePtrType = type; }
    Pointer* EndPtr() const { return endPtr; }
    void SetEndPtr(Pointer* endPtr_) { endPtr = endPtr_; }
private:
    ContainerClassTemplateKind kind;
    Debugger& debugger;
    uint64_t address;
    int64_t count;
    DIType* valueNodePtrType;
    Pointer* endPtr;
    std::unordered_map<uint64_t, Pointer*> pointerMap;
    std::vector<std::unique_ptr<Pointer>> pointers;
    std::unordered_map<int64_t, Iterator*> iteratorMap;
    std::vector<std::unique_ptr<Iterator>> iterators;
};

class ForwardContainer : public Container
{
public:
    ForwardContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address);
    std::unique_ptr<JsonValue> Subscript(const std::string& containerVarExpr, int64_t index) override;
};

class ListContainer : public ForwardContainer
{
public:
    ListContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address);
    int64_t Count(const std::string& containerVarExpr) override;
};

class HashtableContainer : public ForwardContainer
{
public:
    HashtableContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address);
    void ClearBrowsingData() override;
    void Init();
    int64_t Count(const std::string& containerVarExpr) override;
    Iterator* Begin(const std::string& containerVarExpr) override;
    Iterator* End(const std::string& containerVarExpr) override;
private:
    std::unique_ptr<Pointer> nullBucketPtr;
    int64_t bucketCount;
};

class TreeContainer : public ForwardContainer
{
public:
    TreeContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address);
    void ClearBrowsingData() override;
    void Init(const std::string& containerVarExpr);
    int64_t Count(const std::string& containerVarExpr) override;
    Iterator* Begin(const std::string& containerVarExpr) override;
    Iterator* End(const std::string& containerVarExpr) override;
private:
    std::unique_ptr<Pointer> headerPtr;
};

class ForwardList : public ForwardContainer
{
public:
    ForwardList(Debugger& debugger, uint64_t address);
    void ClearBrowsingData() override;
    void Init(const std::string& containerVarExpr);
    int64_t Count(const std::string& containerVarExpr) override { return -1; }
    Iterator* Begin(const std::string& containerVarExpr) override;
    Iterator* End(const std::string& containerVarExpr) override;
private:
    std::unique_ptr<Pointer> headPtr;
    std::unique_ptr<Pointer> nullPtr;
};

class LinkedList : public ListContainer
{
public:
    LinkedList(Debugger& debugger, uint64_t address);
    void ClearBrowsingData() override;
    void Init(const std::string& containerVarExpr);
    Iterator* Begin(const std::string& containerVarExpr) override;
    Iterator* End(const std::string& containerVarExpr) override;
private:
    std::unique_ptr<Pointer> headPtr;
    std::unique_ptr<Pointer> nullPtr;
};

class HashMap : public HashtableContainer
{
public:
    HashMap(Debugger& debugger, uint64_t address);
};

class HashSet : public HashtableContainer
{
public:
    HashSet(Debugger& debugger, uint64_t address);
};

class List : public ListContainer
{
public:
    List(Debugger& debugger, uint64_t address);
    void ClearBrowsingData() override;
    Iterator* Begin(const std::string& containerVarExpr) override;
    Iterator* End(const std::string& containerVarExpr) override;
    std::unique_ptr<JsonValue> Subscript(const std::string& containerVarExpr, int64_t index) override;
private:
    std::unique_ptr<Pointer> itemsPtr;
};

class ListRepContainer : public Container
{
public:
    ListRepContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address);
    int64_t Count(const std::string& containerVarExpr) override;
    Iterator* Begin(const std::string& containerVarExpr) override;
    Iterator* End(const std::string& containerVarExpr) override;
    std::unique_ptr<JsonValue> Subscript(const std::string& containerVarExpr, int64_t index) override;
private:
    List rep;
};

class Map : public TreeContainer
{
public:
    Map(Debugger& debugger, uint64_t address);
};

class Queue : public ListRepContainer
{
public:
    Queue(Debugger& debugger, uint64_t address);
};

class Set : public TreeContainer
{
public:
    Set(Debugger& debugger, uint64_t address);
};

class Stack : public ListRepContainer
{
public:
    Stack(Debugger& debugger, uint64_t address);
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_CONTAINER_INCLUDED
