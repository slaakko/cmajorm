// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Time.hpp>
#include <ctime>
#include <chrono>

namespace soulng { namespace util {

std::string Date::ToString() const
{
    return ToString(false);
}

std::string Date::ToString(bool omitDashes) const
{
    std::string date;
    date.append(1, static_cast<char>(static_cast<int16_t>('0') + ((year / 1000) % 10)));
    date.append(1, static_cast<char>(static_cast<int16_t>('0') + ((year / 100) % 10)));
    date.append(1, static_cast<char>(static_cast<int16_t>('0') + ((year / 10) % 10)));
    date.append(1, static_cast<char>(static_cast<int16_t>('0') + (year % 10)));
    if (!omitDashes)
    {
        date.append(1, '-');
    }
    date.append(1, static_cast<char>(static_cast<int8_t>('0') + ((static_cast<int8_t>(month) / 10) % 10)));
    date.append(1, static_cast<char>(static_cast<int8_t>('0') + (static_cast<int8_t>(month) % 10)));
    if (!omitDashes)
    {
        date.append(1, '-');
    }
    date.append(1, static_cast<char>(static_cast<int8_t>('0') + ((day / 10) % 10)));
    date.append(1, static_cast<char>(static_cast<int8_t>('0') + (day % 10)));
    return date;
}

Date GetCurrentDate()
{
    std::time_t currentTime;
    std::time(&currentTime);
    struct tm* localTime = nullptr;
    localTime = std::localtime(&currentTime);
    return Date(1900 + localTime->tm_year, static_cast<Month>(1 + localTime->tm_mon), static_cast<int8_t>(localTime->tm_mday));
}

std::string DateTime::ToString() const
{
    return ToString(false, false, false, false);
}

std::string DateTime::ToString(bool omitDashes, bool omitColons, bool omitMins, bool omitSecs) const
{
    std::string dateTime;
    dateTime.append(date.ToString(omitDashes));
    dateTime.append(1, 'T');
    int32_t hh = Hours() % 24;
    int32_t mm = Minutes() % 60;
    int32_t ss = Seconds() % 60;
    dateTime.append(1, static_cast<char>(static_cast<int32_t>('0') + ((hh / 10) % 10)));
    dateTime.append(1, static_cast<char>(static_cast<int32_t>('0') + (hh % 10)));
    if (!omitMins)
    {
        if (!omitColons)
        {
            dateTime.append(1, ':');
        }
        dateTime.append(1, static_cast<char>(static_cast<int32_t>('0') + ((mm / 10) % 10)));
        dateTime.append(1, static_cast<char>(static_cast<int>('0') + (mm % 10)));
        if (!omitSecs)
        {
            if (!omitColons)
            {
                dateTime.append(1, ':');
            }
            dateTime.append(1, static_cast<char>(static_cast<int32_t>('0') + ((ss / 10) % 10)));
            dateTime.append(1, static_cast<char>(static_cast<int32_t>('0') + (ss % 10)));
        }
    }
    return dateTime;
}

std::string FormatTimeMs(int32_t milliseconds)
{
    int32_t hh = milliseconds / 3600000;
    int32_t mm = milliseconds / 60000 % 60;
    int32_t ss = milliseconds / 1000 % 60;
    int32_t ms = milliseconds % 1000;
    std::string time;
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + ((hh / 10) % 10)));
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + (hh % 10)));
    time.append(1, ':');
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + ((mm / 10) % 10)));
    time.append(1, static_cast<char>(static_cast<int>('0') + (mm % 10)));
    time.append(1, ':');
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + ((ss / 10) % 10)));
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + (ss % 10)));
    time.append(1, '.');
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + ((ms / 100) % 10)));
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + ((ms / 10) % 10)));
    time.append(1, static_cast<char>(static_cast<int32_t>('0') + (ms % 10)));
    return time;
}

DateTime GetCurrentDateTime()
{
    std::time_t currentTime;
    std::time(&currentTime);
    struct tm* localTime = nullptr;
    localTime = std::localtime(&currentTime);
    return DateTime(Date(1900 + localTime->tm_year, static_cast<Month>(1 + localTime->tm_mon), static_cast<int8_t>(localTime->tm_mday)), localTime->tm_hour * 3600 + localTime->tm_min * 60 + localTime->tm_sec);
}

std::int64_t CurrentMs()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point()).count();
}

} } // namespace soulng::util
