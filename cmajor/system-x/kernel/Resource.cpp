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

Resource::Resource(soulng::util::MappedInputFile* file_, int64_t start_, int64_t length_) : File(file_->FileName()), file(file_), start(start_), length(length_)
{
}

void Resource::Close(cmsx::machine::Process* process)
{
    file.reset();
}

uint8_t Resource::operator[](int64_t offset) const
{
    if (offset >= 0 && offset < length)
    {
        return static_cast<uint8_t>(*(file->Begin() + start + offset));
    }
    else
    {
        throw SystemError(EMEMORYACCESS, "invalid resource offset " + std::to_string(offset) + ", should be between 0 and " + std::to_string(length));
    }
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
        std::unique_ptr<MappedInputFile> file(new MappedInputFile(process->FilePath()));
        std::unique_ptr<Resource> resource(new Resource(file.release(), start, length));
        ProcessFileTable& fileTable = process->GetFileTable();
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
    for (int64_t i = 0; i < length; ++i)
    {
        int64_t ofs = offset + i;
        uint8_t value = (*resource)[ofs];
        mem.WriteByte(process->RV(), bufferAddr + i, value, cmsx::machine::Protection::write);
    }
}

} // namespace cmsx::kernel
