// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/InitDone.hpp>
#include <soulng/util/Unicode.hpp>

namespace soulng { namespace util {

void Init()
{
    soulng::unicode::UnicodeInit();
}

void Done()
{
    soulng::unicode::UnicodeDone();
}

} } // namespace soulng::util

