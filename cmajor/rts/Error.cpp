// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Error.hpp>
#include <cmajor/rts/CallStack.hpp>
#include <cmajor/rts/Io.hpp>
#include <cmajor/rts/Classes.hpp>
#include <soulng/util/Error.hpp>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <stdlib.h>
#include <memory>

namespace cmajor { namespace rt {

class ErrorTable
{
public:
    static void Init();
    static void Done();
    static ErrorTable& Instance() { Assert(instance, "error table not initialized"); return *instance; }
    int32_t InstallError(const std::string& errorMessage);
    const char* GetError(int32_t errorId);
    void DisposeError(int32_t errorId);
private:
    static std::unique_ptr<ErrorTable> instance;
    std::mutex mtx;
    std::unordered_map<int32_t, std::string> errorMap;
    int32_t nextErrorId;
    ErrorTable();
};

std::unique_ptr<ErrorTable> ErrorTable::instance;

void ErrorTable::Init()
{
    instance.reset(new ErrorTable());
}

void ErrorTable::Done()
{
    instance.reset();
}

ErrorTable::ErrorTable() : nextErrorId(1)
{
}

int32_t ErrorTable::InstallError(const std::string& errorMessage)
{
    std::lock_guard<std::mutex> lock(mtx);
    int32_t errorId = nextErrorId++;
    errorMap[errorId] = errorMessage;
    return errorId;
}

const char* ErrorTable::GetError(int32_t errorId)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = errorMap.find(errorId);
    if (it != errorMap.cend())
    {
        return it->second.c_str();
    }
    else
    {
        return "<error message not found>";
    }
}

void ErrorTable::DisposeError(int32_t errorId)
{
    std::lock_guard<std::mutex> lock(mtx);
    errorMap.erase(errorId);
}

int32_t InstallError(const std::string& errorMessage)
{
    return -ErrorTable::Instance().InstallError(errorMessage);
}

const char* GetError(int32_t errorId)
{
    return ErrorTable::Instance().GetError(-errorId);
}

void DisposeError(int32_t errorId)
{
    ErrorTable::Instance().DisposeError(-errorId);
}

#ifdef _WIN32

__declspec(thread) void* currentException = nullptr;
__declspec(thread) uint64_t currentExceptionClassId[2];

#else

__thread void* currentException = nullptr;
__thread uint64_t currentExceptionClassId[2];

#endif

void InitError()
{
    ErrorTable::Init();
}

void DoneError()
{
    ErrorTable::Done();
}

} } // namespace cmajor::rt

extern "C" RT_API void RtFailAssertion(const char* assertion, const char* function, const char* sourceFilePath, int lineNumber)
{
    std::stringstream s;
    s << "assertion '" << assertion << "' failed in function '" << function << "' at " << sourceFilePath << ":" << lineNumber << "\n";
    std::string str = s.str();
    RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
    RtPrintCallStack(stdErrFileHandle);
    exit(exitCodeAssertionFailed);
}

extern "C" RT_API const char* RtGetError(int32_t errorId)
{
    return cmajor::rt::GetError(errorId);
}

extern "C" RT_API void RtDisposeError(int32_t errorId)
{
    cmajor::rt::DisposeError(errorId);
}

extern "C" RT_API void RtThrowException(void* exception, void* exceptionTypeId)
{
    cmajor::rt::currentException = exception;
    boost::uuids::uuid* exTypeId = reinterpret_cast<boost::uuids::uuid*>(exceptionTypeId);
    boost::multiprecision::uint128_t exceptionClassId = cmajor::rt::GetClassId(*exTypeId);
    cmajor::rt::currentExceptionClassId[0] = static_cast<uint64_t>(exceptionClassId >> 64);
    cmajor::rt::currentExceptionClassId[1] = static_cast<uint64_t>(exceptionClassId);
    throw cmajor::eh::Exception();
}

extern "C" RT_API void RtCaptureException(void** exception, uint64_t& exceptionClassIdHi, uint64_t& exceptionClassIdLo)
{
    *exception = cmajor::rt::currentException;
    cmajor::rt::currentException = nullptr;
    exceptionClassIdHi = static_cast<uint64_t>(cmajor::rt::currentExceptionClassId[0]);
    exceptionClassIdLo = static_cast<uint64_t>(cmajor::rt::currentExceptionClassId[1]);
    cmajor::rt::currentExceptionClassId[0] = 0;
    cmajor::rt::currentExceptionClassId[1] = 0;
}

extern "C" RT_API void RtThrowCapturedException(void* exception, uint64_t exceptionClassIdHi, uint64_t exceptionClassIdLo)
{
    cmajor::rt::currentException = exception;
    cmajor::rt::currentExceptionClassId[0] = exceptionClassIdHi;
    cmajor::rt::currentExceptionClassId[1] = exceptionClassIdLo;
    throw cmajor::eh::Exception();
}

extern "C" RT_API bool RtHandleException(void* exceptionTypeId)
{
    boost::multiprecision::uint128_t currentExceptionClassId = boost::multiprecision::uint128_t(cmajor::rt::currentExceptionClassId[0]) << 64 | cmajor::rt::currentExceptionClassId[1];
    boost::uuids::uuid* exTypeId = reinterpret_cast<boost::uuids::uuid*>(exceptionTypeId);
    boost::multiprecision::uint128_t exceptionClassId = cmajor::rt::GetClassId(*exTypeId);
    bool handle = currentExceptionClassId % exceptionClassId == 0;
    return handle;
}

extern "C" RT_API void* RtGetException()
{
    return cmajor::rt::currentException;
}

extern "C" RT_API void* RtGetExceptionTypeId()
{
    return EhGetExceptionTypeId();
}