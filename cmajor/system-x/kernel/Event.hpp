// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_EVENT_INCLUDED
#define CMSX_KERNEL_EVENT_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <stdint.h>

namespace cmsx::kernel {

enum class EventKind
{
    childExitEvent, alarmEvent
};

struct CMSX_KERNEL_API Event
{
    Event(EventKind kind_, int32_t id_) : kind(kind_), id(id_) {}
    EventKind kind;
    int32_t id;
};

CMSX_KERNEL_API inline bool operator<(const Event& left, const Event& right)
{
    if (left.kind < right.kind) return true;
    if (left.kind > right.kind) return false;
    return left.id < right.id;
}

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_EVENT_INCLUDED
