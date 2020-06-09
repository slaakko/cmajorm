// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_PROCESS_INCLUDED
#define SOULNG_UTIL_PROCESS_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <string>

namespace soulng { namespace util {

class UTIL_API Process
{
public:
    enum class ProcessIOHandle : int
    {
        out = 1, err = 2
    };
    Process(const std::string& filePath_);
    std::string ReadLine(ProcessIOHandle handle);
    void WriteLine(const std::string& line);
private:
    std::string filePath;
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_PROCESS_INCLUDED
