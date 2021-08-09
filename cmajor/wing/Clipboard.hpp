// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_CLIPBOARD_INCLUDED
#define CMAJOR_WING_CLIPBOARD_INCLUDED
#include <wing/Wing.hpp>

namespace cmajor { namespace wing {

const int textClipboardFormat = 1;
const int unicodeTextClipboardFormat = 13;

class WING_API ClipboardFormat
{
public:
    ClipboardFormat(int formatNumber_);
    ClipboardFormat(const std::string& formatName_);
    int Number() const { return formatNumber; }
private:
    std::string formatName;
    int formatNumber;
};

class WING_API MemoryHandle
{
public:
    MemoryHandle();
    MemoryHandle(HGLOBAL handle_);
    MemoryHandle(const MemoryHandle&) = delete;
    MemoryHandle& operator=(const MemoryHandle&) = delete;
    MemoryHandle(MemoryHandle&& that) noexcept;
    MemoryHandle& operator=(MemoryHandle&& that) noexcept;
    ~MemoryHandle();
    static MemoryHandle AllocateMemory(int64_t size);
    HANDLE GetHandle() const { return handle; }
    void* Memory() const { return memory; }
    int64_t Size() const;
private:
    HGLOBAL handle;
    void* memory;
};

class WING_API ClipboardListener
{
public:
    ClipboardListener(HWND handle_);
    ~ClipboardListener();
private:
    HWND handle;
};

class WING_API Clipboard
{
public:
    Clipboard(HWND handle_);
    ~Clipboard();
    bool IsClipboardFormatAvailable(const ClipboardFormat& format);
    void SetEmpty();
    MemoryHandle GetData(const ClipboardFormat& format);
    void SetData(const ClipboardFormat& format, const MemoryHandle& memoryHandle);
    std::u32string GetStringData(const ClipboardFormat& format) ;
    void SetStringData(const ClipboardFormat& format, const std::u32string& str);
private:
    HWND handle;
};

} } // cmajor::wing

#endif // CMAJOR_WING_CLIPBOARD_INCLUDED
