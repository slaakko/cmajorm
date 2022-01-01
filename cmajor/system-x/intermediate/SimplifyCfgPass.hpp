// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_SIMPLIFY_CFG_PASS_INCLUDED
#define CMSX_INTERMEDIATE_SIMPLIFY_CFG_PASS_INCLUDED
#include <system-x/intermediate/Pass.hpp>

namespace cmsx::intermediate {

CMSX_INTERMEDIATE_API void Simplify(Function& function);

class CMSX_INTERMEDIATE_API SimplifyCfgPass : public FunctionPass
{
public:
    SimplifyCfgPass();
    void Run(Function& function) override;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_SIMPLIFY_CFG_PASS_INCLUDED
