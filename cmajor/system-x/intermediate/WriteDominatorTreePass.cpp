// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/WriteDominatorTreePass.hpp>
#include <system-x/intermediate/DominatorTreePass.hpp>
#include <system-x/intermediate/PassManager.hpp>
#include <system-x/intermediate/DominatorTreeCooper.hpp>
#include <system-x/intermediate/Context.hpp>
#include <soulng/util/Path.hpp>

namespace cmsx::intermediate {

using namespace soulng::util;

WriteDominatorTreePass::WriteDominatorTreePass() : FunctionPass("write-dominator-tree")
{
}

void WriteDominatorTreePass::Run(Function& function)
{
    Pass* pass = PassManager::Instance().GetPass("dominator-tree");
    DominatorTreePass* dominatorTreePass = static_cast<DominatorTreePass*>(pass);
    DominatorTreeCooper* dominatorTree = dominatorTreePass->GetDominatorTree();
    if (dominatorTree->GetFunction() != &function)
    {
        throw std::runtime_error("invalid pass depencency");
    }
    std::string libDir = Path::GetDirectoryName(function.Parent()->GetContext()->FilePath());
    std::string dominatorTreeXmlFilePath = Path::Combine(libDir, function.Name() + ".dominator.tree.xml");
    dominatorTree->WriteXmlDocument(dominatorTreeXmlFilePath);
}

} // cmsx::intermediate
