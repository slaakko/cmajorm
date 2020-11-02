// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/util/Multiprecision.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/lexical_cast.hpp>

namespace soulng { namespace util {

// BigInt

BigInt::BigInt() : nativeBigInt(new boost::multiprecision::cpp_int())
{
}

BigInt::BigInt(const BigInt& that) : nativeBigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_int*>(that.nativeBigInt)))
{
}

BigInt::BigInt(BigInt&& that) noexcept : nativeBigInt(that.nativeBigInt)
{
    that.nativeBigInt = nullptr;
}

BigInt& BigInt::operator=(const BigInt& that)
{
    *static_cast<boost::multiprecision::cpp_int*>(nativeBigInt) = *static_cast<boost::multiprecision::cpp_int*>(that.nativeBigInt);
    return *this;
}

BigInt::BigInt(void* that) : nativeBigInt(that)
{
}

BigInt::BigInt(int32_t v) : nativeBigInt(new boost::multiprecision::cpp_int(v))
{
}

BigInt::BigInt(uint32_t v) : nativeBigInt(new boost::multiprecision::cpp_int(v))
{
}

BigInt::BigInt(int64_t v) : nativeBigInt(new boost::multiprecision::cpp_int(v))
{
}

BigInt::BigInt(uint64_t v) : nativeBigInt(new boost::multiprecision::cpp_int(v))
{
}

BigInt::BigInt(const std::string& rep) : nativeBigInt(new boost::multiprecision::cpp_int(rep))
{
}

BigInt::~BigInt()
{
    if (nativeBigInt)
    {
        delete(static_cast<boost::multiprecision::cpp_int*>(nativeBigInt));
    }
}

BigInt operator-(const BigInt& operand)
{
    return BigInt(new boost::multiprecision::cpp_int(-*static_cast<boost::multiprecision::cpp_int*>(operand.nativeBigInt)));
}

BigInt operator+(const BigInt& operand)
{
    return BigInt(new boost::multiprecision::cpp_int(+*static_cast<boost::multiprecision::cpp_int*>(operand.nativeBigInt)));
}

BigInt operator~(const BigInt& operand)
{
    return BigInt(new boost::multiprecision::cpp_int(~*static_cast<boost::multiprecision::cpp_int*>(operand.nativeBigInt)));
}

BigInt operator+(const BigInt& left, const BigInt& right)
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) + *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt)));
}

BigInt operator-(const BigInt& left, const BigInt& right)
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) - *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt)));
}

BigInt operator*(const BigInt& left, const BigInt& right)
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) * *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt)));
}

BigInt operator/(const BigInt& left, const BigInt& right)
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) / *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt)));
}

BigInt operator%(const BigInt& left, const BigInt& right)
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) % *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt)));
}

BigInt operator&(const BigInt& left, const BigInt& right)
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) & *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt)));
}

BigInt operator|(const BigInt& left, const BigInt& right)
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) | *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt)));
}

BigInt operator^(const BigInt& left, const BigInt& right)
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) ^ *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt)));
}

BigInt operator<<(const BigInt& left, int32_t shift)
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) << shift));
}

BigInt operator>>(const BigInt& left, int32_t shift)
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) >> shift));
}

bool operator==(const BigInt& left, const BigInt& right)
{
    return *static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) == *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt);
}

bool operator!=(const BigInt& left, const BigInt& right)
{
    return *static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) != *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt);
}

bool operator<(const BigInt& left, const BigInt& right)
{
    return *static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) < *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt);
}

bool operator>(const BigInt& left, const BigInt& right)
{
    return *static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) > *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt);
}

bool operator<=(const BigInt& left, const BigInt& right)
{
    return *static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) <= *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt);
}

bool operator>=(const BigInt& left, const BigInt& right)
{
    return *static_cast<boost::multiprecision::cpp_int*>(left.nativeBigInt) >= *static_cast<boost::multiprecision::cpp_int*>(right.nativeBigInt);
}

std::string BigInt::ToString() const
{
    return boost::lexical_cast<std::string>(*static_cast<boost::multiprecision::cpp_int*>(nativeBigInt));
}

int32_t BigInt::ToInt() const
{
    return static_cast<int32_t>(*static_cast<boost::multiprecision::cpp_int*>(nativeBigInt));
}

uint32_t BigInt::ToUInt() const
{
    return static_cast<uint32_t>(*static_cast<boost::multiprecision::cpp_int*>(nativeBigInt));
}

