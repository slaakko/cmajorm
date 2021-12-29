// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_FUNCTION_PASS_DRIVER_INCLUDED
#define CMSX_INTERMEDIATE_FUNCTION_PASS_DRIVER_INCLUDED
#include <system-x/intermediate/Pass.hpp>
#include <system-x/intermediate/Visitor.hpp>
#include <vector>

namespace cmsx::intermediate {

class CMSX_INTERMEDIATE_API FunctionPassDriver : public Visitor
{
public:
    FunctionPassDriver(Context* context_, const std::vector<FunctionPass*>& functionPasses_);
    void Visit(Function& function);
private:
    void InvalidatePasses(FunctionPass* pass);
    const std::vector<FunctionPass*>& functionPasses;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_FUNCTION_PASS_DRIVER_INCLUDED
