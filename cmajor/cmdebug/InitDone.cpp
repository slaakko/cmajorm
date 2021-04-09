// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/InitDone.hpp>
#include <cmajor/cmdebug/Gdb.hpp>
#include <cmajor/cmdebug/CmdbSession.hpp>
#include <cmajor/cmdebug/KillChannel.hpp>

namespace cmajor { namespace debug {

void Init()
{
    InitGDB();
    CmdbSessionInit();
    InitKillChannel();
}

void Done()
{
    DoneKillChannel();
    CmdbSessionDone();
    DoneGDB();
}

} } // namespace cmajor::debug