int64_t BigInt::ToLong() const
{
    return static_cast<int64_t>(*static_cast<boost::multiprecision::cpp_int*>(nativeBigInt));
}

uint64_t BigInt::ToULong() const
{
    return static_cast<uint64_t>(*static_cast<boost::multiprecision::cpp_int*>(nativeBigInt));
}

// BigRational

BigRational::BigRational() : nativeBigRational(new boost::multiprecision::cpp_rational())
{
}

BigRational::BigRational(const BigRational& that) : nativeBigRational(new boost::multiprecision::cpp_rational(*static_cast<boost::multiprecision::cpp_rational*>(that.nativeBigRational)))
{
}

BigRational::BigRational(BigRational&& that) noexcept : nativeBigRational(that.nativeBigRational)
{
    that.nativeBigRational = nullptr;
}

BigRational& BigRational::operator=(const BigRational& that)
{
    *static_cast<boost::multiprecision::cpp_rational*>(nativeBigRational) = *static_cast<boost::multiprecision::cpp_rational*>(that.nativeBigRational);
    return *this;
}

BigRational::BigRational(void* that) : nativeBigRational(that)
{
}

BigRational::BigRational(int32_t v) : nativeBigRational(new boost::multiprecision::cpp_rational(v))
{
}

BigRational::BigRational(uint32_t v) : nativeBigRational(new boost::multiprecision::cpp_rational(v))
{
}

BigRational::BigRational(int64_t v) : nativeBigRational(new boost::multiprecision::cpp_rational(v))
{
}

BigRational::BigRational(uint64_t v) : nativeBigRational(new boost::multiprecision::cpp_rational(v))
{
}

BigRational::BigRational(const std::string& rep) : nativeBigRational(new boost::multiprecision::cpp_rational(rep))
{
}

BigRational::BigRational(const BigInt& v) : nativeBigRational(new boost::multiprecision::cpp_rational(*static_cast<boost::multiprecision::cpp_int*>(v.nativeBigInt)))
{
}

BigRational::BigRational(const BigInt& numerator, const BigInt& denominator) :
    nativeBigRational(new boost::multiprecision::cpp_rational(
    *static_cast<boost::multiprecision::cpp_int*>(numerator.nativeBigInt),
    *static_cast<boost::multiprecision::cpp_int*>(denominator.nativeBigInt)))
{
}

BigRational::~BigRational()
{
    if (nativeBigRational)
    {
        delete(static_cast<boost::multiprecision::cpp_rational*>(nativeBigRational));
    }
}

BigRational operator-(const BigRational& operand)
{
    return BigRational(new boost::multiprecision::cpp_rational(-*static_cast<boost::multiprecision::cpp_rational*>(operand.nativeBigRational)));
}

BigRational operator+(const BigRational& operand)
{
    return BigRational(new boost::multiprecision::cpp_rational(+*static_cast<boost::multiprecision::cpp_rational*>(operand.nativeBigRational)));
}

BigRational operator+(const BigRational& left, const BigRational& right)
{
    return BigRational(new boost::multiprecision::cpp_rational(*static_cast<boost::multiprecision::cpp_rational*>(left.nativeBigRational) + *static_cast<boost::multiprecision::cpp_rational*>(right.nativeBigRational)));
}

BigRational operator-(const BigRational& left, const BigRational& right)
{
    return BigRational(new boost::multiprecision::cpp_rational(*static_cast<boost::multiprecision::cpp_rational*>(left.nativeBigRational) - *static_cast<boost::multiprecision::cpp_rational*>(right.nativeBigRational)));
}

BigRational operator*(const BigRational& left, const BigRational& right)
{
    return BigRational(new boost::multiprecision::cpp_rational(*static_cast<boost::multiprecision::cpp_rational*>(left.nativeBigRational) * *static_cast<boost::multiprecision::cpp_rational*>(right.nativeBigRational)));
}

BigRational operator/(const BigRational& left, const BigRational& right)
{
    return BigRational(new boost::multiprecision::cpp_rational(*static_cast<boost::multiprecision::cpp_rational*>(left.nativeBigRational) / *static_cast<boost::multiprecision::cpp_rational*>(right.nativeBigRational)));
}

bool operator==(const BigRational& left, const BigRational& right)
{
    return *static_cast<boost::multiprecision::cpp_rational*>(left.nativeBigRational) == *static_cast<boost::multiprecision::cpp_rational*>(right.nativeBigRational);
}

