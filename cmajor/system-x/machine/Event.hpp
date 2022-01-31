// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_EVENT_INCLUDED
#define CMSX_MACHINE_EVENT_INCLUDED
#include <system-x/machine/Api.hpp>
#include <stdint.h>

namespace cmsx::machine {

enum class EventKind
{
    none, childExitEvent, alarmEvent, blockFreeEvent, inodeFreeEvent, ioEvent
};

struct CMSX_MACHINE_API Event
{
    Event() : kind(EventKind::none), id(-1) {}
    Event(EventKind kind_, int32_t id_) : kind(kind_), id(id_) {}
    EventKind kind;
    int32_t id;
};

CMSX_MACHINE_API inline bool operator<(const Event& left, const Event& right)
{
    if (left.kind < right.kind) return true;
    if (left.kind > right.kind) return false;
    return left.id < right.id;
}

} // namespace cmsx::machine

#endif // CMSX_MACHINE_EVENT_INCLUDED
