// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_INFO_TO_XML_INCLUDED
#define CMAJOR_DEBUG_INFO_TO_XML_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <sngxml/dom/Document.hpp>

namespace cmajor { namespace debug {

DEBUG_API std::unique_ptr<sngxml::dom::Document> GetDebugInfoAsXml(const std::string& cmdbFilePath);
DEBUG_API void WriteXmlDebugInfo(sngxml::dom::Document* debugInfoXmlDoc, const std::string& xmlDebugInfoFilePath);

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_INFO_TO_XML_INCLUDED
