// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_MODULE_PASS_DRIVER_INCLUDED
#define CMSX_INTERMEDIATE_MODULE_PASS_DRIVER_INCLUDED
#include <system-x/intermediate/Pass.hpp>
#include <vector>

namespace cmsx::intermediate {

class CMSX_INTERMEDIATE_API ModulePassDriver
{
public:
    ModulePassDriver(Context* context_, const std::vector<ModulePass*>& passes_);
    void Run();
private:
    void InvalidatePasses(ModulePass* pass);
    Context* context;
    const std::vector<ModulePass*>& passes;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_MODULE_PASS_DRIVER_INCLUDED
