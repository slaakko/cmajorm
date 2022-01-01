// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Multiprecision.hpp>
#include <cmajor/rts/String.hpp>
#include <soulng/util/Multiprecision.hpp>
#include <string.h>

void* RtCreateDefaultBigInt(int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt();
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigIntFromInt(int32_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigIntFromUInt(uint32_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigIntFromLong(int64_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigIntFromULong(uint64_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigIntFromStr(const char* v, int32_t& errorStrHandle)
{
    try
    {
        std::string s(v);
        return new soulng::util::BigInt(s);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigIntFromCopy(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(*static_cast<soulng::util::BigInt*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void RtDestroyBigInt(void* handle)
{
    delete static_cast<soulng::util::BigInt*>(handle);
}

const char* RtBigIntToCharPtr(void* handle, int32_t& errorStrHandle)
{
    try
    {
        std::string s = static_cast<soulng::util::BigInt*>(handle)->ToString();
        int n = s.length() + 1;
        char* p = new char[n];
        memcpy(p, s.c_str(), n);
        return p;
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void RtDeleteCharPtr(const char* ptr)
{
    delete[] ptr;
}

int32_t RtBigIntToInt(void* handle, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return static_cast<soulng::util::BigInt*>(handle)->ToInt();
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return 0;
    }
}

uint32_t RtBigIntToUInt(void* handle, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return static_cast<soulng::util::BigInt*>(handle)->ToUInt();
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return 0;
    }
}

int64_t RtBigIntToLong(void* handle, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return static_cast<soulng::util::BigInt*>(handle)->ToLong();
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return 0;
    }
}

uint64_t RtBigIntToULong(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return static_cast<soulng::util::BigInt*>(handle)->ToULong();
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return 0;
    }
}

void* RtNegBigInt(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(-*static_cast<soulng::util::BigInt*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtPosBigInt(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(+*static_cast<soulng::util::BigInt*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCplBigInt(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(~*static_cast<soulng::util::BigInt*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtAddBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(*static_cast<soulng::util::BigInt*>(left) + *static_cast<soulng::util::BigInt*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtSubBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(*static_cast<soulng::util::BigInt*>(left) - *static_cast<soulng::util::BigInt*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtMulBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(*static_cast<soulng::util::BigInt*>(left) * *static_cast<soulng::util::BigInt*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtDivBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(*static_cast<soulng::util::BigInt*>(left) / *static_cast<soulng::util::BigInt*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtModBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(*static_cast<soulng::util::BigInt*>(left) % *static_cast<soulng::util::BigInt*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtAndBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(*static_cast<soulng::util::BigInt*>(left) & *static_cast<soulng::util::BigInt*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtOrBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(*static_cast<soulng::util::BigInt*>(left) | *static_cast<soulng::util::BigInt*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtXorBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(*static_cast<soulng::util::BigInt*>(left) ^ *static_cast<soulng::util::BigInt*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtShiftLeftBigInt(void* left, int32_t right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(*static_cast<soulng::util::BigInt*>(left) << right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtShiftRightBigInt(void* left, int32_t right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(*static_cast<soulng::util::BigInt*>(left) >> right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

bool RtEqualBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigInt*>(left) == *static_cast<soulng::util::BigInt*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtNotEqualBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigInt*>(left) != *static_cast<soulng::util::BigInt*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtLessBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigInt*>(left) < *static_cast<soulng::util::BigInt*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtGreaterBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigInt*>(left) > *static_cast<soulng::util::BigInt*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtLessEqualBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigInt*>(left) <= * static_cast<soulng::util::BigInt*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtGreaterEqualBigInt(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigInt*>(left) >= *static_cast<soulng::util::BigInt*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

void* RtCreateDefaultBigRational(int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational();
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigRationalFromInt(int32_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigRationalFromUInt(uint32_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigRationalFromLong(int64_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigRationalFromULong(uint64_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigRationalFromStr(const char* v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigRationalFromCopy(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(*static_cast<soulng::util::BigRational*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigRationalFromBigInt(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(*static_cast<soulng::util::BigInt*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigRationalFromBigInts(void* numerator, void* denominator, int32_t & errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(*static_cast<soulng::util::BigInt*>(numerator), *static_cast<soulng::util::BigInt*>(denominator));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void RtDestroyBigRational(void* handle)
{
    delete static_cast<soulng::util::BigRational*>(handle);
}

const char* RtBigRationalToCharPtr(void* handle, int32_t& errorStrHandle)
{
    try
    {
        std::string s = static_cast<soulng::util::BigRational*>(handle)->ToString();
        int n = s.length() + 1;
        char* p = new char[n];
        memcpy(p, s.c_str(), n);
        return p;
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtBigRationalToBigInt(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(static_cast<soulng::util::BigRational*>(handle)->ToBigInt());
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtNegBigRational(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(-*static_cast<soulng::util::BigRational*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtPosBigRational(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(+*static_cast<soulng::util::BigRational*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtAddBigRational(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(*static_cast<soulng::util::BigRational*>(left) + *static_cast<soulng::util::BigRational*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtSubBigRational(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(*static_cast<soulng::util::BigRational*>(left) - *static_cast<soulng::util::BigRational*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtMulBigRational(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(*static_cast<soulng::util::BigRational*>(left) * *static_cast<soulng::util::BigRational*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtDivBigRational(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(*static_cast<soulng::util::BigRational*>(left) / *static_cast<soulng::util::BigRational*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

bool RtEqualBigRational(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigRational*>(left) == *static_cast<soulng::util::BigRational*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtNotEqualBigRational(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigRational*>(left) != *static_cast<soulng::util::BigRational*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtLessBigRational(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigRational*>(left) < *static_cast<soulng::util::BigRational*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtGreaterBigRational(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigRational*>(left) > *static_cast<soulng::util::BigRational*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtLessEqualBigRational(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigRational*>(left) <= * static_cast<soulng::util::BigRational*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtGreaterEqualBigRational(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigRational*>(left) >= *static_cast<soulng::util::BigRational*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

void* RtNumeratorBigRational(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(soulng::util::Numerator(*static_cast<soulng::util::BigRational*>(handle)));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtDenominatorBigRational(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(soulng::util::Denominator(*static_cast<soulng::util::BigRational*>(handle)));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateDefaultBigFloat(int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat();
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigFloatFromInt(int32_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigFloatFromUInt(uint32_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigFloatFromLong(int64_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigFloatFromULong(uint64_t v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigFloatFromDouble(double v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigFloatFromStr(const char* v, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(v);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigFloatFromCopy(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(*static_cast<soulng::util::BigFloat*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigFloatFromBigInt(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(*static_cast<soulng::util::BigInt*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtCreateBigFloatFromBigRational(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(*static_cast<soulng::util::BigRational*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void RtDestroyBigFloat(void* handle)
{
    delete static_cast<soulng::util::BigFloat*>(handle);
}

const char* RtBigFloatToCharPtr(void* handle, int32_t& errorStrHandle)
{
    try
    {
        std::string s = static_cast<soulng::util::BigFloat*>(handle)->ToString();
        int n = s.length() + 1;
        char* p = new char[n];
        memcpy(p, s.c_str(), n);
        return p;
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

double RtBigFloatToDouble(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return static_cast<soulng::util::BigFloat*>(handle)->ToDouble();
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return 0.0;
    }
}

void* RtBigFloatToBigInt(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigInt(static_cast<soulng::util::BigFloat*>(handle)->ToBigInt());
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtBigFloatToBigRational(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigRational(static_cast<soulng::util::BigFloat*>(handle)->ToBigRational());
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtNegBigFloat(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(-*static_cast<soulng::util::BigFloat*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtPosBigFloat(void* handle, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(+*static_cast<soulng::util::BigFloat*>(handle));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtAddBigFloat(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(*static_cast<soulng::util::BigFloat*>(left) + *static_cast<soulng::util::BigFloat*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtSubBigFloat(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(*static_cast<soulng::util::BigFloat*>(left) - *static_cast<soulng::util::BigFloat*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtMulBigFloat(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(*static_cast<soulng::util::BigFloat*>(left) * *static_cast<soulng::util::BigFloat*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

void* RtDivBigFloat(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        return new soulng::util::BigFloat(*static_cast<soulng::util::BigFloat*>(left) / *static_cast<soulng::util::BigFloat*>(right));
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return nullptr;
    }
}

bool RtEqualBigFloat(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigFloat*>(left) == *static_cast<soulng::util::BigFloat*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtNotEqualBigFloat(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigFloat*>(left) != *static_cast<soulng::util::BigFloat*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtLessBigFloat(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigFloat*>(left) < *static_cast<soulng::util::BigFloat*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtGreaterBigFloat(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigFloat*>(left) > *static_cast<soulng::util::BigFloat*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtLessEqualBigFloat(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigFloat*>(left) <= * static_cast<soulng::util::BigFloat*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}

bool RtGreaterEqualBigFloat(void* left, void* right, int32_t& errorStrHandle)
{
    try
    {
        errorStrHandle = -1;
        return *static_cast<soulng::util::BigFloat*>(left) >= *static_cast<soulng::util::BigFloat*>(right);
    }
    catch (const std::exception& ex)
    {
        errorStrHandle = cmajor::rt::InstallString(ex.what());
        return false;
    }
}
