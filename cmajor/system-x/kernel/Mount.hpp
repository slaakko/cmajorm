// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_MOUNT_INCLUDED
#define CMSX_KERNEL_MOUNT_INCLUDED
#include <system-x/kernel/INodeManager.hpp>
#include <system-x/machine/Machine.hpp>
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace cmsx::kernel {

class Filesystem;

const int32_t rootFSNumber = 0;
const int32_t hostFSNumber = 1;

class CMSX_KERNEL_API MountTable
{
public:
    MountTable(cmsx::machine::Machine* machine_);
    MountTable(const MountTable&) = delete;
    MountTable& operator=(const MountTable&) = delete;
    void AddFilesystem(Filesystem* filesystem);
    Filesystem* GetFilesystem(int32_t fsNumber) const;
    Filesystem* GetMountedFilesystem(INodeKey mountPoint) const;
    int32_t NextFileSystemId() { return nextFileSystemId++; }
    void ClearProcessData(cmsx::machine::Process* process);
    void MapDrive(const std::string& drive, const std::string& mountedPath);
    std::string GetMappedDrivePrefix(std::string& driveStr) const;
    std::string MapPath(const std::string& windowsFilePath) const;
private:
    int32_t nextFileSystemId;
    cmsx::machine::Machine* machine;
    std::vector<std::unique_ptr<Filesystem>> filesystems;
    std::map<int32_t, Filesystem*> filesystemMap;
    std::map<std::string, std::string> driveMap;
};

void Mount(const std::string& hostPath, const std::string& dirPath, cmsx::machine::Process* process);
void MapDrive(const std::string& drive, const std::string& mountedPath);
std::string MapPath(const std::string& windowsFilePath);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_MOUNT_INCLUDED
