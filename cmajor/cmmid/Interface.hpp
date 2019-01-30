// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_MID_INTERFACE_INCLUDED
#define CMAJOR_MID_INTERFACE_INCLUDED
#include <cmajor/cmmid/CmmApi.hpp>
#include <cmajor/cmmid/BackEnd.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <cmajor/ir/EmittingContext.hpp>

extern "C" CMM_API cmajor::ir::EmittingContext* CmmCreateEmittingContext(int optimizationLevel);
extern "C" CMM_API void CmmDestroyEmittingContext(cmajor::ir::EmittingContext* emittingContext);
extern "C" CMM_API cmajor::ir::Emitter* CmmCreateEmitter(cmajor::ir::EmittingContext* emittingContext);
extern "C" CMM_API void CmmDestroyEmitter(cmajor::ir::Emitter* emitterHandle);
extern "C" CMM_API cmajor::mid::BackEndKind GetBackEnd();

#endif // CMAJOR_MID_BACK_END_INCLUDED

