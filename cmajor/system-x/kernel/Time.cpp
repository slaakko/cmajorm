// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Time.hpp>

namespace cmsx::kernel {

Date CurrentDate()
{
    time_t t = std::time(nullptr);
    struct tm lt = *std::localtime(&t);
    int16_t year = 1900 + lt.tm_year;
    int8_t month = 1 + lt.tm_mon;
    int8_t day = lt.tm_mday;
    return Date(year, month, day);
}

DateTime CurrentDateTime()
{
    time_t t = std::time(nullptr);
    struct tm lt = *std::localtime(&t);
    int32_t secs = 3600 * lt.tm_hour + 60 * lt.tm_min + lt.tm_sec;
    DateTime dt(CurrentDate(), secs);
    return dt;
}

void WriteDate(const Date& date, int64_t yearAddr, int64_t monthAddr, int64_t dayAddr, uint64_t rv, cmsx::machine::Memory& mem)
{
    mem.WriteWyde(rv, yearAddr, date.year, cmsx::machine::Protection::write);
    mem.WriteByte(rv, monthAddr, date.month, cmsx::machine::Protection::write);
    mem.WriteByte(rv, dayAddr, date.day, cmsx::machine::Protection::write);
}

void WriteDateTime(const DateTime& dateTime, int64_t yearAddr, int64_t monthAddr, int64_t dayAddr, int64_t secsAddr, uint64_t rv, cmsx::machine::Memory& mem)
{
    WriteDate(dateTime.date, yearAddr, monthAddr, dayAddr, rv, mem);
    mem.WriteTetra(rv, secsAddr, dateTime.secs, cmsx::machine::Protection::write);
}

} // namespace cmsx::kernel
