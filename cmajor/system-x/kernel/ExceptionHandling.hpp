// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_EXCEPTION_HANDLING_INCLUDED
#define CMSX_KERNEL_EXCEPTION_HANDLING_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <string>
#include <vector>

namespace cmsx::kernel {

class Process;

CMSX_KERNEL_API std::string GetStackTrace(Process* process);
CMSX_KERNEL_API void DispatchException(uint64_t exceptionAddress, uint64_t exceptionClassId, Process* process, uint64_t& setFP, uint64_t& setSP, uint64_t& setPC);
CMSX_KERNEL_API void ContinueExceptionDispatch(Process* process, uint64_t& setFP, uint64_t& setSP, uint64_t& setPC);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_EXCEPTION_HANDLING_INCLUDED
