// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/ExecutionContext.hpp>
#include <cmajor/build/BuildOption.hpp>
#include <cmajor/build/Log.hpp>

namespace cmajor { namespace build {

ExecutionContext::ExecutionContext()
{
    if (GetBuildOption(BuildOptions::debug))
    {
        log = CreateCoutLog();
    }
}

ExecutionContext::~ExecutionContext()
{
}

} } // namespace cmajor::build
