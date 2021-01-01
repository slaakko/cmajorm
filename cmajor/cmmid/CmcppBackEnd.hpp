// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_MID_CMCPP_BACK_END_INCLUDED
#define CMAJOR_MID_CMCPP_BACK_END_INCLUDED
#include <cmajor/cmmid/BackEnd.hpp>

namespace cmajor { namespace mid {

class CmcppBackEnd : public BackEnd
{
public:
    void Init() override;
    void Done() override;
    cmajor::ir::EmittingContext* CreateEmittingContext(int optimizationLevel) override;
    void DestroyEmittingContext(cmajor::ir::EmittingContext* emittingContext) override;
    cmajor::ir::Emitter* CreateEmitter(cmajor::ir::EmittingContext* emittingContext) override;
    void DestroyEmitter(cmajor::ir::Emitter* emitter) override;
    BackEndKind GetBackEndKind() override { return BackEndKind::cmcppBackEnd; }
};

} } // namespace cmajor::mid

#endif // CMAJOR_MID_CMCPP_BACK_END_INCLUDED
