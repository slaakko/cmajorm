// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlClassRegistry.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngxml { namespace xmlser {

using namespace soulng::unicode;

XmlSerializableExtractor::~XmlSerializableExtractor()
{
}

std::unique_ptr<XmlClassRegistry> XmlClassRegistry::instance;

void XmlClassRegistry::Init()
{
    instance.reset(new XmlClassRegistry());
}

void XmlClassRegistry::Done()
{
    instance.reset();
}

XmlClassRegistry::XmlClassRegistry()
{
}

void XmlClassRegistry::Register(int classId, FactoryFunction factoryFunction, sngxml::xmlser::XmlSerializableExtractor* extractor)
{
    factoryMap[classId] = factoryFunction;
    if (extractor)
    {
        extractorMap[classId] = extractor;
        extractors.push_back(std::unique_ptr<sngxml::xmlser::XmlSerializableExtractor>(extractor));
    }
}

XmlSerializable* XmlClassRegistry::CreateXmlSerializable(int classId) const
{
    auto it = extractorMap.find(classId);
    if (it != extractorMap.cend())
    {
        XmlSerializableExtractor* extractor = it->second;
        void* object = Create(classId);
        return extractor->ExtractXmlSerializable(object);
    }
    else
    {
        throw std::runtime_error("XmlClassRegistry: interface extractor for class id " + ToString(classId) + " not registered");
    }
}

void* XmlClassRegistry::Create(int classId) const
{
    auto it = factoryMap.find(classId);
    if (it != factoryMap.cend())
    {
        FactoryFunction factoryFunction = it->second;
        return factoryFunction();
    }
    else
    {
        throw std::runtime_error("XmlClassRegistry: class having id " + ToString(classId) + " not registered");
    }
}

void XmlClassRegistryInit()
{
    XmlClassRegistry::Init();
}

void XmlClassRegistryDone()
{
    XmlClassRegistry::Done();
}

} } // namespace sngxml::xmlser
