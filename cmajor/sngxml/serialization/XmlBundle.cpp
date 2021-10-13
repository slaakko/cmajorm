// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlBundle.hpp>
#include <sngxml/serialization/XmlSerializable.hpp>
#include <sngxml/serialization/XmlPtr.hpp>
#include <sngxml/serialization/XmlClassRegistry.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/xpath/XPathEvaluate.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

namespace sngxml { namespace xmlser {

using namespace soulng::unicode;

XmlBundle::XmlBundle() : XmlContainer(), rootObjectId(boost::uuids::nil_uuid()), owning(false)
{
}

XmlBundle::~XmlBundle()
{
    if (owning)
    {
        DestroyNonownedObjects();
    }
}

void XmlBundle::DestroyNonownedObjects()
{
    std::vector<XmlSerializable*> toBeDestroyed;
    for (const auto& p : IdMap())
    {
        XmlSerializable* serializable = p.second;
        if (!serializable->IsOwned())
        {
            toBeDestroyed.push_back(serializable);
        }
    }
    for (XmlSerializable* d : toBeDestroyed)
    {
        d->DestroyObject();
    }
    Clear();
}

void XmlBundle::SetRootObjectId(const boost::uuids::uuid& rootObjectId_)
{
    rootObjectId = rootObjectId_;
}

XmlSerializable* XmlBundle::Root() const
{
    if (!rootObjectId.is_nil())
    {
        return Get(rootObjectId);
    }
    else
    {
        return nullptr;
    }
}

XmlSerializable* XmlBundle::ReleaseRoot()
{
    if (!rootObjectId.is_nil())
    {
        XmlSerializable* serializable = Get(rootObjectId);
        Remove(rootObjectId);
        return serializable;
    }
    else
    {
        return nullptr;
    }
}


std::unique_ptr<sngxml::dom::Document> XmlBundle::ToXmlDocument() const
{
    std::unique_ptr<sngxml::dom::Element> rootElement(new sngxml::dom::Element(U"xmlBundle"));
    rootElement->SetAttribute(U"rootObjectId", ToUtf32(boost::uuids::to_string(rootObjectId)));
    for (const auto& p : IdMap())
    {
        XmlSerializable* serializable = p.second;
        XmlSerializationContext ctx;
        std::unique_ptr<sngxml::dom::Element> element = serializable->ToXml("object", ctx);
        rootElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(element.release()));
    }
    std::unique_ptr<sngxml::dom::Document> document(new sngxml::dom::Document());
    document->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootElement.release()));
    return document;
}

std::string XmlBundle::ToXmlString() const
{
    return ToXmlString(1);
}

std::string XmlBundle::ToXmlString(int indent) const
{
    std::unique_ptr<sngxml::dom::Document> document = ToXmlDocument();
    std::stringstream stream;
    CodeFormatter formatter(stream);
    formatter.SetIndentSize(indent);
    document->Write(formatter);
    return stream.str();
}

void XmlBundle::ResolveXmlPtrs()
{
    for (const auto& p : IdMap())
    {
        XmlSerializable* serializable = p.second;
        std::vector<XmlPtrBase*> ptrs = serializable->GetPtrs();
        for (XmlPtrBase* ptr : ptrs)
        {
            ptr->Resolve(this);
        }
    }
}

std::unique_ptr<XmlBundle> ToXmlBundle(const std::string& xmlStr)
{
    return ToXmlBundle(xmlStr, "string");
}

std::unique_ptr<XmlBundle> ToXmlBundle(const std::string& xmlStr, const std::string& systemId)
{
    return ToXmlBundle(xmlStr, systemId, true);
}

std::unique_ptr<XmlBundle> ToXmlBundle(const std::string& xmlStr, const std::string& systemId, bool owning)
{
    std::unique_ptr<XmlBundle> bundle(new XmlBundle());
    bundle->SetOwning(owning);
    std::u32string content = ToUtf32(xmlStr);
    std::unique_ptr<sngxml::dom::Document> document = sngxml::dom::ParseDocument(content, systemId);
    std::unique_ptr<sngxml::xpath::XPathObject> bundleObject = sngxml::xpath::Evaluate(U"xmlBundle", document.get());
    if (bundleObject)
    {
        if (bundleObject->Type() == sngxml::xpath::XPathObjectType::nodeSet)
        {
            sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(bundleObject.get());
            int n = nodeSet->Length();
            if (n == 1)
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    sngxml::dom::Element* rootElement = static_cast<sngxml::dom::Element*>(node);
                    std::u32string rootObjectIdAttr = rootElement->GetAttribute(U"rootObjectId");
                    if (!rootObjectIdAttr.empty())
                    {
                        boost::uuids::uuid rootObjectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(rootObjectIdAttr));
                        bundle->SetRootObjectId(rootObjectId);
                    }
                }
                std::unique_ptr<sngxml::xpath::XPathObject> object = sngxml::xpath::Evaluate(U"xmlBundle/object", document.get());
                if (object)
                {
                    if (object->Type() == sngxml::xpath::XPathObjectType::nodeSet)
                    {
                        sngxml::xpath::XPathNodeSet* nodeSet = static_cast<sngxml::xpath::XPathNodeSet*>(object.get());
                        int n = nodeSet->Length();
                        for (int i = 0; i < n; ++i)
                        {
                            sngxml::dom::Node* node = (*nodeSet)[i];
                            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                            {
                                sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
                                std::u32string classIdAttr = element->GetAttribute(U"classId");
                                if (!classIdAttr.empty())
                                {
                                    int classId = boost::lexical_cast<int>(ToUtf8(classIdAttr));
                                    XmlSerializable* serializable = XmlClassRegistry::Instance().CreateXmlSerializable(classId);
                                    serializable->FromXml(element);
                                    bundle->Add(serializable);
                                }
                            }
                        }
                    }
                }
                bundle->ResolveXmlPtrs();
            }
            else
            {
                throw std::runtime_error("single 'xmlBundle' element expected");
            }
        }
    }
    return bundle;
}

} } // namespace sngxml::xmlser
