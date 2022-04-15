// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_TIME_INCLUDED
#define SOULNG_UTIL_TIME_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <chrono>
#include <ctime>
#include <stdint.h>
#include <string>

namespace soulng { namespace util {

enum class Month : int8_t
{
    january = 1, february, march, april, may, june, july, august, september, october, november, december
};

UTIL_API int GetMonthDays(Month month, int year);

class UTIL_API Date
{
public:
    Date() : year(0), month(Month::january), day(1)
    {
    }
    Date(short year_, Month month_, int8_t day_) : year(year_), month(month_), day(day_)
    {
    }
    int16_t Year() const
    {
        return year;
    }
    Month GetMonth() const
    {
        return month;
    }
    int8_t Day() const
    {
        return day;
    }
    Date AddDays(int n);
    Date AddMonths(int n);
    Date AddYears(int n);
    std::string ToString() const;
    std::string ToString(bool omitDashes) const;
private:
    int16_t year;
    Month month;
    int8_t day;
};

UTIL_API Date GetCurrentDate();

UTIL_API bool operator==(const Date& left, const Date& right);

UTIL_API inline bool operator!=(const Date& left, const Date& right)
{
    return !(left == right);
}

UTIL_API bool operator<(const Date& left, const Date& right);

UTIL_API inline bool operator>(const Date& left, const Date& right)
{
    return right < left;
}

UTIL_API inline bool operator<=(const Date& left, const Date& right)
{
    return !(right > left);
}

UTIL_API inline bool operator>=(const Date& left, const Date& right)
{
    return !(left < right);
}

UTIL_API Date ParseDate(const std::string& dateStr);

class UTIL_API DateTime
{
public:
    DateTime() : date(), secs(0)
    {
    }
    DateTime(Date date_) : date(date_), secs(0)
    {
    }
    DateTime(Date date_, int32_t secs_) : date(date_), secs(secs_)
    {
    }
    Date GetDate() const
    {
        return date;
    }
    int32_t Hours() const
    {
        return secs / 3600;
    }
    int32_t Minutes() const
    {
        return secs / 60;
    }
    int32_t Seconds() const
    {
        return secs;
    }
    std::string ToString() const;
    std::string ToString(bool omitDashes, bool omitColons, bool omitMins, bool omitSecs) const;
private:
    Date date;
    int32_t secs;
};

UTIL_API DateTime GetCurrentDateTime();

UTIL_API bool operator==(const DateTime& left, const DateTime& right);

UTIL_API inline bool operator!=(const DateTime& left, const DateTime& right)
{
    return !(left == right);
}

UTIL_API bool operator<(const DateTime& left, const DateTime& right);

UTIL_API inline bool operator>(const DateTime& left, const DateTime& right)
{
    return right < left;
}

UTIL_API inline bool operator<=(const DateTime& left, const DateTime& right)
{
    return !(right > left);
}

UTIL_API inline bool operator>=(const DateTime& left, const DateTime& right)
{
    return !(left < right);
}

UTIL_API DateTime ParseDateTime(const std::string& dateTimeStr);

class UTIL_API Timestamp
{
public:
    Timestamp() : dateTime(), nanosecs(0) {}
    Timestamp(Date date_) : dateTime(date_), nanosecs(0) {}
    Timestamp(Date date_, int32_t secs_) : dateTime(date_, secs_), nanosecs(0) {}
    Timestamp(const DateTime& dateTime_) : dateTime(dateTime_), nanosecs(0) {}
    Timestamp(const DateTime& dateTime_, int32_t nanosecs_) : dateTime(dateTime_), nanosecs(nanosecs_) {}
    const DateTime& GetDateTime() const { return dateTime; }
    int32_t Nanoseconds() const { return nanosecs; }
    std::string ToString() const;
private:
    DateTime dateTime;
    int32_t nanosecs;
};

const int secsInDay = 24 * 3600;

UTIL_API bool operator==(const Timestamp& left, const Timestamp& right);

UTIL_API inline bool operator!=(const Timestamp& left, const Timestamp& right)
{
    return !(left == right);
}

UTIL_API bool operator<(const Timestamp& left, const Timestamp& right);

UTIL_API inline bool operator>(const Timestamp& left, const Timestamp& right)
{
    return right < left;
}

UTIL_API inline bool operator<=(const Timestamp& left, const Timestamp& right)
{
    return !(right > left);
}

UTIL_API inline bool operator>=(const Timestamp& left, const Timestamp& right)
{
    return !(left < right);
}

UTIL_API Timestamp GetCurrentTimestamp();

UTIL_API Timestamp ParseTimestamp(const std::string& timestampStr);

UTIL_API std::string FormatTimeMs(int32_t milliseconds);

UTIL_API std::int64_t CurrentMs();

UTIL_API int64_t GetCurrentTime();

UTIL_API std::string DurationStr(const std::chrono::nanoseconds& duration);

UTIL_API std::time_t Time();

UTIL_API std::time_t MkTime(const DateTime& dt);

UTIL_API DateTime ToDateTime(time_t time);

UTIL_API std::string TimeToString(std::time_t time);

UTIL_API void TimeInit();
UTIL_API void TimeDone();

} } // namespace soulng::util

#endif // SOULNG_UTIL_TIME_INCLUDED
