// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_EMITTER_EMITTER_INCLUDED
#define CMAJOR_EMITTER_EMITTER_INCLUDED
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/emitter/EmittingContext.hpp>

namespace cmajor { namespace emitter {

using namespace cmajor::binder;

void GenerateCode(EmittingContext& emittingContext, BoundCompileUnit& boundCompileUnit);

} } // namespace cmajor::emitter

#endif // CMAJOR_EMITTER_EMITTER_INCLUDED
