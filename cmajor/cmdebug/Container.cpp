// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/Container.hpp>
#include <cmajor/cmdebug/Debugger.hpp>
#include <cmajor/cmdebug/EvaluationGuard.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>

namespace cmajor { namespace debug {

using namespace soulng::unicode;

uint64_t ParseHex(const std::string& str)
{
    std::stringstream s(str);
    uint64_t value = 0;
    s >> std::hex >> value;
    return value;
}

std::string ContainerName(ContainerClassTemplateKind containerKind)
{
    switch (containerKind)
    {
        case ContainerClassTemplateKind::forwardList: return "forwardList";
        case ContainerClassTemplateKind::hashMap: return "hashMap";
        case ContainerClassTemplateKind::hashSet: return "hashSet";
        case ContainerClassTemplateKind::linkedList: return "linkedList";
        case ContainerClassTemplateKind::list: return "list";
        case ContainerClassTemplateKind::map: return "map";
        case ContainerClassTemplateKind::queue: return "queue";
        case ContainerClassTemplateKind::set: return "set";
        case ContainerClassTemplateKind::stack: return "stack";
    }
    return std::string();
}

Pointer::Pointer(Container& container_, const std::string& expression_) : container(container_), expression(expression_), address(0), hasAddress(false)
{
}

Pointer::Pointer(Container& container_) : container(container_), address(0), hasAddress(true)
{
}

bool Pointer::IsNull()
{
    return Address() == 0;
}

DIType* Pointer::Type()
{
    return container.GetDebugger().GetType(expression);
}

std::unique_ptr<JsonValue> Pointer::Value()
{
    if (hasAddress && address == 0)
    {
        return std::unique_ptr<JsonValue>(new JsonNull());
    }
    std::unique_ptr<JsonValue> value;
    Debugger& debugger = container.GetDebugger();
    EvaluationGuard guard(debugger);
    debugger.Evaluate(expression);
    value.reset(debugger.ReleaseResult());
    return value;
}

std::unique_ptr<JsonValue> Pointer::Deref()
{
    if (hasAddress && address == 0)
    {
        throw std::runtime_error(expression + ": dereferencing null pointer");
    }
    std::unique_ptr<JsonValue> value;
    Debugger& debugger = container.GetDebugger();
    EvaluationGuard guard(debugger);
    debugger.Evaluate("*(" + expression + ")");
    value.reset(debugger.ReleaseResult());
    return value;
}

uint64_t Pointer::Address()
{
    if (hasAddress)
    {
        return address;
    }
    std::unique_ptr<JsonValue> value = Value();
    if (value->Type() == JsonValueType::object)
    {
        JsonObject* jsonObject = static_cast<JsonObject*>(value.get());
        JsonValue* successValue = jsonObject->GetField(U"success");
        bool success = false;
        if (successValue && successValue->Type() == JsonValueType::boolean)
        {
            success = static_cast<JsonBool*>(successValue)->Value();
        }
        if (success)
        {
            JsonValue* addrValue = jsonObject->GetField(U"value");
            if (addrValue && addrValue->Type() == JsonValueType::string)
            {
                JsonString* addrStr = static_cast<JsonString*>(addrValue);
                std::string addressStr = ToUtf8(addrStr->Value());
                address = ParseHex(addressStr);
                hasAddress = true;
                return address;
            }
        }
    }
    hasAddress = true;
    address = 0;
    return address;
}

Pointer* Pointer::PtrMember(const std::string& name)
{
    std::unique_ptr<Pointer> memberPtr(new Pointer(container, "(*" + expression + ")." + name));
    uint64_t address = memberPtr->Address();
    Pointer* prevPtr = container.GetPointer(address);
    if (prevPtr)
    {
        if (!prevPtr->Expression().empty() && prevPtr->Expression().length() < memberPtr->Expression().length())
        {
            return prevPtr;
        }
    }
    container.AddPointer(memberPtr.get());
    return memberPtr.release();
}

std::unique_ptr<JsonValue> Pointer::ValueMember(const std::string& name)
{
    std::unique_ptr<Pointer> memberPtr(new Pointer(container, "(*" + expression + ")." + name));
    return memberPtr->Value();
}

std::string Pointer::ValueMemberStr(const std::string& name)
{
    std::unique_ptr<Pointer> memberPtr(new Pointer(container, "(*" + expression + ")." + name));
    return memberPtr->Expression();
}

Iterator::Iterator(Container& container_, int64_t index_) : container(container_), index(index_)
{
}

Iterator::~Iterator()
{
}

uint64_t GetContainerAddress(Debugger& debugger, ContainerClassTemplateKind containerKind, const std::string& containerVarExpr)
{
    uint64_t address = 0;
    std::string addressExpr;
    switch (containerKind)
    {
        case ContainerClassTemplateKind::forwardList:
        case ContainerClassTemplateKind::linkedList:
        {
            addressExpr = "&(" + containerVarExpr + ").head";
            break;
        }
        case ContainerClassTemplateKind::hashMap:
        case ContainerClassTemplateKind::hashSet:
        {
            addressExpr = "&(" + containerVarExpr + ").table";
            break;
        }
        case ContainerClassTemplateKind::list:
        {
            addressExpr = "&(" + containerVarExpr + ").items";
            break;
        }
        case ContainerClassTemplateKind::set:
        case ContainerClassTemplateKind::map:
        {
            addressExpr = "&(" + containerVarExpr + ").tree";
            break;
        }
        case ContainerClassTemplateKind::queue:
        case ContainerClassTemplateKind::stack:
        {
            addressExpr = "&(" + containerVarExpr + ").items";
            break;
        }
    }
    if (!addressExpr.empty())
    {
        EvaluationGuard evaluationGuard(debugger);
        debugger.Evaluate(addressExpr);
        JsonValue* resultValue = debugger.ReleaseResult();
        if (resultValue && resultValue->Type() == JsonValueType::object)
        {
            JsonObject* resultObject = static_cast<JsonObject*>(resultValue);
            JsonValue* successValue = resultObject->GetField(U"success");
            bool success = false;
            if (successValue && successValue->Type() == JsonValueType::boolean)
            {
                JsonBool* successBool = static_cast<JsonBool*>(successValue);
                success = successBool->Value();
            }
            if (success)
            {
                JsonValue* addressValue = resultObject->GetField(U"value");
                if (addressValue && addressValue->Type() == JsonValueType::string)
                {
                    JsonString* addrStr = static_cast<JsonString*>(addressValue);
                    std::string addressStr = ToUtf8(addrStr->Value());
                    address = ParseHex(addressStr);
                }
            }
        }
    }
    return address;
}

Container* CreateContainer(Debugger& debugger, ContainerClassTemplateKind containerKind, uint64_t address)
{
    switch (containerKind)
    {
        case ContainerClassTemplateKind::forwardList:
        {
            return new ForwardList(debugger, address);
        }
        case ContainerClassTemplateKind::linkedList:
        {
            return new LinkedList(debugger, address);
        }
        case ContainerClassTemplateKind::hashMap:
        {
            return new HashMap(debugger, address);
        }
        case ContainerClassTemplateKind::hashSet:
        {
            return new HashSet(debugger, address);
        }
        case ContainerClassTemplateKind::list:
        {
            return new List(debugger, address);
        }
        case ContainerClassTemplateKind::map:
        {
            return new Map(debugger, address);
        }
        case ContainerClassTemplateKind::queue:
        {
            return new Queue(debugger, address);
        }
        case ContainerClassTemplateKind::set:
        {
            return new Set(debugger, address);
        }
        case ContainerClassTemplateKind::stack :
        {
            return new Stack(debugger, address);
        }
    }
    throw std::runtime_error("unknown container template kind");
}

ForwardListIterator::ForwardListIterator(Container& container, int64_t index, Pointer* nodePtr_) : Iterator(container, index), nodePtr(nodePtr_)
{
}

std::unique_ptr<JsonValue> ForwardListIterator::Current()
{
    return nodePtr->ValueMember("value");
}

std::string ForwardListIterator::CurrentStr()
{
    return nodePtr->ValueMemberStr("value");
}

Iterator* ForwardListIterator::Next()
{
    if (IsEnd()) return this;
    Pointer* nextPtr = nodePtr->PtrMember("next");
    Container& container = GetContainer();
    if (nextPtr->IsNull())
    {
        Iterator* end = container.GetIterator(-1);
        if (end)
        {
            return end;
        }
        Pointer * endPtr = container.EndPtr();
        ForwardListIterator* endIterator = new ForwardListIterator(container, -1, endPtr);
        container.AddIterator(endIterator);
        return endIterator;
    }
    int64_t index = Index() + 1;
    Iterator* next = container.GetIterator(index);
    if (next)
    {
        return next;
    }
    ForwardListIterator* nextIterator = new ForwardListIterator(container, index, nextPtr);
    container.AddIterator(nextIterator);
    return nextIterator;
}

LinkedListIterator::LinkedListIterator(Container& container, int64_t index, Pointer* nodePtr_) : Iterator(container, index), nodePtr(nodePtr_)
{
}

std::unique_ptr<JsonValue> LinkedListIterator::Current()
{
    return nodePtr->ValueMember("value");
}

std::string LinkedListIterator::CurrentStr()
{
    return nodePtr->ValueMemberStr("value");
}

Iterator* LinkedListIterator::Next()
{
    if (IsEnd()) return this;
    Pointer* nextPtr = nodePtr->PtrMember("next");
    Container& container = GetContainer();
    if (nextPtr->IsNull())
    {
        Iterator* end = container.GetIterator(-1);
        if (end)
        {
            return end;
        }
        Pointer* endPtr = container.EndPtr();
        LinkedListIterator* endIterator = new LinkedListIterator(container, -1, endPtr);
        container.AddIterator(endIterator);
        return endIterator;
    }
    int64_t index = Index() + 1;
    Iterator* next = container.GetIterator(index);
    if (next)
    {
        return next;
    }
    LinkedListIterator* nextIterator = new LinkedListIterator(container, index, nextPtr);
    container.AddIterator(nextIterator);
    return nextIterator;
}

HashtableIterator::HashtableIterator(Container& container, int64_t index, Pointer* bucketPtr_, int64_t bucketIndex_, int64_t bucketCount_, const std::string& bucketsExpr_):
    Iterator(container, index), bucketPtr(bucketPtr_), bucketIndex(bucketIndex_), bucketCount(bucketCount_), bucketsExpr(bucketsExpr_)
{
}

std::unique_ptr<JsonValue> HashtableIterator::Current()
{
    return bucketPtr->ValueMember("value");
}

std::string HashtableIterator::CurrentStr()
{
    return bucketPtr->ValueMemberStr("value");
}

Iterator* HashtableIterator::Next()
{
    if (IsEnd()) return this;
    Pointer* prev = bucketPtr;
    Pointer* nextPtr = bucketPtr->PtrMember("next");
    Container& container = GetContainer();
    int64_t index = Index() + 1;
    if (nextPtr->IsNull())
    {
        Iterator* next = container.GetIterator(index);
        if (next)
        {
            return next;
        }
        int64_t nextBucketIndex = bucketIndex + 1;
        std::unique_ptr<Pointer> bptr(new Pointer(container, bucketsExpr + "[" + std::to_string(nextBucketIndex) + "]"));
        while (bptr->IsNull() && nextBucketIndex < bucketCount)
        {
            ++nextBucketIndex;
            bptr.reset(new Pointer(container, bucketsExpr + "[" + std::to_string(nextBucketIndex) + "]"));
        }
        if (bptr->IsNull())
        {
            Iterator* endIterator = container.GetIterator(-1);
            if (endIterator)
            {
                return endIterator;
            }
            HashtableIterator* endIter = new HashtableIterator(container, -1, container.EndPtr(), -1, -1, std::string());
            container.AddIterator(endIter);
            return endIter;
        }
        HashtableIterator* nextIterator = new HashtableIterator(container, index, bptr.get(), nextBucketIndex, bucketCount, bucketsExpr);
        container.AddPointer(bptr.release());
        container.AddIterator(nextIterator);
        return nextIterator;
    }
    else
    {
        HashtableIterator* nextIterator = new HashtableIterator(container, index, nextPtr, bucketIndex, bucketCount, bucketsExpr);
        container.AddIterator(nextIterator);
        return nextIterator;
    }
}

Pointer* TreeMin(Pointer* nodePtr)
{
    Pointer* left = nodePtr->PtrMember("left");
    while (!left->IsNull())
    {
        nodePtr = left;
        left = nodePtr->PtrMember("left");
    }
    return nodePtr;
}

Pointer* TreeNext(Pointer* nodePtr)
{
    Pointer* right = nodePtr->PtrMember("right");
    if (!right->IsNull())
    {
        return TreeMin(right);
    }
    else
    {
        Pointer* parent = nodePtr->PtrMember("parent");
        Pointer* parentRight = parent->PtrMember("right");
        while (nodePtr->Address() == parentRight->Address())
        {
            nodePtr = parent;
            parent = parent->PtrMember("parent");
            parentRight = parent->PtrMember("right");
        }
        right = nodePtr->PtrMember("right");
        if (right->Address() != parent->Address())
        {
            return parent;
        }
        return nodePtr;
    }
}

TreeIterator::TreeIterator(Container& container, int64_t index, Pointer* nodePtr_) : Iterator(container, index), nodePtr(nodePtr_)
{
    if (nodePtr == nullptr)
    {
        throw std::runtime_error("tree iterator with null pointer constructed");
    }
}

std::unique_ptr<JsonValue> TreeIterator::Current() 
{
    Container& container = GetContainer();
    std::string valueNodeTypeIdStr = boost::uuids::to_string(container.ValueNodePtrType()->Id());
    Pointer* valueNodePtr = new Pointer(container, "cast<typeid(\"" + valueNodeTypeIdStr + "\")>(" + nodePtr->Expression() + ")");
    container.AddPointer(valueNodePtr);
    return valueNodePtr->ValueMember("value");
}

std::string TreeIterator::CurrentStr()
{
    Container& container = GetContainer();
    std::string valueNodeTypeIdStr = boost::uuids::to_string(container.ValueNodePtrType()->Id());
    Pointer* valueNodePtr = new Pointer(container, "cast<typeid(\"" + valueNodeTypeIdStr + "\")>(" + nodePtr->Expression() + ")");
    container.AddPointer(valueNodePtr);
    return valueNodePtr->ValueMemberStr("value");
}

Iterator* TreeIterator::Next() 
{
    if (IsEnd()) return this;
    int64_t index = Index() + 1;
    Container& container = GetContainer();
    Iterator* next = container.GetIterator(index);
    if (next)
    {
        return next;
    }
    Pointer* nextPtr = TreeNext(nodePtr);
    if (nextPtr->Address() == container.EndPtr()->Address())
    {
        Iterator* endIterator = container.GetIterator(-1);
        if (endIterator)
        {
            return endIterator;
        }
        TreeIterator* endIter = new TreeIterator(container, -1, container.EndPtr());
        container.AddIterator(endIter);
        return endIter;
    }
    TreeIterator* nextIterator = new TreeIterator(container, index, nextPtr);
    container.AddIterator(nextIterator);
    return nextIterator;
}

ListIterator::ListIterator(Container& container, int64_t index, int64_t count_, Pointer* itemsPtr_) : Iterator(container, index), itemsPtr(itemsPtr_), count(count_)
{
}

std::unique_ptr<JsonValue> ListIterator::Current()
{
    Pointer* itemPtr = new Pointer(GetContainer(), itemsPtr->Expression() + "+" + std::to_string(Index()));
    return itemPtr->Deref();
}

std::string ListIterator::CurrentStr()
{
    Pointer* itemPtr = new Pointer(GetContainer(), itemsPtr->Expression() + "+" + std::to_string(Index()));
    return "(*(" + itemPtr->Expression() + "))";
}

Iterator* ListIterator::Next()
{
    if (IsEnd()) return this;
    int64_t index = Index() + 1;
    Container& container = GetContainer();
    Iterator* next = container.GetIterator(index);
    if (next)
    {
        return next;
    }
    if (index < count)
    {
        ListIterator* listNext = new ListIterator(container, index, count, itemsPtr);
        container.AddIterator(listNext);
        return listNext;
    }
    else
    {
        Iterator* endIterator = container.GetIterator(-1);
        if (endIterator)
        {
            return endIterator;
        }
        ListIterator* endIter = new ListIterator(container, -1, count, itemsPtr);
        container.AddIterator(endIter);
        return endIter;
    }
}

Container::Container(ContainerClassTemplateKind kind_, Debugger& debugger_, uint64_t address_) :
    kind(kind_), debugger(debugger_), address(address_), count(-1), valueNodePtrType(nullptr),
    endPtr(nullptr)
{
}

Container::~Container()
{
}

void Container::ClearBrowsingData()
{
    count = -1;
    pointerMap.clear();
    pointers.clear();
    iteratorMap.clear();
    iterators.clear();
}

int64_t Container::Count(const std::string& expr)
{
    if (count != -1) return count;
    EvaluationGuard evaluationGuard(debugger);
    debugger.Evaluate(expr);
    std::unique_ptr<JsonValue> resultValue(debugger.ReleaseResult());
    if (resultValue && resultValue->Type() == JsonValueType::object)
    {
        JsonObject* resultObject = static_cast<JsonObject*>(resultValue.get());
        JsonValue* successValue = resultObject->GetField(U"success");
        bool success = false;
        if (successValue && successValue->Type() == JsonValueType::boolean)
        {
            JsonBool* successBool = static_cast<JsonBool*>(successValue);
            success = successBool->Value();
        }
        if (success)
        {
            JsonValue* countValue = resultObject->GetField(U"value");
            if (countValue && countValue->Type() == JsonValueType::string)
            {
                JsonString* countString = static_cast<JsonString*>(countValue);
                std::string countStr = ToUtf8(countString->Value());
                count = boost::lexical_cast<int64_t>(countStr);
                return count;
            }
        }
    }
    return -1;
}

std::unique_ptr<JsonValue> Container::Range(const std::string& containerVarExpr, int64_t rangeStart, int64_t rangeEnd)
{
    std::unique_ptr<JsonObject> rangeObject(new JsonObject());
    std::unique_ptr<JsonArray> rangeArray(new JsonArray());
    int64_t count = Count(containerVarExpr);
    if (count != 0)
    {
        std::unique_ptr<JsonValue> start = Subscript(containerVarExpr, rangeStart);
        rangeArray->AddItem(std::move(start));
        Iterator* iterator = GetIterator(rangeStart);
        iterator = iterator->Next();
        while (!iterator->IsEnd() && (rangeEnd == -1 || iterator->Index() < rangeEnd))
        {
            std::unique_ptr<JsonValue> value = iterator->Current();
            rangeArray->AddItem(std::move(value));
            iterator = iterator->Next();
        }
    }
    rangeObject->AddField(U"range", std::move(rangeArray));
    rangeObject->AddField(U"success", std::unique_ptr<JsonValue>(new JsonBool(true)));
    return rangeObject;
}

Pointer* Container::GetPointer(uint64_t address) const
{
    auto it = pointerMap.find(address);
    if (it != pointerMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Container::AddPointer(Pointer* ptr)
{
    if (!ptr->IsNull())
    {
        pointerMap[ptr->Address()] = ptr;
    }
    pointers.push_back(std::unique_ptr<Pointer>(ptr));
}

Iterator* Container::GetIterator(int64_t index) const
{
    auto it = iteratorMap.find(index);
    if (it != iteratorMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Container::AddIterator(Iterator* iterator)
{
    iteratorMap[iterator->Index()] = iterator;
    iterators.push_back(std::unique_ptr<Iterator>(iterator));
}

ForwardContainer::ForwardContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address) :
    Container(kind, debugger, address)
{
}

std::unique_ptr<JsonValue> ForwardContainer::Subscript(const std::string& containerVarExpr, int64_t index)
{
    if (index < 0)
    {
        throw std::runtime_error("invalid subscript expression: " + containerVarExpr + "[" + std::to_string(index) + "]");
    }
    int64_t i = index;
    Iterator* iterator = GetIterator(i);
    while (i > 0 && !iterator)
    {
        --i;
        iterator = GetIterator(i);
    }
    if (!iterator && i == 0)
    {
        iterator = Begin(containerVarExpr);
    }
    if (!iterator || iterator->IsEnd())
    {
        throw std::runtime_error("invalid subscript expression: " + containerVarExpr + "[" + std::to_string(index) + "]");
    }
    while (iterator->Index() < index)
    {
        if (iterator->IsEnd())
        {
            throw std::runtime_error("invalid subscript expression: " + containerVarExpr + "[" + std::to_string(index) + "]");
        }
        iterator = iterator->Next();
    }
    return iterator->Current();
}

std::string ForwardContainer::SubscriptExpressionString(const std::string& containerVarExpr, int64_t index)
{
    if (index < 0)
    {
        throw std::runtime_error("invalid subscript expression: " + containerVarExpr + "[" + std::to_string(index) + "]");
    }
    int64_t i = index;
    Iterator* iterator = GetIterator(i);
    while (i > 0 && !iterator)
    {
        --i;
        iterator = GetIterator(i);
    }
    if (!iterator && i == 0)
    {
        iterator = Begin(containerVarExpr);
    }
    if (!iterator || iterator->IsEnd())
    {
        throw std::runtime_error("invalid subscript expression: " + containerVarExpr + "[" + std::to_string(index) + "]");
    }
    while (iterator->Index() < index)
    {
        if (iterator->IsEnd())
        {
            throw std::runtime_error("invalid subscript expression: " + containerVarExpr + "[" + std::to_string(index) + "]");
        }
        iterator = iterator->Next();
    }
    return iterator->CurrentStr();
}

ListContainer::ListContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address) : ForwardContainer(debugger, kind, address)
{
}

int64_t ListContainer::Count(const std::string& containerVarExpr)
{
    return Container::Count("(" + containerVarExpr + ").count");
}

HashtableContainer::HashtableContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address) :
    ForwardContainer(debugger, kind, address), bucketCount(-1)
{
}

void HashtableContainer::ClearBrowsingData()
{
    ForwardContainer::ClearBrowsingData();
    bucketCount = -1;
}

void HashtableContainer::Init()
{
    if (EndPtr()) return;
    Pointer* nullBucketPtr = new Pointer(*this);
    AddPointer(nullBucketPtr);
    SetEndPtr(nullBucketPtr);
}

int64_t HashtableContainer::Count(const std::string& containerVarExpr)
{
    return Container::Count("(" + containerVarExpr + ").table.count");
}

struct CountGuard
{
    CountGuard(Container& container_) : container(container_), prevCount(container.GetCount())
    {
        container.SetCount(-1);
    }
    ~CountGuard()
    {
        container.SetCount(prevCount);
    }
    Container& container;
    int64_t prevCount;
};

Iterator* HashtableContainer::Begin(const std::string& containerVarExpr)
{
    Init();
    if (Count(containerVarExpr) == 0)
    {
        return End(containerVarExpr);
    }
    Iterator* iterator = GetIterator(0);
    if (iterator)
    {
        return iterator;
    }
    if (bucketCount == -1)
    {
        CountGuard guard(*this);
        bucketCount = Container::Count("(" + containerVarExpr + ").table.buckets.count");
    }
    int64_t bucketIndex = 0;
    std::unique_ptr<Pointer> bucketPtr(new Pointer(*this, containerVarExpr + ".table.buckets.items[" + std::to_string(bucketIndex) + "]"));
    while (bucketIndex < bucketCount && bucketPtr->IsNull())
    {
        ++bucketIndex;
        bucketPtr.reset(new Pointer(*this, containerVarExpr + ".table.buckets.items[" + std::to_string(bucketIndex) + "]"));
    }
    iterator = new HashtableIterator(*this, 0, bucketPtr.get(), bucketIndex, bucketCount, containerVarExpr + ".table.buckets.items");
    AddPointer(bucketPtr.release());
    AddIterator(iterator);
    return iterator;
}

Iterator* HashtableContainer::End(const std::string& containerVarExpr)
{
    Init();
    Iterator* end = GetIterator(-1);
    if (end)
    {
        return end;
    }
    Iterator* endIterator = new HashtableIterator(*this, -1, EndPtr(), -1, -1, std::string());
    AddIterator(endIterator);
    return endIterator;
}

TreeContainer::TreeContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address) : ForwardContainer(debugger, kind, address)
{
}

void TreeContainer::ClearBrowsingData()
{
    ForwardContainer::ClearBrowsingData();
    headerPtr.reset();
}

void TreeContainer::Init(const std::string& containerVarExpr)
{
    if (!headerPtr)
    {
        headerPtr.reset(new Pointer(*this, containerVarExpr + ".tree.header.ptr"));
        SetValueNodePtrType(headerPtr->Type());
        SetEndPtr(headerPtr.get());
    }
}

int64_t TreeContainer::Count(const std::string& containerVarExpr)
{
    return Container::Count("(" + containerVarExpr + ").tree.count");
}

Iterator* TreeContainer::Begin(const std::string& containerVarExpr)
{
    Iterator* iterator = GetIterator(0);
    if (iterator)
    {
        return iterator;
    }
    Init(containerVarExpr);
    if (headerPtr->IsNull())
    {
        return End(containerVarExpr);
    }
    else
    {
        Pointer* leftmost = headerPtr->PtrMember("base.left");
        TreeIterator* beginIterator = new TreeIterator(*this, 0, leftmost);
        AddIterator(beginIterator);
        return beginIterator;
    }
}

Iterator* TreeContainer::End(const std::string& containerVarExpr)
{
    Init(containerVarExpr);
    Iterator* end = GetIterator(-1);
    if (end)
    {
        return end;
    }
    Iterator* endIterator = new TreeIterator(*this, -1, EndPtr());
    AddIterator(endIterator);
    return endIterator;
}

ForwardList::ForwardList(Debugger& debugger, uint64_t address) : ForwardContainer(debugger, ContainerClassTemplateKind::forwardList, address)
{
}

void ForwardList::ClearBrowsingData()
{
    ForwardContainer::ClearBrowsingData();
    headPtr.reset();
}

void ForwardList::Init(const std::string& containerVarExpr)
{
    if (!headPtr)
    {
        headPtr.reset(new Pointer(*this, containerVarExpr + ".head"));
    }
    if (!nullPtr)
    {
        nullPtr.reset(new Pointer(*this));
        SetEndPtr(nullPtr.get());
    }
}

Iterator* ForwardList::Begin(const std::string& containerVarExpr)
{
    Iterator* iterator = GetIterator(0);
    if (iterator)
    {
        return iterator;
    }
    Init(containerVarExpr);
    if (headPtr->IsNull())
    {
        return End(containerVarExpr);
    }
    else
    {
        Iterator* beginIterator = new ForwardListIterator(*this, 0, headPtr.get());
        AddIterator(beginIterator);
        return beginIterator;
    }
}

Iterator* ForwardList::End(const std::string& containerVarExpr)
{
    Init(containerVarExpr);
    Iterator* end = GetIterator(-1);
    if (end)
    {
        return end;
    }
    Iterator* endIterator = new ForwardListIterator(*this, -1, EndPtr());
    AddIterator(endIterator);
    return endIterator;
}

LinkedList::LinkedList(Debugger& debugger, uint64_t address) : ListContainer(debugger, ContainerClassTemplateKind::linkedList, address)
{
}

void LinkedList::ClearBrowsingData()
{
    ListContainer::ClearBrowsingData();
    headPtr.reset();
}

void LinkedList::Init(const std::string& containerVarExpr)
{
    if (!headPtr)
    {
        headPtr.reset(new Pointer(*this, containerVarExpr + ".head"));
    }
    if (!nullPtr)
    {
        nullPtr.reset(new Pointer(*this));
        SetEndPtr(nullPtr.get());
    }
}

Iterator* LinkedList::Begin(const std::string& containerVarExpr)
{
    Iterator* iterator = GetIterator(0);
    if (iterator)
    {
        return iterator;
    }
    Init(containerVarExpr);
    if (headPtr->IsNull())
    {
        return End(containerVarExpr);
    }
    else
    {
        Iterator* beginIterator = new LinkedListIterator(*this, 0, headPtr.get());
        AddIterator(beginIterator);
        return beginIterator;
    }
}

Iterator* LinkedList::End(const std::string& containerVarExpr)
{
    Init(containerVarExpr);
    Iterator* end = GetIterator(-1);
    if (end)
    {
        return end;
    }
    Iterator* endIterator = new ForwardListIterator(*this, -1, EndPtr());
    AddIterator(endIterator);
    return endIterator;
}

HashMap::HashMap(Debugger& debugger, uint64_t address) : HashtableContainer(debugger, ContainerClassTemplateKind::hashMap, address)
{
}

HashSet::HashSet(Debugger& debugger, uint64_t address) : HashtableContainer(debugger, ContainerClassTemplateKind::hashSet, address)
{
}

List::List(Debugger& debugger, uint64_t address) : ListContainer(debugger, ContainerClassTemplateKind::list, address), itemsPtr(nullptr)
{
}

void List::ClearBrowsingData()
{
    ListContainer::ClearBrowsingData();
    itemsPtr.reset();
}

Iterator* List::Begin(const std::string& containerVarExpr)
{
    Iterator* iterator = GetIterator(0);
    if (iterator)
    {
        return iterator;
    }
    if (!itemsPtr)
    {
        itemsPtr.reset(new Pointer(*this, containerVarExpr + ".items"));
    }
    if (itemsPtr->IsNull())
    {
        return End(containerVarExpr);
    }
    else
    {
        int64_t count = Count(containerVarExpr);
        Iterator* beginIterator = new ListIterator(*this, 0, count, itemsPtr.get());
        AddIterator(beginIterator);
        return beginIterator;
    }
}

Iterator* List::End(const std::string& containerVarExpr)
{
    if (!itemsPtr)
    {
        itemsPtr.reset(new Pointer(*this, containerVarExpr + ".items"));
    }
    Iterator* end = GetIterator(-1);
    if (end)
    {
        return end;
    }
    int64_t count = Count(containerVarExpr);
    if (itemsPtr->IsNull())
    {
        Iterator* endIterator = new ListIterator(*this, -1, count, itemsPtr.get());
        AddIterator(endIterator);
        return endIterator;
    }
    else
    {
        Iterator* endIterator = new ListIterator(*this, -1, count, itemsPtr.get());
        AddIterator(endIterator);
        return endIterator;
    }
}

std::unique_ptr<JsonValue> List::Subscript(const std::string& containerVarExpr, int64_t index)
{
    if (index < 0)
    {
        throw std::runtime_error("invalid subscript expression: " + containerVarExpr + "[" + std::to_string(index) + "]");
    }
    Iterator* iterator = GetIterator(index);
    if (iterator)
    {
        return iterator->Current();
    }
    if (!itemsPtr)
    {
        itemsPtr.reset(new Pointer(*this, "(" + containerVarExpr + ").items"));
    }
    int64_t count = Count(containerVarExpr);
    ListIterator* indexIterator = new ListIterator(*this, index, count, itemsPtr.get());
    AddIterator(indexIterator);
    return indexIterator->Current();
}

std::string List::SubscriptExpressionString(const std::string& containerVarExpr, int64_t index)
{
    if (index < 0)
    {
        throw std::runtime_error("invalid subscript expression: " + containerVarExpr + "[" + std::to_string(index) + "]");
    }
    Iterator* iterator = GetIterator(index);
    if (iterator)
    {
        return iterator->CurrentStr();
    }
    if (!itemsPtr)
    {
        itemsPtr.reset(new Pointer(*this, "(" + containerVarExpr + ").items"));
    }
    int64_t count = Count(containerVarExpr);
    ListIterator* indexIterator = new ListIterator(*this, index, count, itemsPtr.get());
    AddIterator(indexIterator);
    return indexIterator->CurrentStr();
}

ListRepContainer::ListRepContainer(Debugger& debugger, ContainerClassTemplateKind kind, uint64_t address) : Container(kind, debugger, address),
    rep(debugger, address)
{
}

int64_t ListRepContainer::Count(const std::string& containerVarExpr)
{
    return rep.Count("(" + containerVarExpr + ").items");
}

Iterator* ListRepContainer::Begin(const std::string& containerVarExpr)
{
    return rep.Begin("(" + containerVarExpr + ").items");
}

Iterator* ListRepContainer::End(const std::string& containerVarExpr)
{
    return rep.End("(" + containerVarExpr + ").items");
}

std::unique_ptr<JsonValue> ListRepContainer::Subscript(const std::string& containerVarExpr, int64_t index)
{
    return rep.Subscript("(" + containerVarExpr + ").items", index);
}

std::string ListRepContainer::SubscriptExpressionString(const std::string& containerVarExpr, int64_t index)
{
    return rep.SubscriptExpressionString("(" + containerVarExpr + ").items", index);
}

Map::Map(Debugger& debugger, uint64_t address) : TreeContainer(debugger, ContainerClassTemplateKind::map, address)
{
}

Queue::Queue(Debugger& debugger, uint64_t address) : ListRepContainer(debugger, ContainerClassTemplateKind::queue, address)
{
}

Set::Set(Debugger& debugger, uint64_t address) : TreeContainer(debugger, ContainerClassTemplateKind::set, address)
{
}

Stack::Stack(Debugger& debugger, uint64_t address) : ListRepContainer(debugger, ContainerClassTemplateKind::stack, address)
{
}

} } // namespace cmajor::debug

