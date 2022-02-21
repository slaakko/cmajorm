// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_COMPRESSION_INCLUDED
#define CMSX_KERNEL_COMPRESSION_INCLUDED
#include <system-x/kernel/File.hpp>
#include <soulng/util/MemoryStream.hpp>

namespace cmsx::kernel {

class CMSX_KERNEL_API DecompressionFile : public File
{
public:
    DecompressionFile(Process* process, uint64_t sourceBufferAddr, uint64_t count);
    void Close(cmsx::kernel::Process* process) override;
    bool IsReadable() const override { return false; }
    bool IsWritable() const override { return false; }
    bool IsConsole() const override { return false; }
    bool IsHostTextFile() const override { return false; }
    bool IsResource() const override { return false; }
    bool IsDecompressionFile() const override { return true; }
    bool HasColors() const override { return false; }
    int Columns() const override { return 0; }
    int Rows() const override { return 0; }
    int64_t Size() const { return decompressedMemoryStream.Content().size(); }
    void GetData(Process* process, uint64_t targetBufferAddr, uint64_t count);
private:
    std::vector<uint8_t> compressedData;
    soulng::util::MemoryStream decompressedMemoryStream;
};

CMSX_KERNEL_API int32_t Decompress(Process* process, int64_t bufferAddr, int64_t count);
CMSX_KERNEL_API int64_t GetDecompressedDataSize(Process* process, int32_t dd);
CMSX_KERNEL_API void GetDecompressedData(Process* process, int32_t dd, int64_t bufferAddr, int64_t count);
CMSX_KERNEL_API void CloseDecompression(Process* process, int32_t dd);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_COMPRESSION_INCLUDED
