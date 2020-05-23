// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/rts/Multiprecision.hpp>
#include <cmajor/rts/String.hpp>
#define CMRT_IMPORT
#include <cmajor/system/ext/gmp/gmpintf.h>

extern "C" RT_API void* RtCreateBigInt()
{
    return create_mpz();
}

extern "C" RT_API void RtDestroyBigInt(void* bigInt)
{
    destroy_mpz(bigInt);
}

extern "C" RT_API void RtAssignBigIntBigInt(void* left, void* right)
{
    assign_mpz(left, right);
}

extern "C" RT_API void RtAssignBigIntInt(void* bigInt, int32_t i)
{
    assign_mpz_si(bigInt, i);
}

extern "C" RT_API void RtAssignBigIntUInt(void* bigInt, uint32_t ui)
{
    assign_mpz_ui(bigInt, ui);
}

extern "C" RT_API int32_t RtAssignBigIntStr(void* bigInt, const char* str, int32_t base)
{
    return assign_mpz_str(bigInt, str, base);
}

extern "C" RT_API int32_t RtGetBigIntStrHandle(void* bigInt, int32_t base)
{
    char* str = get_mpz_str(bigInt, base);
    std::string s = str;
    free_mpz_str(str);
    int32_t stringHandle = cmajor::rt::InstallString(s);
    return stringHandle;
}

extern "C" RT_API const char* RtGetBigIntStr(int32_t bigIntStrHandle)
{
    return cmajor::rt::GetString(bigIntStrHandle);
}

extern "C" RT_API void RtFreeBigIntStr(int32_t bigIntStrHandle)
{
    cmajor::rt::DisposeString(bigIntStrHandle);
}

extern "C" RT_API void RtNegBigInt(void* result, void* bigInt)
{
    neg_mpz(result, bigInt);
}

extern "C" RT_API void RtAbsBigInt(void* result, void* bigInt)
{
    abs_mpz(result, bigInt);
}

extern "C" RT_API void RtAddBigInt(void* result, void* left, void* right)
{
    add_mpz(result, left, right);
}

extern "C" RT_API void RtSubBigInt(void* result, void* left, void* right)
{
    sub_mpz(result, left, right);
}

extern "C" RT_API void RtMulBigInt(void* result, void* left, void* right)
{
    mul_mpz(result, left, right);
}

extern "C" RT_API void RtDivBigInt(void* result, void* left, void* right)
{
    div_mpz(result, left, right);
}

extern "C" RT_API void RtRemBigInt(void* result, void* left, void* right)
{
    rem_mpz(result, left, right);
}

extern "C" RT_API int32_t RtCmpBigInt(void* left, void* right)
{
    return cmp_mpz(left, right);
}

extern "C" RT_API void RtAndBigInt(void* result, void* left, void* right)
{
    and_mpz(result, left, right);
}

extern "C" RT_API void RtOrBigInt(void* result, void* left, void* right)
{
    or_mpz(result, left, right);
}

extern "C" RT_API void RtXorBigInt(void* result, void* left, void* right)
{
    xor_mpz(result, left, right);
}

extern "C" RT_API void RtCplBigInt(void* result, void* bigInt)
{
    cpl_mpz(result, bigInt);
}

extern "C" RT_API void RtSetBitBigInt(void* bigInt, uint32_t bitIndex)
{
    setbit_mpz(bigInt, bitIndex);
}

extern "C" RT_API void RtClearBitBigInt(void* bigInt, uint32_t bitIndex)
{
    clrbit_mpz(bigInt, bitIndex);
}

extern "C" RT_API void RtToggleBitBigInt(void* bigInt, uint32_t bitIndex)
{
    cplbit_mpz(bigInt, bitIndex);
}

extern "C" RT_API int32_t RtTestBitBigInt(void* bigInt, uint32_t bitIndex)
{
    return tstbit_mpz(bigInt, bitIndex);
}

extern "C" RT_API void* RtCreateBigRational()
{
    return create_mpq();
}

extern "C" RT_API void RtDestroyBigRational(void* bigRational)
{
    destroy_mpq(bigRational);
}

extern "C" RT_API void RtCanonicalizeBigRational(void* bigRational)
{
    canonicalize_mpq(bigRational);
}

extern "C" RT_API void RtAssignBigRational(void* left, void* right)
{
    assign_mpq(left, right);
}

extern "C" RT_API int32_t RtAssignBigRationalStr(void* left, const char* str, int32_t base)
{
    return assign_mpq_str(left, str, base);
}

extern "C" RT_API void RtSetBigRationalInt(void* bigRational, int32_t numerator, int32_t denominator)
{
    set_mpq_si(bigRational, numerator, denominator);
}

extern "C" RT_API void RtSetBigRationalUInt(void* bigRational, uint32_t numerator, uint32_t denominator)
{
    set_mpq_ui(bigRational, numerator, denominator);
}

extern "C" RT_API int32_t RtGetBigRationalStrHandle(void* bigRational, int32_t base)
{
    char* str = get_mpq_str(bigRational, base);
    std::string s = str;
    free_mpq_str(str);
    int32_t stringHandle = cmajor::rt::InstallString(s);
    return stringHandle;
}

extern "C" RT_API const char* RtGetBigRationalStr(int32_t bigRationalStrHandle)
{
    return cmajor::rt::GetString(bigRationalStrHandle);
}

extern "C" RT_API void RtFreeBigRationalStr(int32_t bigRationalStrHandle)
{
    cmajor::rt::DisposeString(bigRationalStrHandle);
}

