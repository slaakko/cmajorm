// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_PARSING_DATA_INCLUDED
#define CMAJOR_PARSING_PARSING_DATA_INCLUDED
#include <cmajor/parsing/ParsingApi.hpp>
#include <cmajor/util/Error.hpp>
#include <memory>
#include <stack>
#include <vector>

namespace cmajor { namespace parsing {

struct PARSING_API Context
{
    Context();
    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&) noexcept = default;
    Context& operator=(Context&&) noexcept = default;
    virtual ~Context();
};

class PARSING_API ContextStack
{
public:
    ContextStack();
    ContextStack(const ContextStack&) = delete;
    ContextStack& operator=(const ContextStack&) = delete;
    ContextStack(ContextStack&&) noexcept = default;
    ContextStack& operator=(ContextStack&&) noexcept = default;
    void Push(Context* context) { stack.push(std::unique_ptr<Context>(context)); }
    Context* Pop() { Assert(!stack.empty(), "context stack is empty"); Context* context = stack.top().release(); stack.pop(); return context; }
private:
    std::stack<std::unique_ptr<Context>> stack;
};

struct PARSING_API RuleData
{
    RuleData();
    RuleData(const RuleData&) = delete;
    RuleData& operator=(const RuleData&) = delete;
    RuleData(RuleData&&) noexcept = default;
    RuleData& operator=(RuleData&&) noexcept = default;
    std::unique_ptr<Context> context;
    ContextStack contextStack;
};

class PARSING_API ParsingData
{
public:
    ParsingData(int numRules);
    ParsingData(const ParsingData&) = delete;
    ParsingData& operator=(const ParsingData&) = delete;
    ParsingData(ParsingData&&) noexcept = default;
    ParsingData& operator=(ParsingData&&) noexcept = default;
    void PushContext(int ruleId, Context* context) 
    { 
        Assert(ruleId >= 0 && ruleId < ruleData.size(), "invalid rule id"); 
        ruleData[ruleId]->contextStack.Push(ruleData[ruleId]->context.release()); 
        ruleData[ruleId]->context.reset(context);
    }
    void PopContext(int ruleId) 
    { 
        Assert(ruleId >= 0 && ruleId < ruleData.size(), "invalid rule id"); 
        ruleData[ruleId]->context.reset(ruleData[ruleId]->contextStack.Pop());  
    }
    Context* GetContext(int ruleId) const 
    { 
        Assert(ruleId >= 0 && ruleId < ruleData.size(), "invalid rule id"); 
        return ruleData[ruleId]->context.get();  
    }
private:
    std::vector<std::unique_ptr<RuleData>> ruleData;
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_PARSING_DATA_INCLUDED
