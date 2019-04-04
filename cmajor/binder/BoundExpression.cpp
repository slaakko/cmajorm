// =================================
// Copyright (c) 2019 Seppo Laakko
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
#include <cmajor/ir/Emitter.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::unicode;
 
BoundExpression::BoundExpression(Module* module_, const Span& span_, BoundNodeType boundNodeType_, TypeSymbol* type_) : 
    BoundNode(module_, span_, boundNodeType_), type(type_), flags(BoundExpressionFlags::none)
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

BoundParameter::BoundParameter(Module* module_, const Span& span_, ParameterSymbol* parameterSymbol_) :
    BoundExpression(module_, parameterSymbol_->GetSpan(), BoundNodeType::boundParameter, parameterSymbol_->GetType()), parameterSymbol(parameterSymbol_)
{
}

BoundExpression* BoundParameter::Clone()
{
    return new BoundParameter(GetModule(), GetSpan(), parameterSymbol);
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
        throw Exception(GetModule(), "cannot take address of a parameter", GetSpan());
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

BoundLocalVariable::BoundLocalVariable(Module* module_, const Span& span_, LocalVariableSymbol* localVariableSymbol_) :
    BoundExpression(module_, span_, BoundNodeType::boundLocalVariable, localVariableSymbol_->GetType()), localVariableSymbol(localVariableSymbol_)
{
}

BoundExpression* BoundLocalVariable::Clone()
{
    return new BoundLocalVariable(GetModule(), GetSpan(), localVariableSymbol);
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
        throw Exception(GetModule(), "cannot store to address of a local variable", GetSpan());
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

BoundMemberVariable::BoundMemberVariable(Module* module_, const Span& span_, MemberVariableSymbol* memberVariableSymbol_) :
    BoundExpression(module_, span_, BoundNodeType::boundMemberVariable, memberVariableSymbol_->GetType()), memberVariableSymbol(memberVariableSymbol_), staticInitNeeded(false)
{
}

BoundExpression* BoundMemberVariable::Clone()
{
    BoundMemberVariable* clone = new BoundMemberVariable(GetModule(), GetSpan(), memberVariableSymbol);
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
                BoundFunctionCall staticConstructorCall(GetModule(), GetSpan(), classType->StaticConstructor());
                staticConstructorCall.Load(emitter, OperationFlags::none);
            }
        }
        emitter.Stack().Push(classType->StaticObject(emitter, false));
    }
    else
    {
        if (!classPtr)
        {
            throw Exception(GetModule(), "class pointer of the member variable not set", GetSpan());
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
        throw Exception(GetModule(), "cannot store to the address of a member variable", GetSpan());
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
                    BoundFunctionCall staticConstructorCall(GetModule(), GetSpan(), classType->StaticConstructor());
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

BoundConstant::BoundConstant(Module* module_, const Span& span_, ConstantSymbol* constantSymbol_) : 
    BoundExpression(module_, span_, BoundNodeType::boundConstant, constantSymbol_->GetType()), constantSymbol(constantSymbol_)
{
}

BoundExpression* BoundConstant::Clone()
{
    return new BoundConstant(GetModule(), GetSpan(), constantSymbol);
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
            throw Exception(GetModule(), "cannot take address of a constant", GetSpan());
        }
        else if ((flags & OperationFlags::deref) != OperationFlags::none)
        {
            throw Exception(GetModule(), "cannot dereference a constant", GetSpan());
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
    throw Exception(GetModule(), "cannot store to a constant", GetSpan());
}

void BoundConstant::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundEnumConstant::BoundEnumConstant(Module* module_, const Span& span_, EnumConstantSymbol* enumConstantSymbol_) : 
    BoundExpression(module_, span_, BoundNodeType::boundEnumConstant, enumConstantSymbol_->GetType()), enumConstantSymbol(enumConstantSymbol_)
{
}

BoundExpression* BoundEnumConstant::Clone()
{
    return new BoundEnumConstant(GetModule(), GetSpan(), enumConstantSymbol);
}

void BoundEnumConstant::Load(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception(GetModule(), "cannot take address of an enumeration constant", GetSpan());
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        throw Exception(GetModule(), "cannot dereference an enumeration constant", GetSpan());
    }
    else
    {
        emitter.Stack().Push(enumConstantSymbol->GetValue()->IrValue(emitter));
    }
    DestroyTemporaries(emitter);
}

void BoundEnumConstant::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to an enumeration constant", GetSpan());
}

void BoundEnumConstant::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundLiteral::BoundLiteral(Module* module_, std::unique_ptr<Value>&& value_, TypeSymbol* type_) : 
    BoundExpression(module_, value_->GetSpan(), BoundNodeType::boundLiteral, type_), value(std::move(value_))
{
}

BoundExpression* BoundLiteral::Clone()
{
    std::unique_ptr<Value> clonedValue;
    clonedValue.reset(value->Clone());
    return new BoundLiteral(GetModule(), std::move(clonedValue), GetType());
}

void BoundLiteral::Load(Emitter& emitter, OperationFlags flags)
{
    emitter.SetCurrentDebugLocation(GetSpan());
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception(GetModule(), "cannot take address of a literal", GetSpan());
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        throw Exception(GetModule(), "cannot dereference a literal", GetSpan());
    }
    else
    {
        emitter.Stack().Push(value->IrValue(emitter));
    }
    DestroyTemporaries(emitter);
}

void BoundLiteral::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to a literal", GetSpan());
}

void BoundLiteral::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

