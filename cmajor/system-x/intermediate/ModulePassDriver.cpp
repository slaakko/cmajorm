// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/ModulePassDriver.hpp>

namespace cmsx::intermediate {

void GetRequiredModulePasses(ModulePass* pass, std::vector<ModulePass*>& requiredPasses, std::set<ModulePass*>& visited)
{
    visited.insert(pass);
    for (Pass* requiredPass : pass->RequiredPasses())
    {
        if (requiredPass->Kind() == PassKind::module || requiredPass->Kind() == PassKind::composite)
        {
            ModulePass* requiredModulePass = static_cast<ModulePass*>(requiredPass);
            GetRequiredModulePasses(requiredModulePass, requiredPasses, visited);
            requiredPasses.push_back(requiredModulePass);
        }
    }
}

ModulePassDriver::ModulePassDriver(Context* context_, const std::vector<ModulePass*>& passes_) : context(context_), passes(passes_)
{
}

void ModulePassDriver::Run()
{
    int n = passes.size();
    for (int i = 0; i < n; ++i)
    {
        ModulePass* pass = passes[i];
        std::vector<ModulePass*> requiredPasses;
        std::set<ModulePass*> visited;
        GetRequiredModulePasses(pass, requiredPasses, visited);
        for (ModulePass* requiredPass : requiredPasses)
        {
            if (!requiredPass->Valid())
            {
                requiredPass->Clear(context);
                requiredPass->Run(context);
                requiredPass->SetValid();
                InvalidatePasses(requiredPass);
            }
        }
        pass->Run(context);
        pass->SetValid();
        InvalidatePasses(pass);
    }
}

void ModulePassDriver::InvalidatePasses(ModulePass* pass)
{
    for (Pass* invalidatedPass : pass->InvalidatedPasses())
    {
        if (invalidatedPass->Kind() == PassKind::module || invalidatedPass->Kind() == PassKind::composite)
        {
            ModulePass* modulePass = static_cast<ModulePass*>(invalidatedPass);
            modulePass->ResetValid();
        }
    }
}

} // cmsx::intermediate
