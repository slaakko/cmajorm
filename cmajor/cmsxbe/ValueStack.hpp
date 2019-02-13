// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXBE_VALUE_STACK_INCLUDED
#define CMAJOR_CMSXBE_VALUE_STACK_INCLUDED
#include <cmajor/ir/ValueStack.hpp>
#include <cmajor/util/Error.hpp>
#include <cmajor/cmsxi/Value.hpp>
#include <vector>

namespace cmsxbe {

class ValueStack : public cmajor::ir::ValueStack
{
public:
    void Push(void* val) override
    {
        cmsxi::Value* value = static_cast<cmsxi::Value*>(val);
        s.push_back(value);
    }
    void* Pop() override
    {
        Assert(!s.empty(), "value stack is empty");
        cmsxi::Value* top = s.back();
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
    std::vector<cmsxi::Value*> s;
};

} // namespace cmsxbe

#endif // CMAJOR_CMSXBE_VALUE_STACK_INCLUDED
