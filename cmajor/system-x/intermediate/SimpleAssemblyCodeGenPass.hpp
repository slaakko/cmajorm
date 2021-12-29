// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_SIMPLE_ASSEMBLY_CODE_GEN_PASS_INCLUDED
#define CMSX_INTERMEDIATE_SIMPLE_ASSEMBLY_CODE_GEN_PASS_INCLUDED
#include <system-x/intermediate/Pass.hpp>

namespace cmsx::intermediate {

class CMSX_INTERMEDIATE_API SimpleAssemblyCodeGenPass : public ModulePass
{
public:
    SimpleAssemblyCodeGenPass();
    void Run(Context* context) override;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_SIMPLE_ASSEMBLY_CODE_GEN_PASS_INCLUDED
