// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundExpression.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/EnumSymbol.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;
 
BoundExpression::BoundExpression(const Span& span_, const boost::uuids::uuid& moduleId_, BoundNodeType boundNodeType_, TypeSymbol* type_) :
    BoundNode(span_, moduleId_, boundNodeType_), type(type_), flags(BoundExpressionFlags::none)
{
}

void BoundExpression::AddTemporaryDestructorCall(std::unique_ptr<BoundFunctionCall>&& destructorCall)
{
    temporaryDestructorCalls.push_back(std::move(destructorCall));
}

void BoundExpression::MoveTemporaryDestructorCallsTo(BoundExpression& expression)
{
    for (std::unique_ptr<BoundFunctionCall>& destructorCall : temporaryDestructorCalls)
    {
        expression.AddTemporaryDestructorCall(std::move(destructorCall));
    }
    temporaryDestructorCalls.clear();
}

void BoundExpression::DestroyTemporaries(Emitter& emitter)
{
    for (const std::unique_ptr<BoundFunctionCall>& destructorCall : temporaryDestructorCalls)
    {
        destructorCall->Load(emitter, OperationFlags::none);
    }
}

BoundParameter::BoundParameter(const Span& span_, const boost::uuids::uuid& moduleId_, ParameterSymbol* parameterSymbol_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundParameter, parameterSymbol_->GetType()), parameterSymbol(parameterSymbol_)
{
}

BoundExpression* BoundParameter::Clone()
{
    return new BoundParameter(GetSpan(), ModuleId(), parameterSymbol);
}

void BoundParameter::Load(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        emitter.Stack().Push(parameterSymbol->IrObject(emitter));
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        void* value = emitter.CreateLoad(parameterSymbol->IrObject(emitter));
        uint8_t n = GetDerefCount(flags);
        for (uint8_t i = 0; i < n; ++i)
        {
            value = emitter.CreateLoad(value);
        }
        emitter.Stack().Push(value);
    }
    else
    {
        emitter.Stack().Push(emitter.CreateLoad(parameterSymbol->IrObject(emitter)));
    }
    DestroyTemporaries(emitter);
}

void BoundParameter::Store(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    void* value = emitter.Stack().Pop();
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot take address of a parameter", GetSpan(), ModuleId());
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        void* ptr = emitter.CreateLoad(parameterSymbol->IrObject(emitter));
        uint8_t n = GetDerefCount(flags);
        for (uint8_t i = 1; i < n; ++i)
        {
            ptr = emitter.CreateLoad(ptr);
        }
        emitter.CreateStore(value, ptr);
    }
    else
    {
        emitter.CreateStore(value, parameterSymbol->IrObject(emitter));
    }
    DestroyTemporaries(emitter);
}

void BoundParameter::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundLocalVariable::BoundLocalVariable(const Span& span_, const boost::uuids::uuid& moduleId_, LocalVariableSymbol* localVariableSymbol_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundLocalVariable, localVariableSymbol_->GetType()), localVariableSymbol(localVariableSymbol_)
{
}

BoundExpression* BoundLocalVariable::Clone()
{
    return new BoundLocalVariable(GetSpan(), ModuleId(), localVariableSymbol);
}

void BoundLocalVariable::Load(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        emitter.Stack().Push(localVariableSymbol->IrObject(emitter));
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        void* value = emitter.CreateLoad(localVariableSymbol->IrObject(emitter));
        uint8_t n = GetDerefCount(flags);
        for (uint8_t i = 0; i < n; ++i)
        {
            value = emitter.CreateLoad(value);
        }
        emitter.Stack().Push(value);
    }
    else
    {
        emitter.Stack().Push(emitter.CreateLoad(localVariableSymbol->IrObject(emitter)));
    }
    DestroyTemporaries(emitter);
}

void BoundLocalVariable::Store(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    void* value = emitter.Stack().Pop();
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot store to address of a local variable", GetSpan(), ModuleId());
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        void* ptr = emitter.CreateLoad(localVariableSymbol->IrObject(emitter));
        uint8_t n = GetDerefCount(flags);
        for (uint8_t i = 1; i < n; ++i)
        {
            ptr = emitter.CreateLoad(ptr);
        }
        emitter.CreateStore(value, ptr);
    }
    else
    {
        emitter.CreateStore(value, localVariableSymbol->IrObject(emitter));
    }
    DestroyTemporaries(emitter);
}

void BoundLocalVariable::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundMemberVariable::BoundMemberVariable(const Span& span_, const boost::uuids::uuid& moduleId_, MemberVariableSymbol* memberVariableSymbol_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundMemberVariable, memberVariableSymbol_->GetType()), 
    memberVariableSymbol(memberVariableSymbol_), staticInitNeeded(false)
{
}

BoundExpression* BoundMemberVariable::Clone()
{
    BoundMemberVariable* clone = new BoundMemberVariable(GetSpan(), ModuleId(), memberVariableSymbol);
    if (classPtr)
    {
        clone->classPtr.reset(classPtr->Clone());
    }
    if (staticInitNeeded)
    {
        clone->staticInitNeeded = true;
    }
    return clone;
}

void BoundMemberVariable::Load(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    Assert(memberVariableSymbol->LayoutIndex() != -1, "layout index of the member variable not set");
    if (memberVariableSymbol->IsStatic())
    {
        ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(memberVariableSymbol->Parent());
        if (staticInitNeeded)
        {
            if (classType->StaticConstructor())
            {
                BoundFunctionCall staticConstructorCall(classType->StaticConstructor()->GetSpan(), classType->StaticConstructor()->SourceModuleId(), classType->StaticConstructor());
                staticConstructorCall.Load(emitter, OperationFlags::none);
            }
        }
        emitter.Stack().Push(classType->StaticObject(emitter, false));
    }
    else
    {
        if (!classPtr)
        {
            throw Exception("class pointer of the member variable not set", GetSpan(), ModuleId());
        }
        classPtr->Load(emitter, OperationFlags::none);
    }
    void* ptr = emitter.Stack().Pop();
    void* memberVariablePtr = emitter.GetMemberVariablePtr(ptr, memberVariableSymbol->LayoutIndex());
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        emitter.Stack().Push(memberVariablePtr);
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        void* value = emitter.CreateLoad(memberVariablePtr);
        uint8_t n = GetDerefCount(flags);
        for (uint8_t i = 0; i < n; ++i)
        {
            value = emitter.CreateLoad(value);
        }
        emitter.Stack().Push(value);
    }
    else
    {
        emitter.Stack().Push(emitter.CreateLoad(memberVariablePtr));
    }
    DestroyTemporaries(emitter);
}

void BoundMemberVariable::Store(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    Assert(memberVariableSymbol->LayoutIndex() != -1, "layout index of the member variable not set");
    void* value = emitter.Stack().Pop();
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot store to the address of a member variable", GetSpan(), ModuleId());
    }
    else 
    {
        if (memberVariableSymbol->IsStatic())
        {
            ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(memberVariableSymbol->Parent());
            if (staticInitNeeded)
            {
                if (classType->StaticConstructor())
                {
                    BoundFunctionCall staticConstructorCall(classType->StaticConstructor()->GetSpan(), classType->StaticConstructor()->SourceModuleId(), classType->StaticConstructor());
                    staticConstructorCall.Load(emitter, OperationFlags::none);
                }
            }
            emitter.Stack().Push(classType->StaticObject(emitter, false));
        }
        else
        {
            classPtr->Load(emitter, OperationFlags::none);
        }
        void* ptr = emitter.Stack().Pop();
        void* memberVariablePtr = emitter.GetMemberVariablePtr(ptr, memberVariableSymbol->LayoutIndex());
        if ((flags & OperationFlags::deref) != OperationFlags::none)
        {
            void* ptr = emitter.CreateLoad(memberVariablePtr);
            uint8_t n = GetDerefCount(flags);
            for (uint8_t i = 1; i < n; ++i)
            {
                ptr = emitter.CreateLoad(ptr);
            }
            emitter.CreateStore(value, ptr);
        }
        else
        {
            emitter.CreateStore(value, memberVariablePtr);
        }
    }
    DestroyTemporaries(emitter);
}

void BoundMemberVariable::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

void BoundMemberVariable::SetClassPtr(std::unique_ptr<BoundExpression>&& classPtr_)
{
    classPtr = std::move(classPtr_);
}

BoundConstant::BoundConstant(const Span& span_, const boost::uuids::uuid& moduleId_, ConstantSymbol* constantSymbol_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundConstant, constantSymbol_->GetType()), constantSymbol(constantSymbol_)
{
}

BoundExpression* BoundConstant::Clone()
{
    return new BoundConstant(GetSpan(), ModuleId(), constantSymbol);
}

void BoundConstant::Load(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    if (constantSymbol->GetValue()->GetValueType() == ValueType::arrayValue && (flags & OperationFlags::addr) != OperationFlags::none)
    {
        emitter.Stack().Push(constantSymbol->ArrayIrObject(emitter, false));
    }
    else
    {
        if ((flags & OperationFlags::addr) != OperationFlags::none)
        {
            throw Exception("cannot take address of a constant", GetSpan(), ModuleId());
        }
        else if ((flags & OperationFlags::deref) != OperationFlags::none)
        {
            throw Exception("cannot dereference a constant", GetSpan(), ModuleId());
        }
        else
        {
            emitter.Stack().Push(constantSymbol->GetValue()->IrValue(emitter));
        }
    }
    DestroyTemporaries(emitter);
}

