// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BDT2DOM_BDT2DOM_INCLUDED
#define CMAJOR_BDT2DOM_BDT2DOM_INCLUDED
#include <cmajor/bdt2dom/Bdt2DomApi.hpp>
#include <cmajor/binder/BoundNode.hpp>
#include <sngxml/dom/Document.hpp>

namespace cmajor { namespace bdt2dom {

using namespace sngxml::dom;

std::unique_ptr<sngxml::dom::Document> GenerateBdtDocument(cmajor::binder::BoundNode* node);

} } // namespace cmajor::bdt2dom

#endif // CMAJOR_BDT2DOM_BDT2DOM_INCLUDED
