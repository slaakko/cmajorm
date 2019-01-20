// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BDT2DOM_BDT2DOM_INCLUDED
#define CMAJOR_BDT2DOM_BDT2DOM_INCLUDED
#include <cmajor/binder/BoundNode.hpp>
#include <cmajor/dom/Document.hpp>

namespace cmajor { namespace bdt2dom {

using namespace cmajor::dom;

std::unique_ptr<dom::Document> GenerateBdtDocument(cmajor::binder::BoundNode* node);

} } // namespace cmajor::bdt2dom

#endif // CMAJOR_BDT2DOM_BDT2DOM_INCLUDED
