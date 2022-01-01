// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_MID_BACK_END_INCLUDED
#define CMAJOR_MID_BACK_END_INCLUDED
#include <cmajor/ir/Emitter.hpp>
#include <cmajor/ir/EmittingContext.hpp>

namespace cmajor { namespace mid {

enum class BackEndKind : int
{
    llvmBackEnd = 0, cmsxBackEnd = 1, cmcppBackEnd = 2
};

class BackEnd
{
public:
    virtual ~BackEnd();
    virtual void Init() = 0;
    virtual void Done() = 0;
    virtual cmajor::ir::EmittingContext* CreateEmittingContext(int optimizationLevel) = 0;
    virtual void DestroyEmittingContext(cmajor::ir::EmittingContext* emittingContext) = 0;
    virtual cmajor::ir::Emitter* CreateEmitter(cmajor::ir::EmittingContext* emittingContext) = 0;
    virtual void DestroyEmitter(cmajor::ir::Emitter* emitter) = 0;
    virtual BackEndKind GetBackEndKind() = 0;
};

void SetBackEnd(BackEndKind backEndKind);
BackEndKind GetBackEnd();

extern BackEnd* backEnd;

} } // namespace cmajor::mid

#endif // CMAJOR_MID_BACK_END_INCLUDED