std::unique_ptr<Value> BoundLiteral::ToValue(BoundCompileUnit& boundCompileUnit) const
{ 
    switch (value->GetValueType())
    {
        case ValueType::stringValue: 
        {
            StringValue* stringValue = static_cast<StringValue*>(value.get());
            return std::unique_ptr<Value>(new PointerValue(GetSpan(), boundCompileUnit.GetSymbolTable().GetTypeByName(U"char")->AddPointer(Span()), boundCompileUnit.GetUtf8CharPtr(stringValue->StringId()))); 
        }
        case ValueType::wstringValue:
        {
            WStringValue* wstringValue = static_cast<WStringValue*>(value.get());
            return std::unique_ptr<Value>(new PointerValue(GetSpan(), boundCompileUnit.GetSymbolTable().GetTypeByName(U"wchar")->AddPointer(Span()), boundCompileUnit.GetUtf16CharPtr(wstringValue->StringId())));
        }
        case ValueType::ustringValue:
        {
            UStringValue* ustringValue = static_cast<UStringValue*>(value.get());
            return std::unique_ptr<Value>(new PointerValue(GetSpan(), boundCompileUnit.GetSymbolTable().GetTypeByName(U"uchar")->AddPointer(Span()), boundCompileUnit.GetUtf32CharPtr(ustringValue->StringId())));
        }
        default: 
        {
            return std::unique_ptr<Value>(value->Clone());
        }
    }
    return std::unique_ptr<Value>();
}

BoundGlobalVariable::BoundGlobalVariable(Module* module_, const Span& span_, GlobalVariableSymbol* globalVariableSymbol_) :
    BoundExpression(module_, span_, BoundNodeType::boundGlobalVariable, globalVariableSymbol_->GetType()), globalVariableSymbol(globalVariableSymbol_)
{
}

BoundExpression* BoundGlobalVariable::Clone()
{
    return new BoundGlobalVariable(GetModule(), GetSpan(), globalVariableSymbol);
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
        throw Exception(GetModule(), "cannot store to the address of a global variable", GetSpan());
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

BoundTemporary::BoundTemporary(Module* module_, std::unique_ptr<BoundExpression>&& rvalueExpr_, std::unique_ptr<BoundLocalVariable>&& backingStore_) :
    BoundExpression(module_, rvalueExpr_->GetSpan(), BoundNodeType::boundTemporary, rvalueExpr_->GetType()), rvalueExpr(std::move(rvalueExpr_)), backingStore(std::move(backingStore_))
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
    return new BoundTemporary(GetModule(), std::move(clonedRvalueExpr), std::move(clonedBackingStore));
}

void BoundTemporary::Load(Emitter& emitter, OperationFlags flags)
{
    rvalueExpr->Load(emitter, OperationFlags::none);
    if (!backingStore)
    {
        throw Exception(GetModule(), "backing store of temporary not set", GetSpan());
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
    throw Exception(GetModule(), "cannot store to a temporary", GetSpan());
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

BoundSizeOfExpression::BoundSizeOfExpression(Module* module_, const Span& span_, TypeSymbol* type_, TypeSymbol* pointerType_) :
    BoundExpression(module_, span_, BoundNodeType::boundSizeOfExpression, type_), pointerType(pointerType_)
{
}

BoundExpression* BoundSizeOfExpression::Clone()
{
    return new BoundSizeOfExpression(GetModule(), GetSpan(), GetType(), pointerType);
}

void BoundSizeOfExpression::Load(Emitter& emitter, OperationFlags flags)
{
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception(GetModule(), "cannot take address of a sizeof expression", GetSpan());
    }
    else if ((flags & OperationFlags::deref) != OperationFlags::none)
    {
        throw Exception(GetModule(), "cannot dereference a sizeof expression", GetSpan());
    }
    else
    {
        emitter.Stack().Push(emitter.SizeOf(pointerType->IrType(emitter)));
    }
    DestroyTemporaries(emitter);
}

void BoundSizeOfExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to a sizeof expression", GetSpan());
}

void BoundSizeOfExpression::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundAddressOfExpression::BoundAddressOfExpression(Module* module_, std::unique_ptr<BoundExpression>&& subject_, TypeSymbol* type_)  :
    BoundExpression(module_, subject_->GetSpan(), BoundNodeType::boundAddressOfExpression, type_), subject(std::move(subject_))
{
    subject->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundAddressOfExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedSubject;
    clonedSubject.reset(subject->Clone());
    return new BoundAddressOfExpression(GetModule(), std::move(clonedSubject), GetType());
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

BoundDereferenceExpression::BoundDereferenceExpression(Module* module_, std::unique_ptr<BoundExpression>&& subject_, TypeSymbol* type_) :
    BoundExpression(module_, subject_->GetSpan(), BoundNodeType::boundDereferenceExpression, type_), subject(std::move(subject_))
{
    subject->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundDereferenceExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedSubject;
    clonedSubject.reset(subject->Clone());
    return new BoundDereferenceExpression(GetModule(), std::move(clonedSubject), GetType());
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

BoundReferenceToPointerExpression::BoundReferenceToPointerExpression(Module* module_, std::unique_ptr<BoundExpression>&& subject_, TypeSymbol* type_) :
    BoundExpression(module_, subject_->GetSpan(), BoundNodeType::boundReferenceToPointerExpression, type_), subject(std::move(subject_))
{
    subject->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundReferenceToPointerExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedSubject;
    clonedSubject.reset(subject->Clone());
    return new BoundReferenceToPointerExpression(GetModule(), std::move(clonedSubject), GetType());
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

BoundFunctionCall::BoundFunctionCall(Module* module_, const Span& span_, FunctionSymbol* functionSymbol_) :
    BoundExpression(module_, span_, BoundNodeType::boundFunctionCall, functionSymbol_->ReturnType()), functionSymbol(functionSymbol_)
{
}

BoundExpression* BoundFunctionCall::Clone()
{
    BoundFunctionCall* clone = new BoundFunctionCall(GetModule(), GetSpan(), functionSymbol);
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
            functionSymbol->GenerateCall(emitter, genObjects, flags, GetSpan());
        }
        else
        {
            throw Exception(GetModule(), "cannot take address of a function call", GetSpan());
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
            emitter.SetLineNumber(GetSpan().LineNumber());
        }
        if (functionSymbol->Parent()->GetSymbolType() == SymbolType::interfaceTypeSymbol)
        {
            InterfaceTypeSymbol* interfaceType = static_cast<InterfaceTypeSymbol*>(functionSymbol->Parent());
            MemberFunctionSymbol* interfaceMemberFunction = static_cast<MemberFunctionSymbol*>(functionSymbol);
            interfaceType->GenerateCall(emitter, genObjects, callFlags, interfaceMemberFunction, GetSpan());
        }
        else
        {
            functionSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan());
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
        throw Exception(GetModule(), "cannot take address of a function call", GetSpan());
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
            emitter.SetLineNumber(GetSpan().LineNumber());
        }
        if (functionSymbol->IsArrayElementAccess())
        {
            functionSymbol->GenerateCall(emitter, genObjects, callFlags | OperationFlags::addr, GetSpan());
            void* ptr = emitter.Stack().Pop();
            emitter.CreateStore(value, ptr);
        }
        else
        {
            functionSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan());
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

BoundDelegateCall::BoundDelegateCall(Module* module_, const Span& span_, DelegateTypeSymbol* delegateType_) :
    BoundExpression(module_, span_, BoundNodeType::boundDelegateCall, delegateType_->ReturnType()), delegateTypeSymbol(delegateType_), arguments()
{
}

BoundExpression* BoundDelegateCall::Clone()
{
    return new BoundDelegateCall(GetModule(), GetSpan(), delegateTypeSymbol);
}

void BoundDelegateCall::Load(Emitter& emitter, OperationFlags flags)
{
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception(GetModule(), "cannot take address of a delegate call", GetSpan());
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
            emitter.SetLineNumber(GetSpan().LineNumber());
        }
        delegateTypeSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan());
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
        throw Exception(GetModule(), "cannot take address of a function call", GetSpan());
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
            emitter.SetLineNumber(GetSpan().LineNumber());
        }
        delegateTypeSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan());
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

