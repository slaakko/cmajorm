// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlClassRegistry.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngxml { namespace xmlser {

using namespace soulng::unicode;

XmlFactory::XmlFactory(const std::string& className_) : className(className_)
{
}

XmlFactory::~XmlFactory()
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

void XmlClassRegistry::Register(XmlFactory* factory)
{
    factories.push_back(std::unique_ptr<XmlFactory>(factory));
    factoryMap[factory->ClassName()] = factory;
}

void* XmlClassRegistry::Create(sngxml::dom::Element* element)
{
    std::u32string classNameAttr = element->GetAttribute(U"className");
    if (classNameAttr.empty())
    {
        throw std::runtime_error("element '" + ToUtf8(element->Name()) + "' does not contain 'className' attribute");
    }
    std::string className = ToUtf8(classNameAttr);
    auto it = factoryMap.find(className);
    if (it != factoryMap.cend())
    {
        XmlFactory* factory = it->second;
        return factory->Create(element);
    }
    else
    {
        throw std::runtime_error("class '" + className + "' not registered to XML class registry");
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
