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
    Resource(soulng::util::MappedInputFile* file_, int64_t start_, int64_t length_);
    void Close(cmsx::machine::Process* process) override;
    bool IsReadable() const override { return false; }
    bool IsWritable() const override { return false; }
    bool IsConsole() const override { return false; }
    bool IsHostTextFile() const override { return false; }
    bool IsResource() const override { return true; }
    bool IsDecompressionFile() const override { return false; }
    bool HasColors() const override { return false; }
    int Columns() const override { return 0; }
    int Rows() const override { return 0; }
    int64_t Start() const { return start; }
    int64_t Length() const { return length; }
    uint8_t operator[](int64_t offset) const;
    const uint8_t* Address(int64_t offset) const;
private:
    std::unique_ptr<soulng::util::MappedInputFile> file;
    int64_t start;
    int64_t length;
};

CMSX_KERNEL_API int32_t OpenResource(Process* process, uint64_t resourceNameAddr);
CMSX_KERNEL_API void CloseResource(Process* process, int32_t rd);
CMSX_KERNEL_API int64_t GetResourceSize(Process* process, int32_t rd);
CMSX_KERNEL_API void ReadResource(Process* process, int32_t rd, int64_t offset, int64_t length, int64_t bufferAddr);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_RESOURCE_INCLUDED
