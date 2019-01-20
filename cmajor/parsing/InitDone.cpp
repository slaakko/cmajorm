// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/InitDone.hpp>
#include <cmajor/parsing/ParsingDomain.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/codedom/InitDone.hpp>

namespace cmajor { namespace parsing {

void Init()
{
    cmajor::codedom::Init();
    ParsingDomainInit();
    KeywordInit();
}

void Done()
{
    KeywordDone();
    ParsingDomainDone();
    cmajor::codedom::Done();
}

} } // namespace cmajor::parsing

