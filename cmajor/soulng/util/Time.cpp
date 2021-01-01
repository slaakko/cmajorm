// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Time.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
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

void ThrowRuntimeError(const std::string& message)
{
    throw std::runtime_error(message);
}

void ThrowInvalidDate(const std::string& s)
{
    ThrowRuntimeError("cannot parse date from string '" + s + "': not in format YYYY-MM-DD or YYYYMMDD");
}

void ThrowInvalidDateTime(const std::string& s)
{
    ThrowRuntimeError("cannot parse date time from string '" + s + "': not in format YYYY[-]MM[-]DD or YYYY[-]MM[-]DDTHH[[:]MM[[:]SS]");
}

Date ParseDate(const std::string& dateStr, int& dateEnd)
{
    if (dateStr.length() < 4 + 2 + 2)
    {
        ThrowInvalidDate(dateStr);
    }
    int16_t year = boost::lexical_cast<short>(dateStr.substr(0, 4));
    int monthStart = 4;
    if (dateStr[4] == '-')
    {
        ++monthStart;
    }
    if (dateStr.length() < monthStart + 2)
    {
        ThrowInvalidDate(dateStr);
    }
    int8_t month = static_cast<int8_t>(boost::lexical_cast<int>(dateStr.substr(monthStart, 2)));
    if (month < 1 || month > 12)
    {
        ThrowInvalidDate(dateStr);
    }
    int dayStart = monthStart + 2;
    if (dateStr[dayStart] == '-')
    {
        ++dayStart;
    }
    if (dateStr.length() < dayStart + 2)
    {
        ThrowInvalidDate(dateStr);
    }
    int8_t day = static_cast<int8_t>(boost::lexical_cast<int>(dateStr.substr(dayStart, 2)));
    if (day < 1 || day > 31)
    {
        ThrowInvalidDate(dateStr);
    }
    dateEnd = dayStart + 2;
    return Date(year, static_cast<Month>(month), day);
}

Date ParseDate(const std::string& dateStr)
{
    int dateEnd = 0;
    return ParseDate(dateStr, dateEnd);
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

DateTime ParseDateTime(const std::string& dateTimeStr)
{
    int dateEnd = 0;
    Date date = ParseDate(dateTimeStr, dateEnd);
    int hours = 0;
    int mins = 0;
    int secs = 0;
    if (dateTimeStr.length() > dateEnd)
    {
        if (dateTimeStr[dateEnd] == 'T')
        {
            int hoursStart = dateEnd + 1;
            hours = boost::lexical_cast<int>(dateTimeStr.substr(hoursStart, 2));
            if (hours < 0 || hours > 24)
            {
                ThrowInvalidDateTime(dateTimeStr);
            }
            if (dateTimeStr.length() > hoursStart + 2)
            {
                int minsStart = hoursStart + 2;
                if (dateTimeStr[minsStart] == ':')
                {
                    ++minsStart;
                }
                mins = boost::lexical_cast<int>(dateTimeStr.substr(minsStart, 2));
                if (mins < 0 || mins >= 60)
                {
                    ThrowInvalidDateTime(dateTimeStr);
                }
                if (dateTimeStr.length() > minsStart + 2)
                {
                    int secsStart = minsStart + 2;
                    if (dateTimeStr[secsStart] == ':')
                    {
                        ++secsStart;
                    }
                    secs = boost::lexical_cast<int>(dateTimeStr.substr(secsStart, 2));
                    if (secs < 0 || secs > 60) // 60 is valid if leap second exists
                    {
                        ThrowInvalidDateTime(dateTimeStr);
                    }
                }
            }
        }
    }
    int totalSecs = hours * 3600 + mins * 60 + secs;
    return DateTime(date, totalSecs);
}

std::int64_t CurrentMs()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - std::chrono::steady_clock::time_point()).count();
}

int64_t GetCurrentTime()
{
    std::time_t currentTime;
    std::time(&currentTime);
    return currentTime;
}

} } // namespace soulng::util