void BoundConstant::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to a constant", GetSpan(), ModuleId());
}

void BoundConstant::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundEnumConstant::BoundEnumConstant(const Span& span_, const boost::uuids::uuid& moduleId_, EnumConstantSymbol* enumConstantSymbol_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundEnumConstant, enumConstantSymbol_->GetType()), enumConstantSymbol(enumConstantSymbol_)
{
}

BoundExpression* BoundEnumConstant::Clone()
{
    return new BoundEnumConstant(GetSpan(), ModuleId(), enumConstantSymbol);
}

void BoundEnumConstant::Load(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot take address of an enumeration constant", GetSpan(), ModuleId());
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        throw Exception("cannot dereference an enumeration constant", GetSpan(), ModuleId());
    }
    else
    {
        emitter.Stack().Push(enumConstantSymbol->GetValue()->IrValue(emitter));
    }
    DestroyTemporaries(emitter);
}

void BoundEnumConstant::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to an enumeration constant", GetSpan(), ModuleId());
}

void BoundEnumConstant::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundLiteral::BoundLiteral(std::unique_ptr<Value>&& value_, TypeSymbol* type_) : 
    BoundExpression(value_->GetSpan(), value_->ModuleId(), BoundNodeType::boundLiteral, type_), value(std::move(value_))
{
}

BoundExpression* BoundLiteral::Clone()
{
    std::unique_ptr<Value> clonedValue;
    clonedValue.reset(value->Clone());
    return new BoundLiteral(std::move(clonedValue), GetType());
}

void BoundLiteral::Load(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot take address of a literal", GetSpan(), ModuleId());
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        throw Exception("cannot dereference a literal", GetSpan(), ModuleId());
    }
    else
    {
        emitter.Stack().Push(value->IrValue(emitter));
    }
    DestroyTemporaries(emitter);
}

void BoundLiteral::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to a literal", GetSpan(), ModuleId());
}

void BoundLiteral::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::unique_ptr<Value> BoundLiteral::ToValue(BoundCompileUnit& boundCompileUnit) const
{ 
    return std::unique_ptr<Value>(value->Clone());
}

BoundGlobalVariable::BoundGlobalVariable(const Span& span_, const boost::uuids::uuid& moduleId_, GlobalVariableSymbol* globalVariableSymbol_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundGlobalVariable, globalVariableSymbol_->GetType()), globalVariableSymbol(globalVariableSymbol_)
{
}

BoundExpression* BoundGlobalVariable::Clone()
{
    return new BoundGlobalVariable(GetSpan(), ModuleId(), globalVariableSymbol);
}

void BoundGlobalVariable::Load(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    void* globalVariablePtr = globalVariableSymbol->IrObject(emitter);
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        emitter.Stack().Push(globalVariablePtr);
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        void* value = emitter.CreateLoad(globalVariablePtr);
        uint8_t n = GetDerefCount(flags);
        for (uint8_t i = 0; i < n; ++i)
        {
            value = emitter.CreateLoad(value);
        }
        emitter.Stack().Push(value);
    }
    else
    {
        emitter.Stack().Push(emitter.CreateLoad(globalVariablePtr));
    }
    DestroyTemporaries(emitter);
}

void BoundGlobalVariable::Store(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot store to the address of a global variable", GetSpan(), ModuleId());
    }
    else
    {
        void* value = emitter.Stack().Pop();
        void* ptr = globalVariableSymbol->IrObject(emitter);
        if ((flags & OperationFlags::deref) != OperationFlags::none)
        {
            void* loadedPtr = emitter.CreateLoad(ptr);
            uint8_t n = GetDerefCount(flags);
            for (uint8_t i = 1; i < n; ++i)
            {
                loadedPtr = emitter.CreateLoad(loadedPtr);
            }
            emitter.CreateStore(value, loadedPtr);
        }
        else
        {
            emitter.CreateStore(value, ptr);
        }
    }
    DestroyTemporaries(emitter);
}

void BoundGlobalVariable::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundTemporary::BoundTemporary(std::unique_ptr<BoundExpression>&& rvalueExpr_, std::unique_ptr<BoundLocalVariable>&& backingStore_) :
    BoundExpression(rvalueExpr_->GetSpan(), rvalueExpr_->ModuleId(), BoundNodeType::boundTemporary, rvalueExpr_->GetType()), rvalueExpr(std::move(rvalueExpr_)), backingStore(std::move(backingStore_))
{
    rvalueExpr->MoveTemporaryDestructorCallsTo(*this);
    if (backingStore)
    {
        backingStore->MoveTemporaryDestructorCallsTo(*this);
    }
}

BoundExpression* BoundTemporary::Clone()
{
    std::unique_ptr<BoundExpression> clonedRvalueExpr;
    clonedRvalueExpr.reset(rvalueExpr->Clone());
    std::unique_ptr<BoundLocalVariable> clonedBackingStore;
    if (backingStore)
    {
        clonedBackingStore.reset(static_cast<BoundLocalVariable*>(backingStore->Clone()));
    }
    return new BoundTemporary(std::move(clonedRvalueExpr), std::move(clonedBackingStore));
}

void BoundTemporary::Load(Emitter& emitter, OperationFlags flags)
{
    rvalueExpr->Load(emitter, OperationFlags::none);
    if (!backingStore)
    {
        throw Exception("backing store of temporary not set", GetSpan(), ModuleId());
    }
    backingStore->Store(emitter, OperationFlags::none);
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        backingStore->Load(emitter, OperationFlags::addr);
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        backingStore->Load(emitter, SetDerefCount(OperationFlags::deref, GetDerefCount(flags) + 1));
    }
    else
    {
        backingStore->Load(emitter, OperationFlags::none);
    }
    DestroyTemporaries(emitter);
}

void BoundTemporary::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to a temporary", GetSpan(), ModuleId());
}

void BoundTemporary::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::unique_ptr<Value> BoundTemporary::ToValue(BoundCompileUnit& boundCompileUnit) const
{
    return rvalueExpr->ToValue(boundCompileUnit);
}

bool BoundTemporary::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    if (rvalueExpr->ContainsExceptionCapture())
    {
        return true;
    }
    return false;
}

BoundSizeOfExpression::BoundSizeOfExpression(const Span& span_, const boost::uuids::uuid& moduleId_, TypeSymbol* type_, TypeSymbol* pointerType_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundSizeOfExpression, type_), pointerType(pointerType_)
{
}

BoundExpression* BoundSizeOfExpression::Clone()
{
    return new BoundSizeOfExpression(GetSpan(), ModuleId(), GetType(), pointerType);
}

void BoundSizeOfExpression::Load(Emitter& emitter, OperationFlags flags)
{
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot take address of a sizeof expression", GetSpan(), ModuleId());
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        throw Exception("cannot dereference a sizeof expression", GetSpan(), ModuleId());
    }
    else
    {
        emitter.Stack().Push(emitter.SizeOf(pointerType->IrType(emitter)));
    }
    DestroyTemporaries(emitter);
}

void BoundSizeOfExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to a sizeof expression", GetSpan(), ModuleId());
}

void BoundSizeOfExpression::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundAddressOfExpression::BoundAddressOfExpression(std::unique_ptr<BoundExpression>&& subject_, TypeSymbol* type_)  :
    BoundExpression(subject_->GetSpan(), subject_->ModuleId(), BoundNodeType::boundAddressOfExpression, type_), subject(std::move(subject_))
{
    subject->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundAddressOfExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedSubject;
    clonedSubject.reset(subject->Clone());
    return new BoundAddressOfExpression(std::move(clonedSubject), GetType());
}

void BoundAddressOfExpression::Load(Emitter& emitter, OperationFlags flags)
{
    if (subject->GetBoundNodeType() != BoundNodeType::boundDereferenceExpression)
    {
        subject->Load(emitter, OperationFlags::addr);
    }
    else
    {
        BoundDereferenceExpression* derefExpr = static_cast<BoundDereferenceExpression*>(subject.get());
        derefExpr->Subject()->Load(emitter, flags);
    }
    DestroyTemporaries(emitter);
}

void BoundAddressOfExpression::Store(Emitter& emitter, OperationFlags flags)
{
    if (subject->GetBoundNodeType() != BoundNodeType::boundDereferenceExpression)
    {
        subject->Store(emitter, flags);
    }
    else
    {
        BoundDereferenceExpression* derefExpr = static_cast<BoundDereferenceExpression*>(subject.get());
        derefExpr->Subject()->Store(emitter, flags);
    }
    DestroyTemporaries(emitter);
}

void BoundAddressOfExpression::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::unique_ptr<Value> BoundAddressOfExpression::ToValue(BoundCompileUnit& boundCompileUnit) const
{
    return subject->ToValue(boundCompileUnit);
}

bool BoundAddressOfExpression::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    if (subject->ContainsExceptionCapture())
    {
        return true;
    }
    return false;
}

BoundDereferenceExpression::BoundDereferenceExpression(std::unique_ptr<BoundExpression>&& subject_, TypeSymbol* type_) :
    BoundExpression(subject_->GetSpan(), subject_->ModuleId(), BoundNodeType::boundDereferenceExpression, type_), subject(std::move(subject_))
{
    subject->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundDereferenceExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedSubject;
    clonedSubject.reset(subject->Clone());
    return new BoundDereferenceExpression(std::move(clonedSubject), GetType());
}

