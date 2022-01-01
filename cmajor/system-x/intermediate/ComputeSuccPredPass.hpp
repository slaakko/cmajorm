// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Pass.hpp>

namespace cmsx::intermediate {

CMSX_INTERMEDIATE_API void ComputeSuccessorsAndPredecessors(Function& function);
CMSX_INTERMEDIATE_API void ClearSuccessorsAndPredecessors(Function& function);

class CMSX_INTERMEDIATE_API ComputeSuccPredPass : public FunctionPass
{
public:
    ComputeSuccPredPass();
    void Run(Function& function) override;
    void Clear(Function& function) override;
};

} // cmsx::intermediate
