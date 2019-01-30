// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_PATH_INCLUDED
#define CMAJOR_UTIL_PATH_INCLUDED
#include <cmajor/util/UtilApi.hpp>
#include <stdexcept>

namespace cmajor { namespace util {

class InvalidPathException: public std::runtime_error
{
public:
    InvalidPathException(const std::string& message_);
};

UTIL_API std::string GetCurrentWorkingDirectory();
UTIL_API bool FileExists(const std::string& filePath);
UTIL_API bool DirectoryExists(const std::string& directoryPath);
UTIL_API bool PathExists(const std::string& path);
UTIL_API std::string GetFullPath(const std::string& path);

class UTIL_API Path
{
public:
    static std::string MakeCanonical(const std::string& path);
    static std::string ChangeExtension(const std::string& path, const std::string& extension);
    static bool HasExtension(const std::string& path);
    static std::string GetExtension(const std::string& path);
    static std::string GetFileName(const std::string& path);
    static std::string GetFileNameWithoutExtension(const std::string& path);
    static std::string GetDirectoryName(const std::string& path);
    static std::string Combine(const std::string& path1, const std::string& path2);
    static bool IsAbsolute(const std::string& path);
    static bool IsRelative(const std::string& path);
};

} } // namespace cmajor::util

#endif // CMAJOR_UTIL_PATH_INCLUDED
