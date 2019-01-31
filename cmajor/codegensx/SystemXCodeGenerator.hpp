// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGENSX_SYSTEMX_CODEGENERATOR_INCLUDED
#define CMAJOR_CODEGENSX_SYSTEMX_CODEGENERATOR_INCLUDED
#include <cmajor/codegensx/CodeGenSxApi.hpp>
#include <cmajor/codegenbase/CodeGenerator.hpp>
#include <cmajor/cmsx/CompileUnit.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/Module.hpp>

namespace cmajor { namespace codegensx {

using namespace cmajor::binder;
using namespace cmajor::symbols;

class CODEGENSX_API SystemXCodeGenerator : public cmajor::codegenbase::CodeGenerator, public BoundNodeVisitor
{
public:
    SystemXCodeGenerator(cmajor::ir::EmittingContext& emittingContext_);
    void GenerateCode(void* boundCompileUnit) override;
    void Visit(BoundCompileUnit& boundCompileUnit) override;
private:
    cmajor::ir::Emitter* emitter;
    cmajor::ir::EmittingContext* emittingContext;
    SymbolTable* symbolTable;
    Module* module;
    std::unique_ptr<cmsx::CompileUnit> compileUnit;
};

} } // namespace cmajor::codegensx

#endif // CMAJOR_CODEGENSX_SYSTEMX_CODEGENERATOR_INCLUDED
