// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_WRITE_DOMINATOR_TREE_PASS_INCLUDED
#define CMSX_INTERMEDIATE_WRITE_DOMINATOR_TREE_PASS_INCLUDED
#include <system-x/intermediate/Pass.hpp>

namespace cmsx::intermediate {

class CMSX_INTERMEDIATE_API WriteDominatorTreePass : public FunctionPass
{
public:
    WriteDominatorTreePass();
    void Run(Function& function) override;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_WRITE_DOMINATOR_TREE_PASS_INCLUDED