void BoundDereferenceExpression::Load(Emitter& emitter, OperationFlags flags)
{
    if (subject->GetBoundNodeType() != BoundNodeType::boundAddressOfExpression)
    {
        if (GetDerefCount(flags) == 0 && (flags & OperationFlags::addr) != OperationFlags::none)
        {
            subject->Load(emitter, OperationFlags::none);
        }
        else
        {
            subject->Load(emitter, SetDerefCount(OperationFlags::deref, GetDerefCount(flags) + 1));
        }
    }
    else
    {
        BoundAddressOfExpression* addressOfExpr = static_cast<BoundAddressOfExpression*>(subject.get());
        addressOfExpr->Subject()->Load(emitter, flags);
    }
    DestroyTemporaries(emitter);
}

void BoundDereferenceExpression::Store(Emitter& emitter, OperationFlags flags)
{
    if (subject->GetBoundNodeType() != BoundNodeType::boundAddressOfExpression)
    {
        subject->Store(emitter, SetDerefCount(OperationFlags::deref | (flags & OperationFlags::functionCallFlags), GetDerefCount(flags) + 1));
    }
    else
    {
        BoundAddressOfExpression* addressOfExpr = static_cast<BoundAddressOfExpression*>(subject.get());
        addressOfExpr->Subject()->Store(emitter, flags | (flags & OperationFlags::functionCallFlags));
    }
    DestroyTemporaries(emitter);
}

void BoundDereferenceExpression::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundDereferenceExpression::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    if (subject->ContainsExceptionCapture())
    {
        return true;
    }
    return false;
}

BoundReferenceToPointerExpression::BoundReferenceToPointerExpression(std::unique_ptr<BoundExpression>&& subject_, TypeSymbol* type_) :
    BoundExpression(subject_->GetSpan(), subject_->ModuleId(), BoundNodeType::boundReferenceToPointerExpression, type_), subject(std::move(subject_))
{
    subject->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundReferenceToPointerExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedSubject;
    clonedSubject.reset(subject->Clone());
    return new BoundReferenceToPointerExpression(std::move(clonedSubject), GetType());
}

void BoundReferenceToPointerExpression::Load(Emitter& emitter, OperationFlags flags)
{
    subject->Load(emitter, flags);
    DestroyTemporaries(emitter);
}

void BoundReferenceToPointerExpression::Store(Emitter& emitter, OperationFlags flags)
{
    subject->Store(emitter, flags);
    DestroyTemporaries(emitter);
}

void BoundReferenceToPointerExpression::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundReferenceToPointerExpression::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    if (subject->ContainsExceptionCapture())
    {
        return true;
    }
    return false;
}

BoundFunctionCall::BoundFunctionCall(const Span& span_, const boost::uuids::uuid& moduleId_, FunctionSymbol* functionSymbol_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundFunctionCall, functionSymbol_->ReturnType()), functionSymbol(functionSymbol_)
{
}

BoundExpression* BoundFunctionCall::Clone()
{
    BoundFunctionCall* clone = new BoundFunctionCall(GetSpan(), ModuleId(), functionSymbol);
    for (std::unique_ptr<BoundExpression>& argument : arguments)
    {
        clone->AddArgument(std::unique_ptr<BoundExpression>(argument->Clone()));
    }
    for (const auto& p : temporaries)
    {
        clone->AddTemporary(std::unique_ptr<BoundLocalVariable>(static_cast<BoundLocalVariable*>(p->Clone())));
    }
    return clone;
}

void BoundFunctionCall::AddArgument(std::unique_ptr<BoundExpression>&& argument)
{
    argument->MoveTemporaryDestructorCallsTo(*this);
    arguments.push_back(std::move(argument));
}

void BoundFunctionCall::SetArguments(std::vector<std::unique_ptr<BoundExpression>>&& arguments_)
{
    arguments = std::move(arguments_);
}

void BoundFunctionCall::AddTemporary(std::unique_ptr<BoundLocalVariable>&& temporary)
{
    temporaries.push_back(std::move(temporary));
}

bool BoundFunctionCall::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    for (const std::unique_ptr<BoundExpression>& arg : arguments)
    {
        if (arg->ContainsExceptionCapture())
        {
            return true;
        }
    }
    return false;
}

std::vector<std::unique_ptr<GenObject>> BoundFunctionCall::ReleaseTemporaries()
{
    std::vector<std::unique_ptr<GenObject>> temps;
    for (std::unique_ptr<BoundLocalVariable>& temp : temporaries)
    {
        temps.push_back(std::move(temp));
    }
    return temps;
}

void BoundFunctionCall::Load(Emitter& emitter, OperationFlags flags)
{
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        if (functionSymbol->IsArrayElementAccess())
        {
            std::vector<GenObject*> genObjects;
            for (const std::unique_ptr<BoundExpression>& argument : arguments)
            {
                genObjects.push_back(argument.get());
                genObjects.back()->SetType(argument->GetType());
            }
            functionSymbol->GenerateCall(emitter, genObjects, flags, GetSpan(), ModuleId());
        }
        else
        {
            throw Exception("cannot take address of a function call", GetSpan(), ModuleId());
        }
    }
    else
    {
        std::vector<GenObject*> genObjects;
        for (const std::unique_ptr<BoundExpression>& argument : arguments)
        {
            genObjects.push_back(argument.get());
            genObjects.back()->SetType(argument->GetType());
        }
        for (const std::unique_ptr<BoundLocalVariable>& temporary : temporaries)
        {
            genObjects.push_back(temporary.get());
            genObjects.back()->SetType(temporary->GetType());
        }
        OperationFlags callFlags = flags & OperationFlags::functionCallFlags;
        if (GetFlag(BoundExpressionFlags::virtualCall))
        {
            Assert(!arguments.empty(), "nonempty argument list expected");
            genObjects[0]->SetType(arguments[0]->GetType());
            callFlags = callFlags | OperationFlags::virtualCall;
        }
        if (!functionSymbol->DontThrow())
        {
            emitter.SetLineNumber(GetSpan().line);
        }
        if (functionSymbol->Parent()->GetSymbolType() == SymbolType::interfaceTypeSymbol && functionSymbol->GetSymbolType() == SymbolType::memberFunctionSymbol)
        {
            InterfaceTypeSymbol* interfaceType = static_cast<InterfaceTypeSymbol*>(functionSymbol->Parent());
            MemberFunctionSymbol* interfaceMemberFunction = static_cast<MemberFunctionSymbol*>(functionSymbol);
            interfaceType->GenerateCall(emitter, genObjects, callFlags, interfaceMemberFunction, GetSpan(), ModuleId());
        }
        else
        {
            functionSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan(), ModuleId());
        }
        if ((flags & OperationFlags::deref) != OperationFlags::none)
        {
            void* value = emitter.Stack().Pop();
            uint8_t n = GetDerefCount(flags);
            for (uint8_t i = 0; i < n; ++i)
            {
                value = emitter.CreateLoad(value);
            }
            emitter.Stack().Push(value);
        }
    }
    DestroyTemporaries(emitter);
}

void BoundFunctionCall::Store(Emitter& emitter, OperationFlags flags)
{
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot take address of a function call", GetSpan(), ModuleId());
    }
    else
    {
        void* value = emitter.Stack().Pop();
        std::vector<GenObject*> genObjects;
        for (const std::unique_ptr<BoundExpression>& argument : arguments)
        {
            genObjects.push_back(argument.get());
            genObjects.back()->SetType(argument->GetType());
        }
        OperationFlags callFlags = OperationFlags::none;
        if (GetFlag(BoundExpressionFlags::virtualCall))
        {
            callFlags = callFlags | OperationFlags::virtualCall;
        }
        if (!functionSymbol->DontThrow())
        {
            emitter.SetLineNumber(GetSpan().line);
        }
        if (functionSymbol->IsArrayElementAccess())
        {
            functionSymbol->GenerateCall(emitter, genObjects, callFlags | OperationFlags::addr, GetSpan(), ModuleId());
            void* ptr = emitter.Stack().Pop();
            emitter.CreateStore(value, ptr);
        }
        else
        {
            functionSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan(), ModuleId());
            void* ptr = emitter.Stack().Pop();
            if ((flags & OperationFlags::leaveFirstArg) != OperationFlags::none)
            {
                emitter.SaveObjectPointer(ptr);
            }
            if ((flags & OperationFlags::deref) != OperationFlags::none || GetFlag(BoundExpressionFlags::deref))
            {
                uint8_t n = GetDerefCount(flags);
                for (uint8_t i = 1; i < n; ++i)
                {
                    ptr = emitter.CreateLoad(ptr);
                }
                emitter.CreateStore(value, ptr);
            }
            else
            {
                emitter.CreateStore(emitter.CreateLoad(value), ptr);
            }
        }
    }
    DestroyTemporaries(emitter);
}

void BoundFunctionCall::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundFunctionCall::HasValue() const
{ 
    return functionSymbol->ReturnType() && functionSymbol->ReturnType()->GetSymbolType() != SymbolType::voidTypeSymbol; 
}

