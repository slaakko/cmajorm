// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Event.hpp>

namespace cmsx::machine {

std::string EventKindStr(EventKind eventKind)
{
    switch (eventKind)
    {
        case EventKind::childExitEvent:
        {
            return "CHILD_EXIT";
        }
        case EventKind::alarmEvent:
        {
            return "ALARM";
        }
        case EventKind::blockFreeEvent:
        {
            return "BLOCK_FREE";
        }
        case EventKind::inodeFreeEvent:
        {
            return "INODE_FREE";
        }
        case EventKind::ioEvent:
        {
            return "IO";
        }
    }
    return std::string();
}

std::string Event::ToString() const
{
    return "EVENT:" + EventKindStr(kind) + ":ID=" + std::to_string(id);
}

} // namespace cmsx::machine
