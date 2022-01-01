// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RTS_UNIT_TEST_INCLUDED
#define CMAJOR_RTS_UNIT_TEST_INCLUDED
#include <cmajor/rts/RtsApi.hpp>
#include <stdint.h>

extern "C" RT_API void RtStartUnitTest(int32_t numAssertions, const char* unitTestFilePath, int64_t numberOfPolymorphicClassIds, const uint64_t* polymorphicClassIdArray,
    int64_t numberOfStaticClassIds, const uint64_t* staticClassIdArray);
extern "C" RT_API void RtEndUnitTest(const char* testName, int32_t exitCode);
extern "C" RT_API void RtSetUnitTestAssertionResult(int32_t assertionIndex, bool assertionResult, int32_t line);
extern "C" RT_API void RtSetUnitTestException(const char* exceptionStr);

#endif // CMAJOR_RTS_UNIT_TEST_INCLUDED
