// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Error.hpp>
#include <soulng/util/Unicode.hpp>
#include <Windows.h>

namespace cmsx::kernel {

using namespace soulng::unicode;

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

SystemError::SystemError() : std::runtime_error(""), errorCode(0), hostErrorCode(0), message()
{
}

SystemError::SystemError(int errorCode_, const std::string& message_) : 
    std::runtime_error(ErrorCodeStr(errorCode_) + "(" + std::to_string(errorCode_) + "): " + ErrorMsg(errorCode_) + ": " + message_), 
    errorCode(errorCode_), hostErrorCode(0), message(message_)
{
}

std::string HostErrorMessage(uint64_t errorCode)
{
    char16_t buf[4096];
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errorCode, LANG_SYSTEM_DEFAULT, (LPWSTR)buf, 4096, nullptr);
    return ToUtf8(buf);
}

int64_t GetLastHostErrorCode()
{
    return GetLastError();
}

void ThrowLastHostError()
{
    int64_t hostErrorCode = GetLastHostErrorCode();
    SystemError systemError(EHOST, HostErrorMessage(hostErrorCode));
    systemError.SetHostErrorCode(hostErrorCode);
    throw systemError;
}

} // namespace cmsx::kernel
