// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/EntryExitPass.hpp>
#include <system-x/intermediate/Code.hpp>

namespace cmsx::intermediate {

EntryExitPass::EntryExitPass() : FunctionPass("entry-exit")
{
}

void EntryExitPass::Run(Function& function)
{
    function.AddEntryAndExitBlocks();
}

void EntryExitPass::Clear(Function& function)
{
    function.RemoveEntryAndExitBlocks();
}

} // cmsx::intermediate
