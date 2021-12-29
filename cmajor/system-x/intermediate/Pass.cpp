// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Pass.hpp>

namespace cmsx::intermediate {

Pass::Pass(const std::string& name_, PassKind kind_) : name(name_), kind(kind_), valid(false)
{
}

Pass::~Pass()
{
}

void Pass::AddRequiredPass(Pass* pass)
{
    requiredPasses.insert(pass);
}

void Pass::AddInvalidatedPass(Pass* pass)
{
    invalidatedPasses.insert(pass);
}

FunctionPass::FunctionPass(const std::string& name_) : Pass(name_, PassKind::function)
{
}

void FunctionPass::Run(Function& function)
{
}

void FunctionPass::Clear(Function& function)
{
}

ModulePass::ModulePass(const std::string& name_) : Pass(name_, PassKind::module)
{
}

ModulePass::ModulePass(const std::string& name_, PassKind kind_) : Pass(name_, kind_)
{
}

void ModulePass::Run(Context* context)
{
}

void ModulePass::Clear(Context* context)
{
}

CompositePass::CompositePass(const std::string& name_) : ModulePass(name_, PassKind::composite)
{
}

} // cmsx::intermediate
