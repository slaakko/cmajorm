// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlContainer.hpp>
#include <sngxml/serialization/XmlSerializable.hpp>
#include <sngxml/serialization/XmlBundle.hpp>
#include <sngxml/serialization/XmlPtr.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace sngxml { namespace xmlser {

XmlContainer::XmlContainer() 
{
}

XmlContainer::~XmlContainer()
{
}

void XmlContainer::Clear()
{
    idMap.clear();
}

void XmlContainer::Add(XmlSerializable* serializable)
{
    const boost::uuids::uuid& objectId = serializable->ObjectId();
    XmlSerializable* prev = Get(objectId);
    if (prev)
    {
        throw std::runtime_error("object '" + boost::uuids::to_string(objectId) + "' already added to XML container");
    }
    idMap[objectId] = serializable;
    serializable->SetContainer(this);
}

void XmlContainer::AddOrReplace(XmlSerializable* serializable)
{
    Remove(serializable->ObjectId());
    Add(serializable);
}

void XmlContainer::Remove(const boost::uuids::uuid& objectId)
{
    idMap.erase(objectId);
}

XmlSerializable* XmlContainer::Get(const boost::uuids::uuid& objectId) const
{
    auto it = idMap.find(objectId);
    if (it != idMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void XmlContainer::SetRootObjectId(const boost::uuids::uuid& objectId)
{
}

void XmlContainer::AddToBundle(XmlBundle& bundle, XmlBundleKind kind, int hops, XmlSerializable* object, std::unordered_set<boost::uuids::uuid>& addedSet)
{
    if (object->ObjectId().is_nil()) return;
    if (addedSet.find(object->ObjectId()) != addedSet.cend()) return;
    bundle.Add(object);
    addedSet.insert(object->ObjectId());
    if (kind == XmlBundleKind::deep && (hops == -1 || hops > 0))
    {
        std::vector<XmlPtrBase*> ptrs = object->GetPtrs();
        for (XmlPtrBase* ptr : ptrs)
        {
            if (!ptr->TargetObjectId().is_nil())
            {
                XmlSerializable* targetObject = Get(ptr->TargetObjectId());
                if (targetObject)
                {
                    int nextHops = -1;
                    if (hops > 0)
                    {
                        nextHops = hops - 1;
                    }
                    AddToBundle(bundle, kind, nextHops, targetObject, addedSet);
                }
                else
                {
                    XmlSerializable* s = ptr->GetPtr();
                    if (s)
                    {
                        XmlContainer* container = s->Container();
                        if (container)
                        {
                            targetObject = container->Get(ptr->TargetObjectId());
                            if (targetObject)
                            {
                                int nextHops = -1;
                                if (hops > 0)
                                {
                                    nextHops = hops - 1;
                                }
                                container->AddToBundle(bundle, kind, nextHops, targetObject, addedSet);
                            }
                        }
                    }
                }
            }
        }
    }
}

std::unique_ptr<XmlBundle> XmlContainer::CreateBundle(XmlSerializable* object)
{
    return CreateBundle(object, XmlBundleKind::shallow);
}

std::unique_ptr<XmlBundle> XmlContainer::CreateBundle(XmlSerializable* object, XmlBundleKind kind)
{
    return CreateBundle(object, kind, -1);
}

std::unique_ptr<XmlBundle> XmlContainer::CreateBundle(XmlSerializable* object, XmlBundleKind kind, int hops)
{
    std::unique_ptr<XmlBundle> bundle(new XmlBundle());
    std::unordered_set<boost::uuids::uuid> addedSet;
    bundle->SetRootObjectId(object->ObjectId());
    AddToBundle(*bundle, kind, hops, object, addedSet);
    return bundle;
}

} } // namespace sngxml::xmlser
