// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/xpath/InitDone.hpp>
#include <sngxml/xpath/XPathFunction.hpp>
#include <sngxml/xpath/XPathDebug.hpp>

namespace sngxml { namespace xpath {

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

} } // namespace sngxml::xpath
