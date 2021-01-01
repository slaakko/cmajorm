// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_MULTIPRECISION_INCLUDED
#define CMAJOR_RT_MULTIPRECISION_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <stdint.h>

//  Integer functions:

extern "C" RT_API void* RtCreateDefaultBigInt(int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigIntFromInt(int32_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigIntFromUInt(uint32_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigIntFromLong(int64_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigIntFromULong(uint64_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigIntFromStr(const char* v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigIntFromCopy(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void RtDestroyBigInt(void* handle);
extern "C" RT_API const char* RtBigIntToCharPtr(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void RtDeleteCharPtr(const char* ptr);
extern "C" RT_API int32_t RtBigIntToInt(void* handle, int32_t & errorStrHandle);
extern "C" RT_API uint32_t RtBigIntToUInt(void* handle, int32_t & errorStrHandle);
extern "C" RT_API int64_t RtBigIntToLong(void* handle, int32_t & errorStrHandle);
extern "C" RT_API uint64_t RtBigIntToULong(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtNegBigInt(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtPosBigInt(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtCplBigInt(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtAddBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtSubBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtMulBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtDivBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtModBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtAndBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtOrBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtXorBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtShiftLeftBigInt(void* left, int32_t right, int32_t & errorStrHandle);
extern "C" RT_API void* RtShiftRightBigInt(void* left, int32_t right, int32_t & errorStrHandle);
extern "C" RT_API bool RtEqualBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtNotEqualBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtLessBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtGreaterBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtLessEqualBigInt(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtGreaterEqualBigInt(void* left, void* right, int32_t & errorStrHandle);

//  Rational functions:

extern "C" RT_API void* RtCreateDefaultBigRational(int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigRationalFromInt(int32_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigRationalFromUInt(uint32_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigRationalFromLong(int64_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigRationalFromULong(uint64_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigRationalFromStr(const char* v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigRationalFromCopy(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigRationalFromBigInt(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigRationalFromBigInts(void* numerator, void* denominator, int32_t & errorStrHandle);
extern "C" RT_API void RtDestroyBigRational(void* handle);
extern "C" RT_API const char* RtBigRationalToCharPtr(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtBigRationalToBigInt(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtNegBigRational(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtPosBigRational(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtAddBigRational(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtSubBigRational(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtMulBigRational(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtDivBigRational(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtEqualBigRational(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtNotEqualBigRational(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtLessBigRational(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtGreaterBigRational(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtLessEqualBigRational(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtGreaterEqualBigRational(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtNumeratorBigRational(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtDenominatorBigRational(void* handle, int32_t & errorStrHandle);

// Float functions:

extern "C" RT_API void* RtCreateDefaultBigFloat(int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigFloatFromInt(int32_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigFloatFromUInt(uint32_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigFloatFromLong(int64_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigFloatFromULong(uint64_t v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigFloatFromDouble(double v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigFloatFromStr(const char* v, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigFloatFromCopy(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigFloatFromBigInt(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtCreateBigFloatFromBigRational(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void RtDestroyBigFloat(void* handle);
extern "C" RT_API const char* RtBigFloatToCharPtr(void* handle, int32_t & errorStrHandle);
extern "C" RT_API double RtBigFloatToDouble(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtBigFloatToBigInt(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtBigFloatToBigRational(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtNegBigFloat(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtPosBigFloat(void* handle, int32_t & errorStrHandle);
extern "C" RT_API void* RtAddBigFloat(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtSubBigFloat(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtMulBigFloat(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API void* RtDivBigFloat(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtEqualBigFloat(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtNotEqualBigFloat(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtLessBigFloat(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtGreaterBigFloat(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtLessEqualBigFloat(void* left, void* right, int32_t & errorStrHandle);
extern "C" RT_API bool RtGreaterEqualBigFloat(void* left, void* right, int32_t & errorStrHandle);

#endif // CMAJOR_RT_MULTIPRECISION_INCLUDED
