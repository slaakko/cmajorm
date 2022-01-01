// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGEN_INTERFACE_INCLUDED
#define CMAJOR_CODEGEN_INTERFACE_INCLUDED
#include <cmajor/codegen/CodeGenApi.hpp>
#include <cmajor/codegen/EmittingContext.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>

namespace cmajor { namespace codegen {

CODEGEN_API void GenerateCode(cmajor::codegen::EmittingContext& emittingContext, cmajor::binder::BoundCompileUnit& boundCompileUnit);

} } // namespace cmajor::codegen

#endif // CMAJOR_CODEGEN_INTERFACE_INCLUDED
