// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/InitDone.hpp>
#include <sngxml/serialization/XmlClassRegistry.hpp>

namespace sngxml { namespace xmlser {

void Init()
{
    XmlClassRegistryInit();
}

void Done()
{
    XmlClassRegistryDone();
}

} } // namespace sngxml::xmlser
