// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_CLASS_REGISTRY_INCLUDED
#define SNGXML_XML_CLASS_REGISTRY_INCLUDED
#include <sngxml/serialization/XmlExport.hpp>
#include <sngxml/dom/Element.hpp>
#include <unordered_map>
#include <vector>
#include <memory>

namespace sngxml { namespace xmlser {

using FactoryFunction = void* (*)();

class XmlSerializable;

struct  SNGXML_SERIALIZATION_API XmlSerializableExtractor
{
    virtual ~XmlSerializableExtractor();
    virtual XmlSerializable* ExtractXmlSerializable(void* object) const = 0;
};

template<class T>
struct ConcreteXmlSerializableExtractor : public XmlSerializableExtractor
{
    XmlSerializable* ExtractXmlSerializable(void* object) const override
    {
        T* t = static_cast<T*>(object);
        if (XmlSerializable* serializable = dynamic_cast<XmlSerializable*>(t))
        {
            return serializable;
        }
        else
        {
            throw std::runtime_error("ConcreteXmlSerializableExtractor: XmlSerializable expected");
        }
    }
};

template<class T>
inline XmlSerializableExtractor* MakeXmlSerializableExtractor()
{
    return new ConcreteXmlSerializableExtractor<T>();
}

class SNGXML_SERIALIZATION_API XmlClassRegistry
{
public:
    XmlClassRegistry(const XmlClassRegistry&) = delete;
    XmlClassRegistry(XmlClassRegistry&&) = delete;
    XmlClassRegistry& operator=(const XmlClassRegistry&) = delete;
    XmlClassRegistry& operator=(XmlClassRegistry&&) = delete;
    static void Init();
    static void Done();
    static XmlClassRegistry& Instance() { return *instance; }
    void Register(int classId, FactoryFunction factoryFunction, sngxml::xmlser::XmlSerializableExtractor* extractor);
    XmlSerializable* CreateXmlSerializable(int classId) const;
    void* Create(int classId) const;
private:
    static std::unique_ptr<XmlClassRegistry> instance;
    std::map<int, FactoryFunction> factoryMap;
    std::map<int, XmlSerializableExtractor*> extractorMap;
    std::vector<std::unique_ptr<XmlSerializableExtractor>> extractors;
    XmlClassRegistry();
};

SNGXML_SERIALIZATION_API void XmlClassRegistryInit();
SNGXML_SERIALIZATION_API void XmlClassRegistryDone();

template<typename T>
void XmlRegister(int classId, FactoryFunction factoryFunction)
{
    XmlClassRegistry::Instance().Register(classId, factoryFunction, MakeXmlSerializableExtractor<T>());
}

} } // namespace sngxml::xmlser

#endif // SNGXML_XML_CLASS_REGISTRY_INCLUDED
