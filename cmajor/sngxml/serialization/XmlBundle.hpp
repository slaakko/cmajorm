// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XML_BUNDLE_INCLUDED
#define SNGXML_XML_BUNDLE_INCLUDED
#include <sngxml/serialization/XmlContainer.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/Document.hpp>

namespace sngxml { namespace xmlser {

enum class XmlBundleKind : int
{
    shallow = 0, deep = 1
};

class SNGXML_SERIALIZATION_API XmlBundle : public XmlContainer
{
public:
    XmlBundle();
    ~XmlBundle();
    const boost::uuids::uuid& RootObjectId() const { return rootObjectId; }
    void SetRootObjectId(const boost::uuids::uuid& rootObjectId_) override;
    void SetOwning(bool owning_) { owning = owning_; }
    bool IsOwning() const { return owning; }
    XmlSerializable* Root() const;
    XmlSerializable* ReleaseRoot();
    std::unique_ptr<sngxml::dom::Document> ToXmlDocument() const;
    std::string ToXmlString() const;
    std::string ToXmlString(int indent) const;
    void ResolveXmlPtrs();
    void DestroyNonownedObjects();
private:
    boost::uuids::uuid rootObjectId;
    bool owning;
};

SNGXML_SERIALIZATION_API std::unique_ptr<XmlBundle> ToXmlBundle(const std::string& xmlStr);
SNGXML_SERIALIZATION_API std::unique_ptr<XmlBundle> ToXmlBundle(const std::string& xmlStr, const std::string& systemId);
SNGXML_SERIALIZATION_API std::unique_ptr<XmlBundle> ToXmlBundle(const std::string& xmlStr, const std::string& systemId, bool owning);

} } // namespace sngxml::xmlser

#endif // SNGXML_XML_BUNDLE_INCLUDED
