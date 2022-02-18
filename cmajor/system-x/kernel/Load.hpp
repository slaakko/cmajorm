// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_LOAD_INCLUDED
#define CMSX_KERNEL_LOAD_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/object/BinaryFile.hpp>

namespace cmsx::kernel {

class Process;

class TextSegmentWriteProtectionGuard
{
public:
    TextSegmentWriteProtectionGuard(uint64_t rv_, cmsx::machine::Memory& mem_);
    ~TextSegmentWriteProtectionGuard();
private:
    uint64_t rv;
    cmsx::machine::Memory& mem;
};


CMSX_KERNEL_API void Load(Process* process, const std::vector<std::string>& args, const std::vector<std::string>& env, cmsx::machine::Machine& machine);
CMSX_KERNEL_API void Load(Process* process, cmsx::object::BinaryFile* binaryFile,
    const std::vector<std::string>& args, const std::vector<std::string>& env, cmsx::machine::Machine& machine, uint64_t rv, bool addRunnable);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_LOAD_INCLUDED
