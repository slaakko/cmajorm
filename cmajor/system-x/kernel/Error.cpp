// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Error.hpp>

namespace cmsx::kernel {

std::string ErrorCodeStr(int errorCode)
{
    switch (errorCode)
    {
        case EFAIL: return "EFAIL";
        case EBADF: return "EBADF";
        case EPARAM: return "EINVAL";
        case EMEMORYACCESS: return "EMEMORYACCESS";
        case ENOTFOUND: return "ENOTFOUND";
        case EALREADYEXISTS: return "EALREADYEXISTS";
        case ELIMITEXCEEDED: return "ELIMITEXCEEDED";
    }
    return std::string();
}

std::string ErrorMsg(int errorCode)
{
    switch (errorCode)
    {
        case EFAIL: return "Host system failure";
        case EBADF: return "Bad file descriptor";
        case EPARAM: return "Invalid parameter";
        case EMEMORYACCESS: return "Memory access error";
        case ENOTFOUND: return "Not found";
        case EALREADYEXISTS: return "Already exists";
        case ELIMITEXCEEDED: return "Resource limit exceeeded";
    }
    return std::string();
}

SystemError::SystemError() : std::runtime_error(""), errorCode(0), message()
{
}

SystemError::SystemError(int errorCode_, const std::string& message_) : 
    std::runtime_error(ErrorCodeStr(errorCode_) + "(" + std::to_string(errorCode_) + "): " + ErrorMsg(errorCode_) + ": " + message_), errorCode(errorCode_), message(message_)
{
}

} // namespace cmsx::kernel
