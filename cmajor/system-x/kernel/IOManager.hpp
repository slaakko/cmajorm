// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_IO_MANAGER_INCLUDED
#define CMSX_KERNEL_IO_MANAGER_INCLUDED
#include <system-x/kernel/Error.hpp>
#include <system-x/machine/Machine.hpp>

namespace cmsx::kernel {

class Block;

enum class RequestKind : int32_t
{
    read, write
};

class CMSX_KERNEL_API IORequest
{
public:
    IORequest(RequestKind kind_, int32_t id_, int32_t hostFileId_, Block* block_);
    RequestKind Kind() const { return kind; }
    int32_t Id() const { return id; }
    int32_t HostFileId() const { return hostFileId; }
    Block* GetBlock() const { return block; }
    void SetOverlapped(void* overlapped_) { overlapped = overlapped_; }
    void* Overlapped() const { return overlapped; }
    int64_t BytesTransferred() const { return bytesTransferred; }
    void SetBytesTransferred(int64_t bytesTransferred_) { bytesTransferred = bytesTransferred_; }
    bool Failed() const { return error.ErrorCode() != 0; }
    const SystemError& Error() const { return error; }
    void SetError(const SystemError& error_);
private:
    RequestKind kind;
    int32_t id;
    int32_t hostFileId;
    Block* block;
    SystemError error;
    void* overlapped;
    int64_t bytesTransferred;
};

CMSX_KERNEL_API int32_t Read(int32_t hostFileId, Block* block);
CMSX_KERNEL_API int32_t Write(int32_t hostFileId, Block* block);
CMSX_KERNEL_API IORequest* GetRequest(int32_t requestId);
CMSX_KERNEL_API void DeleteRequest(int32_t requestId);
CMSX_KERNEL_API void InitIOManager();
CMSX_KERNEL_API void DoneIOManager();
CMSX_KERNEL_API void StartIOManager();
CMSX_KERNEL_API void StopIOManager();
CMSX_KERNEL_API void SetIOManagerMachine(cmsx::machine::Machine* machine);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_IO_MANAGER_INCLUDED
