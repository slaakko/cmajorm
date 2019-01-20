// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/emitter/EmittingContext.hpp>
#include <cmajor/emitter/EmittingContextImpl.hpp>

namespace cmajor { namespace emitter {

EmittingContext::EmittingContext() : emittingContextImpl(new EmittingContextImpl())
{
}

EmittingContext::~EmittingContext()
{
    delete emittingContextImpl;
}

} } // namespace cmajor::emitter
