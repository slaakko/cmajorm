// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/emitter/EmittingContext.hpp>
#include <cmajor/cmmid/Interface.hpp>

namespace cmajor { namespace emitter {

EmittingContext::EmittingContext() : handle(CmmCreateEmittingContext())
{

}

EmittingContext::~EmittingContext()
{
    CmmDestroyEmittingContext(handle);
}

} } // namespace cmajor::emitter