bool BoundFunctionCall::IsLvalueExpression() const
{
    if (functionSymbol->IsArrayElementAccess()) return true;
    TypeSymbol* returnType = functionSymbol->ReturnType();
    if (returnType && returnType->GetSymbolType() != SymbolType::voidTypeSymbol)
    {
        return !returnType->IsConstType() && returnType->IsLvalueReferenceType();
    }
    return false;
}

BoundDelegateCall::BoundDelegateCall(const Span& span_, const boost::uuids::uuid& moduleId_, DelegateTypeSymbol* delegateType_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundDelegateCall, delegateType_->ReturnType()), delegateTypeSymbol(delegateType_), arguments()
{
}

BoundExpression* BoundDelegateCall::Clone()
{
    return new BoundDelegateCall(GetSpan(), ModuleId(), delegateTypeSymbol);
}

void BoundDelegateCall::Load(Emitter& emitter, OperationFlags flags)
{
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot take address of a delegate call", GetSpan(), ModuleId());
    }
    else
    {
        std::vector<GenObject*> genObjects;
        for (const std::unique_ptr<BoundExpression>& argument : arguments)
        {
            genObjects.push_back(argument.get());
            genObjects.back()->SetType(argument->GetType());
        }
        OperationFlags callFlags = flags & OperationFlags::functionCallFlags;
        if (!delegateTypeSymbol->IsNothrow())
        {
            emitter.SetLineNumber(GetSpan().line);
        }
        delegateTypeSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan(), ModuleId());
        if ((flags & OperationFlags::deref) != OperationFlags::none)
        {
            void* value = emitter.Stack().Pop();
            uint8_t n = GetDerefCount(flags);
            for (uint8_t i = 0; i < n; ++i)
            {
                value = emitter.CreateLoad(value);
            }
            emitter.Stack().Push(value);
        }
    }
    DestroyTemporaries(emitter);
}

void BoundDelegateCall::Store(Emitter& emitter, OperationFlags flags)
{
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot take address of a delegate call", GetSpan(), ModuleId());
    }
    else
    {
        void* value = emitter.Stack().Pop();
        std::vector<GenObject*> genObjects;
        for (const std::unique_ptr<BoundExpression>& argument : arguments)
        {
            genObjects.push_back(argument.get());
            genObjects.back()->SetType(argument->GetType());
        }
        OperationFlags callFlags = OperationFlags::none;
        if (GetFlag(BoundExpressionFlags::virtualCall))
        {
            callFlags = callFlags | OperationFlags::virtualCall;
        }
        if (!delegateTypeSymbol->IsNothrow())
        {
            emitter.SetLineNumber(GetSpan().line);
        }
        delegateTypeSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan(), ModuleId());
        void* ptr = emitter.Stack().Pop();
        if ((flags & OperationFlags::leaveFirstArg) != OperationFlags::none)
        {
            emitter.SaveObjectPointer(ptr);
        }
        if ((flags & OperationFlags::deref) != OperationFlags::none || GetFlag(BoundExpressionFlags::deref))
        {
            uint8_t n = GetDerefCount(flags);
            for (uint8_t i = 1; i < n; ++i)
            {
                ptr = emitter.CreateLoad(ptr);
            }
            emitter.CreateStore(value, ptr);
        }
        else
        {
            emitter.CreateStore(emitter.CreateLoad(value), ptr);
        }
    }
    DestroyTemporaries(emitter);
}

void BoundDelegateCall::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundDelegateCall::HasValue() const
{
    return delegateTypeSymbol->ReturnType()->GetSymbolType() != SymbolType::voidTypeSymbol;
}

bool BoundDelegateCall::IsLvalueExpression() const
{
    TypeSymbol* returnType = delegateTypeSymbol->ReturnType();
    if (returnType->GetSymbolType() != SymbolType::voidTypeSymbol)
    {
        return !returnType->IsConstType() && returnType->IsLvalueReferenceType();
    }
    return false;
}

void BoundDelegateCall::AddArgument(std::unique_ptr<BoundExpression>&& argument)
{
    arguments.push_back(std::move(argument));
}

bool BoundDelegateCall::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    for (const std::unique_ptr<BoundExpression>& arg : arguments)
    {
        if (arg->ContainsExceptionCapture())
        {
            return true;
        }
    }
    return false;
}

BoundClassDelegateCall::BoundClassDelegateCall(const Span& span_, const boost::uuids::uuid& moduleId_, ClassDelegateTypeSymbol* classDelegateType_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundClassDelegateCall, classDelegateType_->ReturnType()), classDelegateTypeSymbol(classDelegateType_), arguments()
{
}

BoundExpression* BoundClassDelegateCall::Clone()
{
    return new BoundClassDelegateCall(GetSpan(), ModuleId(), classDelegateTypeSymbol);
}

void BoundClassDelegateCall::Load(Emitter& emitter, OperationFlags flags)
{
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot take address of a class delegate call", GetSpan(), ModuleId());
    }
    else
    {
        std::vector<GenObject*> genObjects;
        for (const std::unique_ptr<BoundExpression>& argument : arguments)
        {
            genObjects.push_back(argument.get());
            genObjects.back()->SetType(argument->GetType());
        }
        OperationFlags callFlags = flags & OperationFlags::functionCallFlags;
        if (!classDelegateTypeSymbol->IsNothrow())
        {
            emitter.SetLineNumber(GetSpan().line);
        }
        classDelegateTypeSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan(), ModuleId());
        if ((flags & OperationFlags::deref) != OperationFlags::none)
        {
            void* value = emitter.Stack().Pop();
            uint8_t n = GetDerefCount(flags);
            for (uint8_t i = 0; i < n; ++i)
            {
                value = emitter.CreateLoad(value);
            }
            emitter.Stack().Push(value);
        }
    }
    DestroyTemporaries(emitter);
}

void BoundClassDelegateCall::Store(Emitter& emitter, OperationFlags flags)
{
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot take address of a clas delegate call", GetSpan(), ModuleId());
    }
    else
    {
        void* value = emitter.Stack().Pop();
        std::vector<GenObject*> genObjects;
        for (const std::unique_ptr<BoundExpression>& argument : arguments)
        {
            genObjects.push_back(argument.get());
            genObjects.back()->SetType(argument->GetType());
        }
        OperationFlags callFlags = OperationFlags::none;
        if (GetFlag(BoundExpressionFlags::virtualCall))
        {
            callFlags = callFlags | OperationFlags::virtualCall;
        }
        if (!classDelegateTypeSymbol->IsNothrow())
        {
            emitter.SetLineNumber(GetSpan().line);
        }
        classDelegateTypeSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan(), ModuleId());
        void* ptr = emitter.Stack().Pop();
        if ((flags & OperationFlags::leaveFirstArg) != OperationFlags::none)
        {
            emitter.SaveObjectPointer(ptr);
        }
        if ((flags & OperationFlags::deref) != OperationFlags::none || GetFlag(BoundExpressionFlags::deref))
        {
            uint8_t n = GetDerefCount(flags);
            for (uint8_t i = 1; i < n; ++i)
            {
                ptr = emitter.CreateLoad(ptr);
            }
            emitter.CreateStore(value, ptr);
        }
        else
        {
            emitter.CreateStore(emitter.CreateLoad(value), ptr);
        }
    }
    DestroyTemporaries(emitter);
}

void BoundClassDelegateCall::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundClassDelegateCall::HasValue() const
{
    return classDelegateTypeSymbol->ReturnType()->GetSymbolType() != SymbolType::voidTypeSymbol;
}

bool BoundClassDelegateCall::IsLvalueExpression() const
{
    TypeSymbol* returnType = classDelegateTypeSymbol->ReturnType();
    if (returnType->GetSymbolType() != SymbolType::voidTypeSymbol)
    {
        return !returnType->IsConstType() && returnType->IsLvalueReferenceType();
    }
    return false;
}

void BoundClassDelegateCall::AddArgument(std::unique_ptr<BoundExpression>&& argument)
{
    arguments.push_back(std::move(argument));
}

bool BoundClassDelegateCall::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    for (const std::unique_ptr<BoundExpression>& arg : arguments)
    {
        if (arg->ContainsExceptionCapture())
        {
            return true;
        }
    }
    return false;
}

BoundConstructExpression::BoundConstructExpression(std::unique_ptr<BoundExpression>&& constructorCall_, TypeSymbol* resultType_) :
    BoundExpression(constructorCall_->GetSpan(), constructorCall_->ModuleId(), BoundNodeType::boundConstructExpression, resultType_), constructorCall(std::move(constructorCall_))
{
    constructorCall->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundConstructExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedConstructorCall;
    clonedConstructorCall.reset(constructorCall->Clone());
    return new BoundConstructExpression(std::move(clonedConstructorCall), GetType());
}

void BoundConstructExpression::Load(Emitter& emitter, OperationFlags flags)
{
    void* prevObjectPointer = emitter.GetObjectPointer();
    emitter.SetObjectPointer(nullptr);
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception("cannot take address of a construct expression", GetSpan(), ModuleId());
    }
    else
    {
        constructorCall->Load(emitter, OperationFlags::leaveFirstArg);
        void* objectPointer = emitter.GetObjectPointer();
        if (!objectPointer)
        {
            throw Exception("do not have object pointer", GetSpan(), ModuleId());
        }
        else
        {
            emitter.Stack().Push(objectPointer);
        }
    }
    DestroyTemporaries(emitter);
    emitter.SetObjectPointer(prevObjectPointer);
}

void BoundConstructExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to construct expression", GetSpan(), ModuleId());
}

void BoundConstructExpression::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundConstructExpression::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    return constructorCall->ContainsExceptionCapture();
}

BoundConstructAndReturnTemporaryExpression::BoundConstructAndReturnTemporaryExpression(std::unique_ptr<BoundExpression>&& constructorCall_, std::unique_ptr<BoundExpression>&& boundTemporary_) :
    BoundExpression(constructorCall_->GetSpan(), constructorCall_->ModuleId(), BoundNodeType::boundConstructAndReturnTemporary, boundTemporary_->GetType()), constructorCall(std::move(constructorCall_)),
    boundTemporary(std::move(boundTemporary_))
{
    constructorCall->MoveTemporaryDestructorCallsTo(*this);
    boundTemporary->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundConstructAndReturnTemporaryExpression::Clone()
{
    return new BoundConstructAndReturnTemporaryExpression(std::unique_ptr<BoundExpression>(constructorCall->Clone()), std::unique_ptr<BoundExpression>(boundTemporary->Clone()));
}

void BoundConstructAndReturnTemporaryExpression::Load(Emitter& emitter, OperationFlags flags)
{
    constructorCall->Load(emitter, OperationFlags::none);
    if (boundTemporary->GetType()->IsClassTypeSymbol() || boundTemporary->GetType()->GetSymbolType() == SymbolType::classDelegateTypeSymbol)
    {
        flags = flags | OperationFlags::addr;
    }
    boundTemporary->Load(emitter, flags);
    DestroyTemporaries(emitter);
}

void BoundConstructAndReturnTemporaryExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to construct and return temporary expression", GetSpan(), ModuleId());
}

void BoundConstructAndReturnTemporaryExpression::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundConstructAndReturnTemporaryExpression::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    if (constructorCall->ContainsExceptionCapture()) 
    {
        return true;
    }
    if (boundTemporary->ContainsExceptionCapture())
    {
        return true;
    }
    return false;
}

BoundClassOrClassDelegateConversionResult::BoundClassOrClassDelegateConversionResult(std::unique_ptr<BoundExpression>&& conversionResult_, std::unique_ptr<BoundFunctionCall>&& conversionFunctionCall_) :
    BoundExpression(conversionResult_->GetSpan(), conversionResult_->ModuleId(), BoundNodeType::boundClassOrClassDelegateConversionResult, conversionResult_->GetType()),
    conversionResult(std::move(conversionResult_)), conversionFunctionCall(std::move(conversionFunctionCall_))
{
}

BoundExpression* BoundClassOrClassDelegateConversionResult::Clone()
{
    return new BoundClassOrClassDelegateConversionResult(std::unique_ptr<BoundExpression>(conversionResult->Clone()),
        std::unique_ptr<BoundFunctionCall>(static_cast<BoundFunctionCall*>(conversionFunctionCall->Clone())));
}

void BoundClassOrClassDelegateConversionResult::Load(Emitter& emitter, OperationFlags flags)
{
    conversionFunctionCall->Load(emitter, OperationFlags::none);
    conversionResult->Load(emitter, flags);
}

void BoundClassOrClassDelegateConversionResult::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to class conversion result", GetSpan(), ModuleId());
}

void BoundClassOrClassDelegateConversionResult::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundClassOrClassDelegateConversionResult::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    if (conversionResult->ContainsExceptionCapture())
    {
        return true;
    }
    if (conversionFunctionCall->ContainsExceptionCapture())
    {
        return true;
    }
    return false;
}

BoundConversion::BoundConversion(std::unique_ptr<BoundExpression>&& sourceExpr_, FunctionSymbol* conversionFun_) :
    BoundExpression(sourceExpr_->GetSpan(), sourceExpr_->ModuleId(), BoundNodeType::boundConversion, conversionFun_->ConversionTargetType()), sourceExpr(std::move(sourceExpr_)), conversionFun(conversionFun_)
{
    sourceExpr->MoveTemporaryDestructorCallsTo(*this);
}

void BoundConversion::AddTemporary(std::unique_ptr<BoundLocalVariable>&& temporary)
{
    temporaries.push_back(std::move(temporary));
}

BoundExpression* BoundConversion::Clone()
{
    std::unique_ptr<BoundExpression> clonedSourceExpr;
    clonedSourceExpr.reset(sourceExpr->Clone());
    BoundConversion* clone = new BoundConversion(std::move(clonedSourceExpr), conversionFun);
    for (const auto& p : temporaries)
    {
        clone->AddTemporary(std::unique_ptr<BoundLocalVariable>(static_cast<BoundLocalVariable*>(p->Clone())));
    }
    return clone;
}

void BoundConversion::Load(Emitter& emitter, OperationFlags flags)
{
    sourceExpr->Load(emitter, flags);
    std::vector<GenObject*> genObjects;
    for (const std::unique_ptr<BoundLocalVariable>& temporary : temporaries)
    {
        genObjects.push_back(temporary.get());
        genObjects.back()->SetType(temporary->GetType());
    }
    conversionFun->GenerateCall(emitter, genObjects, OperationFlags::none, GetSpan(), ModuleId());
    DestroyTemporaries(emitter);
}

void BoundConversion::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to a conversion", GetSpan(), ModuleId());
}

bool BoundConversion::IsLvalueExpression() const
{
    if (conversionFun->GetSymbolType() == SymbolType::conversionFunctionSymbol) return true;
    if (conversionFun->IsClassToInterfaceTypeConversion()) return true;
    if (conversionFun->IsMemberFunctionToClassDelegateConversion()) return true;
    return false;
}

void BoundConversion::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::unique_ptr<Value> BoundConversion::ToValue(BoundCompileUnit& boundCompileUnit) const
{ 
    std::unique_ptr<Value> sourceValue = sourceExpr->ToValue(boundCompileUnit);
    if (sourceValue)
    {
        return conversionFun->ConvertValue(sourceValue);
    }
    return std::unique_ptr<Value>();
}

bool BoundConversion::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    return sourceExpr->ContainsExceptionCapture();
}

BoundIsExpression::BoundIsExpression(std::unique_ptr<BoundExpression>&& expr_, ClassTypeSymbol* rightClassType_, TypeSymbol* boolType_,
    std::unique_ptr<BoundLocalVariable>&& leftClassIdVar_, std::unique_ptr<BoundLocalVariable>&& rightClassIdVar_) :
    BoundExpression(expr_->GetSpan(), expr_->ModuleId(), BoundNodeType::boundIsExpression, boolType_), expr(std::move(expr_)), rightClassType(rightClassType_),
    leftClassIdVar(std::move(leftClassIdVar_)), rightClassIdVar(std::move(rightClassIdVar_))
{
}

BoundExpression* BoundIsExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedExpr;
    clonedExpr.reset(expr->Clone());
    return new BoundIsExpression(std::move(clonedExpr), rightClassType, GetType(),
        std::unique_ptr<BoundLocalVariable>(static_cast<BoundLocalVariable*>(leftClassIdVar->Clone())),
        std::unique_ptr<BoundLocalVariable>(static_cast<BoundLocalVariable*>(rightClassIdVar->Clone())));
}

