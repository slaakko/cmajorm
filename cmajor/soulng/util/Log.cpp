// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Log.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <iostream>
#include <mutex>
#include <list>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <atomic>

namespace soulng { namespace util {

using namespace soulng::unicode;

#ifndef __MINGW32__
std::mutex logMutex;
std::condition_variable messageEnqueuedOrEndLog;
#endif
LogMode logMode = LogMode::console;
bool endLog = false;
std::list<std::string> log;

void SetLogMode(LogMode mode)
{
    logMode = mode;
}

void StartLog()
{
    endLog = false;
}

void EndLog()
{
#ifndef __MINGW32__
    for (int i = 0; i < 10; ++i)
    {
        if (!log.empty())
        {
            messageEnqueuedOrEndLog.notify_one();
            std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
        }
        else
        {
            break;
        }
    }
    endLog = true;
    messageEnqueuedOrEndLog.notify_one();
#endif 
}

void LogMessage(int logStreamId, const std::string& message)
{
#ifndef __MINGW32__
    std::lock_guard<std::mutex> lock(logMutex);
#endif
    if (logMode == LogMode::console)
    {
        if (logStreamId == -1)
        {
            std::cout << message << std::endl;
        }
        else
        {
            std::cout << Format(std::to_string(logStreamId), 2, FormatWidth::min, FormatJustify::right, '0') << ">" << message << std::endl;
        }
    }
    else if (logMode == LogMode::queue)
    {
        if (logStreamId == -1)
        {
            log.push_back(message);
        }
        else
        {
            log.push_back(Format(std::to_string(logStreamId), 2, FormatWidth::min, FormatJustify::right, '0') + ">" + message);
        }
#ifndef __MINGW32__
        messageEnqueuedOrEndLog.notify_one();
#endif
    }
}

void LogMessage(int logStreamId, const std::string& message, int indent)
{
    LogMessage(logStreamId, std::string(indent, ' ') + message);
}

std::string logMessage;

int WaitForLogMessage()
{
#ifndef __MINGW32__
    std::unique_lock<std::mutex> lock(logMutex);
    messageEnqueuedOrEndLog.wait(lock, []{ return !log.empty() || endLog; });
    if (!log.empty())
    {
        logMessage = log.front();
        log.pop_front();
        return logMessage.length();
    }
    else 
    {
        return -1;
    }
#else
    return -1;
#endif
}

int FetchLogMessage(char16_t* buf, int size)
{
    std::u16string utf16LogMessage = ToUtf16(logMessage);
    if (size <= utf16LogMessage.length())
    {
        return -1;
    }
    else
    {
        int n = utf16LogMessage.length();
        for (int i = 0; i < n; ++i)
        {
            char16_t c = utf16LogMessage[i];
            buf[i] = c;
        }
        buf[n] = u'\0';
        return n;
    }
}

std::string FetchLogMessage(bool& endOfLog, int timeoutMs, bool& timeout)
{
#ifndef __MINGW32__
    endOfLog = false;
    std::unique_lock<std::mutex> lock(logMutex);
    if (timeoutMs)
    {
        if (!messageEnqueuedOrEndLog.wait_for(lock, std::chrono::milliseconds{ timeoutMs }, [] { return !log.empty() || endLog; }))
        {
            timeout = true;
            return std::string();
        }
    }
    else
    {
        messageEnqueuedOrEndLog.wait(lock, [] { return !log.empty() || endLog; });
    }
    if (!log.empty())
    {
        logMessage = log.front();
        log.pop_front();
        return logMessage;
    }
#endif
    endOfLog = true;
    return std::string();
}

} } // namespace soulng::util
