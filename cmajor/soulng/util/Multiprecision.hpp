// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_UTIL_MULTIPRECISION_INCLUDED
#define SOULNG_UTIL_MULTIPRECISION_INCLUDED
#include <soulng/util/UtilApi.hpp>
#include <stdint.h>
#include <string>

namespace soulng { namespace util {

class BigRational;

class UTIL_API BigInt
{
public:
    BigInt();
    BigInt(BigInt&& that) noexcept;
    BigInt(const BigInt& that);
    BigInt& operator=(BigInt&&) = default;
    BigInt& operator=(const BigInt& that);
    BigInt(int32_t v);
    BigInt(uint32_t v);
    BigInt(int64_t v);
    BigInt(uint64_t v);
    BigInt(const std::string& rep);
    ~BigInt();
    UTIL_API friend BigInt operator-(const BigInt& operand);
    UTIL_API friend BigInt operator+(const BigInt& operand);
    UTIL_API friend BigInt operator~(const BigInt& operand);
    UTIL_API friend BigInt operator+(const BigInt& left, const BigInt& right);
    UTIL_API friend BigInt operator-(const BigInt& left, const BigInt& right);
    UTIL_API friend BigInt operator*(const BigInt& left, const BigInt& right);
    UTIL_API friend BigInt operator/(const BigInt& left, const BigInt& right);
    UTIL_API friend BigInt operator%(const BigInt& left, const BigInt& right);
    UTIL_API friend BigInt operator&(const BigInt& left, const BigInt& right);
    UTIL_API friend BigInt operator|(const BigInt& left, const BigInt& right);
    UTIL_API friend BigInt operator^(const BigInt& left, const BigInt& right);
    UTIL_API friend BigInt operator<<(const BigInt& left, int32_t shift);
    UTIL_API friend BigInt operator>>(const BigInt& left, int32_t shift);
    UTIL_API friend bool operator==(const BigInt& left, const BigInt& right);
    UTIL_API friend bool operator!=(const BigInt& left, const BigInt& right);
    UTIL_API friend bool operator<(const BigInt& left, const BigInt& right);
    UTIL_API friend bool operator>(const BigInt& left, const BigInt& right);
    UTIL_API friend bool operator<=(const BigInt& left, const BigInt& right);
    UTIL_API friend bool operator>=(const BigInt& left, const BigInt& right);
    std::string ToString() const;
    int32_t ToInt() const;
    uint32_t ToUInt() const;
    int64_t ToLong() const;
    uint64_t ToULong() const;
    friend class BigRational;
    friend class BigFloat;
    UTIL_API friend BigInt Numerator(const BigRational& operand);
    UTIL_API friend BigInt Denominator(const BigRational& operand);
private:
    void* nativeBigInt;
    BigInt(void* that);
};

class UTIL_API BigRational
{
public:
    BigRational();
    BigRational(BigRational&& that) noexcept;
    BigRational(const BigRational& that);
    BigRational& operator=(BigRational&&) = default;
    BigRational& operator=(const BigRational& that);
    BigRational(int32_t v);
    BigRational(uint32_t v);
    BigRational(int64_t v);
    BigRational(uint64_t v);
    BigRational(const std::string& rep);
    BigRational(const BigInt& v);
    BigRational(const BigInt& numerator, const BigInt& denominator);
    ~BigRational();
    UTIL_API friend BigRational operator-(const BigRational& operand);
    UTIL_API friend BigRational operator+(const BigRational& operand);
    UTIL_API friend BigRational operator+(const BigRational& left, const BigRational& right);
    UTIL_API friend BigRational operator-(const BigRational& left, const BigRational& right);
    UTIL_API friend BigRational operator*(const BigRational& left, const BigRational& right);
    UTIL_API friend BigRational operator/(const BigRational& left, const BigRational& right);
    UTIL_API friend bool operator==(const BigRational& left, const BigRational& right);
    UTIL_API friend bool operator!=(const BigRational& left, const BigRational& right);
    UTIL_API friend bool operator<(const BigRational& left, const BigRational& right);
    UTIL_API friend bool operator>(const BigRational& left, const BigRational& right);
    UTIL_API friend bool operator<=(const BigRational& left, const BigRational& right);
    UTIL_API friend bool operator>=(const BigRational& left, const BigRational& right);
    UTIL_API friend BigInt Numerator(const BigRational& operand);
    UTIL_API friend BigInt Denominator(const BigRational& operand);
    friend class BigFloat;
    std::string ToString() const;
    BigInt ToBigInt();
private:
    void* nativeBigRational;
    BigRational(void* that);
};

UTIL_API BigInt Numerator(const BigRational& operand);
UTIL_API BigInt Denominator(const BigRational& operand);

class UTIL_API BigFloat
{
public:
    BigFloat();
    BigFloat(BigFloat&& that) noexcept;
    BigFloat(const BigFloat& that);
    BigFloat& operator=(BigFloat&&) = default;
    BigFloat& operator=(const BigFloat& that);
    BigFloat(int32_t v);
    BigFloat(uint32_t v);
    BigFloat(int64_t v);
    BigFloat(uint64_t v);
    BigFloat(double v);
    BigFloat(const std::string& rep);
    BigFloat(const BigInt& v);
    BigFloat(const BigRational& v);
    ~BigFloat();
    UTIL_API friend BigFloat operator-(const BigFloat& operand);
    UTIL_API friend BigFloat operator+(const BigFloat& operand);
    UTIL_API friend BigFloat operator+(const BigFloat& left, const BigFloat& right);
    UTIL_API friend BigFloat operator-(const BigFloat& left, const BigFloat& right);
    UTIL_API friend BigFloat operator*(const BigFloat& left, const BigFloat& right);
    UTIL_API friend BigFloat operator/(const BigFloat& left, const BigFloat& right);
    UTIL_API friend bool operator==(const BigFloat& left, const BigFloat& right);
    UTIL_API friend bool operator!=(const BigFloat& left, const BigFloat& right);
    UTIL_API friend bool operator<(const BigFloat& left, const BigFloat& right);
    UTIL_API friend bool operator>(const BigFloat& left, const BigFloat& right);
    UTIL_API friend bool operator<=(const BigFloat& left, const BigFloat& right);
    UTIL_API friend bool operator>=(const BigFloat& left, const BigFloat& right);
    std::string ToString() const;
    double ToDouble() const;
    BigInt ToBigInt();
    BigRational ToBigRational();
private:
    void* nativeBigFloat;
    BigFloat(void* that);
};

} } // namespace soulng::util

#endif // SOULNG_UTIL_MULTIPRECISION_INCLUDED