void BoundIsExpression::Load(Emitter& emitter, OperationFlags flags)
{
    if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
    {
        expr->Load(emitter, OperationFlags::none);
        void* thisPtr = emitter.Stack().Pop();
        TypeSymbol* exprType = static_cast<TypeSymbol*>(expr->GetType());
        Assert(exprType->IsPointerType(), "pointer type expected");
        TypeSymbol* leftType = exprType->RemovePointer(GetSpan(), ModuleId());
        Assert(leftType->IsClassTypeSymbol(), "class type expected");
        ClassTypeSymbol* leftClassType = static_cast<ClassTypeSymbol*>(leftType);
        ClassTypeSymbol* leftVmtPtrHolderClass = leftClassType->VmtPtrHolderClass();
        if (leftClassType != leftVmtPtrHolderClass)
        {
            thisPtr = emitter.CreateBitCast(thisPtr, leftVmtPtrHolderClass->AddPointer(GetSpan(), ModuleId())->IrType(emitter));
        }
        void* vmtPtr = emitter.GetVmtPtr(thisPtr, leftVmtPtrHolderClass->VmtPtrIndex(), leftClassType->VmtPtrType(emitter));
        void* leftClassIdPtr = emitter.GetClassIdPtr(vmtPtr, GetClassIdVmtIndexOffset());
        void* rightClassTypeVmtObject = rightClassType->VmtObject(emitter, false);
        void* rightClassIdPtr = emitter.GetClassIdPtr(rightClassTypeVmtObject, GetClassIdVmtIndexOffset());
        void* retType = emitter.GetIrTypeForBool();
        std::vector<void*> paramTypes;
        paramTypes.push_back(emitter.GetIrTypeForVoidPtrType());
        paramTypes.push_back(emitter.GetIrTypeForVoidPtrType());
        void* dynamicInitAndCompareFnType = emitter.GetIrTypeForFunction(retType, paramTypes);
        void* dynamicInitAndCompareFn = emitter.GetOrInsertFunction("DynamicInitVmtsAndCompare", dynamicInitAndCompareFnType, true);
        std::vector<void*> args;
        args.push_back(emitter.CreateBitCast(leftClassIdPtr, emitter.GetIrTypeForVoidPtrType()));
        args.push_back(emitter.CreateBitCast(rightClassIdPtr, emitter.GetIrTypeForVoidPtrType()));
        emitter.Stack().Push(emitter.CreateCall(dynamicInitAndCompareFn, args));
        DestroyTemporaries(emitter);
    }
    else if (GetBackEnd() == BackEnd::cmsx)
    {
        expr->Load(emitter, OperationFlags::none);
        void* thisPtr = emitter.Stack().Pop();
        TypeSymbol* exprType = static_cast<TypeSymbol*>(expr->GetType());
        Assert(exprType->IsPointerType(), "pointer type expected");
        TypeSymbol* leftType = exprType->RemovePointer(GetSpan(), ModuleId());
        Assert(leftType->IsClassTypeSymbol(), "class type expected");
        ClassTypeSymbol* leftClassType = static_cast<ClassTypeSymbol*>(leftType);
        ClassTypeSymbol* leftVmtPtrHolderClass = leftClassType->VmtPtrHolderClass();
        if (leftClassType != leftVmtPtrHolderClass)
        {
            thisPtr = emitter.CreateBitCast(thisPtr, leftVmtPtrHolderClass->AddPointer(GetSpan(), ModuleId())->IrType(emitter));
        }
        void* vmtPtr = emitter.GetVmtPtr(thisPtr, leftVmtPtrHolderClass->VmtPtrIndex(), leftClassType->VmtPtrType(emitter));
        void* leftClassIdPtr = emitter.GetClassIdPtr(vmtPtr, GetClassIdVmtIndexOffset());
        void* leftClassId = emitter.CreatePtrToInt(emitter.CreateLoad(leftClassIdPtr), emitter.GetIrTypeForULong());
        void* rightClassTypeVmtObject = rightClassType->VmtObject(emitter, false);
        void* rightClassIdPtr = emitter.GetClassIdPtr(rightClassTypeVmtObject, GetClassIdVmtIndexOffset());
        void* rightClassId = emitter.CreatePtrToInt(emitter.CreateLoad(rightClassIdPtr), emitter.GetIrTypeForULong());
        void* remainder = emitter.CreateURem(leftClassId, rightClassId);
        void* remainderIsZero = emitter.CreateICmpEQ(remainder, emitter.CreateDefaultIrValueForULong());
        emitter.Stack().Push(remainderIsZero);
        DestroyTemporaries(emitter);
    }
}

void BoundIsExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to a 'is' expression", GetSpan(), ModuleId());
}

void BoundIsExpression::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundIsExpression::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    return expr->ContainsExceptionCapture();
}

BoundAsExpression::BoundAsExpression(std::unique_ptr<BoundExpression>&& expr_, ClassTypeSymbol* rightClassType_, std::unique_ptr<BoundLocalVariable>&& variable_,
    std::unique_ptr<BoundLocalVariable>&& leftClassIdVar_, std::unique_ptr<BoundLocalVariable>&& rightClassIdVar_) :
    BoundExpression(expr_->GetSpan(), expr_->ModuleId(), BoundNodeType::boundAsExpression, rightClassType_->AddPointer(expr_->GetSpan(), expr_->ModuleId())),
    expr(std::move(expr_)), rightClassType(rightClassType_), variable(std::move(variable_)), 
    leftClassIdVar(std::move(leftClassIdVar_)), rightClassIdVar(std::move(rightClassIdVar_))
{
}

BoundExpression* BoundAsExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedExpr;
    clonedExpr.reset(expr->Clone());
    std::unique_ptr<BoundLocalVariable> clonedVariable;
    clonedVariable.reset(static_cast<BoundLocalVariable*>(variable->Clone()));
    return new BoundAsExpression(std::move(clonedExpr), rightClassType, std::move(clonedVariable),
        std::unique_ptr<BoundLocalVariable>(static_cast<BoundLocalVariable*>(leftClassIdVar->Clone())), 
        std::unique_ptr<BoundLocalVariable>(static_cast<BoundLocalVariable*>(rightClassIdVar->Clone())));
}

void BoundAsExpression::Load(Emitter& emitter, OperationFlags flags)
{
    if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
    {
        expr->Load(emitter, OperationFlags::none);
        void* thisPtr = emitter.Stack().Pop();
        TypeSymbol* exprType = static_cast<TypeSymbol*>(expr->GetType());
        Assert(exprType->IsPointerType(), "pointer type expected");
        TypeSymbol* leftType = exprType->RemovePointer(GetSpan(), ModuleId());
        Assert(leftType->IsClassTypeSymbol(), "class type expected");
        ClassTypeSymbol* leftClassType = static_cast<ClassTypeSymbol*>(leftType);
        ClassTypeSymbol* leftVmtPtrHolderClass = leftClassType->VmtPtrHolderClass();
        if (leftClassType != leftVmtPtrHolderClass)
        {
            thisPtr = emitter.CreateBitCast(thisPtr, leftVmtPtrHolderClass->AddPointer(GetSpan(), ModuleId())->IrType(emitter));
        }
        void* vmtPtr = emitter.GetVmtPtr(thisPtr, leftVmtPtrHolderClass->VmtPtrIndex(), leftClassType->VmtPtrType(emitter));
        void* leftClassIdPtr = emitter.GetClassIdPtr(vmtPtr, GetClassIdVmtIndexOffset());
        void* rightClassTypeVmtObject = rightClassType->VmtObject(emitter, false);
        void* rightClassIdPtr = emitter.GetClassIdPtr(rightClassTypeVmtObject, GetClassIdVmtIndexOffset());

        void* retType = emitter.GetIrTypeForBool();
        std::vector<void*> paramTypes;
        paramTypes.push_back(emitter.GetIrTypeForVoidPtrType());
        paramTypes.push_back(emitter.GetIrTypeForVoidPtrType());
        void* dynamicInitAndCompareFnType = emitter.GetIrTypeForFunction(retType, paramTypes);
        void* dynamicInitAndCompareFn = emitter.GetOrInsertFunction("DynamicInitVmtsAndCompare", dynamicInitAndCompareFnType, true);
        std::vector<void*> args;
        args.push_back(emitter.CreateBitCast(leftClassIdPtr, emitter.GetIrTypeForVoidPtrType()));
        args.push_back(emitter.CreateBitCast(rightClassIdPtr, emitter.GetIrTypeForVoidPtrType()));
        emitter.Stack().Push(emitter.CreateCall(dynamicInitAndCompareFn, args));
        void* remainderIsZero = emitter.Stack().Pop();
        void* trueBlock = emitter.CreateBasicBlock("true");
        void* falseBlock = emitter.CreateBasicBlock("false");
        void* continueBlock = emitter.CreateBasicBlock("continue");
        emitter.CreateCondBr(remainderIsZero, trueBlock, falseBlock);
        emitter.SetCurrentBasicBlock(trueBlock);
        emitter.Stack().Push(emitter.CreateBitCast(thisPtr, rightClassType->AddPointer(GetSpan(), ModuleId())->IrType(emitter)));
        variable->Store(emitter, OperationFlags::none);
        emitter.CreateBr(continueBlock);
        emitter.SetCurrentBasicBlock(falseBlock);
        emitter.Stack().Push(emitter.CreateDefaultIrValueForPtrType(rightClassType->AddPointer(GetSpan(), ModuleId())->IrType(emitter)));
        variable->Store(emitter, OperationFlags::none);
        emitter.CreateBr(continueBlock);
        emitter.SetCurrentBasicBlock(continueBlock);
        variable->Load(emitter, OperationFlags::none);
        DestroyTemporaries(emitter);
    }
    else if (GetBackEnd() == BackEnd::cmsx)
    {
        expr->Load(emitter, OperationFlags::none);
        void* thisPtr = emitter.Stack().Pop();
        TypeSymbol* exprType = static_cast<TypeSymbol*>(expr->GetType());
        Assert(exprType->IsPointerType(), "pointer type expected");
        TypeSymbol* leftType = exprType->RemovePointer(GetSpan(), ModuleId());
        Assert(leftType->IsClassTypeSymbol(), "class type expected");
        ClassTypeSymbol* leftClassType = static_cast<ClassTypeSymbol*>(leftType);
        ClassTypeSymbol* leftVmtPtrHolderClass = leftClassType->VmtPtrHolderClass();
        if (leftClassType != leftVmtPtrHolderClass)
        {
            thisPtr = emitter.CreateBitCast(thisPtr, leftVmtPtrHolderClass->AddPointer(GetSpan(), ModuleId())->IrType(emitter));
        }
        void* vmtPtr = emitter.GetVmtPtr(thisPtr, leftVmtPtrHolderClass->VmtPtrIndex(), leftClassType->VmtPtrType(emitter));
        void* leftClassIdPtr = emitter.GetClassIdPtr(vmtPtr, GetClassIdVmtIndexOffset());
        void* leftClassId = emitter.CreatePtrToInt(emitter.CreateLoad(leftClassIdPtr), emitter.GetIrTypeForULong());
        void* rightClassTypeVmtObject = rightClassType->VmtObject(emitter, false);
        void* rightClassIdPtr = emitter.GetClassIdPtr(rightClassTypeVmtObject, GetClassIdVmtIndexOffset());
        void* rightClassId = emitter.CreatePtrToInt(emitter.CreateLoad(rightClassIdPtr), emitter.GetIrTypeForULong());
        void* remainder = emitter.CreateURem(leftClassId, rightClassId);
        void* remainderIsZero = emitter.CreateICmpEQ(remainder, emitter.CreateDefaultIrValueForULong());
        void* trueBlock = emitter.CreateBasicBlock("true");
        void* falseBlock = emitter.CreateBasicBlock("false");
        void* continueBlock = emitter.CreateBasicBlock("continue");
        emitter.CreateCondBr(remainderIsZero, trueBlock, falseBlock);
        emitter.SetCurrentBasicBlock(trueBlock);
        emitter.Stack().Push(emitter.CreateBitCast(thisPtr, rightClassType->AddPointer(GetSpan(), ModuleId())->IrType(emitter)));
        variable->Store(emitter, OperationFlags::none);
        emitter.CreateBr(continueBlock);
        emitter.SetCurrentBasicBlock(falseBlock);
        emitter.Stack().Push(emitter.CreateDefaultIrValueForPtrType(rightClassType->AddPointer(GetSpan(), ModuleId())->IrType(emitter)));
        variable->Store(emitter, OperationFlags::none);
        emitter.CreateBr(continueBlock);
        emitter.SetCurrentBasicBlock(continueBlock);
        variable->Load(emitter, OperationFlags::none);
        DestroyTemporaries(emitter);
    }
}

void BoundAsExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to an 'as' expression", GetSpan(), ModuleId());
}

void BoundAsExpression::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundAsExpression::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    return expr->ContainsExceptionCapture();
}

BoundTypeNameExpression::BoundTypeNameExpression(std::unique_ptr<BoundExpression>&& classPtr_, TypeSymbol* constCharPtrType_) :
    BoundExpression(classPtr_->GetSpan(), classPtr_->ModuleId(), BoundNodeType::boundTypeNameExpression, constCharPtrType_), classPtr(std::move(classPtr_))
{
    classPtr->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundTypeNameExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedClassPtr;
    clonedClassPtr.reset(classPtr->Clone());
    return new BoundTypeNameExpression(std::move(clonedClassPtr), GetType());
}

void BoundTypeNameExpression::Load(Emitter& emitter, OperationFlags flags)
{
    classPtr->Load(emitter, OperationFlags::none);
    void* thisPtr = emitter.Stack().Pop();
    TypeSymbol* classPtrType = static_cast<TypeSymbol*>(classPtr->GetType());
    Assert(classPtrType->IsPointerType(), "pointer type expected");
    TypeSymbol* type = classPtrType->BaseType();
    Assert(type->IsClassTypeSymbol(), "class type expected");
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type);
    ClassTypeSymbol* vmtPtrHolderClass = classType->VmtPtrHolderClass();
    if (classType != vmtPtrHolderClass)
    {
        thisPtr = emitter.CreateBitCast(thisPtr, vmtPtrHolderClass->AddPointer(GetSpan(), ModuleId())->IrType(emitter));
    }
    void* vmtPtr = emitter.GetVmtPtr(thisPtr, vmtPtrHolderClass->VmtPtrIndex(), classType->VmtPtrType(emitter));
    void* className = emitter.GetClassName(vmtPtr, GetClassNameVmtIndexOffset());
    emitter.Stack().Push(className);
    DestroyTemporaries(emitter);
}

void BoundTypeNameExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to typename expression", GetSpan(), ModuleId());
}

void BoundTypeNameExpression::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundTypeNameExpression::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    return classPtr->ContainsExceptionCapture();
}

BoundTypeIdExpression::BoundTypeIdExpression(std::unique_ptr<BoundExpression>&& classPtr_, TypeSymbol* ulongType_) :
    BoundExpression(classPtr_->GetSpan(), classPtr_->ModuleId(), BoundNodeType::boundTypeIdExpression, ulongType_), classPtr(std::move(classPtr_))
{
    classPtr->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundTypeIdExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedClassPtr;
    clonedClassPtr.reset(classPtr->Clone());
    return new BoundTypeIdExpression(std::move(clonedClassPtr), GetType());
}

void BoundTypeIdExpression::Load(Emitter& emitter, OperationFlags flags)
{
    classPtr->Load(emitter, OperationFlags::none);
    void* thisPtr = emitter.Stack().Pop();
    TypeSymbol* classPtrType = static_cast<TypeSymbol*>(classPtr->GetType());
    Assert(classPtrType->IsPointerType(), "pointer type expected");
    TypeSymbol* type = classPtrType->BaseType();
    Assert(type->IsClassTypeSymbol(), "class type expected");
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type);
    ClassTypeSymbol* vmtPtrHolderClass = classType->VmtPtrHolderClass();
    if (classType != vmtPtrHolderClass)
    {
        thisPtr = emitter.CreateBitCast(thisPtr, vmtPtrHolderClass->AddPointer(GetSpan(), ModuleId())->IrType(emitter));
    }
    void* vmtPtr = emitter.GetVmtPtr(thisPtr, vmtPtrHolderClass->VmtPtrIndex(), classType->VmtPtrType(emitter));
    void* classIdPtr = emitter.GetClassIdPtr(vmtPtr, GetClassIdVmtIndexOffset());
    void* classId = emitter.CreatePtrToInt(emitter.CreateLoad(classIdPtr), emitter.GetIrTypeForULong());
    emitter.Stack().Push(classId);
    DestroyTemporaries(emitter);
}

void BoundTypeIdExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to typeid expression", GetSpan(), ModuleId());
}

void BoundTypeIdExpression::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundTypeIdExpression::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    return classPtr->ContainsExceptionCapture();
}

BoundBitCast::BoundBitCast(std::unique_ptr<BoundExpression>&& expr_, TypeSymbol* type_) : 
    BoundExpression(expr_->GetSpan(), expr_->ModuleId(), BoundNodeType::boundBitCast, type_), expr(std::move(expr_))
{
    expr->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundBitCast::Clone()
{
    return new BoundBitCast(std::unique_ptr<BoundExpression>(expr->Clone()), GetType());
}

void BoundBitCast::Load(Emitter& emitter, OperationFlags flags)
{
    expr->Load(emitter, OperationFlags::none);
    void* value = emitter.Stack().Pop();
    void* casted = emitter.CreateBitCast(value, GetType()->IrType(emitter));
    emitter.Stack().Push(casted);
    DestroyTemporaries(emitter);
}

void BoundBitCast::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to bit cast", GetSpan(), ModuleId());
}

void BoundBitCast::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

bool BoundBitCast::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    return expr->ContainsExceptionCapture();
}

BoundFunctionPtr::BoundFunctionPtr(const Span& span_, const boost::uuids::uuid& moduleId_, FunctionSymbol* function_, TypeSymbol* type_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundFunctionPtr, type_), function(function_)
{
}

BoundExpression* BoundFunctionPtr::Clone()
{
    return new BoundFunctionPtr(GetSpan(), ModuleId(), function, GetType());
}

void BoundFunctionPtr::Load(Emitter& emitter, OperationFlags flags)
{
    void* irObject = emitter.GetOrInsertFunction(ToUtf8(function->MangledName()), function->IrType(emitter), function->DontThrow());
    emitter.Stack().Push(irObject);
    DestroyTemporaries(emitter);
}

void BoundFunctionPtr::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to function ptr expression", GetSpan(), ModuleId());
}

void BoundFunctionPtr::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundDisjunction::BoundDisjunction(const Span& span_, const boost::uuids::uuid& moduleId_, std::unique_ptr<BoundExpression>&& left_, std::unique_ptr<BoundExpression>&& right_, TypeSymbol* boolType_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundDisjunction, boolType_), left(std::move(left_)), right(std::move(right_))
{
}

BoundExpression* BoundDisjunction::Clone()
{
    return new BoundDisjunction(GetSpan(), ModuleId(), std::unique_ptr<BoundExpression>(left->Clone()), std::unique_ptr<BoundExpression>(right->Clone()), GetType());
}

void BoundDisjunction::Load(Emitter& emitter, OperationFlags flags)
{
    temporary->Load(emitter, OperationFlags::addr);
    void* temp = emitter.Stack().Pop();
    left->Load(emitter, OperationFlags::none);
    void* leftValue = emitter.Stack().Pop();
    void* trueBlock = emitter.CreateBasicBlock("true");
    void* rightBlock = emitter.CreateBasicBlock("right");
    void* falseBlock = emitter.CreateBasicBlock("false");
    void* nextBlock = emitter.CreateBasicBlock("next");
    emitter.CreateCondBr(leftValue, trueBlock, rightBlock);
    emitter.SetCurrentBasicBlock(rightBlock);
    right->Load(emitter, OperationFlags::none);
    void* rightValue = emitter.Stack().Pop();
    emitter.CreateCondBr(rightValue, trueBlock, falseBlock);
    emitter.SetCurrentBasicBlock(trueBlock);
    emitter.CreateStore(emitter.CreateIrValueForBool(true), temp);
    emitter.CreateBr(nextBlock);
    emitter.SetCurrentBasicBlock(falseBlock);
    emitter.CreateStore(emitter.CreateIrValueForBool(false), temp);
    emitter.CreateBr(nextBlock);
    emitter.SetCurrentBasicBlock(nextBlock);
    void* value = emitter.CreateLoad(temp);
    emitter.Stack().Push(value);
    DestroyTemporaries(emitter);
}

