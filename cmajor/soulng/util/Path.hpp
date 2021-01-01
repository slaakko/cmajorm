// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_PATH_INCLUDED
#define SOULNG_UTIL_PATH_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <stdexcept>

namespace soulng { namespace util {

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
UTIL_API std::string MakeRelativeDirPath(const std::string& dirPath, const std::string& referenceDirPath);

class UTIL_API Path
{
public:
    static std::string MakeCanonical(const std::string& path);
    static std::string ChangeExtension(const std::string& path, const std::string& extension);
    static bool HasExtension(const std::string& path);
    static std::string GetExtension(const std::string& path);
    static std::string GetDrive(const std::string& path);
    static std::string GetFileName(const std::string& path);
    static std::string GetFileNameWithoutExtension(const std::string& path);
    static std::string GetDirectoryName(const std::string& path);
    static std::string Combine(const std::string& path1, const std::string& path2);
    static bool IsAbsolute(const std::string& path);
    static bool IsRelative(const std::string& path);
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_PATH_INCLUDED
