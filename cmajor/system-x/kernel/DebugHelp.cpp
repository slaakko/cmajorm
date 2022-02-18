// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <boost/filesystem.hpp>

namespace cmsx::kernel {

void DebugBreak()
{
    std::vector<int> x;
    if (boost::filesystem::exists("foo"))
    {
        x.push_back(1);
    }
}

} // namespace cmsx::kernel
