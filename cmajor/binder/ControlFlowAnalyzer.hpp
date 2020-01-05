// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_CONTROL_FLOW_ANALYZER_INCLUDED
#define CMAJOR_BINDER_CONTROL_FLOW_ANALYZER_INCLUDED
#include <cmajor/binder/BinderApi.hpp>

namespace cmajor { namespace binder {

class BoundCompileUnit;

BINDER_API void AnalyzeControlFlow(BoundCompileUnit& boundCompileUUnit);

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_CONTROL_FLOW_ANALYZER_INCLUDED
