// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codegen/CodeGenerator.hpp>
#include <cmajor/cmmid/Interface.hpp>
#ifdef _WIN32
#include <cmajor/codegenwin/WindowsCodeGenerator.hpp>
#endif
#include <cmajor/codegensx/SystemXCodeGenerator.hpp>

namespace cmajor { namespace codegen {

cmajor::codegenbase::CodeGenerator* CreateCodeGenerator(cmajor::ir::EmittingContext& emittingContext)
{
    cmajor::mid::BackEndKind backEnd = GetBackEnd();
    switch (backEnd)
    {
        case cmajor::mid::BackEndKind::llvmBackEnd:
        {
#ifdef _WIN32
            return new cmajor::codegenwin::WindowsCodeGenerator(emittingContext);
#else
            return new cmajor::codegenlinux::LinuxCodeGenerator(emittingContext);
#endif
        }
        case cmajor::mid::BackEndKind::cmsxBackEnd:
        {
            return new cmajor::codegensx::SystemXCodeGenerator(emittingContext);
        }
    }
    return nullptr;
}

void DestroyCodeGenerator(cmajor::codegenbase::CodeGenerator* codeGenerator)
{
    delete codeGenerator;
}

CodeGenerator::CodeGenerator(cmajor::ir::EmittingContext& emittingContext) : baseCodeGenerator(CreateCodeGenerator(emittingContext))
{
}

CodeGenerator::~CodeGenerator()
{
    DestroyCodeGenerator(baseCodeGenerator);
}

} } // namespace cmajor::codegen
