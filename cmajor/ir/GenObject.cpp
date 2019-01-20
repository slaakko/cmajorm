// =================================
// Copyright (c) 2019 Seppo Laakko
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

void LlvmValue::Load(Emitter& emitter, OperationFlags flags)
{ 
    emitter.Stack().Push(value); 
}

void LlvmValue::Store(Emitter& emitter, OperationFlags flags)
{ 
    llvm::Value* val = emitter.Stack().Pop();
    emitter.Builder().CreateStore(val, value);
}

} } // namespace cmajor::ir
