// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/FileStream.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <stdexcept>

namespace soulng { namespace util {

FileStream::FileStream(int handle) : filePath(), file(nullptr), needToClose(false)
{
    switch (handle)
    {
        case 0:
        {
            file = stdin;
            filePath = "STDIN";
            break;
        }
        case 1:
        {
            file = stdout;
            filePath = "STDOUT";
            break;
        }
        case 2:
        {
            file = stderr;
            filePath = "STDERR";
            break;
        }
        default:
        {
            throw std::runtime_error("invalid handle value");
        }
    }
}

FileStream::FileStream(const std::string& filePath_, OpenMode openMode) : filePath(filePath_), file(nullptr), needToClose(false)
{
    std::string mode;
    if ((openMode & OpenMode::read) != OpenMode::none && (openMode & OpenMode::write) == OpenMode::none)
    {
        mode += "r";
    }
    if ((openMode & OpenMode::write) != OpenMode::none)
    {
        mode += "w";
        if ((openMode & OpenMode::read) != OpenMode::none)
        {
            mode += "+";
        }
    }
    if ((openMode & OpenMode::append) != OpenMode::none)
    {
        if ((openMode & (OpenMode::read | OpenMode::write)) != OpenMode::none)
        {
            throw std::runtime_error("could not open file '" + std::string(filePath) + "': open mode not supported");
        }
        else
        {
            mode += "a";
        }
    }
    if ((openMode & OpenMode::binary) != OpenMode::none)
    {
        mode += "b";
    }
    file = std::fopen(filePath.c_str(), mode.c_str());
    if (!file)
    {
        throw std::runtime_error("could not open file '" + std::string(filePath) + "': " + PlatformStringToUtf8(std::strerror(errno)));
    }
    needToClose = true;
}

FileStream::~FileStream()
{
    if (file && needToClose)
    {
        std::fclose(file);
    }
}

int FileStream::ReadByte()
{
    int result = std::fgetc(file);
    if (result == EOF)
    {
        if (std::feof(file))
        {
            return -1;
        }
        else
        {
            throw std::runtime_error("could not read from file '" + filePath + "': " + PlatformStringToUtf8(std::strerror(errno)));
        }
    }
    SetPosition(Position() + 1);
    return result;
}

int64_t FileStream::Read(uint8_t* buf, int64_t count)
{
    int64_t result = std::fread(buf, 1, count, file);
    if (std::ferror(file))
    {
        throw std::runtime_error("could not read from file '" + filePath + "': " + PlatformStringToUtf8(std::strerror(errno)));
    }
    SetPosition(Position() + result);
    return result;
}

void FileStream::Write(uint8_t x)
{
    int result = std::fputc(x, file);
    if (result == EOF)
    {
        throw std::runtime_error("could not write to file '" + filePath + "': " + PlatformStringToUtf8(std::strerror(errno)));
    }
    SetPosition(Position() + 1);
}

void FileStream::Write(uint8_t* buf, int64_t count)
{
    int64_t result = std::fwrite(buf, 1, count, file);
    if (result != count)
    {
        throw std::runtime_error("could not write to file '" + filePath + "': " + PlatformStringToUtf8(std::strerror(errno)));
    }
    SetPosition(Position() + result);
}

void FileStream::Flush()
{
    int result = std::fflush(file);
    if (result != 0)
    {
        throw std::runtime_error("could not flush file '" + filePath + "': " + PlatformStringToUtf8(std::strerror(errno)));
    }
}

void FileStream::Seek(int64_t pos, Origin origin)
{
    int seekOrigin = SEEK_SET;
    switch (origin)
    {
        case Origin::seekCur:
        {
            seekOrigin = SEEK_CUR;
            break;
        }
        case Origin::seekEnd:
        {
            seekOrigin = SEEK_END;
            break;
        }
        case Origin::seekSet:
        {
            seekOrigin = SEEK_SET;
            break;
        }
    }
    int result = std::fseek(file, pos, seekOrigin);
    if (result != 0)
    {
        throw std::runtime_error("could not seek file '" + filePath + "': " + PlatformStringToUtf8(std::strerror(errno)));
    }
    switch (origin)
    {
        case Origin::seekCur:
        {
            SetPosition(Position() + pos);
            break;
        }
        case Origin::seekEnd:
        {
            SetPosition(Size() + pos);
            break;
        }
        case Origin::seekSet:
        {
            SetPosition(pos);
            break;
        }
    }
}

int64_t FileStream::Tell()
{
    int64_t result = std::ftell(file);
    if (result == -1)
    {
        throw std::runtime_error("could not tell file '" + filePath + "': " + PlatformStringToUtf8(std::strerror(errno)));
    }
    return result;
}

int64_t FileStream::Size() const
{
    return boost::filesystem::file_size(filePath);
}

} } // namespace soulng::util
