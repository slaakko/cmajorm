// =================================
// Copyright (c) 2018 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCM_COMPILER_INCLUDED
#define CMAJOR_CMCM_COMPILER_INCLUDED
#include <cmajor/cmcm/Api.hpp>
#include <stdint.h>

extern "C" CMCM_API void Init();
extern "C" CMCM_API void Done();
extern "C" CMCM_API int Compile(const char16_t* compileXmlRequest);
extern "C" CMCM_API int GetCompileResultLength(int compileResultHandle);
extern "C" CMCM_API int GetCompileResult(int compileResultHandle, char16_t* buf, int size);
extern "C" CMCM_API void StopBuild();
extern "C" CMCM_API int WaitForLogMessage();
extern "C" CMCM_API int FetchLogMessage(char16_t* buf, int size);
extern "C" CMCM_API void StartLog();
extern "C" CMCM_API void EndLog();
extern "C" CMCM_API void ResetModuleCache();
extern "C" CMCM_API void SetUseModuleCache(bool useModuleCache_);

#endif // CMAJOR_CMCM_COMPILER_INCLUDED
