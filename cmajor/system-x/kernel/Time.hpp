// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_TIME_INCLUDED
#define CMSX_KERNEL_TIME_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Memory.hpp>
#include <stdint.h>

namespace cmsx::kernel {

struct CMSX_KERNEL_API Date
{
    Date(int16_t year_, int8_t month_, int8_t day_) : year(year_), month(month_), day(day_) {}
    int16_t year;
    int8_t month;
    int8_t day;
};

struct CMSX_KERNEL_API DateTime
{
    Date date;
    int32_t secs;
};

CMSX_KERNEL_API Date CurrentDate();
CMSX_KERNEL_API DateTime CurrentDateTime();
CMSX_KERNEL_API void WriteDate(const Date& date, int64_t yearAddr, int64_t monthAddr, int64_t dayAddr, uint64_t rv, cmsx::machine::Memory& mem);
CMSX_KERNEL_API void WriteDateTime(const DateTime& dateTime, int64_t yearAddr, int64_t monthAddr, int64_t dayAddr, int64_t secsAddr, uint64_t rv, cmsx::machine::Memory& mem);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_TIME_INCLUDED
