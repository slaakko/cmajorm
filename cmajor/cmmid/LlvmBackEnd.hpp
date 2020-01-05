// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_MID_LLVM_BACK_END_INCLUDED
#define CMAJOR_MID_LLVM_BACK_END_INCLUDED
#include <cmajor/cmmid/BackEnd.hpp>

namespace cmajor { namespace mid {

class LLvmBackEnd : public BackEnd
{
public:
    void Init() override;
    void Done() override;
    cmajor::ir::EmittingContext* CreateEmittingContext(int optimizationLevel) override;
    void DestroyEmittingContext(cmajor::ir::EmittingContext* emittingContext) override;
    cmajor::ir::Emitter* CreateEmitter(cmajor::ir::EmittingContext* emittingContext) override;
    void DestroyEmitter(cmajor::ir::Emitter* emitter) override;
    BackEndKind GetBackEndKind() { return BackEndKind::llvmBackEnd; }
};

} } // namespace cmajor::mid

#endif // CMAJOR_MID_LLVM_BACK_END_INCLUDED
