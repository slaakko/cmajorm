// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codegen/EmittingContext.hpp>
#include <cmajor/cmmid/Interface.hpp>

namespace cmajor { namespace codegen {

EmittingContext::EmittingContext(int optimizationLevel) : baseEmittingContext(CmmCreateEmittingContext(optimizationLevel))
{
}

EmittingContext::~EmittingContext()
{
    CmmDestroyEmittingContext(baseEmittingContext);
}

} } // namespace cmajor::codegen

