// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_GLOBAL_FLAGS_INCLUDED
#define CMAJOR_SYMBOLS_GLOBAL_FLAGS_INCLUDED
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
    debugCompile = 1 << 23
};

void ResetGlobalFlags();
void SetGlobalFlag(GlobalFlags flag);
void ResetGlobalFlag(GlobalFlags flag);
bool GetGlobalFlag(GlobalFlags flag);

std::string GetConfig();
int GetOptimizationLevel();
void SetOptimizationLevel(int optimizationLevel_);
int GetNumBuildThreads();
void SetNumBuildThreads(int numBuildThreads_);

void SetCompilerVersion(const std::string& compilerVersion_);
std::string GetCompilerVersion();

void DefineCommandLineConditionalSymbol(const std::u32string& symbol);
std::set<std::u32string> GetCommandLineDefines();

bool BeginUnitTest();
bool InUnitTest();
void ResetUnitTestAssertionNumber();
int32_t GetNextUnitTestAssertionNumber();
int32_t GetNumUnitTestAssertions();
void EndUnitTest(bool prevUnitTest);
void SetAssertionLineNumberVector(std::vector<int32_t>* assertionLineNumberVector_);
void AddAssertionLineNumber(int32_t lineNumber);

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_GLOBAL_FLAGS_INCLUDED
