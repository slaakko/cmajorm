// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/symbols/Exception.hpp>

namespace cmajor { namespace binder {

BoundClass::BoundClass(Module* module_, ClassTypeSymbol* classTypeSymbol_) : 
    BoundNode(module_, classTypeSymbol_->GetSpan(), BoundNodeType::boundClass), classTypeSymbol(classTypeSymbol_), inlineFunctionContainer(false)
{
}

void BoundClass::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

void BoundClass::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(),  "cannot load from class", GetSpan());
}

void BoundClass::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to class", GetSpan());
}

void BoundClass::AddMember(std::unique_ptr<BoundNode>&& member)
{
    members.push_back(std::move(member));
}

bool BoundClass::ContainsSourceFunctions() const
{
    for (const auto& member : members)
    {
        if (member->GetBoundNodeType() == BoundNodeType::boundFunction)
        {
            BoundFunction* boundFunction = static_cast<BoundFunction*>(member.get());
            if (boundFunction->GetFunctionSymbol()->HasSource())
            {
                return true;
            }
        }
    }
    return false;
}

} } // namespace cmajor::binder
