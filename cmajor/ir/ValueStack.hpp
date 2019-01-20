// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_IR_VALUE_STACK_INCLUDED
#define CMAJOR_IR_VALUE_STACK_INCLUDED
#include <cmajor/util/Error.hpp>
#include <llvm/IR/Value.h>
#include <vector>

namespace cmajor { namespace ir {

class ValueStack
{
public:
    void Push(llvm::Value* value)
    {
        s.push_back(value);
    }
    llvm::Value* Pop()
    {
        Assert(!s.empty(), "value stack is empty");
        llvm::Value* top = s.back();
        s.pop_back();
        return top;
    }
    void Dup()
    {
        s.push_back(s.back());
    }
    void Swap()
    {
        std::swap(s.back(), s[s.size() - 2]);
    }
    void Rotate()
    {
        std::swap(s[s.size() - 3], s[s.size() - 2]);
        std::swap(s.back(), s[s.size() - 2]);
    }
private:
    std::vector<llvm::Value*> s;
};

} } // namespace cmajor::ir

#endif // CMAJOR_IR_VALUE_STACK_INCLUDED
