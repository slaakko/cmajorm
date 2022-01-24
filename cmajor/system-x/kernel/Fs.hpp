// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_FS_INCLUDED
#define CMSX_KERNEL_FS_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <stdint.h>
#include <list>
#include <boost/pool/pool_alloc.hpp>

namespace cmsx::kernel {

class CMSX_KERNEL_API SuperBlock
{
public:

private:
    uint16_t fsId;
    uint16_t inodeCount;
    uint16_t inodeFreeListBlockNumber;
    uint16_t diskBlockFreeListBlockNumber;
};

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_FS_INCLUDED

