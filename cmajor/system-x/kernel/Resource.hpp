// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_RESOURCE_INCLUDED
#define CMSX_KERNEL_RESOURCE_INCLUDED
#include <system-x/kernel/File.hpp>
#include <soulng/util/MappedInputFile.hpp>

namespace cmsx::kernel {

class Process;

class CMSX_KERNEL_API Resource : public File
{
public:
    Resource(int32_t fd_, int64_t start_, int64_t length_);
    bool IsShareable() const override { return false; }
    void Close(cmsx::kernel::Process* process) override;
    bool IsReadable() const override { return false; }
    bool IsWritable() const override { return false; }
    bool IsConsole() const override { return false; }
    bool IsHostTextFile() const override { return false; }
    bool IsResource() const override { return true; }
    bool IsDecompressionFile() const override { return false; }
    bool HasColors() const override { return false; }
    int Columns() const override { return 0; }
    int Rows() const override { return 0; }
    int CursorX() const override { return 0; }
    int CursorY() const override { return 0; }
    int64_t Start() const { return start; }
    int64_t Length() const { return length; }
    int32_t Fd() const { return fd; }
private:
    int32_t fd;
    int64_t start;
    int64_t length;
};

CMSX_KERNEL_API int32_t OpenResource(Process* process, uint64_t resourceNameAddr);
CMSX_KERNEL_API void CloseResource(Process* process, int32_t rd);
CMSX_KERNEL_API int64_t GetResourceSize(Process* process, int32_t rd);
CMSX_KERNEL_API void ReadResource(Process* process, int32_t rd, int64_t offset, int64_t length, int64_t bufferAddr);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_RESOURCE_INCLUDED
