// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/String.hpp>
#include <mutex>

namespace cmajor { namespace rt {

std::unique_ptr<StringTable> StringTable::instance;

void StringTable::Init()
{
    instance.reset(new StringTable());
}

void StringTable::Done()
{
    instance.reset();
}

StringTable::StringTable() : nextStringHandle(1)
{
}

std::mutex mtx;

int32_t StringTable::InstallString(const std::string& str)
{
    std::lock_guard<std::mutex> lock(mtx);
    int32_t stringHandle = nextStringHandle++;
    stringMap[stringHandle] = str;
    return stringHandle;
}

const char* StringTable::GetString(int32_t stringHandle)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = stringMap.find(stringHandle);
    if (it != stringMap.cend())
    {
        return it->second.c_str();
    }
    return "";
}

void StringTable::DisposeString(int32_t stringHandle)
{
    std::lock_guard<std::mutex> lock(mtx);
    stringMap.erase(stringHandle);
}

int32_t InstallString(const std::string& str)
{
    return StringTable::Instance().InstallString(str);
}

const char* GetString(int32_t stringHandle)
{
    return StringTable::Instance().GetString(stringHandle);
}

void DisposeString(int32_t stringHandle)
{
    StringTable::Instance().DisposeString(stringHandle);
}

void InitString()
{
    StringTable::Init();
}

void DoneString()
{
    StringTable::Done();
}

} } // namespace cmajor::rt