BoundClassDelegateCall::BoundClassDelegateCall(Module* module_, const Span& span_, ClassDelegateTypeSymbol* classDelegateType_) :
    BoundExpression(module_, span_, BoundNodeType::boundClassDelegateCall, classDelegateType_->ReturnType()), classDelegateTypeSymbol(classDelegateType_), arguments()
{
}

BoundExpression* BoundClassDelegateCall::Clone()
{
    return new BoundClassDelegateCall(GetModule(), GetSpan(), classDelegateTypeSymbol);
}

void BoundClassDelegateCall::Load(Emitter& emitter, OperationFlags flags)
{
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception(GetModule(), "cannot take address of a delegate call", GetSpan());
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
            emitter.SetLineNumber(GetSpan().LineNumber());
        }
        classDelegateTypeSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan());
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
        throw Exception(GetModule(), "cannot take address of a function call", GetSpan());
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
            emitter.SetLineNumber(GetSpan().LineNumber());
        }
        classDelegateTypeSymbol->GenerateCall(emitter, genObjects, callFlags, GetSpan());
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

BoundConstructExpression::BoundConstructExpression(Module* module_, std::unique_ptr<BoundExpression>&& constructorCall_, TypeSymbol* resultType_) :
    BoundExpression(module_, constructorCall_->GetSpan(), BoundNodeType::boundConstructExpression, resultType_), constructorCall(std::move(constructorCall_))
{
    constructorCall->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundConstructExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedConstructorCall;
    clonedConstructorCall.reset(constructorCall->Clone());
    return new BoundConstructExpression(GetModule(), std::move(clonedConstructorCall), GetType());
}

