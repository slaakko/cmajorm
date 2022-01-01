// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmpm/InitDone.hpp>
#include <cmajor/cmpm/PortMapServer.hpp>

namespace cmajor { namespace cmpm {

void Init()
{
    InitPortMapServer();
}

void Done()
{
    DonePortMapServer();
}

} } // namespace cmajor::cmpm
