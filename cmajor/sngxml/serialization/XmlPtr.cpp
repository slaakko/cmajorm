// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlPtr.hpp>
#include <sngxml/serialization/XmlContainer.hpp>
#include <boost/uuid/nil_generator.hpp>

namespace sngxml { namespace xmlser {

XmlPtrBase::XmlPtrBase() : targetObjectId(boost::uuids::nil_uuid())
{
}

XmlPtrBase::~XmlPtrBase()
{
}

void XmlPtrBase::SetTargetObjectId(const boost::uuids::uuid& targetObjectId_)
{
    targetObjectId = targetObjectId_;
}

void XmlPtrBase::Resolve(XmlContainer* container)
{
    XmlSerializable* serializable = container->Get(targetObjectId);
    if (serializable)
    {
        SetPtr(serializable);
    }
}

} } // namespace sngxml::xmlser
