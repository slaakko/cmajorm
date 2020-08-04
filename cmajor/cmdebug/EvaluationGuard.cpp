// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/EvaluationGuard.hpp>
#include <cmajor/cmdebug/Debugger.hpp>

namespace cmajor { namespace debug {

EvaluationGuard::EvaluationGuard(Debugger& debugger_) : debugger(debugger_), value(debugger.ReleaseResult())
{
}

EvaluationGuard::~EvaluationGuard()
{
    debugger.ResetResult(value);
}

} } // namespace cmajor::debug