// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Compression.hpp>
#include <system-x/kernel/IO.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/machine/Processor.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Memory.hpp>
#include <soulng/util/DeflateStream.hpp>
#include <soulng/util/MemoryReader.hpp>
#include <algorithm>

namespace cmsx::kernel {

DecompressionFile::DecompressionFile(Process* process, uint64_t sourceBufferAddr, uint64_t count) : File("DECOMPRESSION")
{
    compressedData = ReadProcessMemory(process, sourceBufferAddr, count);
    soulng::util::MemoryReader reader(compressedData.data(), 8);
    soulng::util::MemoryStream compressedMemoryStream(compressedData.data() + 8, std::max(static_cast<int64_t>(0), static_cast<int64_t>(compressedData.size()) - 8));
    soulng::util::DeflateStream extractStream(soulng::util::CompressionMode::decompress, compressedMemoryStream);
    int64_t size = reader.ReadLong();
    for (int64_t i = 0; i < size; ++i)
    {
        int x = extractStream.ReadByte();
        if (x != -1)
        {
            decompressedMemoryStream.Write(static_cast<uint8_t>(x));
        }
    }
}

void DecompressionFile::Close(cmsx::kernel::Process* process)
{
    compressedData.clear();
    delete this;
}

void DecompressionFile::GetData(Process* process, uint64_t targetBufferAddr, uint64_t count)
{
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    mem.NCopy(decompressedMemoryStream.Content().data(), process->RV(), targetBufferAddr, count);
}

int32_t Decompress(Process* process, int64_t bufferAddr, int64_t count)
{
    DecompressionFile* file = new DecompressionFile(process, bufferAddr, count);
    return process->GetFileTable().AddFile(file);
}

int64_t GetDecompressedDataSize(Process* process, int32_t dd)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(dd);
    if (file->IsDecompressionFile())
    {
        DecompressionFile* decompressionFile = static_cast<DecompressionFile*>(file);
        return decompressionFile->Size();
    }
    else
    {
        throw SystemError(EBADF, std::to_string(dd) + " is not a decompression file descriptor");
    }
}

void GetDecompressedData(Process* process, int32_t dd, int64_t bufferAddr, int64_t count)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(dd);
    if (file->IsDecompressionFile())
    {
        DecompressionFile* decompressionFile = static_cast<DecompressionFile*>(file);
        decompressionFile->GetData(process, bufferAddr, count);
    }
    else
    {
        throw SystemError(EBADF, std::to_string(dd) + " is not a decompression file descriptor");
    }
}

void CloseDecompression(Process* process, int32_t dd)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(dd);
    if (file->IsDecompressionFile())
    {
        fileTable.CloseFile(dd, process);
    }
    else
    {
        throw SystemError(EBADF, std::to_string(dd) + " is not a decompression file descriptor");
    }
}

} // namespace cmsx::kernel
