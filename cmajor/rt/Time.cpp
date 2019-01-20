// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rt/Time.hpp>
#include <chrono>
#include <thread>
#include <ctime>

extern "C" RT_API int64_t RtNow()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

extern "C" RT_API void RtSleep(int64_t nanoseconds)
{
    std::chrono::nanoseconds duration{ nanoseconds };
    std::this_thread::sleep_for(duration);
}

extern "C" RT_API void RtGetCurrentDate(int16_t* year, int8_t* month, int8_t* day)
{
    std::time_t currentTime;
    std::time(&currentTime);
    struct tm* localTime = nullptr;
    localTime = std::localtime(&currentTime);
    *year = 1900 + localTime->tm_year;
    *month = 1 + localTime->tm_mon;
    *day = localTime->tm_mday;
}

extern "C" RT_API void RtGetCurrentDateTime(int16_t* year, int8_t* month, int8_t* day, int32_t* secs)
{
    std::time_t currentTime;
    std::time(&currentTime);
    struct tm* localTime = nullptr;
    localTime = std::localtime(&currentTime);
    *year = 1900 + localTime->tm_year;
    *month = 1 + localTime->tm_mon;
    *day = localTime->tm_mday;
    *secs = localTime->tm_hour * 3600 + localTime->tm_min * 60 + localTime->tm_sec;
}
