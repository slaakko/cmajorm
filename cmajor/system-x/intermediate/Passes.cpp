// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Passes.hpp>
#include <system-x/intermediate/PassManager.hpp>
#include <system-x/intermediate/ComputeSuccPredPass.hpp>
#include <system-x/intermediate/SimplifyCfgPass.hpp>
#include <system-x/intermediate/EntryExitPass.hpp>
#include <system-x/intermediate/DominatorTreeCooper.hpp>
#include <system-x/intermediate/DominatorTreePass.hpp>
#include <system-x/intermediate/Mem2RegPass.hpp>
#include <system-x/intermediate/WriteDominatorTreePass.hpp>
#include <system-x/intermediate/WriteCfgPass.hpp>
#include <system-x/intermediate/WritePass.hpp>
#include <system-x/intermediate/SimpleAssemblyCodeGenPass.hpp>

namespace cmsx::intermediate {

void CreatePasses()
{
    PassManager::Instance().AddPass(new ComputeSuccPredPass());
    PassManager::Instance().AddPass(new SimplifyCfgPass());
    PassManager::Instance().AddPass(new EntryExitPass());
    PassManager::Instance().AddPass(new DominatorTreePass());
    PassManager::Instance().AddPass(new Mem2RegPass());
    PassManager::Instance().AddPass(new WriteDominatorTreePass());
    PassManager::Instance().AddPass(new WriteCfgPass());
    PassManager::Instance().AddPass(new WritePass());
    PassManager::Instance().AddPass(new SimpleAssemblyCodeGenPass());
}

} // cmsx::intermediate
