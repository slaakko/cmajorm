// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Mount.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/Fs.hpp>
#include <system-x/kernel/HostFs.hpp>
#include <system-x/kernel/Kernel.hpp>
#include <system-x/kernel/Debug.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Util.hpp>

namespace cmsx::kernel {

using namespace soulng::util;

MountTable::MountTable(cmsx::machine::Machine* machine_) : machine(machine_), nextFileSystemId(3)
{
}

void MountTable::MapDrive(const std::string& drive, const std::string& mountedPath)
{
    driveMap[drive] = mountedPath;
}

std::string MountTable::GetMappedDrivePrefix(std::string& driveStr) const
{
    auto it = driveMap.find(driveStr);
    if (it != driveMap.cend())
    {
        return it->second;
    }
    else
    {
        return driveStr;
    }
}

std::string MountTable::MapPath(const std::string& windowsFilePath) const
{
    std::string mappedPath;
    std::string fullPath = GetFullPath(windowsFilePath);
    std::vector<std::string> components = Split(fullPath, '/');
    if (!components.empty())
    {
        mappedPath = GetMappedDrivePrefix(components[0]);
    }
    for (int i = 1; i < components.size(); ++i)
    {
        mappedPath = Path::Combine(mappedPath, components[i]);
    }
    return mappedPath;
}

void MountTable::AddFilesystem(Filesystem* filesystem)
{
    filesystem->SetMachine(machine);
    Filesystem* prev = GetFilesystem(filesystem->Id());
    if (prev)
    {
        throw SystemError(EFAIL, "file system " + std::to_string(filesystem->Id()) + " already mounted", __FUNCTION__);
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

void MountHostDir(const std::string& hostPath, const std::string& dirPath, cmsx::machine::Process* process, int32_t mode)
{
    Filesystem* fs = GetFs(rootFSNumber);
    std::string absoluteHostPath = GetFullPath(hostPath);
    if (!PathExists(absoluteHostPath))
    {
        throw SystemError(EPARAM, "could not mount: host path '" + absoluteHostPath + "' not found", __FUNCTION__);
    }
    INodePtr dirINode = PathToINode(dirPath, fs, process, PathToINodeFlags::ignoreMountPoint);
    if (!dirINode.Get())
    {
        dirINode = MakeDirectory(dirPath, fs, process, mode);
        dirINode.Get()->SetMountPoint();
        WriteINode(dirINode.Get(), process);
    }
    INodeKey mountPoint = dirINode.Get()->Key();
    MountTable& mountTable = Kernel::Instance().GetMountTable();
    HostFilesystem* mountedFs = new HostFilesystem(mountTable.NextFileSystemId(), absoluteHostPath);
    mountedFs->SetMountPoint(mountPoint);
    mountTable.AddFilesystem(mountedFs);
}

void MountDevDir(const std::string& dirPath, cmsx::machine::Process* process, int32_t mode)
{
    Filesystem* fs = GetFs(rootFSNumber);
    INodePtr dirINode = PathToINode(dirPath, fs, process, PathToINodeFlags::ignoreMountPoint);
    if (!dirINode.Get())
    {
        dirINode = MakeDirectory(dirPath, fs, process, mode);
        dirINode.Get()->SetMountPoint();
        WriteINode(dirINode.Get(), process);
    }
    INodeKey mountPoint = dirINode.Get()->Key();
    MountTable& mountTable = Kernel::Instance().GetMountTable();
    Filesystem* devFs = mountTable.GetFilesystem(devFSNumber);
    devFs->SetMountPoint(mountPoint);
}

void MapDrive(const std::string& drive, const std::string& mountedPath)
{
    MountTable& mountTable = Kernel::Instance().GetMountTable();
    mountTable.MapDrive(drive, mountedPath);
}

std::string MapPath(const std::string& windowsFilePath)
{
    MountTable& mountTable = Kernel::Instance().GetMountTable();
    return mountTable.MapPath(windowsFilePath);
}

} // namespace cmsx::kernel
