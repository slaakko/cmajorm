// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlSerializable.hpp>
#include <sngxml/serialization/XmlContainer.hpp>
#include <boost/uuid/random_generator.hpp>

namespace sngxml { namespace xmlser {

XmlSerializable::~XmlSerializable()
{
}

void XmlSerializable::SetRandomObjectId()
{
    if (ObjectId().is_nil())
    {
        SetObjectId(boost::uuids::random_generator()());
    }
}

} } // namespace sngxml::xmlser
