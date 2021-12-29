// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_WRITE_PASS_INCLUDED
#define CMSX_INTERMEDIATE_WRITE_PASS_INCLUDED
#include <system-x/intermediate/Pass.hpp>

namespace cmsx::intermediate {

class CMSX_INTERMEDIATE_API WritePass : public ModulePass
{
public:
    WritePass();
    void Run(Context* context) override;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_WRITE_PASS_INCLUDED
