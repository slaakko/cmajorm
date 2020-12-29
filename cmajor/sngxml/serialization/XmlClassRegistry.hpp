// =================================
// Copyright (c) 2020 Seppo Laakko
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

class SNGXML_SERIALIZATION_API XmlFactory
{
public:
    XmlFactory(const std::string& className_);
    virtual ~XmlFactory();
    virtual void* Create(sngxml::dom::Element* element) = 0;
    const std::string& ClassName() const { return className; }
private:
    std::string className;
};

template<class T>
class XmlClassFactory : public XmlFactory
{
public:
    using XmlSerializableClassType = T;
    XmlClassFactory(const std::string& className) : XmlFactory(className) {}
    void* Create(sngxml::dom::Element* element) override
    {
        return new XmlSerializableClassType(element);
    }
};

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
    void Register(XmlFactory* factory);
    void* Create(sngxml::dom::Element* element);
private:
    static std::unique_ptr<XmlClassRegistry> instance;
    std::vector<std::unique_ptr<XmlFactory>> factories;
    std::unordered_map<std::string, XmlFactory*> factoryMap;
    XmlClassRegistry();
};

SNGXML_SERIALIZATION_API void XmlClassRegistryInit();
SNGXML_SERIALIZATION_API void XmlClassRegistryDone();

template<class T>
void RegisterXmlClass()
{
    std::string className = MakeClassNameStr(typeid(T).name());
    XmlClassRegistry::Instance().Register(new XmlClassFactory<T>(className));
}

} } // namespace sngxml::xmlser

#endif // SNGXML_XML_CLASS_REGISTRY_INCLUDED