extern "C" RT_API void RtSetBigRationalBigInt(void* bigRational, void* bigInt)
{
    set_mpq_z(bigRational, bigInt);
}

extern "C" RT_API void RtAddBigRational(void* target, void* left, void* right)
{
    add_mpq(target, left, right);
}

extern "C" RT_API void RtSubBigRational(void* target, void* left, void* right)
{
    sub_mpq(target, left, right);
}

extern "C" RT_API void RtMulBigRational(void* target, void* left, void* right)
{
    mul_mpq(target, left, right);
}

extern "C" RT_API void RtDivBigRational(void* target, void* left, void* right)
{
    div_mpq(target, left, right);
}

extern "C" RT_API void RtNegBigRational(void* left, void* right)
{
    neg_mpq(left, right);
}

extern "C" RT_API void RtAbsBigRational(void* left, void* right)
{
    abs_mpq(left, right);
}

extern "C" RT_API int32_t RtCmpBigRational(void* left, void* right)
{
    return cmp_mpq(left, right);
}

extern "C" RT_API int32_t RtEqualBigRational(void* left, void* right)
{
    return equal_mpq(left, right);
}

extern "C" RT_API void RtGetNumeratorBigRational(void* numeratorBigInt, void* bigRational)
{
    get_numerator_mpq(numeratorBigInt, bigRational);
}

extern "C" RT_API void RtGetDenominatorBigRational(void* denominatorBigInt, void* bigRational)
{
    get_denominator_mpq(denominatorBigInt, bigRational);
}

extern "C" RT_API void RtBigFloatSetDefaultPrec(uint32_t prec)
{
    set_default_prec_mpf(prec);
}

extern "C" RT_API uint32_t RtBigFloatGetDefaultPrec() 
{
    return get_default_prec_mpf();
}

extern "C" RT_API void* RtCreateBigFloat()
{
    return create_mpf();
}

extern "C" RT_API void* RtCreateBitFloatPrec(uint32_t prec)
{
    return create_mpf_prec(prec);
}

extern "C" RT_API void RtDestroyBigFloat(void* bigFloat)
{
    destroy_mpf(bigFloat);
}

extern "C" RT_API uint32_t RtGetBigFloatPrec(void* bigFloat)
{
    return get_prec_mpf(bigFloat);
}

extern "C" RT_API void RtSetBigFloatPrec(void* bigFloat, uint32_t prec)
{
    set_prec_mpf(bigFloat, prec);
}

extern "C" RT_API void RtSetBigFloat(void* left, void* right)
{
    set_mpf(left, right);
}

extern "C" RT_API void RtSetBigFloatUInt(void* left, uint32_t right)
{
    set_mpf_ui(left, right);
}

extern "C" RT_API void RtSetBigFloatInt(void* left, int32_t right)
{
    set_mpf_si(left, right);
}

extern "C" RT_API void RtSetBigFloatDouble(void* left, double right)
{
    set_mpf_d(left, right);
}

extern "C" RT_API void RtSetBigFloatBigInt(void* left, void* right)
{
    set_mpf_z(left, right);
}

extern "C" RT_API void RtSetBigFloatBigRational(void* left, void* right)
{
    set_mpf_q(left, right);
}

extern "C" RT_API int32_t RtSetBigFloatStr(void* left, const char* str, int32_t base)
{
    return set_mpf_str(left, str, base);
}

extern "C" RT_API int32_t RtGetBigFloatStrHandle(void* bigFloat, int32_t base, uint32_t numDigits, int64_t* exponent)
{
    char* str = get_mpf_str(bigFloat, base, numDigits, exponent);
    std::string s = str;
    free_mpf_str(str);
    int32_t stringHandle = cmajor::rt::InstallString(s);
    return stringHandle;
}

extern "C" RT_API const char* RtGetBigFloatStr(int32_t bigFloatStrHandle)
{
    return cmajor::rt::GetString(bigFloatStrHandle);
}

extern "C" RT_API void RtFreeBigFloatStr(int32_t bigFloatStrHandle)
{
    cmajor::rt::DisposeString(bigFloatStrHandle);
}

extern "C" RT_API void RtAddBigFloat(void* target, void* left, void* right)
{
    add_mpf(target, left, right);
}

extern "C" RT_API void RtSubBigFloat(void* target, void* left, void* right)
{
    sub_mpf(target, left, right);
}

extern "C" RT_API void RtMulBigFloat(void* target, void* left, void* right)
{
    mul_mpf(target, left, right);
}

extern "C" RT_API void RtDivBigFloat(void* target, void* left, void* right)
{
    div_mpf(target, left, right);
}

extern "C" RT_API void RtSqrtBigFloat(void* target, void* subject)
{
    sqrt_mpf(target, subject);
}

extern "C" RT_API void RtNegBigFloat(void* target, void* subject)
{
    neg_mpf(target, subject);
}

extern "C" RT_API void RtAbsBigFloat(void* target, void* subject)
{
    abs_mpf(target, subject);
}

extern "C" RT_API int32_t RtCmpBigFloat(void* left, void* right)
{
    return cmp_mpf(left, right);
}

extern "C" RT_API void RtCeilBigFloat(void* target, void* subject)
{
    ceil_mpf(target, subject);
}

extern "C" RT_API void RtFloorBigFloat(void* target, void* subject)
{
    floor_mpf(target, subject);
}

extern "C" RT_API void RtTruncBigFloat(void* target, void* subject)
{
    trunc_mpf(target, subject);
}

extern "C" RT_API double RtGetBigFloatDouble(void* bigFloat)
{
    return get_d_mpf(bigFloat);
}
