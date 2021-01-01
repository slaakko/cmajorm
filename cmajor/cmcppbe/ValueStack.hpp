// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPBE_VALUE_STACK_INCLUDED
#define CMAJOR_CMCPPBE_VALUE_STACK_INCLUDED
#include <cmajor/ir/ValueStack.hpp>
#include <soulng/util/Error.hpp>
#include <cmajor/cmcppi/Value.hpp>
#include <vector>

namespace cmcppbe {

class ValueStack : public cmajor::ir::ValueStack
{
public:
    void Push(void* val) override
    {
        cmcppi::Value* value = static_cast<cmcppi::Value*>(val);
        s.push_back(value);
    }
    void* Pop() override
    {
        Assert(!s.empty(), "value stack is empty");
        cmcppi::Value* top = s.back();
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
    std::vector<cmcppi::Value*> s;
};

} // namespace cmcppbe

#endif // CMAJOR_CMCPPBE_VALUE_STACK_INCLUDED
