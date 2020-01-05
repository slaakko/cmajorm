// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ir/Emitter.hpp>

namespace cmajor { namespace ir {

Emitter::Emitter(ValueStack* stack_) : stack(stack_)
{
}

Emitter::~Emitter()
{
}

} } // namespace cmajor::ir
