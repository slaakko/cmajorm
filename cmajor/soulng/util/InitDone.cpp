// =================================
// Copyright (c) 2022 Seppo Laakko
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
    soulng::unicode::UnicodeInit();
    soulng::util::InitFileLocking();
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
    soulng::util::DoneFileLocking();
    soulng::unicode::UnicodeDone();
}

} } // namespace soulng::util

