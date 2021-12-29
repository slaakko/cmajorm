// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_DOMINATOR_TREE_PASS_INCLUDED
#define CMSX_INTERMEDIATE_DOMINATOR_TREE_PASS_INCLUDED
#include <system-x/intermediate/Pass.hpp>
#include <memory>

namespace cmsx::intermediate {

class DominatorTreeCooper;

class CMSX_INTERMEDIATE_API DominatorTreePass : public FunctionPass
{
public:
    DominatorTreePass();
    void Run(Function& function) override;
    void Clear(Function& function) override;
    DominatorTreeCooper* GetDominatorTree() const { return dominatorTree.get(); }
private:
    std::unique_ptr<DominatorTreeCooper> dominatorTree;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_DOMINATOR_TREE_PASS_INCLUDED
