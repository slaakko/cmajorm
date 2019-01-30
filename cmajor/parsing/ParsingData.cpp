// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/ParsingData.hpp>

namespace cmajor { namespace parsing {

Context::Context()
{
}

Context::~Context()
{
}

ContextStack::ContextStack()
{
}

RuleData::RuleData()
{
}

ParsingData::ParsingData(int numRules) 
{
    ruleData.resize(numRules);
    for (int i = 0; i < numRules; ++i)
    {
        ruleData[i].reset(new RuleData());
    }
}

} } // namespace cmajor::parsing
