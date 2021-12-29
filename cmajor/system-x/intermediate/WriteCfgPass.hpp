// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_WRITE_CFG_PASS_INCLUDED
#define CMSX_INTERMEDIATE_WRITE_CFG_PASS_INCLUDED
#include <system-x/intermediate/Pass.hpp>

namespace cmsx::intermediate {

class CMSX_INTERMEDIATE_API WriteCfgPass : public FunctionPass
{
public:
    WriteCfgPass();
    void Run(Function& function) override;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_WRITE_CFG_PASS_INCLUDED
