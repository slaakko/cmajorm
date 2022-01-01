// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_ENTRY_EXIT_PASS_INCLUDED
#define CMSX_INTERMEDIATE_ENTRY_EXIT_PASS_INCLUDED
#include <system-x/intermediate/Pass.hpp>

namespace cmsx::intermediate {

class CMSX_INTERMEDIATE_API EntryExitPass : public FunctionPass
{
public:
    EntryExitPass();
    void Run(Function& function) override;
    void Clear(Function& function) override;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_ENTRY_EXIT_PASS_INCLUDED
