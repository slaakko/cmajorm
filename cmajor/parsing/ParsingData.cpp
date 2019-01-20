// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/ParsingData.hpp>

namespace cmajor { namespace parsing {

Context::~Context()
{
}

ParsingData::ParsingData(int numRules) 
{
    ruleData.resize(numRules);
}

} } // namespace cmajor::parsing
