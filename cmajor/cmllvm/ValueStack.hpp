// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMLLVM_VALUE_STACK_INCLUDED
#define CMAJOR_CMLLVM_VALUE_STACK_INCLUDED
#include <cmajor/util/Error.hpp>
#include <cmajor/ir/GenObject.hpp>
#include <cmajor/ir/ValueStack.hpp>
#include <llvm/IR/Value.h>
#include <vector>

namespace cmllvm {

class ValueStack : public cmajor::ir::ValueStack
{
public:
    void Push(void* val) override
    {
        llvm::Value* value = static_cast<llvm::Value*>(val);
        s.push_back(value);
    }
    void* Pop() override
    {
        Assert(!s.empty(), "value stack is empty");
        llvm::Value* top = s.back();
        s.pop_back();
        return top;
    }
    void Dup() override
    {
        s.push_back(s.back());
    }
    void Swap() override
    {
        std::swap(s.back(), s[s.size() - 2]);
    }
    void Rotate() override
    {
        std::swap(s[s.size() - 3], s[s.size() - 2]);
        std::swap(s.back(), s[s.size() - 2]);
    }
private:
    std::vector<llvm::Value*> s;
};

} // namespace cmllvm

#endif // CMAJOR_CMLLVM_VALUE_STACK_INCLUDED