void BoundConstructExpression::Load(Emitter& emitter, OperationFlags flags)
{
    void* prevObjectPointer = emitter.GetObjectPointer();
    emitter.SetObjectPointer(nullptr);
    if ((flags & OperationFlags::addr) != OperationFlags::none)
    {
        throw Exception(GetModule(), "cannot take address of a construct expression", GetSpan());
    }
    else
    {
        constructorCall->Load(emitter, OperationFlags::leaveFirstArg);
        void* objectPointer = emitter.GetObjectPointer();
        if (!objectPointer)
        {
            throw Exception(GetModule(), "do not have object pointer", GetSpan());
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
    throw Exception(GetModule(), "cannot store to construct expression", GetSpan());
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

BoundConstructAndReturnTemporaryExpression::BoundConstructAndReturnTemporaryExpression(Module* module_, std::unique_ptr<BoundExpression>&& constructorCall_, std::unique_ptr<BoundExpression>&& boundTemporary_) :
    BoundExpression(module_, constructorCall_->GetSpan(), BoundNodeType::boundConstructAndReturnTemporary, boundTemporary_->GetType()), constructorCall(std::move(constructorCall_)), 
    boundTemporary(std::move(boundTemporary_))
{
    constructorCall->MoveTemporaryDestructorCallsTo(*this);
    boundTemporary->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundConstructAndReturnTemporaryExpression::Clone()
{
    return new BoundConstructAndReturnTemporaryExpression(GetModule(), std::unique_ptr<BoundExpression>(constructorCall->Clone()), std::unique_ptr<BoundExpression>(boundTemporary->Clone()));
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
    throw Exception(GetModule(), "cannot store to construct and return temporary expression", GetSpan());
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

BoundClassOrClassDelegateConversionResult::BoundClassOrClassDelegateConversionResult(Module* module_, std::unique_ptr<BoundExpression>&& conversionResult_, std::unique_ptr<BoundFunctionCall>&& conversionFunctionCall_) :
    BoundExpression(module_, conversionResult_->GetSpan(), BoundNodeType::boundClassOrClassDelegateConversionResult, conversionResult_->GetType()),
    conversionResult(std::move(conversionResult_)), conversionFunctionCall(std::move(conversionFunctionCall_))
{
}

BoundExpression* BoundClassOrClassDelegateConversionResult::Clone()
{
    return new BoundClassOrClassDelegateConversionResult(GetModule(), std::unique_ptr<BoundExpression>(conversionResult->Clone()),
        std::unique_ptr<BoundFunctionCall>(static_cast<BoundFunctionCall*>(conversionFunctionCall->Clone())));
}

void BoundClassOrClassDelegateConversionResult::Load(Emitter& emitter, OperationFlags flags)
{
    conversionFunctionCall->Load(emitter, OperationFlags::none);
    conversionResult->Load(emitter, flags);
}

void BoundClassOrClassDelegateConversionResult::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to class conversion result", GetSpan());
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

BoundConversion::BoundConversion(Module* module_, std::unique_ptr<BoundExpression>&& sourceExpr_, FunctionSymbol* conversionFun_) :
    BoundExpression(module_, sourceExpr_->GetSpan(), BoundNodeType::boundConversion, conversionFun_->ConversionTargetType()), sourceExpr(std::move(sourceExpr_)), conversionFun(conversionFun_)
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
    BoundConversion* clone = new BoundConversion(GetModule(), std::move(clonedSourceExpr), conversionFun);
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
    conversionFun->GenerateCall(emitter, genObjects, OperationFlags::none, GetSpan());
    DestroyTemporaries(emitter);
}

void BoundConversion::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to a conversion", GetSpan());
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

BoundIsExpression::BoundIsExpression(Module* module_, std::unique_ptr<BoundExpression>&& expr_, ClassTypeSymbol* rightClassType_, TypeSymbol* boolType_,
    std::unique_ptr<BoundLocalVariable>&& leftClassIdVar_, std::unique_ptr<BoundLocalVariable>&& rightClassIdVar_) :
    BoundExpression(module_, expr_->GetSpan(), BoundNodeType::boundIsExpression, boolType_), expr(std::move(expr_)), rightClassType(rightClassType_),
    leftClassIdVar(std::move(leftClassIdVar_)), rightClassIdVar(std::move(rightClassIdVar_))
{
}

BoundExpression* BoundIsExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedExpr;
    clonedExpr.reset(expr->Clone());
    return new BoundIsExpression(GetModule(), std::move(clonedExpr), rightClassType, GetType(),
        std::unique_ptr<BoundLocalVariable>(static_cast<BoundLocalVariable*>(leftClassIdVar->Clone())),
        std::unique_ptr<BoundLocalVariable>(static_cast<BoundLocalVariable*>(rightClassIdVar->Clone())));
}

void BoundIsExpression::Load(Emitter& emitter, OperationFlags flags)
{
    if (GetBackEnd() == BackEnd::llvm)
    {
        expr->Load(emitter, OperationFlags::none);
        void* thisPtr = emitter.Stack().Pop();
        TypeSymbol* exprType = static_cast<TypeSymbol*>(expr->GetType());
        Assert(exprType->IsPointerType(), "pointer type expected");
        TypeSymbol* leftType = exprType->RemovePointer(GetSpan());
        Assert(leftType->IsClassTypeSymbol(), "class type expected");
        ClassTypeSymbol* leftClassType = static_cast<ClassTypeSymbol*>(leftType);
        ClassTypeSymbol* leftVmtPtrHolderClass = leftClassType->VmtPtrHolderClass();
        if (leftClassType != leftVmtPtrHolderClass)
        {
            thisPtr = emitter.CreateBitCast(thisPtr, leftVmtPtrHolderClass->AddPointer(GetSpan())->IrType(emitter));
        }
        void* vmtPtr = emitter.GetVmtPtr(thisPtr, leftVmtPtrHolderClass->VmtPtrIndex(), leftClassType->VmtPtrType(emitter));
        void* leftClassIdPtr = emitter.GetClassIdPtr(vmtPtr);
        void* leftClassId = emitter.CreatePtrToInt(emitter.CreateLoad(leftClassIdPtr), emitter.GetIrTypeForULong());
        void* leftClassIdNull = emitter.CreateICmpEQ(leftClassId, emitter.CreateDefaultIrValueForULong());
        void* leftTrueBlock = emitter.CreateBasicBlock("leftTrue");
        void* leftFalseBlock = emitter.CreateBasicBlock("leftFalse");
        void* leftContinueBlock = emitter.CreateBasicBlock("leftContinue");
        emitter.CreateCondBr(leftClassIdNull, leftTrueBlock, leftFalseBlock);
        emitter.SetCurrentBasicBlock(leftTrueBlock);
        void* retType = emitter.GetIrTypeForULong();
        std::vector<void*> paramTypes;
        paramTypes.push_back(emitter.GetIrTypeForVoidPtrType());
        void* dynamicInitFnType = emitter.GetIrTypeForFunction(retType, paramTypes);
        void* dynamicInitFn = emitter.GetOrInsertFunction("RtDynamicInitVmt", dynamicInitFnType);
        std::vector<void*> leftArgs;
        leftArgs.push_back(emitter.CreateBitCast(leftClassIdPtr, emitter.GetIrTypeForVoidPtrType()));
        void* computedLeftClassId = emitter.CreateCall(dynamicInitFn, leftArgs);
        emitter.Stack().Push(computedLeftClassId);
        leftClassIdVar->Store(emitter, OperationFlags::none);
        emitter.CreateBr(leftContinueBlock);
        emitter.SetCurrentBasicBlock(leftFalseBlock);
        emitter.Stack().Push(leftClassId);
        leftClassIdVar->Store(emitter, OperationFlags::none);
        emitter.CreateBr(leftContinueBlock);
        emitter.SetCurrentBasicBlock(leftContinueBlock);
        void* rightClassTypeVmtObject = rightClassType->VmtObject(emitter, false);
        void* rightClassIdPtr = emitter.GetClassIdPtr(rightClassTypeVmtObject);
        void* rightClassId = emitter.CreatePtrToInt(emitter.CreateLoad(rightClassIdPtr), emitter.GetIrTypeForULong());
        void* rightClassIdNull = emitter.CreateICmpEQ(rightClassId, emitter.CreateDefaultIrValueForULong());
        void* rightTrueBlock = emitter.CreateBasicBlock("rightTrue");
        void* rightFalseBlock = emitter.CreateBasicBlock("rightFalse");
        void* rightContinueBlock = emitter.CreateBasicBlock("rightContinue");
        emitter.CreateCondBr(rightClassIdNull, rightTrueBlock, rightFalseBlock);
        emitter.SetCurrentBasicBlock(rightTrueBlock);
        std::vector<void*> rightArgs;
        rightArgs.push_back(emitter.CreateBitCast(rightClassIdPtr, emitter.GetIrTypeForVoidPtrType()));
        void* computedRightClassId = emitter.CreateCall(dynamicInitFn, rightArgs);
        emitter.Stack().Push(computedRightClassId);
        rightClassIdVar->Store(emitter, OperationFlags::none);
        emitter.CreateBr(rightContinueBlock);
        emitter.SetCurrentBasicBlock(rightFalseBlock);
        emitter.Stack().Push(rightClassId);
        rightClassIdVar->Store(emitter, OperationFlags::none);
        emitter.CreateBr(rightContinueBlock);
        emitter.SetCurrentBasicBlock(rightContinueBlock);
        leftClassIdVar->Load(emitter, OperationFlags::none);
        void* loadedLeftClassId = emitter.Stack().Pop();
        rightClassIdVar->Load(emitter, OperationFlags::none);
        void* loadedRightClassId = emitter.Stack().Pop();
        void* remainder = emitter.CreateURem(loadedLeftClassId, loadedRightClassId);
        void* remainderIsZero = emitter.CreateICmpEQ(remainder, emitter.CreateDefaultIrValueForULong());
        emitter.Stack().Push(remainderIsZero);
        DestroyTemporaries(emitter);
    }
    else if (GetBackEnd() == BackEnd::cmsx)
    {
        expr->Load(emitter, OperationFlags::none);
        void* thisPtr = emitter.Stack().Pop();
        TypeSymbol* exprType = static_cast<TypeSymbol*>(expr->GetType());
        Assert(exprType->IsPointerType(), "pointer type expected");
        TypeSymbol* leftType = exprType->RemovePointer(GetSpan());
        Assert(leftType->IsClassTypeSymbol(), "class type expected");
        ClassTypeSymbol* leftClassType = static_cast<ClassTypeSymbol*>(leftType);
        ClassTypeSymbol* leftVmtPtrHolderClass = leftClassType->VmtPtrHolderClass();
        if (leftClassType != leftVmtPtrHolderClass)
        {
            thisPtr = emitter.CreateBitCast(thisPtr, leftVmtPtrHolderClass->AddPointer(GetSpan())->IrType(emitter));
        }
        void* vmtPtr = emitter.GetVmtPtr(thisPtr, leftVmtPtrHolderClass->VmtPtrIndex(), leftClassType->VmtPtrType(emitter));
        void* leftClassIdPtr = emitter.GetClassIdPtr(vmtPtr);
        void* leftClassId = emitter.CreatePtrToInt(emitter.CreateLoad(leftClassIdPtr), emitter.GetIrTypeForULong());
        void* rightClassTypeVmtObject = rightClassType->VmtObject(emitter, false);
        void* rightClassIdPtr = emitter.GetClassIdPtr(rightClassTypeVmtObject);
        void* rightClassId = emitter.CreatePtrToInt(emitter.CreateLoad(rightClassIdPtr), emitter.GetIrTypeForULong());
        void* remainder = emitter.CreateURem(leftClassId, rightClassId);
        void* remainderIsZero = emitter.CreateICmpEQ(remainder, emitter.CreateDefaultIrValueForULong());
        emitter.Stack().Push(remainderIsZero);
        DestroyTemporaries(emitter);
    }
}

void BoundIsExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to a 'is' expression", GetSpan());
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

BoundAsExpression::BoundAsExpression(Module* module_, std::unique_ptr<BoundExpression>&& expr_, ClassTypeSymbol* rightClassType_, std::unique_ptr<BoundLocalVariable>&& variable_,
    std::unique_ptr<BoundLocalVariable>&& leftClassIdVar_, std::unique_ptr<BoundLocalVariable>&& rightClassIdVar_) :
    BoundExpression(module_, expr_->GetSpan(), BoundNodeType::boundAsExpression, rightClassType_->AddPointer(expr_->GetSpan())), 
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
    return new BoundAsExpression(GetModule(), std::move(clonedExpr), rightClassType, std::move(clonedVariable),
        std::unique_ptr<BoundLocalVariable>(static_cast<BoundLocalVariable*>(leftClassIdVar->Clone())), 
        std::unique_ptr<BoundLocalVariable>(static_cast<BoundLocalVariable*>(rightClassIdVar->Clone())));
}

void BoundAsExpression::Load(Emitter& emitter, OperationFlags flags)
{
    if (GetBackEnd() == BackEnd::llvm)
    {
        expr->Load(emitter, OperationFlags::none);
        void* thisPtr = emitter.Stack().Pop();
        TypeSymbol* exprType = static_cast<TypeSymbol*>(expr->GetType());
        Assert(exprType->IsPointerType(), "pointer type expected");
        TypeSymbol* leftType = exprType->RemovePointer(GetSpan());
        Assert(leftType->IsClassTypeSymbol(), "class type expected");
        ClassTypeSymbol* leftClassType = static_cast<ClassTypeSymbol*>(leftType);
        ClassTypeSymbol* leftVmtPtrHolderClass = leftClassType->VmtPtrHolderClass();
        if (leftClassType != leftVmtPtrHolderClass)
        {
            thisPtr = emitter.CreateBitCast(thisPtr, leftVmtPtrHolderClass->AddPointer(GetSpan())->IrType(emitter));
        }
        void* vmtPtr = emitter.GetVmtPtr(thisPtr, leftVmtPtrHolderClass->VmtPtrIndex(), leftClassType->VmtPtrType(emitter));
        void* leftClassIdPtr = emitter.GetClassIdPtr(vmtPtr);
        void* leftClassId = emitter.CreatePtrToInt(emitter.CreateLoad(leftClassIdPtr), emitter.GetIrTypeForULong());
        void* leftClassIdNull = emitter.CreateICmpEQ(leftClassId, emitter.CreateDefaultIrValueForULong());
        void* leftTrueBlock = emitter.CreateBasicBlock("leftTrue");
        void* leftFalseBlock = emitter.CreateBasicBlock("leftFalse");
        void* leftContinueBlock = emitter.CreateBasicBlock("leftContinue");
        emitter.CreateCondBr(leftClassIdNull, leftTrueBlock, leftFalseBlock);
        emitter.SetCurrentBasicBlock(leftTrueBlock);
        void* retType = emitter.GetIrTypeForULong();
        std::vector<void*> paramTypes;
        paramTypes.push_back(emitter.GetIrTypeForVoidPtrType());
        void* dynamicInitFnType = emitter.GetIrTypeForFunction(retType, paramTypes);
        void* dynamicInitFn = emitter.GetOrInsertFunction("RtDynamicInitVmt", dynamicInitFnType);
        std::vector<void*> leftArgs;
        leftArgs.push_back(emitter.CreateBitCast(leftClassIdPtr, emitter.GetIrTypeForVoidPtrType()));
        void* computedLeftClassId = emitter.CreateCall(dynamicInitFn, leftArgs);
        emitter.Stack().Push(computedLeftClassId);
        leftClassIdVar->Store(emitter, OperationFlags::none);
        emitter.CreateBr(leftContinueBlock);
        emitter.SetCurrentBasicBlock(leftFalseBlock);
        emitter.Stack().Push(leftClassId);
        leftClassIdVar->Store(emitter, OperationFlags::none);
        emitter.CreateBr(leftContinueBlock);
        emitter.SetCurrentBasicBlock(leftContinueBlock);
        void* rightClassTypeVmtObject = rightClassType->VmtObject(emitter, false);
        void* rightClassIdPtr = emitter.GetClassIdPtr(rightClassTypeVmtObject);
        void* rightClassId = emitter.CreatePtrToInt(emitter.CreateLoad(rightClassIdPtr), emitter.GetIrTypeForULong());
        void* rightClassIdNull = emitter.CreateICmpEQ(rightClassId, emitter.CreateDefaultIrValueForULong());
        void* rightTrueBlock = emitter.CreateBasicBlock("rightTrue");
        void* rightFalseBlock = emitter.CreateBasicBlock("rightFalse");
        void* rightContinueBlock = emitter.CreateBasicBlock("rightContinue");
        emitter.CreateCondBr(rightClassIdNull, rightTrueBlock, rightFalseBlock);
        emitter.SetCurrentBasicBlock(rightTrueBlock);
        std::vector<void*> rightArgs;
        rightArgs.push_back(emitter.CreateBitCast(rightClassIdPtr, emitter.GetIrTypeForVoidPtrType()));
        void* computedRightClassId = emitter.CreateCall(dynamicInitFn, rightArgs);
        emitter.Stack().Push(computedRightClassId);
        rightClassIdVar->Store(emitter, OperationFlags::none);
        emitter.CreateBr(rightContinueBlock);
        emitter.SetCurrentBasicBlock(rightFalseBlock);
        emitter.Stack().Push(rightClassId);
        rightClassIdVar->Store(emitter, OperationFlags::none);
        emitter.CreateBr(rightContinueBlock);
        emitter.SetCurrentBasicBlock(rightContinueBlock);
        leftClassIdVar->Load(emitter, OperationFlags::none);
        void* loadedLeftClassId = emitter.Stack().Pop();
        rightClassIdVar->Load(emitter, OperationFlags::none);
        void* loadedRightClassId = emitter.Stack().Pop();
        void* remainder = emitter.CreateURem(loadedLeftClassId, loadedRightClassId);
        void* remainderIsZero = emitter.CreateICmpEQ(remainder, emitter.CreateDefaultIrValueForULong());
        void* trueBlock = emitter.CreateBasicBlock("true");
        void* falseBlock = emitter.CreateBasicBlock("false");
        void* continueBlock = emitter.CreateBasicBlock("continue");
        emitter.CreateCondBr(remainderIsZero, trueBlock, falseBlock);
        emitter.SetCurrentBasicBlock(trueBlock);
        emitter.Stack().Push(emitter.CreateBitCast(thisPtr, rightClassType->AddPointer(GetSpan())->IrType(emitter)));
        variable->Store(emitter, OperationFlags::none);
        emitter.CreateBr(continueBlock);
        emitter.SetCurrentBasicBlock(falseBlock);
        emitter.Stack().Push(emitter.CreateDefaultIrValueForPtrType(rightClassType->AddPointer(GetSpan())->IrType(emitter)));
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
        TypeSymbol* leftType = exprType->RemovePointer(GetSpan());
        Assert(leftType->IsClassTypeSymbol(), "class type expected");
        ClassTypeSymbol* leftClassType = static_cast<ClassTypeSymbol*>(leftType);
        ClassTypeSymbol* leftVmtPtrHolderClass = leftClassType->VmtPtrHolderClass();
        if (leftClassType != leftVmtPtrHolderClass)
        {
            thisPtr = emitter.CreateBitCast(thisPtr, leftVmtPtrHolderClass->AddPointer(GetSpan())->IrType(emitter));
        }
        void* vmtPtr = emitter.GetVmtPtr(thisPtr, leftVmtPtrHolderClass->VmtPtrIndex(), leftClassType->VmtPtrType(emitter));
        void* leftClassIdPtr = emitter.GetClassIdPtr(vmtPtr);
        void* leftClassId = emitter.CreatePtrToInt(emitter.CreateLoad(leftClassIdPtr), emitter.GetIrTypeForULong());
        void* rightClassTypeVmtObject = rightClassType->VmtObject(emitter, false);
        void* rightClassIdPtr = emitter.GetClassIdPtr(rightClassTypeVmtObject);
        void* rightClassId = emitter.CreatePtrToInt(emitter.CreateLoad(rightClassIdPtr), emitter.GetIrTypeForULong());
        void* remainder = emitter.CreateURem(leftClassId, rightClassId);
        void* remainderIsZero = emitter.CreateICmpEQ(remainder, emitter.CreateDefaultIrValueForULong());
        void* trueBlock = emitter.CreateBasicBlock("true");
        void* falseBlock = emitter.CreateBasicBlock("false");
        void* continueBlock = emitter.CreateBasicBlock("continue");
        emitter.CreateCondBr(remainderIsZero, trueBlock, falseBlock);
        emitter.SetCurrentBasicBlock(trueBlock);
        emitter.Stack().Push(emitter.CreateBitCast(thisPtr, rightClassType->AddPointer(GetSpan())->IrType(emitter)));
        variable->Store(emitter, OperationFlags::none);
        emitter.CreateBr(continueBlock);
        emitter.SetCurrentBasicBlock(falseBlock);
        emitter.Stack().Push(emitter.CreateDefaultIrValueForPtrType(rightClassType->AddPointer(GetSpan())->IrType(emitter)));
        variable->Store(emitter, OperationFlags::none);
        emitter.CreateBr(continueBlock);
        emitter.SetCurrentBasicBlock(continueBlock);
        variable->Load(emitter, OperationFlags::none);
        DestroyTemporaries(emitter);
    }
}

void BoundAsExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to an 'as' expression", GetSpan());
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

BoundTypeNameExpression::BoundTypeNameExpression(Module* module_, std::unique_ptr<BoundExpression>&& classPtr_, TypeSymbol* constCharPtrType_) :
    BoundExpression(module_, classPtr_->GetSpan(), BoundNodeType::boundTypeNameExpression, constCharPtrType_), classPtr(std::move(classPtr_))
{
    classPtr->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundTypeNameExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedClassPtr;
    clonedClassPtr.reset(classPtr->Clone());
    return new BoundTypeNameExpression(GetModule(), std::move(clonedClassPtr), GetType());
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
        thisPtr = emitter.CreateBitCast(thisPtr, vmtPtrHolderClass->AddPointer(GetSpan())->IrType(emitter));
    }
    void* vmtPtr = emitter.GetVmtPtr(thisPtr, vmtPtrHolderClass->VmtPtrIndex(), classType->VmtPtrType(emitter));
    void* className = emitter.GetClassName(vmtPtr, GetClassNameVmtIndexOffset());
    emitter.Stack().Push(className);
    DestroyTemporaries(emitter);
}

void BoundTypeNameExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to typename expression", GetSpan());
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

BoundTypeIdExpression::BoundTypeIdExpression(Module* module_, std::unique_ptr<BoundExpression>&& classPtr_, TypeSymbol* ulongType_) :
    BoundExpression(module_, classPtr_->GetSpan(), BoundNodeType::boundTypeIdExpression, ulongType_), classPtr(std::move(classPtr_))
{
    classPtr->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundTypeIdExpression::Clone()
{
    std::unique_ptr<BoundExpression> clonedClassPtr;
    clonedClassPtr.reset(classPtr->Clone());
    return new BoundTypeIdExpression(GetModule(), std::move(clonedClassPtr), GetType());
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
        thisPtr = emitter.CreateBitCast(thisPtr, vmtPtrHolderClass->AddPointer(GetSpan())->IrType(emitter));
    }
    void* vmtPtr = emitter.GetVmtPtr(thisPtr, vmtPtrHolderClass->VmtPtrIndex(), classType->VmtPtrType(emitter));
    void* classIdPtr = emitter.GetClassIdPtr(vmtPtr);
    void* classId = emitter.CreatePtrToInt(emitter.CreateLoad(classIdPtr), emitter.GetIrTypeForULong());
    emitter.Stack().Push(classId);
    DestroyTemporaries(emitter);
}

void BoundTypeIdExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to typeid expression", GetSpan());
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

BoundBitCast::BoundBitCast(Module* module_, std::unique_ptr<BoundExpression>&& expr_, TypeSymbol* type_) : 
    BoundExpression(module_, expr_->GetSpan(), BoundNodeType::boundBitCast, type_), expr(std::move(expr_))
{
    expr->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundBitCast::Clone()
{
    return new BoundBitCast(GetModule(), std::unique_ptr<BoundExpression>(expr->Clone()), GetType());
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
    throw Exception(GetModule(), "cannot store to bit cast", GetSpan());
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

BoundFunctionPtr::BoundFunctionPtr(Module* module_, const Span& span_, FunctionSymbol* function_, TypeSymbol* type_) : 
    BoundExpression(module_, span_, BoundNodeType::boundFunctionPtr, type_), function(function_)
{
}

BoundExpression* BoundFunctionPtr::Clone()
{
    return new BoundFunctionPtr(GetModule(), GetSpan(), function, GetType());
}

void BoundFunctionPtr::Load(Emitter& emitter, OperationFlags flags)
{
    void* irObject = emitter.GetOrInsertFunction(ToUtf8(function->MangledName()), function->IrType(emitter));
    emitter.Stack().Push(irObject);
    DestroyTemporaries(emitter);
}

void BoundFunctionPtr::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to function ptr expression", GetSpan());
}

void BoundFunctionPtr::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

BoundDisjunction::BoundDisjunction(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& left_, std::unique_ptr<BoundExpression>&& right_, TypeSymbol* boolType_) :
    BoundExpression(module_, span_, BoundNodeType::boundDisjunction, boolType_), left(std::move(left_)), right(std::move(right_))
{
}

BoundExpression* BoundDisjunction::Clone()
{
    return new BoundDisjunction(GetModule(), GetSpan(), std::unique_ptr<BoundExpression>(left->Clone()), std::unique_ptr<BoundExpression>(right->Clone()), GetType());
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
    throw Exception(GetModule(), "cannot store to disjunction", GetSpan());
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

BoundConjunction::BoundConjunction(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& left_, std::unique_ptr<BoundExpression>&& right_, TypeSymbol* boolType_) :
    BoundExpression(module_, span_, BoundNodeType::boundConjunction, boolType_), left(std::move(left_)), right(std::move(right_))
{
}

BoundExpression* BoundConjunction::Clone()
{
    return new BoundConjunction(GetModule(), GetSpan(), std::unique_ptr<BoundExpression>(left->Clone()), std::unique_ptr<BoundExpression>(right->Clone()), GetType());
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
    throw Exception(GetModule(), "cannot store to conjunction", GetSpan());
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

BoundTypeExpression::BoundTypeExpression(Module* module_, const Span& span_, TypeSymbol* type_) : 
    BoundExpression(module_, span_, BoundNodeType::boundTypeExpression, type_)
{
}

BoundExpression* BoundTypeExpression::Clone()
{
    return new BoundTypeExpression(GetModule(), GetSpan(), GetType());
}

void BoundTypeExpression::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(),"cannot load from a type", GetSpan());
}

void BoundTypeExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to a type", GetSpan());
}

void BoundTypeExpression::Accept(BoundNodeVisitor& visitor)
{
    throw Exception(GetModule(), "cannot visit a type", GetSpan());
}

TypeSymbol* CreateNamespaceTypeSymbol(NamespaceSymbol* ns, Module* module)
{
    TypeSymbol* nsTypeSymbol = new NamespaceTypeSymbol(ns);
    nsTypeSymbol->SetModule(module);
    module->GetSymbolTable().SetTypeIdFor(nsTypeSymbol);
    return nsTypeSymbol;
}

BoundNamespaceExpression::BoundNamespaceExpression(Module* module_, const Span& span_, NamespaceSymbol* ns_) : 
    BoundExpression(module_, span_, BoundNodeType::boundNamespaceExpression, CreateNamespaceTypeSymbol(ns_, module_)), ns(ns_)
{
    nsType.reset(GetType());
}

BoundExpression* BoundNamespaceExpression::Clone()
{
    return new BoundNamespaceExpression(GetModule(), GetSpan(), ns);
}

void BoundNamespaceExpression::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot load from a namespace", GetSpan());
}