bool operator!=(const BigRational& left, const BigRational& right)
{
    return *static_cast<boost::multiprecision::cpp_rational*>(left.nativeBigRational) != *static_cast<boost::multiprecision::cpp_rational*>(right.nativeBigRational);
}

bool operator<(const BigRational& left, const BigRational& right)
{
    return *static_cast<boost::multiprecision::cpp_rational*>(left.nativeBigRational) < *static_cast<boost::multiprecision::cpp_rational*>(right.nativeBigRational);
}

bool operator>(const BigRational& left, const BigRational& right)
{
    return *static_cast<boost::multiprecision::cpp_rational*>(left.nativeBigRational) > * static_cast<boost::multiprecision::cpp_rational*>(right.nativeBigRational);
}

bool operator<=(const BigRational& left, const BigRational& right)
{
    return *static_cast<boost::multiprecision::cpp_rational*>(left.nativeBigRational) <= *static_cast<boost::multiprecision::cpp_rational*>(right.nativeBigRational);
}

bool operator>=(const BigRational& left, const BigRational& right)
{
    return *static_cast<boost::multiprecision::cpp_rational*>(left.nativeBigRational) >= *static_cast<boost::multiprecision::cpp_rational*>(right.nativeBigRational);
}

BigInt Numerator(const BigRational& operand)
{
    return BigInt(new boost::multiprecision::cpp_int(boost::multiprecision::numerator(*static_cast<boost::multiprecision::cpp_rational*>(operand.nativeBigRational))));
}

BigInt Denominator(const BigRational& operand)
{
    return BigInt(new boost::multiprecision::cpp_int(boost::multiprecision::denominator(*static_cast<boost::multiprecision::cpp_rational*>(operand.nativeBigRational))));
}

std::string BigRational::ToString() const
{
    return boost::lexical_cast<std::string>(*static_cast<boost::multiprecision::cpp_rational*>(nativeBigRational));
}

BigInt BigRational::ToBigInt()
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_rational*>(nativeBigRational)));
}

// BitFloat

BigFloat::BigFloat() : nativeBigFloat(new boost::multiprecision::cpp_bin_float_100())
{
}

BigFloat::BigFloat(const BigFloat& that) : nativeBigFloat(new boost::multiprecision::cpp_bin_float_100(*static_cast<boost::multiprecision::cpp_bin_float_100*>(that.nativeBigFloat)))
{
}

BigFloat::BigFloat(BigFloat&& that) noexcept : nativeBigFloat(that.nativeBigFloat)
{
    that.nativeBigFloat = nullptr;
}

BigFloat& BigFloat::operator=(const BigFloat& that)
{
    *static_cast<boost::multiprecision::cpp_bin_float_100*>(nativeBigFloat) = *static_cast<boost::multiprecision::cpp_bin_float_100*>(that.nativeBigFloat);
    return *this;
}

BigFloat::BigFloat(void* that) : nativeBigFloat(that)
{
}

BigFloat::BigFloat(int32_t v) : nativeBigFloat(new boost::multiprecision::cpp_bin_float_100(v))
{
}

BigFloat::BigFloat(uint32_t v) : nativeBigFloat(new boost::multiprecision::cpp_bin_float_100(v))
{
}

BigFloat::BigFloat(int64_t v) : nativeBigFloat(new boost::multiprecision::cpp_bin_float_100(v))
{
}

BigFloat::BigFloat(uint64_t v) : nativeBigFloat(new boost::multiprecision::cpp_bin_float_100(v))
{
}

BigFloat::BigFloat(double v) : nativeBigFloat(new boost::multiprecision::cpp_bin_float_100(v))
{
}

BigFloat::BigFloat(const std::string& rep) : nativeBigFloat(new boost::multiprecision::cpp_bin_float_100(rep))
{
}

BigFloat::BigFloat(const BigInt& v) : nativeBigFloat(new boost::multiprecision::cpp_bin_float_100(*static_cast<boost::multiprecision::cpp_int*>(v.nativeBigInt)))
{
}

BigFloat::BigFloat(const BigRational& v) : nativeBigFloat(new boost::multiprecision::cpp_bin_float_100(*static_cast<boost::multiprecision::cpp_rational*>(v.nativeBigRational)))
{
}

BigFloat::~BigFloat()
{
    if (nativeBigFloat)
    {
        delete(static_cast<boost::multiprecision::cpp_bin_float_100*>(nativeBigFloat));
    }
}

