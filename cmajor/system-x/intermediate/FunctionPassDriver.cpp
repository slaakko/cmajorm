// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/FunctionPassDriver.hpp>

namespace cmsx::intermediate {

void GetRequiredFunctionPasses(FunctionPass* functionPass, std::vector<FunctionPass*>& requiredPasses, std::set<FunctionPass*>& visited)
{
    visited.insert(functionPass);
    for (Pass* requiredPass : functionPass->RequiredPasses())
    {
        if (requiredPass->Kind() == PassKind::function)
        {
            FunctionPass* requiredFunctionPass = static_cast<FunctionPass*>(requiredPass);
            GetRequiredFunctionPasses(requiredFunctionPass, requiredPasses, visited);
            requiredPasses.push_back(requiredFunctionPass);
        }
    }
}

FunctionPassDriver::FunctionPassDriver(Context* context_, const std::vector<FunctionPass*>& functionPasses_) : Visitor(context_), functionPasses(functionPasses_)
{
}

void FunctionPassDriver::Visit(Function& function)
{
    int n = functionPasses.size();
    for (int i = 0; i < n; ++i)
    {
        FunctionPass* functionPass = functionPasses[i];
        std::vector<FunctionPass*> requiredPasses;
        std::set<FunctionPass*> visited;
        GetRequiredFunctionPasses(functionPass, requiredPasses, visited);
        for (FunctionPass* requiredPass : requiredPasses)
        {
            if (!requiredPass->Valid())
            {
                requiredPass->Clear(function);
                requiredPass->Run(function);
                requiredPass->SetValid();
                InvalidatePasses(requiredPass);
            }
        }
        functionPass->Run(function);
        functionPass->SetValid();
        InvalidatePasses(functionPass);
    }
}

void FunctionPassDriver::InvalidatePasses(FunctionPass* pass)
{
    for (Pass* invalidatedPass : pass->InvalidatedPasses())
    {
        if (invalidatedPass->Kind() == PassKind::function)
        {
            FunctionPass* functionPass = static_cast<FunctionPass*>(invalidatedPass);
            functionPass->ResetValid();
        }
    }
}

} // cmsx::intermediate
