// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_XML_SERIALIZATION_CONTEXT_INCLUDED
#define SNGXML_XML_XML_SERIALIZATION_CONTEXT_INCLUDED
#include <sngxml/serialization/XmlSerApi.hpp>

namespace sngxml { namespace xmlser {

enum class XmlSerializationFlags : int
{
    none = 0, suppressMetadata = 1 << 0
};

SNGXML_SERIALIZATION_API inline XmlSerializationFlags operator|(XmlSerializationFlags left, XmlSerializationFlags right)
{
    return XmlSerializationFlags(int(left) | int(right));
}

SNGXML_SERIALIZATION_API inline XmlSerializationFlags operator&(XmlSerializationFlags left, XmlSerializationFlags right)
{
    return XmlSerializationFlags(int(left) & int(right));
}

SNGXML_SERIALIZATION_API inline XmlSerializationFlags operator~(XmlSerializationFlags flag)
{
    return XmlSerializationFlags(~int(flag));
}

class SNGXML_SERIALIZATION_API XmlSerializationContext
{
public:
    XmlSerializationContext();
    bool GetFlag(XmlSerializationFlags flag) { return (flags & flag) != XmlSerializationFlags::none; }
    void SetFlag(XmlSerializationFlags flag) { flags = flags | flag; }
    void ResetFlag(XmlSerializationFlags flag) { flags = flags & ~flag; }
private:
    XmlSerializationFlags flags;
};

} } // namespace sngxml::xmlser

#endif // SNGXML_XML_XML_SERIALIZATION_CONTEXT_INCLUDED
