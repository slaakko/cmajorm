// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/SimpleAssemblyCodeGenPass.hpp>
#include <system-x/intermediate/SimpleAssemblyCodeGenerator.hpp>
#include <system-x/intermediate/Context.hpp>
#include <system-x/assembler/AssemblyFile.hpp>
#include <soulng/util/Path.hpp>

namespace cmsx::intermediate {

using namespace soulng::util;

SimpleAssemblyCodeGenPass::SimpleAssemblyCodeGenPass() : ModulePass("simple-assembly-code-gen")
{
}

void SimpleAssemblyCodeGenPass::Run(Context* context)
{
    std::string assemblyFilePath = Path::ChangeExtension(context->FilePath(), ".s");
    cmsx::assembler::AssemblyFile assemblyFile(assemblyFilePath);
    SimpleAssemblyCodeGenerator codeGenerator(context, &assemblyFile);
    context->GetData().VisitGlobalVariables(codeGenerator);
    context->GetCode().VisitFunctions(codeGenerator);
    codeGenerator.WriteOutputFile();
}

} // cmsx::intermediate
