// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/CallStack.hpp>
#include <cmajor/rt/Error.hpp>
#include <cmajor/rt/Io.hpp>
#include <cmajor/util/Error.hpp>
#include <vector>
#include <sstream>

namespace cmajor { namespace rt {

struct SourceLocation
{
    SourceLocation(const char* functionName_, const char* sourceFilePath_);
    const char* functionName;
    const char* sourceFilePath;
    int32_t lineNumber;
};

SourceLocation::SourceLocation(const char* functionName_, const char* sourceFilePath_) : functionName(functionName_), sourceFilePath(sourceFilePath_), lineNumber(0)
{
}

class CallStack
{
public:
    CallStack();
    std::vector<SourceLocation>& Locations() { return locations; }
private:
    std::vector<SourceLocation> locations;
};

CallStack::CallStack()
{
}

#ifdef _WIN32

__declspec(thread) CallStack* callStack = nullptr;

#else

__thread CallStack* callStack = nullptr;

#endif

#ifdef _WIN32

__declspec(thread) std::string* stackTrace = nullptr;

#else

__thread std::string* stackTrace = nullptr;

#endif

} }  // namespace cmajor::rt

extern "C" RT_API void RtEnterFunction(const char* functionName, const char* sourceFilePath)
{
    cmajor::rt::CallStack* callStack = cmajor::rt::callStack;
    if (!callStack)
    {
        callStack = new cmajor::rt::CallStack();
        cmajor::rt::callStack = callStack;
    }
    callStack->Locations().push_back(cmajor::rt::SourceLocation(functionName, sourceFilePath));
}

extern "C" RT_API void RtSetLineNumber(int32_t lineNumber)
{
    try
    {
        cmajor::rt::CallStack* callStack = cmajor::rt::callStack;
        Assert(callStack && !callStack->Locations().empty(), "call stack is empty");
        callStack->Locations().back().lineNumber = lineNumber;
    }
    catch (const std::exception& ex)
    {
        std::stringstream s;
        s << "internal error: " << ex.what() << "\n";
        std::string str = s.str();
        RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        exit(exitCodeInternalError);
    }
}

extern "C" RT_API void RtExitFunction()
{
    try
    {
        cmajor::rt::CallStack* callStack = cmajor::rt::callStack;
        Assert(callStack && !callStack->Locations().empty(), "call stack is empty");
        callStack->Locations().pop_back();
    }
    catch (const std::exception& ex)
    {
        std::stringstream s;
        s << "internal error: " << ex.what() << "\n";
        std::string str = s.str();
        RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
        exit(exitCodeInternalError);
    }
}

extern "C" RT_API void RtPrintCallStack(int fileHandle)
{
    std::stringstream s;
    cmajor::rt::CallStack* callStack = cmajor::rt::callStack;
    s << "CALL STACK:\n";
    int n = callStack->Locations().size();
    for (int i = n - 1; i >= 0; --i)
    {
        const cmajor::rt::SourceLocation& location = callStack->Locations()[i];
        s << location.functionName << " " << location.sourceFilePath << ":" << location.lineNumber << "\n";
    }
    std::string str = s.str();
    RtWrite(stdErrFileHandle, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
}

extern "C" RT_API const char* RtGetStackTrace()
{
    std::stringstream s;
    cmajor::rt::CallStack* callStack = cmajor::rt::callStack;
    int n = callStack->Locations().size();
    for (int i = n - 1; i >= 0; --i)
    {
        const cmajor::rt::SourceLocation& location = callStack->Locations()[i];
        s << location.functionName << " " << location.sourceFilePath << ":" << location.lineNumber << "\n";
    }
    cmajor::rt::stackTrace = new std::string(s.str());
    return cmajor::rt::stackTrace->c_str();
}

extern "C" RT_API void RtDisposeStackTrace()
{
    delete cmajor::rt::stackTrace;
}
