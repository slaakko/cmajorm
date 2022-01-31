// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_IO_INCLUDED
#define CMSX_KERNEL_IO_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Memory.hpp>
#include <stdint.h>
#include <string>
#include <vector>

namespace cmsx::kernel {

class Process;

CMSX_KERNEL_API std::vector<std::uint8_t> MakeStringBuffer(const std::string& str, int64_t maxLength);
CMSX_KERNEL_API std::vector<uint8_t> ReadProcessMemory(Process* process, int64_t addr, int64_t count);
CMSX_KERNEL_API void WriteProcessMemory(Process* process, int64_t addr, const std::vector<uint8_t>& buffer);
CMSX_KERNEL_API std::string ReadString(Process* process, int64_t address, cmsx::machine::Memory& mem);
CMSX_KERNEL_API int64_t WriteString(Process* process, const std::string& s, int64_t address, cmsx::machine::Memory& mem);
CMSX_KERNEL_API int32_t Create(Process* process, int64_t pathAddr, int32_t mode);
CMSX_KERNEL_API int32_t Open(Process* process, int64_t pathAddr, int32_t flags, int32_t mode);
CMSX_KERNEL_API void Close(Process* process, int32_t fd);
CMSX_KERNEL_API int64_t Write(Process* process, int32_t fd, int64_t bufferAddr, int64_t count);
CMSX_KERNEL_API int64_t Read(Process* process, int32_t fd, int64_t bufferAddr, int64_t count);
CMSX_KERNEL_API void Unlink(Process* process, int64_t pathAddr);
CMSX_KERNEL_API void Seek(Process* process, int32_t fd, int64_t offset, int32_t whence);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_IO_INCLUDED
