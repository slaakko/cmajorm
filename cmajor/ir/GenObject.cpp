// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ir/GenObject.hpp>
#include <cmajor/ir/Emitter.hpp>

namespace cmajor { namespace ir {

GenObject::GenObject() : type(nullptr)
{
}

GenObject::~GenObject()
{
}

void NativeValue::Load(Emitter& emitter, OperationFlags flags)
{
    emitter.Stack().Push(value);
}

void NativeValue::Store(Emitter& emitter, OperationFlags flags)
{
    void* val = emitter.Stack().Pop();
    emitter.CreateStore(val, value);
}

} } // namespace cmajor::ir
