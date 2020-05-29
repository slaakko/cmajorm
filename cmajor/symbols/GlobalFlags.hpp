// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_GLOBAL_FLAGS_INCLUDED
#define CMAJOR_SYMBOLS_GLOBAL_FLAGS_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <string>
#include <vector>
#include <stdint.h>
#include <set>

namespace cmajor { namespace symbols {

enum class GlobalFlags : uint32_t
{
    none = 0,
    verbose = 1 << 0,
    quiet = 1 << 1,
    release = 1 << 2,
    clean = 1 << 3,
    debugParsing = 1 << 4,
    emitLlvm = 1 << 5,
    emitOptLlvm = 1 << 6,
    linkWithDebugRuntime = 1 << 7,
    linkUsingMsLink = 1 << 8,
    ide = 1 << 9,
    strictNothrow = 1 << 10,
    time = 1 << 11,
    info = 1 << 12,
    unitTest = 1 << 13,
    profile = 1 << 14,
    generateDebugInfo = 1 << 15,
    msbuild = 1 << 16,
    ast2xml = 1 << 17,
    sym2xml = 1 << 18,
    bdt2xml = 1 << 19,
    cmdoc = 1 << 20,
    optimizeCmDoc = 1 << 21,
    singleThreadedCompile = 1 << 22,
    debugCompile = 1 << 23,
    rebuild = 1 << 24,
    disableCodeGen = 1 << 25,
    disableSystem = 1 << 26,
    justMyCodeDebugging =  1 << 27
};

enum class BackEnd : int
{
    llvm = 0, cmsx = 1, cmcpp = 2
};

SYMBOLS_API void SetBackEnd(BackEnd backend_);
SYMBOLS_API BackEnd GetBackEnd();

SYMBOLS_API void ResetGlobalFlags();
SYMBOLS_API void SetGlobalFlag(GlobalFlags flag);
SYMBOLS_API void ResetGlobalFlag(GlobalFlags flag);
SYMBOLS_API bool GetGlobalFlag(GlobalFlags flag);

SYMBOLS_API std::string GetConfig();
SYMBOLS_API int GetOptimizationLevel();
SYMBOLS_API void SetOptimizationLevel(int optimizationLevel_);
SYMBOLS_API int GetNumBuildThreads();
SYMBOLS_API void SetNumBuildThreads(int numBuildThreads_);

SYMBOLS_API void SetCompilerVersion(const std::string& compilerVersion_);
SYMBOLS_API std::string GetCompilerVersion();

SYMBOLS_API void DefineCommandLineConditionalSymbol(const std::u32string& symbol);
SYMBOLS_API std::set<std::u32string> GetCommandLineDefines();

SYMBOLS_API bool BeginUnitTest();
SYMBOLS_API bool InUnitTest();
SYMBOLS_API void ResetUnitTestAssertionNumber();
SYMBOLS_API int32_t GetNextUnitTestAssertionNumber();
SYMBOLS_API int32_t GetNumUnitTestAssertions();
SYMBOLS_API void EndUnitTest(bool prevUnitTest);
SYMBOLS_API void SetAssertionLineNumberVector(std::vector<int32_t>* assertionLineNumberVector_);
SYMBOLS_API void AddAssertionLineNumber(int32_t lineNumber);

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_GLOBAL_FLAGS_INCLUDED
