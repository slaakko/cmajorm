// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/DominatorTreePass.hpp>
#include <system-x/intermediate/DominatorTreeCooper.hpp>

namespace cmsx::intermediate {

DominatorTreePass::DominatorTreePass() : FunctionPass("dominator-tree")
{
}

void DominatorTreePass::Run(Function& function)
{
    dominatorTree.reset(new DominatorTreeCooper(function));
    dominatorTree->Compute();
}

void DominatorTreePass::Clear(Function& function)
{
    dominatorTree.reset();
}

} // cmsx::intermediate