BigFloat operator-(const BigFloat& operand)
{
    return BigFloat(new boost::multiprecision::cpp_bin_float_100(-*static_cast<boost::multiprecision::cpp_bin_float_100*>(operand.nativeBigFloat)));
}

BigFloat operator+(const BigFloat& operand)
{
    return BigFloat(new boost::multiprecision::cpp_bin_float_100(+*static_cast<boost::multiprecision::cpp_bin_float_100*>(operand.nativeBigFloat)));
}

BigFloat operator+(const BigFloat& left, const BigFloat& right)
{
    return BigFloat(new boost::multiprecision::cpp_bin_float_100(*static_cast<boost::multiprecision::cpp_bin_float_100*>(left.nativeBigFloat) + *static_cast<boost::multiprecision::cpp_bin_float_100*>(right.nativeBigFloat)));
}

BigFloat operator-(const BigFloat& left, const BigFloat& right)
{
    return BigFloat(new boost::multiprecision::cpp_bin_float_100(*static_cast<boost::multiprecision::cpp_bin_float_100*>(left.nativeBigFloat) - *static_cast<boost::multiprecision::cpp_bin_float_100*>(right.nativeBigFloat)));
}

BigFloat operator*(const BigFloat& left, const BigFloat& right)
{
    return BigFloat(new boost::multiprecision::cpp_bin_float_100(*static_cast<boost::multiprecision::cpp_bin_float_100*>(left.nativeBigFloat) * *static_cast<boost::multiprecision::cpp_bin_float_100*>(right.nativeBigFloat)));
}

BigFloat operator/(const BigFloat& left, const BigFloat& right)
{
    return BigFloat(new boost::multiprecision::cpp_bin_float_100(*static_cast<boost::multiprecision::cpp_bin_float_100*>(left.nativeBigFloat) / *static_cast<boost::multiprecision::cpp_bin_float_100*>(right.nativeBigFloat)));
}

bool operator==(const BigFloat& left, const BigFloat& right)
{
    return *static_cast<boost::multiprecision::cpp_bin_float_100*>(left.nativeBigFloat) == *static_cast<boost::multiprecision::cpp_bin_float_100*>(right.nativeBigFloat);
}

bool operator!=(const BigFloat& left, const BigFloat& right)
{
    return *static_cast<boost::multiprecision::cpp_bin_float_100*>(left.nativeBigFloat) != *static_cast<boost::multiprecision::cpp_bin_float_100*>(right.nativeBigFloat);
}

bool operator<(const BigFloat& left, const BigFloat& right)
{
    return *static_cast<boost::multiprecision::cpp_bin_float_100*>(left.nativeBigFloat) < *static_cast<boost::multiprecision::cpp_bin_float_100*>(right.nativeBigFloat);
}

bool operator>(const BigFloat& left, const BigFloat& right)
{
    return *static_cast<boost::multiprecision::cpp_bin_float_100*>(left.nativeBigFloat) > * static_cast<boost::multiprecision::cpp_bin_float_100*>(right.nativeBigFloat);
}

bool operator<=(const BigFloat& left, const BigFloat& right)
{
    return *static_cast<boost::multiprecision::cpp_bin_float_100*>(left.nativeBigFloat) <= *static_cast<boost::multiprecision::cpp_bin_float_100*>(right.nativeBigFloat);
}

bool operator>=(const BigFloat& left, const BigFloat& right)
{
    return *static_cast<boost::multiprecision::cpp_bin_float_100*>(left.nativeBigFloat) >= *static_cast<boost::multiprecision::cpp_bin_float_100*>(right.nativeBigFloat);
}

std::string BigFloat::ToString() const
{
    return boost::lexical_cast<std::string>(*static_cast<boost::multiprecision::cpp_bin_float_100*>(nativeBigFloat));
}

double BigFloat::ToDouble() const
{
    return static_cast<double>(*static_cast<boost::multiprecision::cpp_bin_float_100*>(nativeBigFloat));
}

BigInt BigFloat::ToBigInt()
{
    return BigInt(new boost::multiprecision::cpp_int(*static_cast<boost::multiprecision::cpp_bin_float_100*>(nativeBigFloat)));
}

BigRational BigFloat::ToBigRational()
{
    return BigRational(new boost::multiprecision::cpp_rational(*static_cast<boost::multiprecision::cpp_bin_float_100*>(nativeBigFloat)));
}

} } // namespace soulng::util
