// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_TIME_INCLUDED
#define SOULNG_UTIL_TIME_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <stdint.h>
#include <string>

namespace soulng { namespace util {

enum class Month : int8_t
{
    january = 1, february, march, april, may, june, july, august, september, october, november, december
};

class Date
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
    std::string ToString() const;
    std::string ToString(bool omitDashes) const;
private:
    int16_t year;
    Month month;
    int8_t day;
};

UTIL_API Date GetCurrentDate();

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

UTIL_API std::string FormatTimeMs(int32_t milliseconds);

UTIL_API std::int64_t CurrentMs();

} } // namespace soulng::util

#endif // SOULNG_UTIL_TIME_INCLUDED
