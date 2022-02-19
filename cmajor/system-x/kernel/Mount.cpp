// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Mount.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/Fs.hpp>
#include <system-x/kernel/HostFs.hpp>
#include <system-x/kernel/Kernel.hpp>
#include <soulng/util/Path.hpp>

namespace cmsx::kernel {

using namespace soulng::util;

MountTable::MountTable(cmsx::machine::Machine* machine_) : machine(machine_), nextFileSystemId(2)
{
}

void MountTable::ClearProcessData(cmsx::machine::Process* process)
{
    for (auto& fs : filesystems)
    {
        fs->ClearProcessData(process);
    }
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

Filesystem* MountTable::GetMountedFilesystem(INodeKey mountPoint) const
{
    for (const auto& fs : filesystems)
    {
        if (fs->MountPoint() == mountPoint)
        {
            return fs.get();
        }
    }
    return nullptr;
}

void Mount(const std::string& hostPath, const std::string& dirPath, cmsx::machine::Process* process)
{
    Filesystem* fs = GetFs(rootFSNumber);
    std::string absoluteHostPath = GetFullPath(hostPath);
    if (!PathExists(absoluteHostPath))
    {
        throw SystemError(EPARAM, "could not mount: host path '" + absoluteHostPath + "' not found");
    }
    INodePtr dirINode = PathToINode(dirPath, fs, process, PathToINodeFlags::ignoreMountPoint);
    if (!dirINode.Get())
    {
        dirINode = MakeDirectory(dirPath, fs, process);
        dirINode.Get()->SetMountPoint();
        WriteINode(dirINode.Get(), process);
    }
    INodeKey mountPoint = dirINode.Get()->Key();
    MountTable& mountTable = Kernel::Instance().GetMountTable();
    HostFilesystem* mountedFs = new HostFilesystem(mountTable.NextFileSystemId(), absoluteHostPath);
    mountedFs->SetMountPoint(mountPoint);
    mountTable.AddFilesystem(mountedFs);
}

} // namespace cmsx::kernel