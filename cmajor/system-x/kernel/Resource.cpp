// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Resource.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/IO.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Processor.hpp>

namespace cmsx::kernel {

using namespace soulng::util;

Resource::Resource(int32_t fd_, int64_t start_, int64_t length_) : File("RESOURCE"), fd(fd_), start(start_), length(length_)
{
}

void Resource::Close(cmsx::kernel::Process* process)
{
    cmsx::kernel::Close(process, fd);
}

int32_t OpenResource(Process* process, uint64_t resourceNameAddr)
{
    if (resourceNameAddr == 0)
    {
        throw SystemError(EPARAM, "resource name is null");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string resourceName = ReadString(process, resourceNameAddr, mem);
    cmsx::object::SymbolTable* symbolTable = process->GetSymbolTable();
    cmsx::object::Symbol* symbol = symbolTable->GetSymbol("@resource." + resourceName);
    if (symbol)
    {
        int64_t start = symbol->Start();
        int64_t length = symbol->Length();
        std::unique_ptr<Resource> resource(new Resource(cmsx::kernel::Open(process, process->FilePath(), static_cast<int32_t>(OpenFlags::read), 0), start, length));
        ProcessFileTable& fileTable = process->GetFileTable();
        cmsx::kernel::Seek(process, resource->Fd(), resource->Start(), static_cast<int32_t>(Origin::seekSet));
        return fileTable.AddFile(resource.release());
    }
    else
    {
        throw SystemError(ENOTFOUND, "resource '" + resourceName + "' not found");
    }
}

void CloseResource(Process* process, int32_t rd)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(rd);
    fileTable.CloseFile(rd, process);
    delete file;
}

int64_t GetResourceSize(Process* process, int32_t rd)
{
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(rd);
    if (file->IsResource())
    {
        Resource* resource = static_cast<Resource*>(file);
        return resource->Length();
    }
    else
    {
        throw SystemError(EBADF, std::to_string(rd) + " is not a resource descriptor");
    }
}

void ReadResource(Process* process, int32_t rd, int64_t offset, int64_t length, int64_t bufferAddr)
{
    Resource* resource = nullptr;
    ProcessFileTable& fileTable = process->GetFileTable();
    File* file = fileTable.GetFile(rd);
    if (file->IsResource())
    {
        resource = static_cast<Resource*>(file);
    }
    else
    {
        throw SystemError(EBADF, std::to_string(rd) + " is not a resource descriptor");
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    if (offset != 0)
    {
        cmsx::kernel::Seek(process, resource->Fd(), resource->Start() + offset, static_cast<int32_t>(Origin::seekSet));
    }
    int64_t bytesRead = cmsx::kernel::Read(process, resource->Fd(), bufferAddr, length);
    length -= bytesRead;
    bufferAddr += bytesRead;
    while (length > 0)
    {
        bytesRead = cmsx::kernel::Read(process, resource->Fd(), bufferAddr, length);
        length -= bytesRead;
        bufferAddr += bytesRead;
    }
}

} // namespace cmsx::kernel
