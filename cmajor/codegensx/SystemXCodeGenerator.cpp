// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codegensx/SystemXCodeGenerator.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/System.hpp>
#include <boost/filesystem.hpp>

namespace cmajor { namespace codegensx {

SystemXCodeGenerator::SystemXCodeGenerator(cmajor::ir::EmittingContext& emittingContext_) :
    cmajor::codegenbase::CodeGenerator(emittingContext_), emitter(GetEmitter()), emittingContext(&emittingContext_), symbolTable(nullptr), module(nullptr)
{
}

void SystemXCodeGenerator::GenerateCode(void* boundCompileUnit)
{
    BoundCompileUnit* compileUnit = static_cast<BoundCompileUnit*>(boundCompileUnit);
    compileUnit->Accept(*this);
}

void SystemXCodeGenerator::Visit(BoundCompileUnit& boundCompileUnit)
{
    symbolTable = &boundCompileUnit.GetSymbolTable();
    module = &boundCompileUnit.GetModule();
    std::string assemblyFilePath = Path::ChangeExtension(boundCompileUnit.ObjectFilePath(), ".s");
    compileUnit.reset(new cmsx::CompileUnit(assemblyFilePath));
    int n = boundCompileUnit.BoundNodes().size();
    for (int i = 0; i < n; ++i)
    {
        BoundNode* node = boundCompileUnit.BoundNodes()[i].get();
        node->Accept(*this);
    }
    compileUnit->Write();
    std::string assembleCommand;
    std::string assemblyErrorFilePath = assemblyFilePath + ".error";
    assembleCommand.append("cmfileredirector -2 " + assemblyErrorFilePath + " cmsxas ").append(assemblyFilePath);
    try
    {
        System(assembleCommand);
        boost::filesystem::remove(boost::filesystem::path(assemblyErrorFilePath));
    }
    catch (const std::exception& ex)
    {
        std::string errors = ReadFile(assemblyErrorFilePath);
        throw std::runtime_error("assembling '" + assemblyFilePath + "' failed: " + ex.what() + ":\nerrors:\n" + errors);
    }
}

} } // namespace cmajor::codegensx
