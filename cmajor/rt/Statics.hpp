// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_RT_STATICS_INCLUDED
#define CMAJOR_RT_STATICS_INCLUDED
#include <cmajor/rt/RtApi.hpp>
#include <boost/uuid/uuid.hpp>
#include <vector>
#include <stdint.h>

extern "C" RT_API void RtBeginStaticInitCriticalSection(void* staticClassId);
extern "C" RT_API void RtEndStaticInitCriticalSection(void* staticClassId);
extern "C" RT_API void RtEnqueueDestruction(void* destructor, void* arg);

namespace cmajor { namespace rt {

void AllocateMutexes(const std::vector<boost::uuids::uuid>& staticClassIds);

void InitStatics();
void DoneStatics();

} }  // namespace cmajor::rt

#endif // CMAJOR_RT_STATICS_INCLUDED
