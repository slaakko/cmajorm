// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/InitDone.hpp>
#include <soulng/util/Ansi.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Socket.hpp>
#include <soulng/util/Trace.hpp>

namespace soulng { namespace util {

void Init()
{
    soulng::unicode::UnicodeInit();
    soulng::util::InitSocket();
    soulng::util::AnsiInit();
    soulng::util::InitTrace();
}

void Done()
{
    soulng::util::DoneTrace();
    soulng::util::AnsiDone();
    soulng::util::DoneSocket();
    soulng::unicode::UnicodeDone();
}

} } // namespace soulng::util

