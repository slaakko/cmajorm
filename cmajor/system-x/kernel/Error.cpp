// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/Debug.hpp>
#include <soulng/util/Unicode.hpp>
#include <Windows.h>
#undef min
#undef max

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
        case ENOCHILD: return "ENOCHILD";
        case EHOST: return "EHOST";
        case EPERMISSION: return "EPERMISSION";
        case ESOCKET: return "ESOCKET";
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
        case ENOCHILD: return "No child";
        case EHOST: return "Host error";
        case EPERMISSION: return "Permission denied";
        case ESOCKET: return "Socket error";
    }
    return std::string();
}

SystemError::SystemError() : std::runtime_error(""), errorCode(0), hostErrorCode(0), message(), function()
{
}

SystemError::SystemError(int errorCode_, const std::string& message_, const std::string& function_) : 
    std::runtime_error(ErrorCodeStr(errorCode_) + "(" + std::to_string(errorCode_) + "): " + ErrorMsg(errorCode_) + ": " + message_), 
    errorCode(errorCode_), hostErrorCode(0), message(message_), function(function_)
{
    if ((GetDebugMode() & debugSystemErrorMode) != 0)
    {
        DebugWrite("kernel.error: " + std::string(what()) + ": " + function);
    }
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

void ThrowLastHostError(const std::string& filePath)
{
    int64_t hostErrorCode = GetLastHostErrorCode();
    std::string file;
    if (!filePath.empty())
    {
        file = " (file='" + filePath + "')";
    }
    SystemError systemError(EHOST, HostErrorMessage(hostErrorCode) + file, __FUNCTION__);
    systemError.SetHostErrorCode(hostErrorCode);
    throw systemError;
}

} // namespace cmsx::kernel
