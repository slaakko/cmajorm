// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmres/InitDone.hpp>
#include <cmajor/cmres/ResourceProcessor.hpp>

namespace cmajor { namespace resources {

void Init()
{
    InitResources();
}

void Done()
{
    DoneResources();
}

} } // namespace cmajor::resources
