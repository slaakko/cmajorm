// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_DOM_DOCUMENT_FRAGMENT_INCLUDED
#define SNGXML_DOM_DOCUMENT_FRAGMENT_INCLUDED
#include <sngxml/dom/Node.hpp>

namespace sngxml { namespace dom {

class SNGXML_DOM_API DocumentFragment : public ParentNode
{
public:
    DocumentFragment();
    DocumentFragment(const DocumentFragment&) = delete;
    DocumentFragment& operator=(const DocumentFragment&) = delete;
    DocumentFragment(DocumentFragment&&) = delete;
    DocumentFragment& operator=(DocumentFragment&&) = delete;
    std::unique_ptr<Node> CloneNode(bool deep) override;
};

} } // namespace sngxml::dom

#endif // SNGXML_DOM_DOCUMENT_FRAGMENT_INCLUDED
