// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DOM_DOCUMENT_FRAGMENT_INCLUDED
#define CMAJOR_DOM_DOCUMENT_FRAGMENT_INCLUDED
#include <cmajor/dom/Node.hpp>

namespace cmajor { namespace dom {

class DocumentFragment : public ParentNode
{
public:
    DocumentFragment();
    DocumentFragment(const DocumentFragment&) = delete;
    DocumentFragment& operator=(const DocumentFragment&) = delete;
    DocumentFragment(DocumentFragment&&) = delete;
    DocumentFragment& operator=(DocumentFragment&&) = delete;
    std::unique_ptr<Node> CloneNode(bool deep) override;
};

} } // namespace cmajor::dom

#endif // CMAJOR_DOM_DOCUMENT_FRAGMENT_INCLUDED
