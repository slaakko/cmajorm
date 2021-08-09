// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Event.hpp>

namespace cmajor { namespace wing {

Event::Event() : nextHandlerId(0)
{
}

void Event::RemoveHandler(int handlerId)
{
    auto it = handlerIdMap.find(handlerId);
    if (it != handlerIdMap.cend())
    {
        int handlerIndex = it->second;
        handlers.erase(handlers.begin() + handlerIndex);
        for (int i = 0; i < nextHandlerId; ++i)
        {
            auto hit = handlerIdMap.find(i);
            if (hit != handlerIdMap.cend())
            {
                int hi = hit->second;
                if (hi > handlerIndex)
                {
                    handlerIdMap[i] = handlerIdMap[i] - 1;
                }
            }
        }
        handlerIdMap.erase(handlerId);
    }
}

} } // cmajor::wing
