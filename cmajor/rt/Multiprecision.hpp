// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_MULTIPRECISION_INCLUDED
#define CMAJOR_RT_MULTIPRECISION_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

//  Integer functions:

extern "C" RT_API void* RtCreateBigInt();
extern "C" RT_API void RtDestroyBigInt(void* bigInt);
extern "C" RT_API void RtAssignBigIntInt(void* bigInt, int32_t i);
extern "C" RT_API void RtAssignBigIntUInt(void* bigInt, uint32_t ui);
extern "C" RT_API int32_t RtAssignBigIntStr(void* bigInt, const char* str, int32_t base);
extern "C" RT_API int32_t RtGetBigIntStrHandle(void* bigInt, int32_t base);
extern "C" RT_API const char* RtGetBigIntStr(int32_t bigIntStrHandle);
extern "C" RT_API void RtFreeBigIntStr(int32_t bigIntStrHandle);
extern "C" RT_API void RtNegBigInt(void* result, void* bigInt);
extern "C" RT_API void RtAbsBigInt(void* result, void* bigInt);
extern "C" RT_API void RtAddBigInt(void* result, void* left, void* right);
extern "C" RT_API void RtSubBigInt(void* result, void* left, void* right);
extern "C" RT_API void RtMulBigInt(void* result, void* left, void* right);
extern "C" RT_API void RtDivBigInt(void* result, void* left, void* right);
extern "C" RT_API void RtRemBigInt(void* result, void* left, void* right);
extern "C" RT_API int32_t RtCmpBigInt(void* left, void* right);
extern "C" RT_API void RtAndBigInt(void* result, void* left, void* right);
extern "C" RT_API void RtOrBigInt(void* result, void* left, void* right);
extern "C" RT_API void RtXorBigInt(void* result, void* left, void* right);
extern "C" RT_API void RtCplBigInt(void* result, void* bigInt);
extern "C" RT_API void RtSetBitBigInt(void* bigInt, uint32_t bitIndex);
extern "C" RT_API void RtClearBitBigInt(void* bigInt, uint32_t bitIndex);
extern "C" RT_API void RtToggleBitBigInt(void* bigInt, uint32_t bitIndex);
extern "C" RT_API int32_t RtTestBitBigInt(void* bigInt, uint32_t bitIndex);

//  Rational functions:

extern "C" RT_API void* RtCreateBigRational();
extern "C" RT_API void RtDestroyBigRational(void* bigRational);
extern "C" RT_API void RtCanonicalizeBigRational(void* bigRational);
extern "C" RT_API void RtAssignBigRational(void* left, void* right);
extern "C" RT_API int32_t RtAssignBigRationalStr(void* left, const char* str, int32_t base);
extern "C" RT_API void RtSetBigRationalInt(void* bigRational, int32_t numerator, int32_t denominator);
extern "C" RT_API void RtSetBigRationalUInt(void* bigRational, uint32_t numerator, uint32_t denominator);
extern "C" RT_API int32_t RtGetBigRationalStrHandle(void* bigRational, int32_t base);
extern "C" RT_API const char* RtGetBigRationalStr(int32_t bigRationalStrHandle);
extern "C" RT_API void RtFreeBigRationalStr(int32_t bigRationalStrHandle);
extern "C" RT_API void RtSetBigRationalBigInt(void* bigRational, void* bigInt);
extern "C" RT_API void RtAddBigRational(void* target, void* left, void* right);
extern "C" RT_API void RtSubBigRational(void* target, void* left, void* right);
extern "C" RT_API void RtMulBigRational(void* target, void* left, void* right);
extern "C" RT_API void RtDivBigRational(void* target, void* left, void* right);
extern "C" RT_API void RtNegBigRational(void* left, void* right);
extern "C" RT_API void RtAbsBigRational(void* left, void* right);
extern "C" RT_API int32_t RtCmpBigRational(void* left, void* right);
extern "C" RT_API int32_t RtEqualBigRational(void* left, void* right);
extern "C" RT_API void RtGetNumeratorBigRational(void* numeratorBigInt, void* bigRational);
extern "C" RT_API void RtGetDenominatorBigRational(void* denominatorBigInt, void* bigRational);

// Float functions:

extern "C" RT_API void RtBigFloatSetDefaultPrec(uint32_t prec);
extern "C" RT_API uint32_t RtBigFloatGetDefaultPrec();
extern "C" RT_API void* RtCreateBigFloat();
extern "C" RT_API void* RtCreateBitFloatPrec(uint32_t prec);
extern "C" RT_API void RtDestroyBigFloat(void* bigFloat);
extern "C" RT_API uint32_t RtGetBigFloatPrec(void* bigFloat);
extern "C" RT_API void RtSetBigFloatPrec(void* bigFloat, uint32_t prec);
extern "C" RT_API void RtSetBigFloat(void* left, void* right);
extern "C" RT_API void RtSetBigFloatUInt(void* left, uint32_t right);
extern "C" RT_API void RtSetBigFloatInt(void* left, int32_t right);
extern "C" RT_API void RtSetBigFloatDouble(void* left, double right);
extern "C" RT_API void RtSetBigFloatBigInt(void* left, void* right);
extern "C" RT_API void RtSetBigFloatBigRational(void* left, void* right);
extern "C" RT_API int32_t RtSetBigFloatStr(void* left, const char* str, int32_t base);
extern "C" RT_API int32_t RtGetBigFloatStrHandle(void* bigFloat, int32_t base, uint32_t numDigits, int64_t* exponent);
extern "C" RT_API const char* RtGetBigFloatStr(int32_t bigFloatStrHandle);
extern "C" RT_API void RtFreeBigFloatStr(int32_t bigFloatStrHandle);
extern "C" RT_API void RtAddBigFloat(void* target, void* left, void* right);
extern "C" RT_API void RtSubBigFloat(void* target, void* left, void* right);
extern "C" RT_API void RtMulBigFloat(void* target, void* left, void* right);
extern "C" RT_API void RtDivBigFloat(void* target, void* left, void* right);
extern "C" RT_API void RtSqrtBigFloat(void* target, void* subject);
extern "C" RT_API void RtNegBigFloat(void* target, void* subject);
extern "C" RT_API void RtAbsBigFloat(void* target, void* subject);
extern "C" RT_API int32_t RtCmpBigFloat(void* left, void* right);
extern "C" RT_API void RtCeilBigFloat(void* target, void* subject);
extern "C" RT_API void RtFloorBigFloat(void* target, void* subject);
extern "C" RT_API void RtTruncBigFloat(void* target, void* subject);
extern "C" RT_API double RtGetBigFloatDouble(void* bigFloat);

#endif // CMAJOR_RT_MULTIPRECISION_INCLUDED
