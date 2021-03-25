// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/InitDone.hpp>
#include <soulng/util/Ansi.hpp>
#include <soulng/util/FileLocking.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Trace.hpp>
#include <soulng/util/Time.hpp>

namespace soulng { namespace util {

void Init()
{
    soulng::util::InitFileLocking();
    soulng::unicode::UnicodeInit();
    soulng::util::InitSocket();
    soulng::util::AnsiInit();
    soulng::util::InitTrace();
    soulng::util::TimeInit();
}

void Done()
{
    soulng::util::TimeDone();
    soulng::util::DoneTrace();
    soulng::util::AnsiDone();
    soulng::util::DoneSocket();
    soulng::unicode::UnicodeDone();
    soulng::util::DoneFileLocking();
}

} } // namespace soulng::util