void BoundNamespaceExpression::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store to a namespace", GetSpan());
}

void BoundNamespaceExpression::Accept(BoundNodeVisitor& visitor)
{
    throw Exception(GetModule(), "cannot visit a namespace", GetSpan());
}

TypeSymbol* CreateFunctionGroupTypeSymbol(FunctionGroupSymbol* functionGroupSymbol, void* boundFunctionGroupExpression, Module* module)
{
    TypeSymbol* functionGroupTypeSymbol = new FunctionGroupTypeSymbol(functionGroupSymbol, boundFunctionGroupExpression);
    functionGroupTypeSymbol->SetModule(module);
    module->GetSymbolTable().SetTypeIdFor(functionGroupTypeSymbol);
    return functionGroupTypeSymbol;
}

BoundFunctionGroupExpression::BoundFunctionGroupExpression(Module* module_, const Span& span_, FunctionGroupSymbol* functionGroupSymbol_) :
    BoundExpression(module_, span_, BoundNodeType::boundFunctionGroupExpression, CreateFunctionGroupTypeSymbol(functionGroupSymbol_, this, module_)), 
    functionGroupSymbol(functionGroupSymbol_), scopeQualified(false), qualifiedScope(nullptr)
{
    functionGroupType.reset(GetType());
}

BoundExpression* BoundFunctionGroupExpression::Clone()
{
    BoundFunctionGroupExpression* clone = new BoundFunctionGroupExpression(GetModule(), GetSpan(), functionGroupSymbol);
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
    throw Exception(GetModule(), "cannot store to a function group", GetSpan());
}

