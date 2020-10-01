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

DEBUG_API uint64_t ParseHex(const std::string& str);
DEBUG_API std::string ContainerName(ContainerClassTemplateKind containerKind);

class Container;
class Debugger;
class DIType;

class DEBUG_API Pointer
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
    std::string ValueMemberStr(const std::string& name);
private:
    Container& container;
    std::string expression;
    bool hasAddress;
    uint64_t address;
};

class DEBUG_API Iterator
{
public:
    Iterator(Container& container_, int64_t index_);
    virtual ~Iterator();
    Container& GetContainer() { return container; }
    int64_t Index() const { return index; }
    bool IsEnd() const { return index == -1; }
    bool Equals(Iterator* that) const { return index == that->index; }
    virtual std::unique_ptr<JsonValue> Current() = 0;
    virtual std::string CurrentStr() = 0;
    virtual Iterator* Next() = 0;
private:
    Container& container;
    int64_t index;
};

class DEBUG_API ForwardListIterator : public Iterator
{
public:
    ForwardListIterator(Container& container, int64_t index, Pointer* nodePtr_);
    std::unique_ptr<JsonValue> Current() override;
    std::string CurrentStr() override;
    Iterator* Next() override;
private:
    Pointer* nodePtr;
};

class DEBUG_API LinkedListIterator : public Iterator
{
public:
    LinkedListIterator(Container& container, int64_t index, Pointer* nodePtr_);
    std::unique_ptr<JsonValue> Current() override;
    std::string CurrentStr() override;
    Iterator* Next() override;
private:
    Pointer* nodePtr;
};

class DEBUG_API HashtableIterator : public Iterator
{
public:
    HashtableIterator(Container& container, int64_t index, Pointer* bucketPtr_, int64_t bucketIndex_, int64_t bucketCount_, const std::string& bucketsExpr_);
    std::unique_ptr<JsonValue> Current() override;
    std::string CurrentStr() override;
    Iterator* Next() override;
private:
    Pointer* bucketPtr;
    int64_t bucketIndex;
    int64_t bucketCount;
    std::string bucketsExpr;
};

class DEBUG_API TreeIterator : public Iterator
{
public:
    TreeIterator(Container& container, int64_t index, Pointer* nodePtr_);
    std::unique_ptr<JsonValue> Current() override;
    std::string CurrentStr() override;
    Iterator* Next() override;
private:
    Pointer* nodePtr;
};

class DEBUG_API ListIterator : public Iterator
{
public:
    ListIterator(Container& container, int64_t index, int64_t count, Pointer* itemsPtr_);
    std::unique_ptr<JsonValue> Current() override;
    std::string CurrentStr() override;
    Iterator* Next() override;
private:
    Pointer* itemsPtr;
    int64_t count;
};

DEBUG_API uint64_t GetContainerAddress(Debugger& debugger, ContainerClassTemplateKind containerKind, const std::string& containerVarExpr);
DEBUG_API Container* CreateContainer(Debugger& debugger, ContainerClassTemplateKind containerKind, uint64_t address);

class DEBUG_API Container
{
public:
    Container(ContainerClassTemplateKind kind_, Debugger& debugger_, uint64_t address_);
    virtual ~Container();
    Container(const Container&) = delete;
    Container(Container&&) = delete;
    Container& operator=(const Container&) = delete;
    Container& operator=(Container&&) = delete;
    virtual void ClearBrowsingData();
    virtual int64_t Count(const std::string& expr);
    virtual Iterator* Begin(const std::string& containerVarExpr) = 0;
    virtual Iterator* End(const std::string& containerVarExpr) = 0;
    virtual std::unique_ptr<JsonValue> Subscript(const std::string& containerVarExpr, int64_t index) = 0;
    virtual std::string SubscriptExpressionString(const std::string& containerVarExpr, int64_t index) = 0;
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

class DEBUG_API ForwardContainer : public Container
{
public:
    ForwardContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address);
    std::unique_ptr<JsonValue> Subscript(const std::string& containerVarExpr, int64_t index) override;
    std::string SubscriptExpressionString(const std::string& containerVarExpr, int64_t index) override;
};

class DEBUG_API ListContainer : public ForwardContainer
{
public:
    ListContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address);
    int64_t Count(const std::string& containerVarExpr) override;
};

class DEBUG_API HashtableContainer : public ForwardContainer
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

class DEBUG_API TreeContainer : public ForwardContainer
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

class DEBUG_API ForwardList : public ForwardContainer
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

class DEBUG_API LinkedList : public ListContainer
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

class DEBUG_API HashMap : public HashtableContainer
{
public:
    HashMap(Debugger& debugger, uint64_t address);
};

class DEBUG_API HashSet : public HashtableContainer
{
public:
    HashSet(Debugger& debugger, uint64_t address);
};

class DEBUG_API List : public ListContainer
{
public:
    List(Debugger& debugger, uint64_t address);
    void ClearBrowsingData() override;
    Iterator* Begin(const std::string& containerVarExpr) override;
    Iterator* End(const std::string& containerVarExpr) override;
    std::unique_ptr<JsonValue> Subscript(const std::string& containerVarExpr, int64_t index) override;
    std::string SubscriptExpressionString(const std::string& containerVarExpr, int64_t index) override;
private:
    std::unique_ptr<Pointer> itemsPtr;
};

class DEBUG_API ListRepContainer : public Container
{
public:
    ListRepContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address);
    int64_t Count(const std::string& containerVarExpr) override;
    Iterator* Begin(const std::string& containerVarExpr) override;
    Iterator* End(const std::string& containerVarExpr) override;
    std::unique_ptr<JsonValue> Subscript(const std::string& containerVarExpr, int64_t index) override;
    std::string SubscriptExpressionString(const std::string& containerVarExpr, int64_t index) override;
private:
    List rep;
};

class DEBUG_API Map : public TreeContainer
{
public:
    Map(Debugger& debugger, uint64_t address);
};

class DEBUG_API Queue : public ListRepContainer
{
public:
    Queue(Debugger& debugger, uint64_t address);
};

class DEBUG_API Set : public TreeContainer
{
public:
    Set(Debugger& debugger, uint64_t address);
};

class DEBUG_API Stack : public ListRepContainer
{
public:
    Stack(Debugger& debugger, uint64_t address);
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_CONTAINER_INCLUDED
