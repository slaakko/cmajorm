// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_CONTAINER_INCLUDED
#define SNGXML_XML_CONTAINER_INCLUDED
#include <sngxml/serialization/XmlSerApi.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_hash.hpp>
#include <unordered_map>
#include <unordered_set>

namespace sngxml { namespace xmlser {

class XmlSerializable;
class XmlBundle;

class SNGXML_SERIALIZATION_API XmlContainer
{
public:
    XmlContainer();
    virtual ~XmlContainer();
    int Count() const { return idMap.size(); }
    bool IsEmpty() const { return idMap.empty(); }
    void Add(XmlSerializable* serializable);
    void AddOrReplace(XmlSerializable* serializable);
    void Remove(const boost::uuids::uuid& objectId);
    XmlSerializable* Get(const boost::uuids::uuid& objectId) const;
    virtual void SetRootObjectId(const boost::uuids::uuid& objectId);
    const std::unordered_map<boost::uuids::uuid, XmlSerializable*>& IdMap() const { return idMap; }
    void AddToBundle(XmlBundle& bundle, XmlSerializable* object, std::unordered_set<boost::uuids::uuid>& addedSet);
    std::unique_ptr<XmlBundle> CreateBundle(XmlSerializable* object);
private:
    std::unordered_map<boost::uuids::uuid, XmlSerializable*> idMap;
};

} } // namespace sngxml::xmlser

#endif // SNGXML_XML_CONTAINER_INCLUDED
