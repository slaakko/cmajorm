// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/InitDone.hpp>
#include <system-x/intermediate/AssemblyConfig.hpp>
#include <system-x/intermediate/PassManager.hpp>
#include <system-x/intermediate/RegisterAllocator.hpp>

namespace cmsx::intermediate {

void Init()
{
    InitAssemblyConfig();
    InitPassManager();
}

void Done()
{
    DoneAssemblyConfig();
    DonePassManager();
}

} // cmsx::intermediate
