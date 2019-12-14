// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Io.hpp>
#include <cmajor/rt/Error.hpp>
#include <soulng/util/Error.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <atomic>
#include <mutex>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else 
#include <sys/types.h>
#include <unistd.h>
#endif

namespace cmajor { namespace rt {

using namespace soulng::unicode;

class FileTable
{
public:
    ~FileTable();
    static void Init();
    static void Done();
    static FileTable& Instance() { Assert(instance, "file table not initialized"); return *instance; }
    int32_t OpenFile(const char* filePath, OpenMode openMode);
    void CloseFile(int32_t fileHandle);
    void WriteFile(int32_t fileHandle, const uint8_t* buffer, int64_t count);
    void WriteByte(int32_t fileHandle, int8_t x);
    int64_t ReadFile(int32_t fileHandle, uint8_t* buffer, int64_t bufferSize);
    int32_t ReadByte(int32_t fileHandle);
    void SeekFile(int32_t fileHandle, int64_t pos, Origin origin);
    int64_t TellFile(int32_t fileHandle);
    void FlushStdoutAndStderr();
    void FlushAll();
private:
    static std::unique_ptr<FileTable> instance;
    const int32_t maxNoLockFileHandles = 256;
    std::vector<FILE*> files;
    std::vector<std::string> filePaths;
    std::unordered_map<int32_t, FILE*> fileMap;
    std::unordered_map<int32_t, std::string> filePathMap;
    Utf8ToUtf32Engine stdoutUtf8ToUtf32Engine;
    Utf8ToUtf32Engine stderrUtf8ToUtf32Engine;
    std::atomic<int32_t> nextFileHandle;
    std::atomic<int32_t> nextLineId;
    std::mutex mtx;
    bool stdinInUtf16Mode;
    std::string stdinBuf;
    bool stdoutInUtf16Mode;
    bool stderrInUtf16Mode;
    FileTable();
};

std::unique_ptr<FileTable> FileTable::instance;

void FileTable::Init()
{
    instance.reset(new FileTable());
}

void FileTable::Done()
{
    instance.reset();
}

FileTable::FileTable() : stdinInUtf16Mode(false), stdoutInUtf16Mode(false), stderrInUtf16Mode(false), nextFileHandle(3), nextLineId(1)
{
    files.resize(maxNoLockFileHandles);
    filePaths.resize(maxNoLockFileHandles);
    files[0] = stdin;
    files[1] = stdout;
    files[2] = stderr;
    std::fflush(stdout);
    std::fflush(stderr);
#ifdef _WIN32
    if (_isatty(0))
    {
        _setmode(0, _O_U16TEXT);
        stdinInUtf16Mode = true;
    }
    if (_isatty(1))
    {
        _setmode(1, _O_U16TEXT);
        stdoutInUtf16Mode = true;
    }
    if (_isatty(2))
    {
        _setmode(2, _O_U16TEXT);
        stderrInUtf16Mode = true;
    }
#endif
}

FileTable::~FileTable()
{
    std::fflush(stdout);
    std::fflush(stderr);
#ifdef _WIN32
    if (stdinInUtf16Mode)
    {
        _setmode(0, _O_TEXT);
    }
    if (stdoutInUtf16Mode)
    {
        _setmode(1, _O_TEXT);
    }
    if (stderrInUtf16Mode)
    {
        _setmode(2, _O_TEXT);
    }
#endif
}

int32_t FileTable::OpenFile(const char* filePath, OpenMode openMode)
{
    const char* mode = nullptr;
    if ((openMode & OpenMode::read) != OpenMode::none)
    {
        if ((openMode & (OpenMode::write | OpenMode::append)) != OpenMode::none)
        {
            throw FileSystemError("open mode not supported");
        }
        if ((openMode & OpenMode::binary) != OpenMode::none)
        {
            mode = "rb";
        }
        else
        {
            mode = "r";
        }
    }
    else if ((openMode & OpenMode::write) != OpenMode::none)
    {
        if ((openMode & (OpenMode::read | OpenMode::append)) != OpenMode::none)
        {
            throw FileSystemError("open mode not supported");
        }
        if ((openMode & OpenMode::binary) != OpenMode::none)
        {
            mode = "wb";
        }
        else
        {
            mode = "w";
        }
    }
    else if ((openMode & OpenMode::append) != OpenMode::none)
    {
        if ((openMode & (OpenMode::read | OpenMode::write)) != OpenMode::none)
        {
            throw FileSystemError("open mode not supported");
        }
        if ((openMode & OpenMode::binary) != OpenMode::none)
        {
            mode = "ab";
        }
        else
        {
            mode = "a";
        }
    }
    else
    {
        throw FileSystemError("open mode supported");
    }
    FILE* file = std::fopen(filePath, mode);
    if (!file)
    {
        throw FileSystemError("could not open file '" + std::string(filePath) + "': " + strerror(errno));
    }
    int32_t fileHandle = nextFileHandle++;
    if (fileHandle < maxNoLockFileHandles)
    {
        files[fileHandle] = file;
        filePaths[fileHandle] = filePath;
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        fileMap[fileHandle] = file;
        filePathMap[fileHandle] = filePath;
    }
    return fileHandle;
}

void FileTable::CloseFile(int32_t fileHandle)
{
    FILE* file = nullptr;
    std::string filePath;
    if (fileHandle < 0)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    else if (fileHandle < maxNoLockFileHandles)
    {
        file = files[fileHandle];
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = fileMap.find(fileHandle);
        if (it != fileMap.cend())
        {
            file = it->second;
        }
        else
        {
            throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
        }
    }
    if (!file)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    int result = fclose(file);
    if (result != 0)
    {
        std::string filePath;
        if (fileHandle < maxNoLockFileHandles)
        {
            filePath = filePaths[fileHandle];
        }
        else
        {
            filePath = filePathMap[fileHandle];
        }
        throw FileSystemError("could not close file '" + filePath + "': " + strerror(errno));
    }
}

void FileTable::WriteFile(int32_t fileHandle, const uint8_t* buffer, int64_t count)
{
    FILE* file = nullptr;
    if (fileHandle < 0)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    else if (fileHandle < maxNoLockFileHandles)
    {
        file = files[fileHandle];
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = fileMap.find(fileHandle);
        if (it != fileMap.cend())
        {
            file = it->second;
        }
        else
        {
            throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
        }
    }
    if (!file)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    int32_t result = 0;
    if (fileHandle == 1 && stdoutInUtf16Mode || fileHandle == 2 && stderrInUtf16Mode)
    {
        std::u32string utf32Chars;
        Utf8ToUtf32Engine* engine = nullptr;
        if (fileHandle == 1)
        {
            engine = &stdoutUtf8ToUtf32Engine;
        }
        else if (fileHandle == 2)
        {
            engine = &stderrUtf8ToUtf32Engine;
        }
        else
        {
            throw std::runtime_error("internal error: unicode conversion engine not set");
        }
        const uint8_t* e = buffer + count;
        for (const uint8_t* p = buffer; p != e; ++p)
        {
            engine->Put(*p);
            if (engine->ResulReady())
            {
                utf32Chars.append(1, engine->Result());
            }
        }
        std::u16string utf16Chars(ToUtf16(utf32Chars));
        if (!utf16Chars.empty())
        {
            result = int32_t(std::fwrite(utf16Chars.c_str(), sizeof(char16_t), utf16Chars.length(), file));
            count = utf16Chars.length();
        }
        else
        {
            result = 0;
            count = 0;
        }
    }
    else
    {
        result = int32_t(std::fwrite(buffer, 1, count, file));
    }
    if (result != count)
    {
        std::string filePath;
        if (fileHandle < maxNoLockFileHandles)
        {
            filePath = filePaths[fileHandle];
        }
        else
        {
            filePath = filePathMap[fileHandle];
        }
        throw FileSystemError("could not write to '" + filePath + "': " + strerror(errno));
    }
}

void FileTable::WriteByte(int32_t fileHandle, int8_t x)
{
    FILE* file = nullptr;
    if (fileHandle < 0)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    else if (fileHandle < maxNoLockFileHandles)
    {
        file = files[fileHandle];
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = fileMap.find(fileHandle);
        if (it != fileMap.cend())
        {
            file = it->second;
        }
        else
        {
            throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
        }
    }
    if (!file)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    int32_t result = 0;
    if (fileHandle == 1 && stdoutInUtf16Mode || fileHandle == 2 && stderrInUtf16Mode)
    {
        std::u32string utf32Chars;
        Utf8ToUtf32Engine* engine = nullptr;
        if (fileHandle == 1)
        {
            engine = &stdoutUtf8ToUtf32Engine;
        }
        else if (fileHandle == 2)
        {
            engine = &stderrUtf8ToUtf32Engine;
        }
        else
        {
            throw std::runtime_error("internal error: unicode conversion engine not set");
        }
        engine->Put(x);
        if (engine->ResulReady())
        {
            utf32Chars.append(1, engine->Result());
        }
        if (!utf32Chars.empty())
        {
            std::u16string utf16Chars(ToUtf16(utf32Chars));
            result = int32_t(std::fwrite(utf16Chars.c_str(), sizeof(char16_t), utf16Chars.length(), file));
            if (result != utf16Chars.length())
            {
                std::string filePath;
                if (fileHandle < maxNoLockFileHandles)
                {
                    filePath = filePaths[fileHandle];
                }
                else
                {
                    filePath = filePathMap[fileHandle];
                }
                throw FileSystemError("could not write to '" + filePath + "': " + strerror(errno));
            }
        }
    }
    else
    {
        result = std::fputc(x, file);
    }
    if (result == EOF)
    {
        std::string filePath;
        if (fileHandle < maxNoLockFileHandles)
        {
            filePath = filePaths[fileHandle];
        }
        else
        {
            filePath = filePathMap[fileHandle];
        }
        throw FileSystemError("could not write to '" + filePath + "': " + strerror(errno));
    }
}

int64_t FileTable::ReadFile(int32_t fileHandle, uint8_t* buffer, int64_t bufferSize)
{
    if (fileHandle == 0)
    {
        FlushStdoutAndStderr();
    }
    FILE* file = nullptr;
    if (fileHandle < 0)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    else if (fileHandle < maxNoLockFileHandles)
    {
        file = files[fileHandle];
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = fileMap.find(fileHandle);
        if (it != fileMap.cend())
        {
            file = it->second;
        }
        else
        {
            throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
        }
    }
    if (!file)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    int64_t result = 0;
    int64_t count = 0;
    if (fileHandle == 0 && stdinInUtf16Mode)
    {
        if (stdinBuf.empty())
        {
            std::unique_ptr<uint16_t[]> wbuf(new uint16_t[bufferSize]);
            result = int32_t(std::fread(wbuf.get(), sizeof(char16_t), bufferSize, file));
            count = bufferSize;
            std::u16string utf16Str;
            for (int i = 0; i < result; ++i)
            {
                utf16Str.append(1, wbuf.get()[i]);
            }
            stdinBuf = ToUtf8(utf16Str);
        }
        if (!stdinBuf.empty())
        {
            result = 0;
            count = bufferSize;
            uint8_t* p = buffer;
            while (result < count && !stdinBuf.empty())
            {
                uint8_t x = static_cast<uint8_t>(stdinBuf[0]);
                *p++ = x;
                stdinBuf.erase(stdinBuf.begin());
                ++result;
            }
        }
    }
    else
    {
        result = int32_t(std::fread(buffer, 1, bufferSize, file));
        count = bufferSize;
    }
    if (result < count)
    {
        if (std::ferror(file) != 0)
        {
            std::string filePath;
            if (fileHandle < maxNoLockFileHandles)
            {
                filePath = filePaths[fileHandle];
            }
            else
            {
                filePath = filePathMap[fileHandle];
            }
            throw FileSystemError("could not read from '" + filePath + "': " + strerror(errno));
        }
    }
    return result;
}

int32_t FileTable::ReadByte(int32_t fileHandle)
{
    if (fileHandle == 0)
    {
        FlushStdoutAndStderr();
    }
    FILE* file = nullptr;
    if (fileHandle < 0)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    else if (fileHandle < maxNoLockFileHandles)
    {
        file = files[fileHandle];
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = fileMap.find(fileHandle);
        if (it != fileMap.cend())
        {
            file = it->second;
        }
        else
        {
            throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
        }
    }
    if (!file)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    int32_t result = 0; 
    if (fileHandle == 0 && stdinInUtf16Mode)
    {
        if (stdinBuf.empty())
        {
            result = std::fgetwc(file);
            if (result == WEOF)
            {
                if (std::ferror(file) != 0)
                {
                    std::string filePath;
                    if (fileHandle < maxNoLockFileHandles)
                    {
                        filePath = filePaths[fileHandle];
                    }
                    else
                    {
                        filePath = filePathMap[fileHandle];
                    }
                    throw FileSystemError("could not read from '" + filePath + "': " + strerror(errno));
                }
                else
                {
                    result = -1;
                }
            }
            else
            {
                char16_t wc = result;
                std::u16string utf16Str(1, wc);
                stdinBuf = ToUtf8(utf16Str);
            }
        }
        if (!stdinBuf.empty())
        {
            result = static_cast<uint8_t>(stdinBuf[0]);
            stdinBuf.erase(stdinBuf.begin());
        }
    }
    else
    {
        result = std::fgetc(file);
        if (result == EOF)
        {
            if (std::ferror(file) != 0)
            {
                std::string filePath;
                if (fileHandle < maxNoLockFileHandles)
                {
                    filePath = filePaths[fileHandle];
                }
                else
                {
                    filePath = filePathMap[fileHandle];
                }
                throw FileSystemError("could not read from '" + filePath + "': " + strerror(errno));
            }
            else
            {
                result = -1;
            }
        }
    }
    return result;
}

void FileTable::SeekFile(int32_t fileHandle, int64_t pos, Origin origin)
{
    FILE* file = nullptr;
    if (fileHandle < 0)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    else if (fileHandle < maxNoLockFileHandles)
    {
        file = files[fileHandle];
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = fileMap.find(fileHandle);
        if (it != fileMap.cend())
        {
            file = it->second;
        }
        else
        {
            throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
        }
    }
    if (!file)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    int o = 0;
    switch (origin)
    {
        case Origin::seekSet: o = SEEK_SET; break;
        case Origin::seekCur: o = SEEK_CUR; break;
        case Origin::seekEnd: o = SEEK_END; break;
    }
#ifdef _WIN32
    int32_t result = _fseeki64(file, pos, o);
#else
    int32_t result = fseek(file, pos, o);
#endif
    if (result != 0)
    {
        std::string filePath;
        if (fileHandle < maxNoLockFileHandles)
        {
            filePath = filePaths[fileHandle];
        }
        else
        {
            filePath = filePathMap[fileHandle];
        }
        throw FileSystemError("could not seek '" + filePath + "': " + strerror(errno));
    }
}

int64_t FileTable::TellFile(int32_t fileHandle)
{
    FILE* file = nullptr;
    if (fileHandle < 0)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
    else if (fileHandle < maxNoLockFileHandles)
    {
        file = files[fileHandle];
    }
    else
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = fileMap.find(fileHandle);
        if (it != fileMap.cend())
        {
            file = it->second;
        }
        else
        {
            throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
        }
    }
    if (!file)
    {
        throw FileSystemError("invalid file handle " + std::to_string(fileHandle));
    }
#ifdef _WIN32
    int64_t result = _ftelli64(file);
#else
    int64_t result = ftell(file);
#endif
    if (result == -1)
    {
        std::string filePath;
        if (fileHandle < maxNoLockFileHandles)
        {
            filePath = filePaths[fileHandle];
        }
        else
        {
            filePath = filePathMap[fileHandle];
        }
        throw FileSystemError("could not tell file position of  '" + filePath + "': " + strerror(errno));
    }
    return result;
}

void FileTable::FlushStdoutAndStderr()
{
    std::fflush(stdout);
    std::fflush(stderr);
}

void FileTable::FlushAll()
{
    std::lock_guard<std::mutex> lock(mtx);
    for (FILE* file : files)
    {
        std::fflush(file);
    }
    for (const auto& p : fileMap)
    {
        std::fflush(p.second);
    }
    FlushStdoutAndStderr();
}

FileSystemError::FileSystemError(const std::string& message_) : std::runtime_error(message_)
{
}

void InitIo()
{
    FileTable::Init();
}

void DoneIo()
{
    FileTable::Done();
}

} }  // namespace cmajor::rt

