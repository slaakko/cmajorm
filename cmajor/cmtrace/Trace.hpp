// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMTRACE_TRACE_INCLUDED
#define CMAJOR_CMTRACE_TRACE_INCLUDED
#include <cmajor/cmtrace/TraceNode.hpp>
#include <sngxml/dom/Document.hpp>

namespace cmtrace {

TRACE_API std::unique_ptr<ContainerNode> ReadTrace(const std::string& traceFilePath);
TRACE_API std::unique_ptr<ContainerNode> FilterTrace(ContainerNode* root, const std::string& filterExpr);
TRACE_API void WriteTextFile(ContainerNode* root, const std::string& textFilePath, const std::string& filterExpr);
TRACE_API std::unique_ptr<sngxml::dom::Document> TraceToXml(ContainerNode* root, const std::string& filterExpr);

}  // namespace cmtrace

#endif // CMAJOR_CMTRACE_TRACE_INCLUDED
