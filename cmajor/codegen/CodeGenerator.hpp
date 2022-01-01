// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGEN_CODEGENERATOR_INCLUDED
#define CMAJOR_CODEGEN_CODEGENERATOR_INCLUDED
#include <cmajor/codegenbase/CodeGenerator.hpp>

namespace cmajor { namespace codegen {

class CodeGenerator
{
public:
    CodeGenerator(cmajor::ir::EmittingContext& emittingContext);
    ~CodeGenerator();
    cmajor::codegenbase::CodeGenerator* GetBaseCodeGenerator() const { return baseCodeGenerator; }
private:
    cmajor::codegenbase::CodeGenerator* baseCodeGenerator;
};

} } // namespace cmajor::codegen

#endif // CMAJOR_CODEGEN_CODEGENERATOR_INCLUDED
