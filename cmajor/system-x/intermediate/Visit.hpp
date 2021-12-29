// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_VISIT_INCLUDED
#define CMSX_INTERMEDIATE_VISIT_INCLUDED
#include <system-x/intermediate/Api.hpp>
#include <vector>

namespace cmsx::intermediate {

class Function;
class BasicBlock;

CMSX_INTERMEDIATE_API std::vector<BasicBlock*> Preorder(Function& function);
CMSX_INTERMEDIATE_API std::vector<BasicBlock*> Postorder(Function& function);
CMSX_INTERMEDIATE_API std::vector<BasicBlock*> ReversePostorder(Function& function);

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_VISIT_INCLUDED
