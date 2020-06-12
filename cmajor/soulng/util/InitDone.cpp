// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/InitDone.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Socket.hpp>

namespace soulng { namespace util {

void Init()
{
    soulng::unicode::UnicodeInit();
    soulng::util::InitSocket();
}

void Done()
{
    soulng::util::DoneSocket();
    soulng::unicode::UnicodeDone();
}

} } // namespace soulng::util

