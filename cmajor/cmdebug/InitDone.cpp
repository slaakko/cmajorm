// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/InitDone.hpp>
#include <cmajor/cmdebug/Gdb.hpp>

namespace cmajor { namespace debug {

void Init()
{
    InitGDB();
}

void Done()
{
    DoneGDB();
}

} } // namespace cmajor::debug
