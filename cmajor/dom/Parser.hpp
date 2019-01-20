// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DOM_PARSER_INCLUDED
#define CMAJOR_DOM_PARSER_INCLUDED
#include <cmajor/dom/Document.hpp>

namespace cmajor { namespace dom {

std::unique_ptr<Document> ParseDocument(const std::u32string& content, const std::string& systemId);
std::unique_ptr<Document> ReadDocument(const std::string& fileName);

} } // namespace cmajor::dom

#endif // CMAJOR_DOM_PARSER_INCLUDED
