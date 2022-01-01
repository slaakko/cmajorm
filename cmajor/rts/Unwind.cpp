// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Unwind.hpp>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <signal.h>

#ifdef _WIN32
    #if defined(__MINGW32__)
        void* unwindList = nullptr;
    #else
        __declspec(thread) void* unwindList = nullptr;
    #endif
#else
__thread void* unwindList = nullptr;
#endif

struct UnwindInfo
{
    UnwindInfo* next;
    void* function;
    int32_t line;
};

void* RtPushUnwindInfo(void* unwindInfo)
{
    void* prevUnwindInfo = unwindList;
    unwindList = unwindInfo;
    return prevUnwindInfo;
}

void RtPopUnwindInfo(void* prevUnwindInfo)
{
    unwindList = prevUnwindInfo;
}

struct FunctionUnwindInfo
{
    FunctionUnwindInfo(const char* functionName_, const char* sourceFilePath_) : functionName(functionName_), sourceFilePath(sourceFilePath_) {}
    std::string functionName;
    std::string sourceFilePath;
};

class GlobalUnwindInfo
{
public:
    static void Init();
    static void Done();
    static bool Initialized() { return instance != nullptr; }
    static GlobalUnwindInfo& Instance() { return *instance; }
    void AddUnwindInfo(void* functionAddress, const char* functionName, const char* sourceFilePath);
    const char* GetCallStack(UnwindInfo* unwindInfoList);
    void DisposeCallStack(UnwindInfo* unwindInfoList);
private:
    static std::unique_ptr<GlobalUnwindInfo> instance;
    GlobalUnwindInfo();
    std::unordered_map<void*, FunctionUnwindInfo*> unwindInfoMap;
    std::vector<std::unique_ptr<FunctionUnwindInfo>> unwindInfoVec;
    std::unordered_map<void*, std::string*> callStackMap;
};

void GlobalUnwindInfo::Init()
{
    instance.reset(new GlobalUnwindInfo());
}

void GlobalUnwindInfo::Done()
{
    instance.reset();
}

std::unique_ptr<GlobalUnwindInfo> GlobalUnwindInfo::instance;

GlobalUnwindInfo::GlobalUnwindInfo()
{
}

void GlobalUnwindInfo::AddUnwindInfo(void* functionAddress, const char* functionName, const char* sourceFilePath)
{
    auto it = unwindInfoMap.find(functionAddress);
    if (it == unwindInfoMap.cend())
    {
        FunctionUnwindInfo* unwindInfo = new FunctionUnwindInfo(functionName, sourceFilePath);
        unwindInfoVec.push_back(std::unique_ptr<FunctionUnwindInfo>(unwindInfo));
        unwindInfoMap[functionAddress] = unwindInfo;
    }
}

const char* GlobalUnwindInfo::GetCallStack(UnwindInfo* unwindInfoList)
{
    if (!unwindInfoList) return "";
    auto it = callStackMap.find(unwindInfoList);
    if (it != callStackMap.cend())
    {
        return it->second->c_str();
    }
    std::unique_ptr<std::string> callStack(new std::string());
    UnwindInfo* unwindInfo = unwindInfoList;
    while (unwindInfo)
    {
        void* function = unwindInfo->function;
        auto it = unwindInfoMap.find(function);
        if (it != unwindInfoMap.cend())
        {
            FunctionUnwindInfo* functionUnwindInfo = it->second;
            callStack->append(functionUnwindInfo->functionName).append(1, ' ').append(functionUnwindInfo->sourceFilePath);
            if (unwindInfo->line != 0 && unwindInfo->line != -1)
            {
                callStack->append(1, ':').append(std::to_string(unwindInfo->line));
            }
            callStack->append("\n");
        }
        unwindInfo = unwindInfo->next;
    }
    const char* callStackStr = callStack->c_str();
    callStackMap[unwindInfoList] = callStack.release();
    return callStackStr;
}

void GlobalUnwindInfo::DisposeCallStack(UnwindInfo* unwindInfoList)
{
    auto it = callStackMap.find(unwindInfoList);
    if (it != callStackMap.cend())
    {
        delete it->second;
        callStackMap.erase(unwindInfoList);
    }
}

void RtAddCompileUnitFunction(void* functionAddress, const char* functionName, const char* sourceFilePath)
{
    if (GlobalUnwindInfo::Initialized())
    {
        GlobalUnwindInfo::Instance().AddUnwindInfo(functionAddress, functionName, sourceFilePath);
    }
}

const char* RtGetCallStack()
{
    if (GlobalUnwindInfo::Initialized())
    {
        return GlobalUnwindInfo::Instance().GetCallStack(static_cast<UnwindInfo*>(unwindList));
    }
    return "";
}

void RtDisposeCallStack()
{
    if (GlobalUnwindInfo::Initialized())
    {
        GlobalUnwindInfo::Instance().DisposeCallStack(static_cast<UnwindInfo*>(unwindList));
    }
}

void InitUnwind()
{
    GlobalUnwindInfo::Init();
}

void DoneUnwind()
{
    GlobalUnwindInfo::Done();
}
