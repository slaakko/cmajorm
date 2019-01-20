// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_CONTROL_FLOW_ANALYZER_INCLUDED
#define CMAJOR_BINDER_CONTROL_FLOW_ANALYZER_INCLUDED

namespace cmajor { namespace binder {

class BoundCompileUnit;

void AnalyzeControlFlow(BoundCompileUnit& boundCompileUUnit);

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_CONTROL_FLOW_ANALYZER_INCLUDED
