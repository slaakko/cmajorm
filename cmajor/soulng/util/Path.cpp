// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <cctype>
#ifdef _WIN32
    #include <direct.h>
#elif defined(__linux) || defined(__unix) || defined(__posix) || defined(__unix__)
    #include <unistd.h>
#endif

namespace soulng { namespace util {

std::string GetCurrentWorkingDirectory()
{
    char buf[4096];
#ifdef _WIN32
    char* wd = _getcwd(buf, 4096);
#elif defined(__linux) || defined(__unix) || defined(__posix)
    char* wd = getcwd(buf, 4096);
#else
    #error unknown platform
#endif
    if (wd != nullptr)
    {
        return Path::MakeCanonical(wd);
    }
    else
    {
        throw std::runtime_error("could not get current working directory");
    }
}

bool FileExists(const std::string& filePath)
{
#ifdef _WIN32
    struct _stat statBuf;
    int result = _stat(filePath.c_str(), &statBuf);
    if (result == 0)
    {
        return (statBuf.st_mode & _S_IFDIR) == 0 ? 1 : 0;
    }
    else
    {
        return 0;
    }
#elif defined(__linux) || defined(__unix) || defined(__posix)
    struct stat statBuf;
    int result = stat(filePath.c_str(), &statBuf);
    if (result == 0)
    {
        return (S_ISDIR(statBuf.st_mode)) == 0 ? 1 : 0;
    }
    else
    {
        return 0;
    }
#else
    #error unknown platform
#endif
}

bool DirectoryExists(const std::string& directoryPath)
{
#ifdef _WIN32
    struct _stat statBuf;
    int result = _stat(directoryPath.c_str(), &statBuf);
    if (result == 0)
    {
        return (statBuf.st_mode & _S_IFDIR) != 0 ? true : false;
    }
    else
    {
        return false;
    }
#elif defined(__linux) || defined(__unix) || defined(__posix)
    struct stat statBuf;
    int result = stat(directoryPath.c_str(), &statBuf);
    if (result == 0)
    {
        return (S_ISDIR(statBuf.st_mode)) != 0 ? true : false;
    }
    else
    {
        return false;
    }
#else
    #error unknown platform
#endif
}

bool PathExists(const std::string& path)
{
#ifdef _WIN32
    struct _stat statBuf;
    return _stat(path.c_str(), &statBuf) == 0 ? true : false;
#elif defined(__linux) || defined(__unix) || defined(__posix)
    struct stat statBuf;
    return stat(path.c_str(), &statBuf) == 0 ? true : false;
#else
    #error unknown platform
#endif
}

InvalidPathException::InvalidPathException(const std::string& message_): std::runtime_error(message_)
{
}

std::string Path::MakeCanonical(const std::string& path)
{
    bool startsWithDriveLetter = false;
#ifdef _WIN32
    if (path.length() >= 2 && std::isalpha(path[0]) && path[1] == ':')
    {
        startsWithDriveLetter = true;
    }
#endif
    std::string result;
    char prev = ' ';
    bool first = true;
    for (char c : path)
    {
        if (first)
        {
            first = false;
            if (startsWithDriveLetter)
            {
                c = std::toupper(static_cast<unsigned char>(c));
            }
        }
        if (c == '\\')
        {
            c = '/';
        }
        if (c == '/')
        {
            if (prev != '/')
            {
                result.append(1, c);
            }
        }
        else
        {
            result.append(1, c);
        }
        prev = c;
    }
    if (result.length() == 3 && std::isalpha(result[0]) && result[1] == ':' && result[2] == '/')
    {
        return result;
    }
    if (result == "/")
    {
        return result;
    }
    if (!result.empty())
    {
        if (result[result.length() - 1] == '/')
        {
            result.substr(0, result.length() - 1);
        }
    }
    return result;
}

std::string Path::ChangeExtension(const std::string& path, const std::string& extension)
{
    std::string::size_type lastDotPos = path.rfind('.');
    if (extension.empty())
    {
        if (lastDotPos != std::string::npos)
        {
            return path.substr(0, lastDotPos);
        }
        else
        {
            return path;
        }
    }
    else
    {
        if (lastDotPos == std::string::npos)
        {
            if (extension[0] == '.')
            {
                return path + extension;
            }
            else
            {
                return path + "." + extension;
            }
        }
        else
        {
            if (extension[0] == '.')
            {
                return path.substr(0, lastDotPos) + extension;
            }
            else
            {
                return path.substr(0, lastDotPos + 1) + extension;
            }
        }
    }
}

bool Path::HasExtension(const std::string& path)
{
    std::string::size_type lastDotPos = path.rfind('.');
    if (lastDotPos != std::string::npos)
    {
        std::string::size_type lastColon = path.find(':', lastDotPos + 1);
        std::string::size_type lastDirSep = path.find('/', lastDotPos + 1);
        if (lastColon > lastDotPos || lastDirSep > lastDotPos)
        {
            return false;
        }
        else if (lastDotPos < path.length() - 1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

std::string Path::GetExtension(const std::string& path)
{
    std::string::size_type lastDotPos = path.rfind('.');
    if (lastDotPos != std::string::npos)
    {
        if (path.find('/', lastDotPos + 1) != std::string::npos)
        {
            return std::string();
        }
        else
        {
            return path.substr(lastDotPos);
        }
    }
    else
    {
        return std::string();
    }
}

std::string Path::GetFileName(const std::string& path)
{
    if (path.empty())
    {
        return std::string();
    }
    else
    {
        char lastChar = path[path.length() - 1];
        if (lastChar == '/' || lastChar == ':')
        {
            return std::string();
        }
        else
        {
            std::string::size_type lastDirSepPos = path.rfind('/');
            if (lastDirSepPos != std::string::npos)
            {
                return path.substr(lastDirSepPos + 1);
            }
            else
            {
                return path;
            }
        }
    }
}

std::string Path::GetFileNameWithoutExtension(const std::string& path)
{
    std::string fileName = GetFileName(path);
    std::string::size_type lastDotPos = fileName.rfind('.');
    if (lastDotPos != std::string::npos)
    {
        return fileName.substr(0, lastDotPos);
    }
    else
    {
        return fileName;
    }
}


std::string Path::GetDirectoryName(const std::string& path)
{
    if (path.empty())
    {
        return std::string();
    }
    else if (path.length() == 3 && std::isalpha(path[0]) && path[1] == ':' && path[2] == '/')
    {
        return std::string();
    }
    else
    {
        std::string::size_type lastDirSepPos = path.rfind('/');
        if (lastDirSepPos != std::string::npos)
        {
            return path.substr(0, lastDirSepPos);
        }
        else
        {
            return std::string();
        }
    }
}

std::string Path::Combine(const std::string& path1, const std::string& path2)
{
    if (path1.empty())
    {
        return path2;
    }
    else if (path2.empty())
    {
        return path1;
    }
    else
    {
        if (IsAbsolute(path2))
        {
            return path2;
        }
        else
        {
            std::string result = path1;
            if (result[result.length() - 1] != '/')
            {
                result.append(1, '/');
            }
            result.append(path2);
            return result;
        }
    }
}

bool Path::IsAbsolute(const std::string& path)
{
    if (path.empty())
    {
        return false;
    }
    else
    {
        if (path[0] == '/')
        {
            return true;
        }
        else if (std::isalpha(path[0]) && path.length() > 2 && path[1] == ':' && path[2] == '/')
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

bool Path::IsRelative(const std::string& path)
{
    return !IsAbsolute(path);
}

std::string GetFullPath(const std::string& path)
{
    std::string p = Path::MakeCanonical(path);
    if (Path::IsRelative(p))
    {
        p = GetCurrentWorkingDirectory();
        p.append(1, '/');
        p.append(Path::MakeCanonical(path));
    }
    std::vector<std::string> components = Split(p, '/');
    int w = 0;
    int n = int(components.size());
    for (int i = 0; i < n; ++i)
    {
        const std::string& c = components[i];
        if (i == 0 || (!c.empty() && c != "."))
        {
            if (c == "..")
            {
                --w;
                if (w < 0)
                {
                    throw InvalidPathException("path '" + path + "' is invalid");
                }
            }
            else
            {
                if (w != i)
                {
                    components[w] = components[i];
                }
                ++w;
            }
        }
    }
    if (w == 0)
    {
        return "/";
    }
    else if (w == 1)
    {
        const std::string& p = components[0];
        if (p.length() == 2 && std::isalpha(p[0]) && p[1] == ':')
        {
            return p + "/";
        }
    }
    std::string result;
    for (int i = 0; i < w; ++i)
    {
        if (i != 0)
        {
            result.append(1, '/');
        }
        result.append(components[i]);
    }
    return result;
}

} }