void BoundDisjunction::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to disjunction", GetSpan(), ModuleId());
}

void BoundDisjunction::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

void BoundDisjunction::SetTemporary(BoundLocalVariable* temporary_)
{
    temporary.reset(temporary_);
}

bool BoundDisjunction::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    if (left->ContainsExceptionCapture())
    {
        return true;
    }
    if (right->ContainsExceptionCapture())
    {
        return true;
    }
    return false;
}

BoundConjunction::BoundConjunction(const Span& span_, const boost::uuids::uuid& moduleId_, std::unique_ptr<BoundExpression>&& left_, std::unique_ptr<BoundExpression>&& right_, TypeSymbol* boolType_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundConjunction, boolType_), left(std::move(left_)), right(std::move(right_))
{
}

BoundExpression* BoundConjunction::Clone()
{
    return new BoundConjunction(GetSpan(), ModuleId(), std::unique_ptr<BoundExpression>(left->Clone()), std::unique_ptr<BoundExpression>(right->Clone()), GetType());
}

void BoundConjunction::Load(Emitter& emitter, OperationFlags flags)
{
    temporary->Load(emitter, OperationFlags::addr);
    void* temp = emitter.Stack().Pop();
    left->Load(emitter, OperationFlags::none);
    void* leftValue = emitter.Stack().Pop();
    void* trueBlock = emitter.CreateBasicBlock("true");
    void* rightBlock = emitter.CreateBasicBlock("right");
    void* falseBlock = emitter.CreateBasicBlock("false");
    void* nextBlock = emitter.CreateBasicBlock("next");
    emitter.CreateCondBr(leftValue, rightBlock, falseBlock);
    emitter.SetCurrentBasicBlock(rightBlock);
    right->Load(emitter, OperationFlags::none);
    void* rightValue = emitter.Stack().Pop();
    emitter.CreateCondBr(rightValue, trueBlock, falseBlock);
    emitter.SetCurrentBasicBlock(trueBlock);
    emitter.CreateStore(emitter.CreateIrValueForBool(true), temp);
    emitter.CreateBr(nextBlock);
    emitter.SetCurrentBasicBlock(falseBlock);
    emitter.CreateStore(emitter.CreateIrValueForBool(false), temp);
    emitter.CreateBr(nextBlock);
    emitter.SetCurrentBasicBlock(nextBlock);
    void* value = emitter.CreateLoad(temp);
    emitter.Stack().Push(value);
    DestroyTemporaries(emitter);
}

void BoundConjunction::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to conjunction", GetSpan(), ModuleId());
}

void BoundConjunction::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

void BoundConjunction::SetTemporary(BoundLocalVariable* temporary_)
{
    temporary.reset(temporary_);
}

bool BoundConjunction::ContainsExceptionCapture() const
{
    if (BoundExpression::ContainsExceptionCapture())
    {
        return true;
    }
    if (left->ContainsExceptionCapture())
    {
        return true;
    }
    if (right->ContainsExceptionCapture())
    {
        return true;
    }
    return false;
}

BoundTypeExpression::BoundTypeExpression(const Span& span_, const boost::uuids::uuid& moduleId_, TypeSymbol* type_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundTypeExpression, type_)
{
}

BoundExpression* BoundTypeExpression::Clone()
{
    return new BoundTypeExpression(GetSpan(), ModuleId(), GetType());
}

void BoundTypeExpression::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot load from a type", GetSpan(), ModuleId());
}

void BoundTypeExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to a type", GetSpan(), ModuleId());
}

void BoundTypeExpression::Accept(BoundNodeVisitor& visitor)
{
    throw Exception("cannot visit a type", GetSpan(), ModuleId());
}

TypeSymbol* CreateNamespaceTypeSymbol(NamespaceSymbol* ns)
{
    TypeSymbol* nsTypeSymbol = new NamespaceTypeSymbol(ns);
    ns->GetModule()->GetSymbolTable().SetTypeIdFor(nsTypeSymbol);
    return nsTypeSymbol;
}

BoundNamespaceExpression::BoundNamespaceExpression(const Span& span_, const boost::uuids::uuid& moduleId_, NamespaceSymbol* ns_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundNamespaceExpression, CreateNamespaceTypeSymbol(ns_)), ns(ns_)
{
    nsType.reset(GetType());
}

BoundExpression* BoundNamespaceExpression::Clone()
{
    return new BoundNamespaceExpression(GetSpan(), ModuleId(), ns);
}

void BoundNamespaceExpression::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot load from a namespace", GetSpan(), ModuleId());
}

void BoundNamespaceExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to a namespace", GetSpan(), ModuleId());
}

void BoundNamespaceExpression::Accept(BoundNodeVisitor& visitor)
{
    throw Exception("cannot visit a namespace", GetSpan(), ModuleId());
}

TypeSymbol* CreateFunctionGroupTypeSymbol(FunctionGroupSymbol* functionGroupSymbol, void* boundFunctionGroupExpression)
{
    TypeSymbol* functionGroupTypeSymbol = new FunctionGroupTypeSymbol(functionGroupSymbol, boundFunctionGroupExpression);
    functionGroupTypeSymbol->SetModule(functionGroupSymbol->GetModule());
    functionGroupSymbol->GetModule()->GetSymbolTable().SetTypeIdFor(functionGroupTypeSymbol);
    return functionGroupTypeSymbol;
}

BoundFunctionGroupExpression::BoundFunctionGroupExpression(const Span& span_, const boost::uuids::uuid& moduleId_, FunctionGroupSymbol* functionGroupSymbol_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundFunctionGroupExpression, CreateFunctionGroupTypeSymbol(functionGroupSymbol_, this)), 
    functionGroupSymbol(functionGroupSymbol_), scopeQualified(false), qualifiedScope(nullptr)
{
    functionGroupType.reset(GetType());
}

BoundExpression* BoundFunctionGroupExpression::Clone()
{
    BoundFunctionGroupExpression* clone = new BoundFunctionGroupExpression(GetSpan(), ModuleId(), functionGroupSymbol);
    if (classPtr)
    {
        clone->classPtr.reset(classPtr->Clone());
    }
    clone->scopeQualified = scopeQualified;
    clone->qualifiedScope = qualifiedScope;
    return clone;
}

void BoundFunctionGroupExpression::Load(Emitter& emitter, OperationFlags flags)
{
    if (classPtr)
    {
        classPtr->Load(emitter, OperationFlags::none);  
    }
    else
    {
        emitter.Stack().Push(nullptr);
    }
}

void BoundFunctionGroupExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to a function group", GetSpan(), ModuleId());
}

void BoundFunctionGroupExpression::Accept(BoundNodeVisitor& visitor)
{
    throw Exception("cannot visit a function group", GetSpan(), ModuleId());
}

void BoundFunctionGroupExpression::SetClassPtr(std::unique_ptr<BoundExpression>&& classPtr_)
{
    classPtr = std::move(classPtr_);
}

void BoundFunctionGroupExpression::SetTemplateArgumentTypes(const std::vector<TypeSymbol*>& templateArgumentTypes_)
{
    templateArgumentTypes = templateArgumentTypes_;
}

TypeSymbol* CreateMemberExpressionTypeSymbol(const Span& span, const boost::uuids::uuid& moduleId_, const std::u32string& name, void* boundMemberExpression)
{
    TypeSymbol* memberExpressionTypeSymbol = new MemberExpressionTypeSymbol(span, moduleId_, name, boundMemberExpression);
    Module* module = GetRootModuleForCurrentThread();
    memberExpressionTypeSymbol->SetModule(module);
    module->GetSymbolTable().SetTypeIdFor(memberExpressionTypeSymbol);
    return memberExpressionTypeSymbol;
}

BoundMemberExpression::BoundMemberExpression(const Span& span_, const boost::uuids::uuid& moduleId_, std::unique_ptr<BoundExpression>&& classPtr_, std::unique_ptr<BoundExpression>&& member_) :
    BoundExpression(span_, moduleId_, BoundNodeType::boundMemberExpression, CreateMemberExpressionTypeSymbol(span_, member_->ModuleId(), member_->GetType()->Name(), this)), classPtr(std::move(classPtr_)), member(std::move(member_))
{
    memberExpressionType.reset(GetType());
    classPtr->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundMemberExpression::Clone()
{
    return new BoundMemberExpression(GetSpan(), ModuleId(), std::unique_ptr<BoundExpression>(classPtr->Clone()), std::unique_ptr<BoundExpression>(member->Clone()));
}

void BoundMemberExpression::Load(Emitter& emitter, OperationFlags flags)
{
    if (classPtr)
    {
        classPtr->Load(emitter, OperationFlags::none);
    }
    else
    {
        emitter.Stack().Push(nullptr);
    }
}

void BoundMemberExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to a member expression", GetSpan(), ModuleId());
}

void BoundMemberExpression::Accept(BoundNodeVisitor& visitor)
{
    throw Exception("cannot visit a member expression", GetSpan(), ModuleId());
}

} } // namespace cmajor::binder
