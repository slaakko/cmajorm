// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/ManualResetEvent.hpp>
#include <Windows.h>

namespace cmajor { namespace wing {

ManualResetEvent::ManualResetEvent() : handle(nullptr)
{
    handle = CreateEventW(nullptr, true, false, nullptr);
}

ManualResetEvent::~ManualResetEvent()
{
    if (handle != nullptr)
    {
        bool retval = CloseHandle(handle);
    }
}

void ManualResetEvent::Reset()
{
    bool retval = ResetEvent(handle);
}

void ManualResetEvent::Set()
{
    bool retval = SetEvent(handle);
}

void ManualResetEvent::WaitFor()
{
    DWORD retval = WaitForSingleObject(handle, INFINITE);
}

} } // cmajor::wing
