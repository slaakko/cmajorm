// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/xpath/InitDone.hpp>
#include <cmajor/xpath/XPathFunction.hpp>
#include <cmajor/xpath/XPathDebug.hpp>

namespace cmajor { namespace xpath {

void Init()
{
    InitFunction();
    InitDebug();
}

void Done()
{
    DoneDebug();
    DoneFunction();
}

} } // namespace cmajor::xpath
