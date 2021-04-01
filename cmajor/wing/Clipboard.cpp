// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Clipboard.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Random.hpp>
#include <thread>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

ClipboardFormat::ClipboardFormat(int formatNumber_) : formatName(), formatNumber(formatNumber_)
{
}

ClipboardFormat::ClipboardFormat(const std::string& formatName_) : formatName(formatName_), formatNumber()
{
    std::u16string wformatName = ToUtf16(formatName);
    formatNumber = static_cast<int>(RegisterClipboardFormat((LPCWSTR)wformatName.c_str()));
}

MemoryHandle::MemoryHandle() : handle(nullptr)
{
}

MemoryHandle::MemoryHandle(HGLOBAL handle_) : handle(handle_), memory(GlobalLock(handle))
{
    if (!memory)
    {
        throw WindowsException(GetLastError());
    }
}

MemoryHandle::MemoryHandle(MemoryHandle&& that) noexcept : handle(that.handle)
{
    that.handle = nullptr;
}

MemoryHandle& MemoryHandle::operator=(MemoryHandle&& that) noexcept
{
    std::swap(handle, that.handle);
    return *this;
}

MemoryHandle::~MemoryHandle()
{
    if (handle)
    {
        GlobalUnlock(handle);
    }
}

MemoryHandle MemoryHandle::AllocateMemory(int64_t size)
{
    HGLOBAL handle = GlobalAlloc(GHND, static_cast<size_t>(size));
    if (handle)
    {
        return MemoryHandle(handle);
    }
    else
    {
        throw WindowsException(GetLastError());
    }
}

int64_t MemoryHandle::Size() const
{
    int64_t size = static_cast<int64_t>(GlobalSize(handle));
    if (!size)
    {
        throw WindowsException(GetLastError());
    }
    return size;
}

ClipboardListener::ClipboardListener(HWND handle_) : handle(handle_)
{
    bool result = AddClipboardFormatListener(handle);
    if (!result)
    {
        throw WindowsException(GetLastError());
    }
}

ClipboardListener::~ClipboardListener()
{
    if (handle)
    {
        RemoveClipboardFormatListener(handle);
    }
}

Clipboard::Clipboard(HWND handle_) : handle(handle_)
{
    int maxWait = 100;
    for (int i = 0; i < 5; ++i)
    {
        bool succeeded = OpenClipboard(handle);
        if (succeeded)
        {
            return;
        }
        else if (i < 4)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(Random() % maxWait));
            maxWait *= 2;
        }
    }
    throw WindowsException(GetLastError());
}

Clipboard::~Clipboard()
{
    CloseClipboard();
}

bool Clipboard::IsClipboardFormatAvailable(const ClipboardFormat& format)
{
    return ::IsClipboardFormatAvailable(format.Number());
}

void Clipboard::SetEmpty()
{
    bool result = EmptyClipboard();
    if (!result)
    {
        throw WindowsException(GetLastError());
    }
}

MemoryHandle Clipboard::GetData(const ClipboardFormat& format)
{
    HANDLE handle = GetClipboardData(format.Number());
    if (!handle)
    {
        throw WindowsException(GetLastError());
    }
    return MemoryHandle(handle);
}

void Clipboard::SetData(const ClipboardFormat& format, const MemoryHandle& memoryHandle)
{
    HANDLE hnd = SetClipboardData(format.Number(), memoryHandle.GetHandle());
    if (!hnd)
    {
        throw WindowsException(GetLastError());
    }
}

std::u32string Clipboard::GetStringData(const ClipboardFormat& format) 
{
    MemoryHandle memoryHandle = GetData(format);
    int64_t size = memoryHandle.Size();
    if (format.Number() == textClipboardFormat)
    {
        std::unique_ptr<char[]> ptr(new char[size]);
        std::strcpy(ptr.get(), static_cast<const char*>(memoryHandle.Memory()));
        std::string str(ptr.get());
        return ToUtf32(str);
    }
    else if (format.Number() == unicodeTextClipboardFormat)
    {
        std::unique_ptr<char16_t[]> ptr(new char16_t[size / sizeof(char16_t)]);
        wcscpy((wchar_t*)ptr.get(), static_cast<const wchar_t*>(memoryHandle.Memory()));
        return ToUtf32(std::u16string(ptr.get()));
    }
    else
    {
        std::unique_ptr<char32_t[]> ptr(new char32_t[size / sizeof(char32_t)]);
        std::memcpy(ptr.get(), memoryHandle.Memory(), size);
        return std::u32string(ptr.get());
    }
}

void Clipboard::SetStringData(const ClipboardFormat& format, const std::u32string& str)
{
    if (format.Number() == textClipboardFormat)
    {
        std::string s = ToUtf8(str);
        MemoryHandle memoryHandle = MemoryHandle::AllocateMemory(s.length() + 1);
        strcpy(static_cast<char*>(memoryHandle.Memory()), s.c_str());
        SetData(format, memoryHandle);
    }
    else if (format.Number() == unicodeTextClipboardFormat)
    {
        std::u16string s = ToUtf16(str);
        MemoryHandle memoryHandle = MemoryHandle::AllocateMemory((s.length() + 1) * sizeof(char16_t));
        wcscpy(static_cast<wchar_t*>(memoryHandle.Memory()), (const wchar_t*)s.c_str());
        SetData(format, memoryHandle);
    }
    else
    {
        MemoryHandle memoryHandle = MemoryHandle::AllocateMemory((str.length() + 1) * sizeof(char32_t));
        std::memcpy(memoryHandle.Memory(), str.c_str(), (str.length() + 1) * sizeof(char32_t));
        SetData(format, memoryHandle);
    }
}

} } // cmajor::wing