void BoundFunctionGroupExpression::Accept(BoundNodeVisitor& visitor)
{
    throw Exception(GetModule(), "cannot visit a function group", GetSpan());
}

void BoundFunctionGroupExpression::SetClassPtr(std::unique_ptr<BoundExpression>&& classPtr_)
{
    classPtr = std::move(classPtr_);
}

void BoundFunctionGroupExpression::SetTemplateArgumentTypes(const std::vector<TypeSymbol*>& templateArgumentTypes_)
{
    templateArgumentTypes = templateArgumentTypes_;
}

TypeSymbol* CreateMemberExpressionTypeSymbol(const Span& span, const std::u32string& name, void* boundMemberExpression, Module* module)
{
    TypeSymbol* memberExpressionTypeSymbol = new MemberExpressionTypeSymbol(span, name, boundMemberExpression);
    memberExpressionTypeSymbol->SetModule(module);
    module->GetSymbolTable().SetTypeIdFor(memberExpressionTypeSymbol);
    return memberExpressionTypeSymbol;
}

BoundMemberExpression::BoundMemberExpression(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& classPtr_, std::unique_ptr<BoundExpression>&& member_) :
    BoundExpression(module_, span_, BoundNodeType::boundMemberExpression, CreateMemberExpressionTypeSymbol(span_, member_->GetType()->Name(), this, module_)), classPtr(std::move(classPtr_)), member(std::move(member_))
{
    memberExpressionType.reset(GetType());
    classPtr->MoveTemporaryDestructorCallsTo(*this);
}

BoundExpression* BoundMemberExpression::Clone()
{
    return new BoundMemberExpression(GetModule(), GetSpan(), std::unique_ptr<BoundExpression>(classPtr->Clone()), std::unique_ptr<BoundExpression>(member->Clone()));
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
    throw Exception(GetModule(), "cannot store to a member expression", GetSpan());
}

void BoundMemberExpression::Accept(BoundNodeVisitor& visitor)
{
    throw Exception(GetModule(), "cannot visit a member expression", GetSpan());
}

} } // namespace cmajor::binder
