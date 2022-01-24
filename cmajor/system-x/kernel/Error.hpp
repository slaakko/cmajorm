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

CMSX_KERNEL_API std::string ErrorCodeStr(int errorCode);
CMSX_KERNEL_API std::string ErrorMsg(int errorCode);

class CMSX_KERNEL_API SystemError : public std::runtime_error
{
public:
    SystemError();
    SystemError(int errorCode_, const std::string& message_);
    int ErrorCode() const { return errorCode; }
    const std::string& Message() const { return message; }
private:
    int errorCode;
    std::string message;
};

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_ERROR_INCLUDED
