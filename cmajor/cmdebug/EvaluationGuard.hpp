// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_EVALUATION_GUARD_INCLUDED
#define CMAJOR_DEBUG_EVALUATION_GUARD_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <soulng/util/Json.hpp>

namespace cmajor { namespace debug {

using namespace soulng::util;

class Debugger;

struct DEBUG_API EvaluationGuard
{
    EvaluationGuard(Debugger& debugger_);
    ~EvaluationGuard();
    Debugger& debugger;
    JsonValue* value;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_EVALUATION_GUARD_INCLUDED
