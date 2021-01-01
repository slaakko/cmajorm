// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_STRING_INCLUDED
#define CMAJOR_RTS_STRING_INCLUDED
#include <soulng/util/Error.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include <stdint.h>

namespace cmajor { namespace rt {

class StringTable
{
public:
    static void Init();
    static void Done();
    static StringTable& Instance() { Assert(instance, "string table not initialized");  return *instance; }
    int32_t InstallString(const std::string& str);
    const char* GetString(int32_t stringHandle);
    void DisposeString(int32_t stringHandle);
private:
    static std::unique_ptr<StringTable> instance;
    StringTable();
    int32_t nextStringHandle;
    std::unordered_map<int32_t, std::string> stringMap;
};

int32_t InstallString(const std::string& str);
const char* GetString(int32_t stringHandle);
void DisposeString(int32_t stringHandle);

void InitString();
void DoneString();

} } // namespace cmajor::rt

#endif // CMAJOR_RTS_STRING_INCLUDED
