// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/IO.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/machine/Machine.hpp>

namespace cmsx::kernel {

std::vector<std::uint8_t> MakeStringBuffer(const std::string& str, int64_t maxLength)
{
    std::vector<std::uint8_t> buffer;
    for (char c : str)
    {
        if (buffer.size() >= maxLength - 1)
        {
            break;
        }
        buffer.push_back(static_cast<uint8_t>(c));
    }
    buffer.push_back(0);
    return buffer;
}

std::vector<uint8_t> ReadProcessMemory(Process* process, int64_t addr, int64_t count)
{
    std::vector<uint8_t> buffer;
    uint64_t rv = process->RV();
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    for (int64_t i = 0; i < count; ++i)
    {
        try
        {
            buffer.push_back(mem.ReadByte(rv, addr + i, cmsx::machine::Protection::read));
        }
        catch (const std::exception& ex)
        {
            throw SystemError(EMEMORYACCESS, ex.what());
        }
    }
    return buffer;
}

void WriteProcessMemory(Process* process, int64_t addr, const std::vector<uint8_t>& buffer)
{
    uint64_t rv = process->RV();
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    int64_t count = buffer.size();
    for (int64_t i = 0; i < count; ++i)
    {
        try
        {
            mem.WriteByte(rv, addr + i, buffer[i], cmsx::machine::Protection::write);
        }
        catch (const std::exception& ex)
        {
            throw SystemError(EMEMORYACCESS, ex.what());
        }
    }
}

int64_t Write(Process* process, int32_t fd, int64_t bufferAddr, int64_t count)
{
    if (bufferAddr == 0)
    {
        throw SystemError(EPARAM, "buffer is null");
    }
    if (count < 0)
    {
        throw SystemError(EPARAM, "count is negative");
    }
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(fd);
    std::vector<uint8_t> buffer = ReadProcessMemory(process, bufferAddr, count);
    return file->Write(buffer);
}

int64_t Read(Process* process, int32_t fd, int64_t bufferAddr, int64_t count)
{
    if (bufferAddr == 0)
    {
        throw SystemError(EPARAM, "buffer is null");
    }
    if (count < 0)
    {
        throw SystemError(EPARAM, "count is negative");
    }
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(fd);
    std::vector<uint8_t> buffer = file->Read(count);
    WriteProcessMemory(process, bufferAddr, buffer);
    return buffer.size();
}

} // namespace cmsx::kernel
