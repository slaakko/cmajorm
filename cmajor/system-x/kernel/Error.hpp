// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_ERROR_INCLUDED
#define CMSX_KERNEL_ERROR_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <string>
#include <stdexcept>
#undef EBADF

namespace cmsx::kernel {

const int EFAIL = 1;
const int EBADF = 2;
const int EPARAM = 3;
const int EMEMORYACCESS = 4;
const int ENOTFOUND = 5;
const int EALREADYEXISTS = 6;
const int ELIMITEXCEEDED = 7;
const int ENOCHILD = 8;
const int EHOST = 9;
const int EPERMISSION = 10;
const int ESOCKET = 11;

CMSX_KERNEL_API std::string ErrorCodeStr(int errorCode);
CMSX_KERNEL_API std::string ErrorMsg(int errorCode);
CMSX_KERNEL_API std::string HostErrorMessage(uint64_t errorCode);
CMSX_KERNEL_API int64_t GetLastHostErrorCode();

class CMSX_KERNEL_API SystemError : public std::runtime_error
{
public:
    SystemError();
    SystemError(int errorCode_, const std::string& message_, const std::string& function_);
    int ErrorCode() const { return errorCode; }
    int64_t HostErrorCode() const { return hostErrorCode; }
    void SetHostErrorCode(int32_t hostErrorCode_) { hostErrorCode = hostErrorCode_; }
    const std::string& Message() const { return message; }
private:
    int errorCode;
    int64_t hostErrorCode;
    std::string message;
    std::string function;
};

CMSX_KERNEL_API void ThrowLastHostError(const std::string& filePath);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_ERROR_INCLUDED
