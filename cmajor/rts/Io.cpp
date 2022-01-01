// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Io.hpp>
#include <cmajor/rts/Error.hpp>
#include <cmajor/rts/Debug.hpp>
#include <soulng/util/Error.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/filesystem.hpp>
#include <cstring>
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else 
#include <sys/types.h>
#include <unistd.h>
#endif

namespace cmajor { namespace rt {

using namespace soulng::unicode;

const int defaultStdBufferSize = 2048;

enum class FileTableFlags : int
{
    none = 0, stdInInUtf16Mode = 1 << 0, stdOutInUtf16Mode = 1 << 1, stdErrInUtf16Mode = 1 << 2
};

inline FileTableFlags operator|(FileTableFlags left, FileTableFlags right)
{
    return FileTableFlags(int(left) | int(right));
}

inline FileTableFlags operator&(FileTableFlags left, FileTableFlags right)
{
    return FileTableFlags(int(left) & int(right));
}

inline FileTableFlags operator~(FileTableFlags flags)
{
    return FileTableFlags(~int(flags));
}

void FlushStdOutAndStdErr();

class File
{
public:
    File();
    virtual ~File();
    virtual bool CanDispose() const { return true; }
    virtual bool Close(int32_t& errorStringHandle) = 0;
    virtual int64_t Write(const uint8_t* buffer, int64_t count, int32_t& errorStringHandle) = 0;
    virtual bool WriteByte(uint8_t x, int32_t& errorStringHandle) = 0;
    virtual int64_t Read(uint8_t* buffer, int64_t count, int32_t& errorStringHandle) = 0;
    virtual int32_t ReadByte() = 0;
    virtual bool Eof() const = 0;
    virtual bool GetError(int32_t& errorStringHandle) const = 0;
    virtual bool Seek(int64_t pos, Origin origin, int32_t& errorStringHandle) = 0;
    virtual int64_t Tell(int32_t& errorStringHandle) = 0;
    virtual bool Flush(int32_t& errorStringHandle) = 0;
};

File::File()
{
}

File::~File()
{
}

class StdInputFile : public File
{
public:
    StdInputFile();
    bool CanDispose() const override { return false; }
    bool Close(int32_t& errorStringHandle) override;
    int64_t Write(const uint8_t* buffer, int64_t count, int32_t& errorStringHandle) override;
    bool WriteByte(uint8_t x, int32_t& errorStringHandle) override;
    int64_t Read(uint8_t* buffer, int64_t count, int32_t& errorStringHandle) override;
    int32_t ReadByte() override;
    bool Eof() const override;
    bool GetError(int32_t& errorStringHandle) const override;
    bool Seek(int64_t pos, Origin origin, int32_t& errorStringHandle) override;
    int64_t Tell(int32_t& errorStringHandle) override;
    bool Flush(int32_t& errorStringHandle) override;
};

StdInputFile::StdInputFile()
{
}

bool StdInputFile::Close(int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    return true;
}

int64_t StdInputFile::Write(const uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot write to STDIN");
    return -1;
}

bool StdInputFile::WriteByte(uint8_t x, int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot write to STDIN");
    return false;
}

int64_t StdInputFile::Read(uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    FlushStdOutAndStdErr();
    if (cmajor::rt::IsCmdbSessionOpen())
    {
        return cmajor::rt::ReadBytesFromCmdbSession(buffer, count);
    }
    errorStringHandle = -1;
    int64_t result = std::fread(buffer, 1, count, stdin);
    if (ferror(stdin))
    {
        errorStringHandle = InstallError("could not read from STDIN: " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return -1;
    }
    return result;
}

int32_t StdInputFile::ReadByte()
{
    FlushStdOutAndStdErr();
    if (cmajor::rt::IsCmdbSessionOpen())
    {
        uint8_t buffer = '\0';
        if (cmajor::rt::ReadBytesFromCmdbSession(&buffer, 1) == 1)
        {
            return buffer;
        }
        else
        {
            return -1;
        }
    }
    int32_t result = std::fgetc(stdin);
    if (result == EOF)
    {
        return -1;
    }
    return result;
}

bool StdInputFile::Eof() const
{
    return std::feof(stdin);
}

bool StdInputFile::GetError(int32_t& errorStringHandle) const
{
    if (std::ferror(stdin))
    {
        errorStringHandle = InstallError("STDIN: " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return true;
    }
    return false;
}

bool StdInputFile::Seek(int64_t pos, Origin origin, int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot seek STDIN");
    return false;
}

int64_t StdInputFile::Tell(int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot tell STDIN");
    return -1;
}

bool StdInputFile::Flush(int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    return true;
}

class StdOutputFile : public File
{
public:
    StdOutputFile(FILE* file_, int handle_, const std::string& name_);
    bool CanDispose() const override { return false; }
    bool Close(int32_t& errorStringHandle) override;
    int64_t Write(const uint8_t* buffer, int64_t count, int32_t& errorStringHandle) override;
    bool WriteByte(uint8_t x, int32_t& errorStringHandle) override;
    int64_t Read(uint8_t* buffer, int64_t count, int32_t& errorStringHandle) override;
    int32_t ReadByte() override;
    bool Eof() const override;
    bool GetError(int32_t& errorStringHandle) const override;
    bool Seek(int64_t pos, Origin origin, int32_t& errorStringHandle) override;
    int64_t Tell(int32_t& errorStringHandle) override;
    bool Flush(int32_t& errorStringHandle) override;
private:
    FILE* file;
    int handle;
    std::string name;
};

StdOutputFile::StdOutputFile(FILE* file_, int handle_, const std::string& name_) : file(file_), handle(handle_), name(name_)
{
}

bool StdOutputFile::Close(int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    return true;
}

int64_t StdOutputFile::Write(const uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    if (cmajor::rt::IsCmdbSessionOpen())
    {
        cmajor::rt::WriteBytesToCmdbSession(handle, buffer, count);
        return count;
    }
    errorStringHandle = -1;
    int64_t result = std::fwrite(buffer, 1, count, file);
    if (result != count)
    {
        errorStringHandle = InstallError("could not write to : " + name + ": " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return -1;
    }
    return result;
}

bool StdOutputFile::WriteByte(uint8_t x, int32_t& errorStringHandle)
{
    if (cmajor::rt::IsCmdbSessionOpen())
    {
        uint8_t buffer = x;
        cmajor::rt::WriteBytesToCmdbSession(handle, &buffer, 1);
        return 1;
    }
    errorStringHandle = -1;
    int result = std::fputc(x, file);
    if (result == EOF)
    {
        errorStringHandle = InstallError("could not write to '" + name + "': " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return false;
    }
    return true;
}

int64_t StdOutputFile::Read(uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot read from " + name);
    return -1;
}

int32_t StdOutputFile::ReadByte()
{
    return -1;
}

bool StdOutputFile::Eof() const
{
    return std::feof(file);
}

bool StdOutputFile::GetError(int32_t& errorStringHandle) const
{
    if (std::ferror(file))
    {
        errorStringHandle = InstallError(name + ": " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return true;
    }
    return false;
}

bool StdOutputFile::Seek(int64_t pos, Origin origin, int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot seek " + name);
    return false;
}

int64_t StdOutputFile::Tell(int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot tell " + name);
    return -1;
}

bool StdOutputFile::Flush(int32_t& errorStringHandle)
{
    int result = fflush(file);
    if (result != 0)
    {
        errorStringHandle = InstallError("could not flush " + name + ": " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return false;
    }
    return true;
}

class StdUnicodeInputFile : public File
{
public:
    StdUnicodeInputFile();
    bool CanDispose() const override { return false; }
    bool Close(int32_t& errorStringHandle) override;
    int64_t Write(const uint8_t* buffer, int64_t count, int32_t& errorStringHandle) override;
    bool WriteByte(uint8_t x, int32_t& errorStringHandle) override;
    int64_t Read(uint8_t* buffer, int64_t count, int32_t& errorStringHandle) override;
    int32_t ReadByte() override;
    bool Eof() const override;
    bool GetError(int32_t& errorStringHandle) const override;
    bool Seek(int64_t pos, Origin origin, int32_t& errorStringHandle) override;
    int64_t Tell(int32_t& errorStringHandle) override;
    bool Flush(int32_t& errorStringHandle) override;
private:
    int bufferSize;
    void AllocateBuffer();
    std::string stdInBuf;
    std::unique_ptr<char16_t[]> utf16buffer;
};

StdUnicodeInputFile::StdUnicodeInputFile() : bufferSize(defaultStdBufferSize)
{
}

void StdUnicodeInputFile::AllocateBuffer()
{
    if (!utf16buffer)
    {
        utf16buffer.reset(new char16_t[bufferSize]);
    }
}

bool StdUnicodeInputFile::Close(int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    return true;
}

int64_t StdUnicodeInputFile::Write(const uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot write to STDIN");
    return -1;
}

bool StdUnicodeInputFile::WriteByte(uint8_t x, int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot write to STDIN");
    return false;
}

int64_t StdUnicodeInputFile::Read(uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    FlushStdOutAndStdErr();
    if (cmajor::rt::IsCmdbSessionOpen())
    {
        return cmajor::rt::ReadBytesFromCmdbSession(buffer, count);
    }
    AllocateBuffer();
    int64_t result = 0;
    if (stdInBuf.empty())
    {
        int64_t utf16result = std::fread(utf16buffer.get(), sizeof(char16_t), bufferSize, stdin);
        std::u16string utf16Str;
        for (int i = 0; i < utf16result; ++i)
        {
            utf16Str.append(1, utf16buffer[i]);
        }
        stdInBuf = ToUtf8(utf16Str);
    }
    if (!stdInBuf.empty())
    {
        result = 0;
        uint8_t* p = buffer;
        while (result < count && !stdInBuf.empty())
        {
            uint8_t x = static_cast<uint8_t>(stdInBuf[0]);
            *p++ = x;
            stdInBuf.erase(stdInBuf.begin());
            ++result;
        }
    }
    if (result < count)
    {
        if (ferror(stdin))
        {
            errorStringHandle = InstallError("could not read from STDIN: " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
            return -1;
        }
    }
    return result;
}

int32_t StdUnicodeInputFile::ReadByte()
{
    FlushStdOutAndStdErr();
    if (cmajor::rt::IsCmdbSessionOpen())
    {
        uint8_t buffer = '\0';
        if (cmajor::rt::ReadBytesFromCmdbSession(&buffer, 1) == 1)
        {
            return buffer;
        }
        else
        {
            return -1;
        }
    }
    if (stdInBuf.empty())
    {
        uint8_t buffer = '\0';
        int errorStringHandle = -1;
        int result = std::fgetwc(stdin);
        if (result == WEOF)
        {
            return -1;
        }
        else
        {
            std::u16string utf16Str(1, result);
            stdInBuf = ToUtf8(utf16Str);
        }
    }
    if (!stdInBuf.empty())
    {
        uint8_t x = static_cast<uint8_t>(stdInBuf[0]);
        stdInBuf.erase(stdInBuf.begin());
        return x;
    }
    return -1;
}

bool StdUnicodeInputFile::Eof() const
{
    return std::feof(stdin);
}

bool StdUnicodeInputFile::GetError(int32_t& errorStringHandle) const
{
    if (std::ferror(stdin))
    {
        errorStringHandle = InstallError("STDIN: " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return true;
    }
    return false;
}

bool StdUnicodeInputFile::Seek(int64_t pos, Origin origin, int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot seek STDIN");
    return false;
}

int64_t StdUnicodeInputFile::Tell(int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot tell STDIN");
    return -1;
}

bool StdUnicodeInputFile::Flush(int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    return true;
}

class StdUnicodeOutputFile : public File
{
public:
    StdUnicodeOutputFile(FILE* file_, int handle_, const std::string& name_);
    bool CanDispose() const override { return false; }
    bool Close(int32_t& errorStringHandle) override;
    int64_t Write(const uint8_t* buffer, int64_t count, int32_t& errorStringHandle) override;
    bool WriteByte(uint8_t x, int32_t& errorStringHandle) override;
    int64_t Read(uint8_t* buffer, int64_t count, int32_t& errorStringHandle) override;
    int32_t ReadByte() override;
    bool Eof() const override;
    bool GetError(int32_t& errorStringHandle) const override;
    bool Seek(int64_t pos, Origin origin, int32_t& errorStringHandle) override;
    int64_t Tell(int32_t& errorStringHandle) override;
    bool Flush(int32_t& errorStringHandle) override;
private:
    FILE* file;
    int handle;
    std::string name;
    Utf8ToUtf32Engine engine;
};

StdUnicodeOutputFile::StdUnicodeOutputFile(FILE* file_, int handle_, const std::string& name_) : file(file_), handle(handle_), name(name_)
{
}

bool StdUnicodeOutputFile::Close(int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    return true;
}

int64_t StdUnicodeOutputFile::Write(const uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    if (cmajor::rt::IsCmdbSessionOpen())
    {
        cmajor::rt::WriteBytesToCmdbSession(handle, buffer, count);
        return count;
    }
    errorStringHandle = -1;
    std::u32string utf32Chars;
    const uint8_t* e = buffer + count;
    for (const uint8_t* p = buffer; p != e; ++p)
    {
        engine.Put(*p);
        if (engine.ResulReady())
        {
            utf32Chars.append(1, engine.Result());
        }
    }
    std::u16string utf16Chars(ToUtf16(utf32Chars));
    if (!utf16Chars.empty())
    {
        int64_t utf16result = std::fwrite(utf16Chars.c_str(), sizeof(char16_t), utf16Chars.length(), file);
        if (utf16result != utf16Chars.length())
        {
            errorStringHandle = InstallError("could not write to : " + name + ": " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
            return -1;
        }
    }
    int64_t result = count;
    return result;
}

bool StdUnicodeOutputFile::WriteByte(uint8_t x, int32_t& errorStringHandle)
{
    if (cmajor::rt::IsCmdbSessionOpen())
    {
        uint8_t buffer = x;
        cmajor::rt::WriteBytesToCmdbSession(handle, &buffer, 1);
        return 1;
    }
    return Write(&x, 1, errorStringHandle) == 1;
}

int64_t StdUnicodeOutputFile::Read(uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot read from " + name);
    return -1;
}

int32_t StdUnicodeOutputFile::ReadByte()
{
    return -1;
}

bool StdUnicodeOutputFile::Eof() const
{
    return std::feof(file);
}

bool StdUnicodeOutputFile::GetError(int32_t& errorStringHandle) const
{
    if (std::ferror(file))
    {
        errorStringHandle = InstallError(name + ": " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return true;
    }
    return false;
}

bool StdUnicodeOutputFile::Seek(int64_t pos, Origin origin, int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot seek " + name);
    return false;
}

int64_t StdUnicodeOutputFile::Tell(int32_t& errorStringHandle)
{
    errorStringHandle = InstallError("cannot tell " + name);
    return -1;
}

bool StdUnicodeOutputFile::Flush(int32_t& errorStringHandle)
{
    int result = fflush(file);
    if (result != 0)
    {
        errorStringHandle = InstallError("could not flush " + name + ": " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return false;
    }
    return true;
}

class FileTable
{
public:
    static void Init();
    static void Done();
    static FileTable& Instance() { return *instance; }
    ~FileTable();
    bool GetFlag(FileTableFlags flag) const { return (flags & flag) != FileTableFlags::none; }
    void SetFlag(FileTableFlags flag) { flags = flags | flag; }
    File* StdInput() const { return stdInput.get(); }
    File* StdOutput() const { return stdOutput.get(); }
    File* StdError() const { return stdError.get(); }
    void FlushStdOutAndStdErr();
private:
    FileTable();
    FileTableFlags flags;
    std::unique_ptr<File> stdInput;
    std::unique_ptr<File> stdOutput;
    std::unique_ptr<File> stdError;
    static std::unique_ptr<FileTable> instance;
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

FileTable::FileTable() : flags(FileTableFlags::none)
{
    FlushStdOutAndStdErr();
#if defined( _WIN32) && !defined(__MINGW32__)
    if (_isatty(0))
    {
        _setmode(0, _O_U16TEXT);
        SetFlag(FileTableFlags::stdInInUtf16Mode);
        stdInput.reset(new StdUnicodeInputFile());
    }
    else
    {
        stdInput.reset(new StdInputFile());
    }
    if (_isatty(1))
    {
        _setmode(1, _O_U16TEXT);
        SetFlag(FileTableFlags::stdOutInUtf16Mode);
        stdOutput.reset(new StdUnicodeOutputFile(stdout, 1, "STDOUT"));
    }
    else
    {
        stdOutput.reset(new StdOutputFile(stdout, 1, "STDOUT"));
    }
    if (_isatty(2))
    {
        _setmode(2, _O_U16TEXT);
        SetFlag(FileTableFlags::stdErrInUtf16Mode);
        stdError.reset(new StdUnicodeOutputFile(stderr, 2, "STDERR"));
    }
    else
    {
        stdError.reset(new StdOutputFile(stderr, 2, "STDERR"));
    }
#else
    stdInput.reset(new StdInputFile());
    stdOutput.reset(new StdOutputFile(stdout, 1, "STDOUT"));
    stdError.reset(new StdOutputFile(stderr, 2, "STDERR"));
#endif
}

FileTable::~FileTable()
{
    FlushStdOutAndStdErr();
#if defined(_WIN32) && !defined(__MINGW32__)
    if (GetFlag(FileTableFlags::stdInInUtf16Mode))
    {
        _setmode(0, _O_TEXT);
    }
    if (GetFlag(FileTableFlags::stdOutInUtf16Mode))
    {
        _setmode(1, _O_TEXT);
    }
    if (GetFlag(FileTableFlags::stdErrInUtf16Mode))
    {
        _setmode(2, _O_TEXT);
    }
#endif
}

void FileTable::FlushStdOutAndStdErr()
{
    std::fflush(stdout);
    std::fflush(stderr);
}

void FlushStdOutAndStdErr()
{
    FileTable::Instance().FlushStdOutAndStdErr();
}

class RegularFile : public File
{
public:
    RegularFile(FILE* file_, const std::string& filePath_);
    ~RegularFile();
    bool Close(int32_t& errorStringHandle) override;
    int64_t Write(const uint8_t* buffer, int64_t count, int32_t& errorStringHandle) override;
    bool WriteByte(uint8_t x, int32_t& errorStringHandle) override;
    int64_t Read(uint8_t* buffer, int64_t count, int32_t& errorStringHandle) override;
    int32_t ReadByte() override;
    bool Eof() const override;
    bool GetError(int32_t& errorStringHandle) const override;
    bool Seek(int64_t pos, Origin origin, int32_t& errorStringHandle) override;
    int64_t Tell(int32_t& errorStringHandle) override;
    bool Flush(int32_t& errorStringHandle) override;
private:
    std::string filePath;
    FILE* file;
};

RegularFile::RegularFile(FILE* file_, const std::string& filePath_) : file(file_), filePath(filePath_)
{
}

RegularFile::~RegularFile()
{
    if (file)
    {
        int32_t errorStringHandle = -1;
        Close(errorStringHandle);
    }
}

bool RegularFile::Close(int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    int result = std::fclose(file);
    file = nullptr;
    if (result != 0)
    {
        errorStringHandle = InstallError("could not close file '" + std::string(filePath) + "': " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return false;
    }
    return true;
}

int64_t RegularFile::Write(const uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    int64_t result = std::fwrite(buffer, 1, count, file);
    if (result != count)
    {
        errorStringHandle = InstallError("could not write to file '" + filePath + "': " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return -1;
    }
    return result;
}

bool RegularFile::WriteByte(uint8_t x, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    int result = std::fputc(x, file);
    if (result == EOF)
    {
        errorStringHandle = InstallError("could not write to file '" + filePath + "': " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return false;
    }
    return true;
}

int64_t RegularFile::Read(uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    int64_t result = std::fread(buffer, 1, count, file);
    if (ferror(file))
    {
        errorStringHandle = InstallError("could not read from file '" + filePath + "': " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return -1;
    }
    return result;
}

int32_t RegularFile::ReadByte()
{
    int result = std::fgetc(file);
    if (result == EOF)
    {
        return -1;
    }
    return result;
}

bool RegularFile::Eof() const
{
    return std::feof(file);
}

bool RegularFile::GetError(int32_t& errorStringHandle) const
{
    if (std::ferror(file))
    {
        errorStringHandle = InstallError(filePath + ": " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return true;
    }
    return false;
}

bool RegularFile::Seek(int64_t pos, Origin origin, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
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
    int result = fseek(file, pos, seekOrigin);
    if (result != 0)
    {
        errorStringHandle = InstallError("could not seek file '" + filePath + "': " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return false;
    }
    return true;
}

int64_t RegularFile::Tell(int32_t& errorStringHandle)
{
    int64_t result = ftell(file);
    if (result == -1)
    {
        errorStringHandle = InstallError("could not tell file '" + filePath + "': " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return -1;
    }
    return result;
}

bool RegularFile::Flush(int32_t& errorStringHandle)
{
    int result = fflush(file);
    if (result != 0)
    {
        errorStringHandle = InstallError("could not flush file '" + filePath + "': " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return false;
    }
    return true;
}

void* OpenFile(const char* filePath, OpenMode openMode, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
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
            errorStringHandle = InstallError("could not open file '" + std::string(filePath) + "': open mode not supported");
            return nullptr;
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
    std::string nativeFilePath = soulng::util::Utf8StringToPlatformString(filePath);
    FILE* file = std::fopen(nativeFilePath.c_str(), mode.c_str());
    if (!file)
    {
        errorStringHandle = InstallError("could not open file '" + std::string(filePath) + "': " + soulng::util::PlatformStringToUtf8(std::strerror(errno)));
        return nullptr;
    }
    return new RegularFile(file, filePath);
}

void* OpenStdFile(int handle, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    switch (handle)
    {
        case 0: return FileTable::Instance().StdInput();
        case 1: return FileTable::Instance().StdOutput();
        case 2: return FileTable::Instance().StdError();
        default:
        {
            errorStringHandle = InstallError("invalid standard file handle " + std::to_string(handle));
            break;
        }
    }
    return nullptr;
}

bool CloseFile(void* fileHandle, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!fileHandle)
    {
        errorStringHandle = InstallError("could not close file: invalid file handle");
        return false;
    }
    File* file = static_cast<File*>(fileHandle);
    return file->Close(errorStringHandle);
}

bool DisposeFile(void* fileHandle, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!fileHandle)
    {
        errorStringHandle = InstallError("could not dispose file: invalid file handle");
        return false;
    }
    File* file = static_cast<File*>(fileHandle);
    if (file->CanDispose())
    {
        delete file;
    }
    return true;
}

int64_t WriteFile(void* fileHandle, const uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!fileHandle)
    {
        errorStringHandle = InstallError("could not write to file: invalid file handle");
        return -1;
    }
    File* file = static_cast<File*>(fileHandle);
    return file->Write(buffer, count, errorStringHandle);
}

bool WriteByte(void* fileHandle, int8_t x, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!fileHandle)
    {
        errorStringHandle = InstallError("could not write to file: invalid file handle");
        return false;
    }
    File* file = static_cast<File*>(fileHandle);
    return file->WriteByte(x, errorStringHandle);
}

int64_t ReadFile(void* fileHandle, uint8_t* buffer, int64_t bufferSize, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!fileHandle)
    {
        errorStringHandle = InstallError("could not read from file: invalid file handle");
        return -1;
    }
    File* file = static_cast<File*>(fileHandle);
    return file->Read(buffer, bufferSize, errorStringHandle);
}

int32_t ReadByte(void* fileHandle)
{
    if (!fileHandle)
    {
        return -1;
    }
    File* file = static_cast<File*>(fileHandle);
    return file->ReadByte();
}

bool Eof(void* fileHandle)
{
    if (!fileHandle) return true;
    File* file = static_cast<File*>(fileHandle);
    return file->Eof();
}

bool GetFileError(void* fileHandle, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!fileHandle)
    {
        errorStringHandle = InstallError("could get file error: invalid file handle");
        return false;
    }
    File* file = static_cast<File*>(fileHandle);
    return file->GetError(errorStringHandle);
}

bool SeekFile(void* fileHandle, int64_t pos, Origin origin, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!fileHandle)
    {
        errorStringHandle = InstallError("could not seek file: invalid file handle");
        return false;
    }
    File* file = static_cast<File*>(fileHandle);
    return file->Seek(pos, origin, errorStringHandle);
}

int64_t TellFile(void* fileHandle, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!fileHandle)
    {
        errorStringHandle = InstallError("could not tell file: invalid file handle");
        return -1;
    }
    File* file = static_cast<File*>(fileHandle);
    return file->Tell(errorStringHandle);
}

bool FlushFile(void* fileHandle, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    if (!fileHandle)
    {
        errorStringHandle = InstallError("could not flush file: invalid file handle");
        return false;
    }
    File* file = static_cast<File*>(fileHandle);
    return file->Flush(errorStringHandle);
}

} }  // namespace cmajor::rt

void* RtOpen(const char* filePath, cmajor::rt::OpenMode openMode, int32_t& errorStringHandle)
{
    return cmajor::rt::OpenFile(filePath, openMode, errorStringHandle);
}

void* RtOpenStdFile(int handle, int32_t& errorStringHandle)
{
    return cmajor::rt::OpenStdFile(handle, errorStringHandle);
}

bool RtClose(void* fileHandle, int32_t& errorStringHandle)
{
    return cmajor::rt::CloseFile(fileHandle, errorStringHandle);
}

bool RtDisposeFile(void* fileHandle, int32_t& errorStringHandle)
{
    return cmajor::rt::DisposeFile(fileHandle, errorStringHandle);
}

int64_t RtWrite(void* fileHandle, const uint8_t* buffer, int64_t count, int32_t& errorStringHandle)
{
    return cmajor::rt::WriteFile(fileHandle, buffer, count, errorStringHandle);
}

bool RtWriteByte(void* fileHandle, uint8_t x, int32_t& errorStringHandle)
{
    return cmajor::rt::WriteByte(fileHandle, x, errorStringHandle);
}

int64_t RtRead(void* fileHandle, uint8_t* buffer, int64_t bufferSize, int32_t& errorStringHandle)
{
    return cmajor::rt::ReadFile(fileHandle, buffer, bufferSize, errorStringHandle);
}

int32_t RtReadByte(void* fileHandle)
{
    return cmajor::rt::ReadByte(fileHandle);
}

bool RtEof(void* fileHandle)
{
    return cmajor::rt::Eof(fileHandle);
}

bool RtGetFileError(void* fileHandle, int32_t& errorStringHandle)
{
    return cmajor::rt::GetFileError(fileHandle, errorStringHandle);
}

bool RtSeek(void* fileHandle, int64_t pos, cmajor::rt::Origin origin, int32_t& errorStringHandle)
{
    return cmajor::rt::SeekFile(fileHandle, pos, origin, errorStringHandle);
}

int64_t RtTell(void* fileHandle, int32_t& errorStringHandle)
{
    return cmajor::rt::TellFile(fileHandle, errorStringHandle);
}

bool RtFlush(void* fileHandle, int32_t& errorStringHandle)
{
    return cmajor::rt::FlushFile(fileHandle, errorStringHandle);
}

bool RtFileExists(const char* filePath)
{
    return boost::filesystem::exists(filePath);
}

bool RtLastWriteTimeLess(const char* filePath1, const char* filePath2)
{
    boost::filesystem::path f1 = filePath1;
    boost::filesystem::path f2 = filePath2;
    if (boost::filesystem::last_write_time(f1) < boost::filesystem::last_write_time(f2))
    {
        return true;
    }
    return false;
}

int64_t RtGetFileSize(const char* filePath, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    boost::system::error_code ec;
    int64_t fileSize = boost::filesystem::file_size(filePath, ec);
    if (ec)
    {
        errorStringHandle = cmajor::rt::InstallError("could not get size of file '" + std::string(filePath) + "': " + soulng::util::PlatformStringToUtf8(ec.message()));
        return -1;
    }
    return fileSize;
}

bool RtRemoveFile(const char* filePath, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    boost::system::error_code ec;
    boost::filesystem::remove(filePath, ec);
    if (ec)
    {
        errorStringHandle = cmajor::rt::InstallError("could not remove file '" + std::string(filePath) + "': " + soulng::util::PlatformStringToUtf8(ec.message()));
        return false;
    }
    return true;
}

bool RtCopyFile(const char* sourceFilePath, const char* targetFilePath, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    boost::system::error_code ec;
    boost::filesystem::copy(sourceFilePath, targetFilePath, ec);
    if (ec)
    {
        errorStringHandle = cmajor::rt::InstallError("could not copy file '" + std::string(sourceFilePath) + "': " + soulng::util::PlatformStringToUtf8(ec.message()));
        return false;
    }
    return true;
}

bool RtMoveFile(const char* sourceFilePath, const char* targetFilePath, int32_t& errorStringHandle)
{
    errorStringHandle = -1;
    boost::system::error_code ec;
    boost::filesystem::rename(sourceFilePath, targetFilePath, ec);
    if (ec)
    {
        errorStringHandle = cmajor::rt::InstallError("could not move file '" + std::string(sourceFilePath) + "': " + soulng::util::PlatformStringToUtf8(ec.message()));
        return false;
    }
    return true;
}

bool RtIsConsoleHandle(int handle)
{
#ifdef _WIN32
    return _isatty(handle);
#else
    return isatty(handle);
#endif
}

extern "C" void InitIo()
{
    cmajor::rt::FileTable::Init();
}

extern "C" void DoneIo()
{
    cmajor::rt::FileTable::Done();
}