extern "C" RT_API int32_t RtOpen(const char* filePath, OpenMode openMode)
{
    try
    {
        return cmajor::rt::FileTable::Instance().OpenFile(filePath, openMode);
    }
    catch (const cmajor::rt::FileSystemError& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

extern "C" RT_API int32_t RtClose(int32_t fileHandle)
{
    try
    {
        cmajor::rt::FileTable::Instance().CloseFile(fileHandle);
        return 0;
    }
    catch (const cmajor::rt::FileSystemError& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

extern "C" RT_API int32_t RtWrite(int32_t fileHandle, const uint8_t* buffer, int64_t count)
{
    try
    {
        cmajor::rt::FileTable::Instance().WriteFile(fileHandle, buffer, count);
        return 0;
    }
    catch (const cmajor::rt::FileSystemError& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
    catch (const soulng::unicode::UnicodeException& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

extern "C" RT_API int32_t RtWriteByte(int32_t fileHandle, uint8_t x)
{
    try
    {
        cmajor::rt::FileTable::Instance().WriteByte(fileHandle, x);
        return 0;
    }
    catch (const cmajor::rt::FileSystemError& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
    catch (const soulng::unicode::UnicodeException& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

extern "C" RT_API int64_t RtRead(int32_t fileHandle, uint8_t* buffer, int64_t bufferSize)
{
    try
    {
        return cmajor::rt::FileTable::Instance().ReadFile(fileHandle, buffer, bufferSize);
    }
    catch (const cmajor::rt::FileSystemError& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

extern "C" RT_API int32_t RtReadByte(int32_t fileHandle)
{
    try
    {
        return cmajor::rt::FileTable::Instance().ReadByte(fileHandle);
    }
    catch (const cmajor::rt::FileSystemError& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

extern "C" RT_API int32_t RtSeek(int32_t fileHandle, int64_t pos, Origin origin)
{
    try
    {
        cmajor::rt::FileTable::Instance().SeekFile(fileHandle, pos, origin);
        return 0;
    }
    catch (const cmajor::rt::FileSystemError& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

extern "C" RT_API int64_t RtTell(int32_t fileHandle)
{
    try
    {
        return cmajor::rt::FileTable::Instance().TellFile(fileHandle);
    }
    catch (const cmajor::rt::FileSystemError& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
}

extern "C" RT_API bool RtFileExists(const char* filePath)
{
    return boost::filesystem::exists(filePath);
}

extern "C" RT_API bool RtLastWriteTimeLess(const char* filePath1, const char* filePath2)
{
    boost::filesystem::path f1 = filePath1;
    boost::filesystem::path f2 = filePath2;
    if (boost::filesystem::last_write_time(f1) < boost::filesystem::last_write_time(f2))
    {
        return true;
    }
    return false;
}

extern "C" RT_API int32_t RtGetFileSize(const char* filePath, uint64_t* fileSize)
{
    try
    {
        boost::system::error_code ec;
        *fileSize = boost::filesystem::file_size(filePath, ec);
        if (ec)
        {
            throw std::runtime_error(ec.message());
        }
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
    return 0;
}

extern "C" RT_API int32_t RtRemoveFile(const char* filePath)
{
    try
    {
        boost::system::error_code ec;
        boost::filesystem::remove(filePath, ec);
        if (ec)
        {
            throw std::runtime_error(ec.message());
        }
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
    return 0;
}

extern "C" RT_API int32_t RtCopyFile(const char* sourceFilePath, const char* targetFilePath)
{
    try
    {
        boost::system::error_code ec;
        boost::filesystem::copy(sourceFilePath, targetFilePath, ec);
        if (ec)
        {
            throw std::runtime_error(ec.message());
        }
    }
    catch (const std::exception& ex)
    {
        return cmajor::rt::InstallError(ex.what());
    }
    return 0;
}

extern "C" RT_API void RtFlushAll()
{
    cmajor::rt::FileTable::Instance().FlushAll();
}
