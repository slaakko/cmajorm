// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsystemx/InitDone.hpp>
#include <cmajor/cmsystemx/OpCode.hpp>

namespace cmsystemx {

void Init()
{
    OpCodeInit();
}

void Done()
{
    OpCodeDone();
}

} // namespace cmsystemx
