// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_HOST_FILE_MANAGER_INCLUDED
#define CMSX_KERNEL_HOST_FILE_MANAGER_INCLUDED
#include <system-x/kernel/File.hpp>
#include <system-x/machine/Machine.hpp>

namespace cmsx::kernel {

class CMSX_KERNEL_API HostFile
{
public:
    HostFile(int32_t id_, const std::string& filePath_, OpenFlags flags_);
    int32_t Id() const { return id; }
    const std::string& FilePath() const { return filePath; }
    bool IsOpened() const { return (openFlags & OpenFlags::opened) != OpenFlags::none; }
    void SetOpened() { openFlags = openFlags | OpenFlags::opened; }
    bool IsRandomAccess() const { return (openFlags & OpenFlags::random_access) != OpenFlags::none;}
    bool Truncate() const { return (openFlags & OpenFlags::truncate) != OpenFlags::none; }
    bool Create() const { return (openFlags & OpenFlags::create) != OpenFlags::none; }
    void* FileHandle() const { return fileHandle; }
    void SetFileHandle(void* fileHandle_) { fileHandle = fileHandle_; }
    OpenFlags GetOpenFlags() const { return openFlags; }
    void SetOpenFlags(OpenFlags flags) { openFlags = flags; }
    int32_t ReferenceCount() const { return referenceCount; }
    void IncrementReferenceCount() { ++referenceCount; }
    void DecrementReferenceCount() { --referenceCount; }
    int32_t OutstandingRequests() const { return outstandingRequests; }
    void IncrementOutstandingRequests() { ++outstandingRequests; }
    void DecrementOutstandingRequests() { --outstandingRequests; }
private:
    int32_t id;
    std::string filePath;
    OpenFlags openFlags;
    void* fileHandle;
    int32_t referenceCount;
    int32_t outstandingRequests;
};

CMSX_KERNEL_API int32_t OpenHostFile(const std::string& filePath, OpenFlags flags);
CMSX_KERNEL_API void CloseHostFile(int32_t hostFileId);
CMSX_KERNEL_API HostFile* GetHostFile(int32_t hostFileId);
CMSX_KERNEL_API void InitHostFileManager();
CMSX_KERNEL_API void DoneHostFileManager();
CMSX_KERNEL_API void StartHostFileManager();
CMSX_KERNEL_API void StopHostFileManager();
CMSX_KERNEL_API void SetHostFileManagerMachine(cmsx::machine::Machine* machine);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_HOST_FILE_MANAGER_INCLUDED
