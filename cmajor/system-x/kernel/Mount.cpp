// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Mount.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/Fs.hpp>

namespace cmsx::kernel {

MountTable::MountTable(cmsx::machine::Machine* machine_) : machine(machine_)
{
}

void MountTable::AddFilesystem(Filesystem* filesystem)
{
    filesystem->SetMachine(machine);
    Filesystem* prev = GetFilesystem(filesystem->Id());
    if (prev)
    {
        throw SystemError(EFAIL, "file system " + std::to_string(filesystem->Id()) + " already mounted");
    }
    filesystems.push_back(std::unique_ptr<Filesystem>(filesystem));
    filesystemMap[filesystem->Id()] = filesystem;
}

Filesystem* MountTable::GetFilesystem(int32_t fsNumber) const
{
    auto it = filesystemMap.find(fsNumber);
    if (it != filesystemMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

} // namespace cmsx::kernel
