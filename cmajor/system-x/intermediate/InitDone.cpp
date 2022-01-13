// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/InitDone.hpp>
#include <system-x/intermediate/PassManager.hpp>
#include <system-x/intermediate/RegisterAllocator.hpp>

namespace cmsx::intermediate {

void Init()
{
    InitPassManager();
}

void Done()
{
    DonePassManager();
}

} // cmsx::intermediate
