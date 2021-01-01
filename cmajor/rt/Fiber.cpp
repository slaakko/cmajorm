// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Fiber.hpp>
#include <soulng/util/Fiber.hpp>

void* RtConvertThreadToFiber(void* param)
{
    return soulng::util::ConvertThreadToFiber(param);
}

void* RtCreateFiber(uint64_t stackSize, void* startAddress, void* param)
{
    return soulng::util::CreateFiber(stackSize, startAddress, param);
}

void RtSwitchToFiber(void* fiber)
{
    soulng::util::SwitchToFiber(fiber);
}

void* RtGetFiberData()
{
    return soulng::util::GetFiberData();
}

void RtDeleteFiber(void* fiber)
{
    soulng::util::DeleteFiber(fiber);
}
