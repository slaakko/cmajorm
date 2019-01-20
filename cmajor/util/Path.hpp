// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_UTIL_PATH_INCLUDED
#define CMAJOR_UTIL_PATH_INCLUDED
#include <stdexcept>

namespace cmajor { namespace util {

class InvalidPathException: public std::runtime_error
{
public:
    InvalidPathException(const std::string& message_);
};

std::string GetCurrentWorkingDirectory();
bool FileExists(const std::string& filePath);
bool DirectoryExists(const std::string& directoryPath);
bool PathExists(const std::string& path);
std::string GetFullPath(const std::string& path);

class Path
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
