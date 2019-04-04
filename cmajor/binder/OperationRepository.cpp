// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/OperationRepository.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/ExpressionBinder.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/OverloadResolution.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/symbols/BasicTypeOperation.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <cmajor/ast/Identifier.hpp>
#include <cmajor/ast/Expression.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::unicode;

class PointerDefaultCtor : public FunctionSymbol
{
public:
    PointerDefaultCtor(TypeSymbol* type_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerDefaultCtor"; }
private:
    TypeSymbol* type;
    void* nullValue;
};

PointerDefaultCtor::PointerDefaultCtor(TypeSymbol* type_, const Span& span) : FunctionSymbol(span, U"@constructor"), type(type_), nullValue(nullptr)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span, U"this");
    thisParam->SetType(type->AddPointer(span));
    AddMember(thisParam);
    ComputeName();
}

void PointerDefaultCtor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "default constructor needs one object");
    if (!nullValue)
    {
        nullValue = emitter.CreateDefaultIrValueForPtrType(type->IrType(emitter));
    }
    emitter.Stack().Push(nullValue);
    genObjects[0]->Store(emitter, OperationFlags::none);
}

class PointerDefaultConstructorOperation : public Operation
{
public:
    PointerDefaultConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerDefaultConstructorOperation::PointerDefaultConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 1, boundCompileUnit_)
{
}

void PointerDefaultConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() <= 1) return;
    if (type->IsReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    TypeSymbol* pointerType = type->RemovePointer(span);
    FunctionSymbol* function = functionMap[pointerType];
    if (!function)
    {
        function = new PointerDefaultCtor(pointerType, span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[pointerType] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class PointerCopyCtor : public FunctionSymbol
{
public:
    PointerCopyCtor(TypeSymbol* type_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerCopyCtor"; }
private:
    TypeSymbol* type;
};

PointerCopyCtor::PointerCopyCtor(TypeSymbol* type_, const Span& span) : FunctionSymbol(span, U"@constructor"), type(type_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span, U"this");
    thisParam->SetType(type->AddPointer(span));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span, U"that");
    thatParam->SetType(type);
    AddMember(thatParam);
    ComputeName();
}

void PointerCopyCtor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "copy constructor needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    if ((flags & OperationFlags::leaveFirstArg) != OperationFlags::none)
    {
        emitter.Stack().Dup();
        void* ptr = emitter.Stack().Pop();
        emitter.SaveObjectPointer(ptr);
    }
    genObjects[0]->Store(emitter, OperationFlags::none);
}

class PointerCopyConstructorOperation : public Operation
{
public:
    PointerCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerCopyConstructorOperation::PointerCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 2, boundCompileUnit_)
{
}

void PointerCopyConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() <= 1) return;
    if (type->IsReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    TypeSymbol* pointerType = type->RemovePointer(span);
    if (!TypesEqual(arguments[1]->GetType(), pointerType->AddRvalueReference(span)) && !arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference))
    {
        FunctionSymbol* function = functionMap[pointerType];
        if (!function)
        {
            function = new PointerCopyCtor(pointerType, span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[pointerType] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class PointerMoveCtor : public FunctionSymbol
{
public:
    PointerMoveCtor(TypeSymbol* type_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerMoveCtor"; }
private:
    TypeSymbol* type;
};

PointerMoveCtor::PointerMoveCtor(TypeSymbol* type_, const Span& span) : FunctionSymbol(span, U"@constructor"), type(type_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span, U"this");
    thisParam->SetType(type->AddPointer(span));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span, U"that");
    thatParam->SetType(type->AddRvalueReference(span));
    AddMember(thatParam);
    ComputeName();
}

void PointerMoveCtor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "move constructor needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* rvalueRefValue = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateLoad(rvalueRefValue));
    if ((flags & OperationFlags::leaveFirstArg) != OperationFlags::none)
    {
        emitter.Stack().Dup();
        void* ptr = emitter.Stack().Pop();
        emitter.SaveObjectPointer(ptr);
    }
    genObjects[0]->Store(emitter, OperationFlags::none);
}

class PointerMoveConstructorOperation : public Operation
{
public:
    PointerMoveConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerMoveConstructorOperation::PointerMoveConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 2, boundCompileUnit_)
{
}

void PointerMoveConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() <= 1) return;
    if (type->IsReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    TypeSymbol* pointerType = type->RemovePointer(span);
    if (TypesEqual(arguments[1]->GetType(), pointerType->AddRvalueReference(span)) || arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference))
    {
        FunctionSymbol* function = functionMap[pointerType];
        if (!function)
        {
            function = new PointerMoveCtor(pointerType, span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[pointerType] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class PointerCopyAssignment : public FunctionSymbol
{
public:
    PointerCopyAssignment(TypeSymbol* type_, TypeSymbol* voidType_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerCopyAssignment"; }
private:
    TypeSymbol* type;
};

PointerCopyAssignment::PointerCopyAssignment(TypeSymbol* type_, TypeSymbol* voidType_, const Span& span) : FunctionSymbol(span, U"operator="), type(type_)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span, U"this");
    thisParam->SetType(type->AddPointer(span));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span, U"that");
    thatParam->SetType(type);
    AddMember(thatParam);
    SetReturnType(voidType_);
    ComputeName();
}

void PointerCopyAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "copy assignment needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    genObjects[0]->Store(emitter, OperationFlags::none);
}

class PointerCopyAssignmentOperation : public Operation
{
public:
    PointerCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerCopyAssignmentOperation::PointerCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator=", 2, boundCompileUnit_)
{
}

void PointerCopyAssignmentOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() <= 1) return;
    if (type->IsReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    TypeSymbol* pointerType = type->RemovePointer(span);
    if (!TypesEqual(arguments[1]->GetType(), pointerType->AddRvalueReference(span)) && !arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference))
    {
        FunctionSymbol* function = functionMap[pointerType];
        if (!function)
        {
            function = new PointerCopyAssignment(pointerType, GetSymbolTable()->GetTypeByName(U"void"), span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[pointerType] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class PointerMoveAssignment : public FunctionSymbol
{
public:
    PointerMoveAssignment(TypeSymbol* type_, TypeSymbol* voidType_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerMoveAssignment"; }
private:
    TypeSymbol* type;
};

PointerMoveAssignment::PointerMoveAssignment(TypeSymbol* type_, TypeSymbol* voidType_, const Span& span) : FunctionSymbol(span, U"operator="), type(type_)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span, U"this");
    thisParam->SetType(type->AddPointer(span));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span, U"that");
    thatParam->SetType(type->AddRvalueReference(span));
    AddMember(thatParam);
    SetReturnType(voidType_);
    ComputeName();
}

void PointerMoveAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "copy assignment needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* rvalueRefValue = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateLoad(rvalueRefValue));
    genObjects[0]->Store(emitter, OperationFlags::none);
}

class PointerMoveAssignmentOperation : public Operation
{
public:
    PointerMoveAssignmentOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerMoveAssignmentOperation::PointerMoveAssignmentOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator=", 2, boundCompileUnit_)
{
}

void PointerMoveAssignmentOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() <= 1) return;
    if (type->IsReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    TypeSymbol* pointerType = type->RemovePointer(span);
    if (TypesEqual(arguments[1]->GetType(), pointerType->AddRvalueReference(span)) || arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference))
    {
        FunctionSymbol* function = functionMap[pointerType];
        if (!function)
        {
            function = new PointerMoveAssignment(pointerType, GetSymbolTable()->GetTypeByName(U"void"), span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[pointerType] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class PointerReturn : public FunctionSymbol
{
public:
    PointerReturn(TypeSymbol* type_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerReturn"; }
private:
    TypeSymbol* type;
};

PointerReturn::PointerReturn(TypeSymbol* type_, const Span& span) : FunctionSymbol(span, U"@return"), type(type_)
{
    SetGroupName(U"@return");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* valueParam = new ParameterSymbol(span, U"value");
    valueParam->SetType(type);
    AddMember(valueParam);
    SetReturnType(type);
    ComputeName();
}

void PointerReturn::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "return needs one object");
    genObjects[0]->Load(emitter, OperationFlags::none);
    if ((flags & OperationFlags::leaveFirstArg) != OperationFlags::none)
    {
        emitter.Stack().Dup();
        void* ptr = emitter.Stack().Pop();
        emitter.SaveObjectPointer(ptr);
    }
}

class PointerReturnOperation : public Operation
{
public:
    PointerReturnOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerReturnOperation::PointerReturnOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@return", 1, boundCompileUnit_)
{
}

void PointerReturnOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (!type->IsPointerType()) return;
    if (type->IsReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    FunctionSymbol* function = functionMap[type];
    if (!function)
    {
        function = new PointerReturn(type, span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[type] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class PointerPlusOffset : public FunctionSymbol
{
public:
    PointerPlusOffset(TypeSymbol* pointerType_, TypeSymbol* longType_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerPlusOffset"; }
};

PointerPlusOffset::PointerPlusOffset(TypeSymbol* pointerType_, TypeSymbol* longType_, const Span& span) : FunctionSymbol(span, U"operator+")
{
    SetGroupName(U"operator+");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* leftParam = new ParameterSymbol(span, U"left");
    leftParam->SetType(pointerType_);
    AddMember(leftParam);
    ParameterSymbol* rightParam = new ParameterSymbol(span, U"right");
    rightParam->SetType(longType_);
    AddMember(rightParam);
    SetReturnType(pointerType_);
    ComputeName();
}

void PointerPlusOffset::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "operator+ needs two objects");
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* left = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* right = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.ComputeAddress(left, right));
}

class PointerPlusOffsetOperation : public Operation
{
public:
    PointerPlusOffsetOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerPlusOffsetOperation::PointerPlusOffsetOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator+", 2, boundCompileUnit_)
{
}

void PointerPlusOffsetOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* leftType = arguments[0]->GetType();
    if (!leftType->IsPointerType()) return;
    TypeSymbol* rightType = arguments[1]->GetType();
    if (!rightType->PlainType(span)->IsIntegralType())
    {
        ArgumentMatch argumentMatch;
        if (!GetBoundCompileUnit().GetConversion(rightType, GetSymbolTable()->GetTypeByName(U"long"), containerScope, currentFunction, span, argumentMatch))
        {
            return;
        }
    }
    if (leftType->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(leftType->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    FunctionSymbol* function = functionMap[leftType];
    if (!function)
    {
        function = new PointerPlusOffset(leftType, GetSymbolTable()->GetTypeByName(U"long"), span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[leftType] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class OffsetPlusPointer : public FunctionSymbol
{
public:
    OffsetPlusPointer(TypeSymbol* longType_, TypeSymbol* pointerType_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "OffsetPlusPointer"; }
};

OffsetPlusPointer::OffsetPlusPointer(TypeSymbol* longType_, TypeSymbol* pointerType_, const Span& span) : FunctionSymbol(span, U"operator+")
{
    SetGroupName(U"operator+");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* leftParam = new ParameterSymbol(span, U"left");
    leftParam->SetType(longType_);
    AddMember(leftParam);
    ParameterSymbol* rightParam = new ParameterSymbol(span, U"right");
    rightParam->SetType(pointerType_);
    AddMember(rightParam);
    SetReturnType(pointerType_);
    ComputeName();
}

void OffsetPlusPointer::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "operator+ needs two objects");
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* left = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* right = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.ComputeAddress(right, left));
}

class OffsetPlusPointerOperation : public Operation
{
public:
    OffsetPlusPointerOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

OffsetPlusPointerOperation::OffsetPlusPointerOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator+", 2, boundCompileUnit_)
{
}

void OffsetPlusPointerOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* leftType = arguments[0]->GetType();
    if (!leftType->PlainType(span)->IsIntegralType())
    {
        ArgumentMatch argumentMatch;
        if (!GetBoundCompileUnit().GetConversion(leftType, GetSymbolTable()->GetTypeByName(U"long"), containerScope, currentFunction, span, argumentMatch))
        {
            return;
        }
    }
    TypeSymbol* rightType = arguments[1]->GetType();
    if (!rightType->IsPointerType()) return;
    TypeSymbol* longType = GetSymbolTable()->GetTypeByName(U"long");
    if (rightType->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(rightType->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    FunctionSymbol* function = functionMap[rightType];
    if (!function)
    {
        function = new OffsetPlusPointer(longType, rightType, span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[rightType] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class PointerMinusOffset : public FunctionSymbol
{
public:
    PointerMinusOffset(TypeSymbol* pointerType_, TypeSymbol* longType_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerMinusOffset"; }
};

PointerMinusOffset::PointerMinusOffset(TypeSymbol* pointerType_, TypeSymbol* longType_, const Span& span) : FunctionSymbol(span, U"operator-")
{
    SetGroupName(U"operator-");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* leftParam = new ParameterSymbol(span, U"left");
    leftParam->SetType(pointerType_);
    AddMember(leftParam);
    ParameterSymbol* rightParam = new ParameterSymbol(span, U"right");
    rightParam->SetType(longType_);
    AddMember(rightParam);
    SetReturnType(pointerType_);
    ComputeName();
}

void PointerMinusOffset::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "operator- needs two objects");
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* left = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* right = emitter.Stack().Pop();
    void* offset = emitter.CreateNeg(right);
    emitter.Stack().Push(emitter.ComputeAddress(left, offset));
}

class PointerMinusOffsetOperation : public Operation
{
public:
    PointerMinusOffsetOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerMinusOffsetOperation::PointerMinusOffsetOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator-", 2, boundCompileUnit_)
{
}

void PointerMinusOffsetOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* leftType = arguments[0]->GetType();
    if (!leftType->IsPointerType()) return;
    TypeSymbol* rightType = arguments[1]->GetType();
    if (!rightType->PlainType(span)->IsIntegralType())
    {
        ArgumentMatch argumentMatch;
        if (!GetBoundCompileUnit().GetConversion(rightType, GetSymbolTable()->GetTypeByName(U"long"), containerScope, currentFunction, span, argumentMatch))
        {
            return;
        }
    }
    if (leftType->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(leftType->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    FunctionSymbol* function = functionMap[leftType];
    if (!function)
    {
        function = new PointerMinusOffset(leftType, GetSymbolTable()->GetTypeByName(U"long"), span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[leftType] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class PointerMinusPointer : public FunctionSymbol
{
public:
    PointerMinusPointer(TypeSymbol* pointerType_, TypeSymbol* longType_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerMinusPointer"; }
};

PointerMinusPointer::PointerMinusPointer(TypeSymbol* pointerType_, TypeSymbol* longType_, const Span& span) : FunctionSymbol(span, U"operator-")
{
    SetGroupName(U"operator-");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* leftParam = new ParameterSymbol(span, U"left");
    leftParam->SetType(pointerType_);
    AddMember(leftParam);
    ParameterSymbol* rightParam = new ParameterSymbol(span, U"right");
    rightParam->SetType(pointerType_);
    AddMember(rightParam);
    SetReturnType(longType_);
    ComputeName();
}

void PointerMinusPointer::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "operator- needs two objects");
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* left = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* right = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreatePtrDiff(left, right));
}

class PointerMinusPointerOperation : public Operation
{
public:
    PointerMinusPointerOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerMinusPointerOperation::PointerMinusPointerOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator-", 2, boundCompileUnit_)
{
}

void PointerMinusPointerOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* leftType = arguments[0]->GetType();
    if (!leftType->IsPointerType()) return;
    TypeSymbol* rightType = arguments[1]->GetType();
    if (!rightType->IsPointerType()) return;
    if (leftType->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(leftType->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    FunctionSymbol* function = functionMap[leftType];
    if (!function)
    {
        function = new PointerMinusPointer(leftType, GetSymbolTable()->GetTypeByName(U"long"), span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[leftType] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class PointerEqual : public FunctionSymbol
{
public:
    PointerEqual(TypeSymbol* pointerType_, TypeSymbol* boolType_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerEqual"; }
};

PointerEqual::PointerEqual(TypeSymbol* pointerType_, TypeSymbol* boolType_, const Span& span) : FunctionSymbol(span, U"operator==")
{
    SetGroupName(U"operator==");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* leftParam = new ParameterSymbol(span, U"left");
    leftParam->SetType(pointerType_);
    AddMember(leftParam);
    ParameterSymbol* rightParam = new ParameterSymbol(span, U"right");
    rightParam->SetType(pointerType_);
    AddMember(rightParam);
    SetReturnType(boolType_);
    ComputeName();
}

void PointerEqual::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "operator== needs two objects");
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* left = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* right = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateICmpEQ(left, right));
}

class PointerEqualOperation : public Operation
{
public:
    PointerEqualOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerEqualOperation::PointerEqualOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator==", 2, boundCompileUnit_)
{
}

void PointerEqualOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* leftType = arguments[0]->GetType();
    if (!leftType->IsPointerType()) return;
    TypeSymbol* rightType = arguments[1]->GetType();
    if (!rightType->IsPointerType()) return;
    if (leftType->IsReferenceType())
    {
        leftType = leftType->PlainType(span);
    }
    if (leftType->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(leftType->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    FunctionSymbol* function = functionMap[leftType];
    if (!function)
    {
        function = new PointerEqual(leftType, GetSymbolTable()->GetTypeByName(U"bool"), span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[leftType] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class PointerLess : public FunctionSymbol
{
public:
    PointerLess(TypeSymbol* pointerType_, TypeSymbol* boolType_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerLess"; }
};

PointerLess::PointerLess(TypeSymbol* pointerType_, TypeSymbol* boolType_, const Span& span) : FunctionSymbol(span, U"operator<")
{
    SetGroupName(U"operator<");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* leftParam = new ParameterSymbol(span, U"left");
    leftParam->SetType(pointerType_);
    AddMember(leftParam);
    ParameterSymbol* rightParam = new ParameterSymbol(span, U"right");
    rightParam->SetType(pointerType_);
    AddMember(rightParam);
    SetReturnType(boolType_);
    ComputeName();
}

void PointerLess::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "operator< needs two objects");
    genObjects[0]->Load(emitter, OperationFlags::none);
    void* left = emitter.Stack().Pop();
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* right = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateICmpULT(left, right));
}

class PointerLessOperation : public Operation
{
public:
    PointerLessOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerLessOperation::PointerLessOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator<", 2, boundCompileUnit_)
{
}

void PointerLessOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* leftType = arguments[0]->GetType();
    if (!leftType->IsPointerType()) return;
    TypeSymbol* rightType = arguments[1]->GetType();
    if (!rightType->IsPointerType()) return;
    if (leftType->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(leftType->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    FunctionSymbol* function = functionMap[leftType];
    if (!function)
    {
        function = new PointerLess(leftType, GetSymbolTable()->GetTypeByName(U"bool"), span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[leftType] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class PointerArrow : public FunctionSymbol
{
public:
    PointerArrow(TypeSymbol* type_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "PointerArrow"; }
private:
    TypeSymbol* type;
};

PointerArrow::PointerArrow(TypeSymbol* type_, const Span& span) : FunctionSymbol(span, U"operator->"), type(type_)
{
    SetGroupName(U"operator->");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* operandParam = new ParameterSymbol(span, U"operand");
    operandParam->SetType(type->AddPointer(span));
    AddMember(operandParam);
    SetReturnType(type);
    ComputeName();
}

void PointerArrow::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "return needs one object");
    genObjects[0]->Load(emitter, OperationFlags::none);
}

class PointerArrowOperation : public Operation
{
public:
    PointerArrowOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

PointerArrowOperation::PointerArrowOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator->", 1, boundCompileUnit_)
{
}

void PointerArrowOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() <= 1) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    TypeSymbol* pointerType = type->RemovePointer(span);
    FunctionSymbol* function = functionMap[pointerType];
    if (!function)
    {
        function = new PointerArrow(pointerType, span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[pointerType] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class LvalueRefefenceCopyCtor : public FunctionSymbol
{
public:
    LvalueRefefenceCopyCtor(TypeSymbol* type_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "LvalueRefefenceCopyCtor"; }
private:
    TypeSymbol* type;
};

LvalueRefefenceCopyCtor::LvalueRefefenceCopyCtor(TypeSymbol* type_, const Span& span) : FunctionSymbol(span, U"@constructor"), type(type_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span, U"this");
    thisParam->SetType(type->AddPointer(span));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span, U"that");
    thatParam->SetType(type);
    AddMember(thatParam);
    ComputeName();
}

void LvalueRefefenceCopyCtor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "reference copy constructor needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    genObjects[0]->Store(emitter, OperationFlags::none);
}

class LvalueReferenceCopyConstructorOperation : public Operation
{
public:
    LvalueReferenceCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

LvalueReferenceCopyConstructorOperation::LvalueReferenceCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 2, boundCompileUnit_)
{
}

void LvalueReferenceCopyConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() < 1 || !type->IsLvalueReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    TypeSymbol* lvalueRefType = type->RemovePointer(span);
    FunctionSymbol* function = functionMap[lvalueRefType];
    if (!function)
    {
        function = new LvalueRefefenceCopyCtor(lvalueRefType, span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[lvalueRefType] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class LvalueReferenceCopyAssignment : public FunctionSymbol
{
public:
    LvalueReferenceCopyAssignment(TypeSymbol* type_, TypeSymbol* voidType_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    bool IsLvalueReferenceCopyAssignment() const { return true; }
    const char* ClassName() const override { return "LvalueReferenceCopyAssignment"; }
private:
    TypeSymbol* type;
};

LvalueReferenceCopyAssignment::LvalueReferenceCopyAssignment(TypeSymbol* type_, TypeSymbol* voidType_, const Span& span) : FunctionSymbol(span, U"operator="), type(type_)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span, U"this");
    thisParam->SetType(type->AddPointer(span));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span, U"that");
    thatParam->SetType(type);
    AddMember(thatParam);
    SetReturnType(voidType_);
    ComputeName();
}

void LvalueReferenceCopyAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "copy assignment needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    genObjects[0]->Store(emitter, OperationFlags::none);
}

class LvalueReferenceCopyAssignmentOperation : public Operation
{
public:
    LvalueReferenceCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

LvalueReferenceCopyAssignmentOperation::LvalueReferenceCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator=", 2, boundCompileUnit_)
{
}

void LvalueReferenceCopyAssignmentOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() < 1 || !type->IsLvalueReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    TypeSymbol* lvalueRefType = type->RemovePointer(span);
    if (lvalueRefType->PlainType(span)->IsClassTypeSymbol()) return;
    if (!TypesEqual(arguments[1]->GetType()->RemoveConst(span), lvalueRefType->PlainType(span)->AddRvalueReference(span)) && !arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference))
    {
        FunctionSymbol* function = functionMap[lvalueRefType];
        if (!function)
        {
            function = new LvalueReferenceCopyAssignment(lvalueRefType, GetSymbolTable()->GetTypeByName(U"void"), span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[lvalueRefType] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class LvalueReferenceMoveAssignment : public FunctionSymbol
{
public:
    LvalueReferenceMoveAssignment(TypeSymbol* type_, TypeSymbol* voidType_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "LvalueReferenceMoveAssignment"; }
private:
    TypeSymbol* type;
};

LvalueReferenceMoveAssignment::LvalueReferenceMoveAssignment(TypeSymbol* type_, TypeSymbol* voidType_, const Span& span) : FunctionSymbol(span, U"operator="), type(type_)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span, U"this");
    thisParam->SetType(type->AddPointer(span));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span, U"that");
    thatParam->SetType(type->RemoveReference(span)->AddRvalueReference(span));
    AddMember(thatParam);
    SetReturnType(voidType_);
    ComputeName();
}

void LvalueReferenceMoveAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "copy assignment needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* rvalueRefValue = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateLoad(rvalueRefValue));
    genObjects[0]->Store(emitter, OperationFlags::none);
}

class LvalueReferenceMoveAssignmentOperation : public Operation
{
public:
    LvalueReferenceMoveAssignmentOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

LvalueReferenceMoveAssignmentOperation::LvalueReferenceMoveAssignmentOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator=", 2, boundCompileUnit_)
{
}

void LvalueReferenceMoveAssignmentOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() < 1 || !type->IsLvalueReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    TypeSymbol* lvalueRefType = type->RemovePointer(span);
    if (lvalueRefType->PlainType(span)->IsClassTypeSymbol()) return;
    if (TypesEqual(arguments[1]->GetType()->RemoveConst(span), lvalueRefType->PlainType(span)->AddRvalueReference(span)) || arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference))
    {
        FunctionSymbol* function = functionMap[lvalueRefType];
        if (!function)
        {
            function = new LvalueReferenceMoveAssignment(lvalueRefType, GetSymbolTable()->GetTypeByName(U"void"), span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[lvalueRefType] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class LvalueReferenceReturn : public FunctionSymbol
{
public:
    LvalueReferenceReturn(TypeSymbol* type_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "LvalueReferenceReturn"; }
private:
    TypeSymbol* type;
};

LvalueReferenceReturn::LvalueReferenceReturn(TypeSymbol* type_, const Span& span) : FunctionSymbol(span, U"@return"), type(type_)
{
    SetGroupName(U"@return");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* valueParam = new ParameterSymbol(span, U"value");
    valueParam->SetType(type);
    AddMember(valueParam);
    SetReturnType(type);
    ComputeName();
}

void LvalueReferenceReturn::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "return needs one object");
    genObjects[0]->Load(emitter, OperationFlags::none);
}

class LvalueReferenceReturnOperation : public Operation
{
public:
    LvalueReferenceReturnOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

LvalueReferenceReturnOperation::LvalueReferenceReturnOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@return", 1, boundCompileUnit_)
{
}

void LvalueReferenceReturnOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (!type->IsLvalueReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    FunctionSymbol* function = functionMap[type];
    if (!function)
    {
        function = new LvalueReferenceReturn(type, span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[type] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class RvalueRefefenceCopyCtor : public FunctionSymbol
{
public:
    RvalueRefefenceCopyCtor(TypeSymbol* type_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "RvalueRefefenceCopyCtor"; }
private:
    TypeSymbol* type;
};

RvalueRefefenceCopyCtor::RvalueRefefenceCopyCtor(TypeSymbol* type_, const Span& span) : FunctionSymbol(span, U"@constructor"), type(type_)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span, U"this");
    thisParam->SetType(type->AddPointer(span));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span, U"that");
    thatParam->SetType(type);
    AddMember(thatParam);
    ComputeName();
}

void RvalueRefefenceCopyCtor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "reference copy constructor needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    genObjects[0]->Store(emitter, OperationFlags::none);
}

class RvalueReferenceCopyConstructorOperation : public Operation
{
public:
    RvalueReferenceCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

RvalueReferenceCopyConstructorOperation::RvalueReferenceCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 2, boundCompileUnit_)
{
}

void RvalueReferenceCopyConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() < 1 || !type->IsRvalueReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    TypeSymbol* rvalueRefType = type->RemovePointer(span);
    if (rvalueRefType->PlainType(span)->IsClassTypeSymbol()) return;
    FunctionSymbol* function = functionMap[rvalueRefType];
    if (!function)
    {
        function = new RvalueRefefenceCopyCtor(rvalueRefType, span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[rvalueRefType] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class RvalueReferenceCopyAssignment : public FunctionSymbol
{
public:
    RvalueReferenceCopyAssignment(TypeSymbol* type_, TypeSymbol* voidType_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "RvalueReferenceCopyAssignment"; }
private:
    TypeSymbol* type;
};

RvalueReferenceCopyAssignment::RvalueReferenceCopyAssignment(TypeSymbol* type_, TypeSymbol* voidType_, const Span& span) : FunctionSymbol(span, U"operator="), type(type_)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(span, U"this");
    thisParam->SetType(type->AddPointer(span));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span, U"that");
    thatParam->SetType(type);
    AddMember(thatParam);
    SetReturnType(voidType_);
    ComputeName();
}

void RvalueReferenceCopyAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 2, "copy assignment needs two objects");
    genObjects[1]->Load(emitter, OperationFlags::none);
    void* rvalueRefValue = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateLoad(rvalueRefValue));
    genObjects[0]->Store(emitter, OperationFlags::none);
}

class RvalueReferenceCopyAssignmentOperation : public Operation
{
public:
    RvalueReferenceCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

RvalueReferenceCopyAssignmentOperation::RvalueReferenceCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator=", 2, boundCompileUnit_)
{
}

void RvalueReferenceCopyAssignmentOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() < 1 || !type->IsRvalueReferenceType()) return;
    TypeSymbol* rvalueRefType = type->RemovePointer(span);
    if (rvalueRefType->PlainType(span)->IsClassTypeSymbol()) return;
    if (rvalueRefType->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(rvalueRefType->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    FunctionSymbol* function = functionMap[rvalueRefType];
    if (!function)
    {
        function = new RvalueReferenceCopyAssignment(rvalueRefType, GetSymbolTable()->GetTypeByName(U"void"), span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[rvalueRefType] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class RvalueReferenceReturn : public FunctionSymbol
{
public:
    RvalueReferenceReturn(TypeSymbol* type_, const Span& span);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span) override;
    bool IsBasicTypeOperation() const override { return true; }
    const char* ClassName() const override { return "RvalueReferenceReturn"; }
private:
    TypeSymbol* type;
};

RvalueReferenceReturn::RvalueReferenceReturn(TypeSymbol* type_, const Span& span) : FunctionSymbol(span, U"@return"), type(type_)
{
    SetGroupName(U"@return");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* valueParam = new ParameterSymbol(span, U"value");
    valueParam->SetType(type);
    AddMember(valueParam);
    SetReturnType(type);
    ComputeName();
}

void RvalueReferenceReturn::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    Assert(genObjects.size() == 1, "return needs one object");
    genObjects[0]->Load(emitter, OperationFlags::none);
}

class RvalueReferenceReturnOperation : public Operation
{
public:
    RvalueReferenceReturnOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<TypeSymbol*, FunctionSymbol*> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

RvalueReferenceReturnOperation::RvalueReferenceReturnOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@return", 1, boundCompileUnit_)
{
}

void RvalueReferenceReturnOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (!type->IsRvalueReferenceType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    FunctionSymbol* function = functionMap[type];
    if (!function)
    {
        function = new RvalueReferenceReturn(type, span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[type] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class ArrayDefaultConstructorOperation : public Operation
{
public:
    ArrayDefaultConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

ArrayDefaultConstructorOperation::ArrayDefaultConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 1, boundCompileUnit_)
{
}

void ArrayDefaultConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || !type->RemovePointer(span)->IsArrayType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    ArrayTypeSymbol* arrayType = static_cast<ArrayTypeSymbol*>(type->RemovePointer(span));
    FunctionSymbol* function = functionMap[arrayType->TypeId()];
    if (!function)
    { 
        std::vector<FunctionScopeLookup> elementLookups;
        elementLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
        elementLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, arrayType->ElementType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
        elementLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
        std::vector<std::unique_ptr<BoundExpression>> elementArguments;
        elementArguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(GetModule(), span, arrayType->ElementType()->AddPointer(span))));
        std::unique_ptr<BoundFunctionCall> elementDefaultConstructor = ResolveOverload(U"@constructor", containerScope, elementLookups, elementArguments, GetBoundCompileUnit(), currentFunction, span);
        FunctionSymbol* elementTypeDefaultConstructor = elementDefaultConstructor->GetFunctionSymbol();
        function = new ArrayTypeDefaultConstructor(arrayType, elementTypeDefaultConstructor, span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[arrayType->TypeId()] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class ArrayCopyConstructorOperation : public Operation
{
public:
    ArrayCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

ArrayCopyConstructorOperation::ArrayCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 2, boundCompileUnit_)
{
}

void ArrayCopyConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || !type->RemovePointer(span)->IsArrayType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    ArrayTypeSymbol* arrayType = static_cast<ArrayTypeSymbol*>(type->RemovePointer(span));
    if (!TypesEqual(arguments[1]->GetType(), arrayType->AddRvalueReference(span)) && !arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference) &&
        TypesEqual(arguments[1]->GetType()->PlainType(span), arrayType))
    {
        FunctionSymbol* function = functionMap[arrayType->TypeId()];
        if (!function)
        {
            std::vector<FunctionScopeLookup> elementLookups;
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, arrayType->ElementType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> elementArguments;
            elementArguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(GetModule(), span, arrayType->ElementType()->AddPointer(span))));
            elementArguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(GetModule(), span, arrayType->ElementType()->AddConst(span)->AddLvalueReference(span))));
            std::unique_ptr<BoundFunctionCall> elementCopyConstructor = ResolveOverload(U"@constructor", containerScope, elementLookups, elementArguments, GetBoundCompileUnit(), currentFunction, span);
            FunctionSymbol* elementTypeCopyConstructor = elementCopyConstructor->GetFunctionSymbol();
            function = new ArrayTypeCopyConstructor(arrayType, elementTypeCopyConstructor, span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[arrayType->TypeId()] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class ArrayMoveConstructorOperation : public Operation
{
public:
    ArrayMoveConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

ArrayMoveConstructorOperation::ArrayMoveConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 2, boundCompileUnit_)
{
}

void ArrayMoveConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || !type->RemovePointer(span)->IsArrayType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    ArrayTypeSymbol* arrayType = static_cast<ArrayTypeSymbol*>(type->RemovePointer(span));
    if (TypesEqual(arguments[1]->GetType(), arrayType->AddRvalueReference(span)) || arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference))
    {
        FunctionSymbol* function = functionMap[arrayType->TypeId()];
        if (!function)
        {
            std::vector<FunctionScopeLookup> elementLookups;
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, arrayType->ElementType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> elementArguments;
            elementArguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(GetModule(), span, arrayType->ElementType()->AddPointer(span))));
            elementArguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(GetModule(), span, arrayType->ElementType()->AddRvalueReference(span))));
            elementArguments.back()->SetFlag(BoundExpressionFlags::bindToRvalueReference);
            std::unique_ptr<BoundFunctionCall> elementMoveConstructor = ResolveOverload(U"@constructor", containerScope, elementLookups, elementArguments, GetBoundCompileUnit(), currentFunction, span);
            FunctionSymbol* elementTypeMoveConstructor = elementMoveConstructor->GetFunctionSymbol();
            function = new ArrayTypeMoveConstructor(arrayType, elementTypeMoveConstructor, span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[arrayType->TypeId()] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class ArrayCopyAssignmentOperation : public Operation
{
public:
    ArrayCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

ArrayCopyAssignmentOperation::ArrayCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator=", 2, boundCompileUnit_)
{
}

void ArrayCopyAssignmentOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || !type->RemovePointer(span)->IsArrayType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    ArrayTypeSymbol* arrayType = static_cast<ArrayTypeSymbol*>(type->RemovePointer(span));
    if (!TypesEqual(arguments[1]->GetType(), arrayType->AddRvalueReference(span)) && !arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference) &&
        TypesEqual(arguments[1]->GetType()->PlainType(span), arrayType))
    {
        FunctionSymbol* function = functionMap[arrayType->TypeId()];
        if (!function)
        {
            std::vector<FunctionScopeLookup> elementLookups;
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, arrayType->ElementType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> elementArguments;
            elementArguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(GetModule(), span, arrayType->ElementType()->AddPointer(span))));
            elementArguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(GetModule(), span, arrayType->ElementType()->AddConst(span)->AddLvalueReference(span))));
            std::unique_ptr<BoundFunctionCall> elementCopyAssignment = ResolveOverload(U"operator=", containerScope, elementLookups, elementArguments, GetBoundCompileUnit(), currentFunction, span);
            FunctionSymbol* elementTypeCopyAssignment = elementCopyAssignment->GetFunctionSymbol();
            function = new ArrayTypeCopyAssignment(arrayType, elementTypeCopyAssignment, span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[arrayType->TypeId()] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class ArrayMoveAssignmentOperation : public Operation
{
public:
    ArrayMoveAssignmentOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

ArrayMoveAssignmentOperation::ArrayMoveAssignmentOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator=", 2, boundCompileUnit_)
{
}

void ArrayMoveAssignmentOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || !type->RemovePointer(span)->IsArrayType()) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    ArrayTypeSymbol* arrayType = static_cast<ArrayTypeSymbol*>(type->RemovePointer(span));
    if (TypesEqual(arguments[1]->GetType(), arrayType->AddRvalueReference(span)) || arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference))
    {
        FunctionSymbol* function = functionMap[arrayType->TypeId()];
        if (!function)
        {
            std::vector<FunctionScopeLookup> elementLookups;
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, arrayType->ElementType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
            elementLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> elementArguments;
            elementArguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(GetModule(), span, arrayType->ElementType()->AddPointer(span))));
            elementArguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(GetModule(), span, arrayType->ElementType()->AddRvalueReference(span))));
            elementArguments.back()->SetFlag(BoundExpressionFlags::bindToRvalueReference);
            std::unique_ptr<BoundFunctionCall> elementMoveAssignment = ResolveOverload(U"operator=", containerScope, elementLookups, elementArguments, GetBoundCompileUnit(), currentFunction, span);
            FunctionSymbol* elementTypeMoveAssignment = elementMoveAssignment->GetFunctionSymbol();
            function = new ArrayTypeMoveAssignment(arrayType, elementTypeMoveAssignment, span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[arrayType->TypeId()] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class ArrayElementAccessOperation : public Operation
{
public:
    ArrayElementAccessOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

ArrayElementAccessOperation::ArrayElementAccessOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator[]", 2, boundCompileUnit_)
{
}

void ArrayElementAccessOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* leftType = arguments[0]->GetType();
    if (!leftType->PlainType(span)->IsArrayType()) return;
    if (leftType->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(leftType->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    ArrayTypeSymbol* arrayType = static_cast<ArrayTypeSymbol*>(leftType->PlainType(span));
    TypeSymbol* rightType = arguments[1]->GetType();
    if (!rightType->PlainType(span)->IsIntegralType())
    {
        ArgumentMatch argumentMatch;
        if (!GetBoundCompileUnit().GetConversion(rightType, GetSymbolTable()->GetTypeByName(U"long"), containerScope, currentFunction, span, argumentMatch))
        {
            return;
        }
    }
    FunctionSymbol* function = functionMap[arrayType->TypeId()];
    if (!function)
    {
        function = new ArrayTypeElementAccess(arrayType, span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[arrayType->TypeId()] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class InterfaceDefaultConstructorOperation : public Operation
{
public:
    InterfaceDefaultConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

InterfaceDefaultConstructorOperation::InterfaceDefaultConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 1, boundCompileUnit_)
{
}

void InterfaceDefaultConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || type->RemovePointer(span)->GetSymbolType() != SymbolType::interfaceTypeSymbol) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    InterfaceTypeSymbol* interfaceType = static_cast<InterfaceTypeSymbol*>(type->RemovePointer(span));
    FunctionSymbol* function = functionMap[interfaceType->TypeId()];
    if (!function)
    {
        function = new InterfaceTypeDefaultConstructor(interfaceType, span);
        function->SetModule(GetModule());
        function->SetParent(&GetSymbolTable()->GlobalNs());
        functionMap[interfaceType->TypeId()] = function;
        functions.push_back(std::unique_ptr<FunctionSymbol>(function));
    }
    viableFunctions.Insert(function);
}

class InterfaceCopyConstructorOperation : public Operation
{
public:
    InterfaceCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

InterfaceCopyConstructorOperation::InterfaceCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 2, boundCompileUnit_)
{
}

void InterfaceCopyConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || type->RemovePointer(span)->GetSymbolType() != SymbolType::interfaceTypeSymbol) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    InterfaceTypeSymbol* interfaceType = static_cast<InterfaceTypeSymbol*>(type->RemovePointer(span));
    if (!TypesEqual(arguments[1]->GetType(), interfaceType->AddRvalueReference(span)) && !arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference) &&
        (TypesEqual(arguments[1]->GetType()->PlainType(span), interfaceType) || arguments[1]->GetType()->PlainType(span)->IsClassTypeSymbol()))
    {
        if (GetBoundCompileUnit().HasCopyConstructorFor(interfaceType->TypeId()))
        {
            viableFunctions.Insert(GetBoundCompileUnit().GetCopyConstructorFor(interfaceType->TypeId()));
            return;
        }
        FunctionSymbol* function = functionMap[interfaceType->TypeId()];
        if (!function)
        {
            function = new InterfaceTypeCopyConstructor(interfaceType, span);
            GetBoundCompileUnit().AddCopyConstructorToMap(interfaceType->TypeId(), function);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[interfaceType->TypeId()] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class InterfaceMoveConstructorOperation : public Operation
{
public:
    InterfaceMoveConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

InterfaceMoveConstructorOperation::InterfaceMoveConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 2, boundCompileUnit_)
{
}

void InterfaceMoveConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || type->RemovePointer(span)->GetSymbolType() != SymbolType::interfaceTypeSymbol) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    InterfaceTypeSymbol* interfaceType = static_cast<InterfaceTypeSymbol*>(type->RemovePointer(span));
    if (TypesEqual(arguments[1]->GetType(), interfaceType->AddRvalueReference(span)) || arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference))
    {
        FunctionSymbol* function = functionMap[interfaceType->TypeId()];
        if (!function)
        {
            function = new InterfaceTypeMoveConstructor(interfaceType, span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[interfaceType->TypeId()] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class InterfaceCopyAssignmentOperation : public Operation
{
public:
    InterfaceCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

InterfaceCopyAssignmentOperation::InterfaceCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator=", 2, boundCompileUnit_)
{
}

void InterfaceCopyAssignmentOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || type->RemovePointer(span)->GetSymbolType() != SymbolType::interfaceTypeSymbol) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    InterfaceTypeSymbol* interfaceType = static_cast<InterfaceTypeSymbol*>(type->RemovePointer(span));
    if (!TypesEqual(arguments[1]->GetType(), interfaceType->AddRvalueReference(span)) && !arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference) &&
        TypesEqual(arguments[1]->GetType()->PlainType(span), interfaceType))
    {
        FunctionSymbol* function = functionMap[interfaceType->TypeId()];
        if (!function)
        {
            function = new InterfaceTypeCopyAssignment(interfaceType, span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[interfaceType->TypeId()] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class InterfaceMoveAssignmentOperation : public Operation
{
public:
    InterfaceMoveAssignmentOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

InterfaceMoveAssignmentOperation::InterfaceMoveAssignmentOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator=", 2, boundCompileUnit_)
{
}

void InterfaceMoveAssignmentOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || type->RemovePointer(span)->GetSymbolType() != SymbolType::interfaceTypeSymbol) return;
    if (type->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type->BaseType());
        GetBoundCompileUnit().FinalizeBinding(specialization);
    }
    InterfaceTypeSymbol* interfaceType = static_cast<InterfaceTypeSymbol*>(type->RemovePointer(span));
    if (TypesEqual(arguments[1]->GetType(), interfaceType->AddRvalueReference(span)) || arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference))
    {
        FunctionSymbol* function = functionMap[interfaceType->TypeId()];
        if (!function)
        {
            function = new InterfaceTypeMoveAssignment(interfaceType, span);
            function->SetModule(GetModule());
            function->SetParent(&GetSymbolTable()->GlobalNs());
            functionMap[interfaceType->TypeId()] = function;
            functions.push_back(std::unique_ptr<FunctionSymbol>(function));
        }
        viableFunctions.Insert(function);
    }
}

class ClassDefaultConstructor : public ConstructorSymbol
{
public:
    ClassDefaultConstructor(ClassTypeSymbol* classType_, const Span& span_);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    bool IsGeneratedFunction() const override { return true; }
    ClassTypeSymbol* ClassType() { return classType; }
    const char* ClassName() const override { return "ClassDefaultConstructor"; }
private:
    ClassTypeSymbol* classType;
};

ClassDefaultConstructor::ClassDefaultConstructor(ClassTypeSymbol* classType_, const Span& span_) :
    ConstructorSymbol(classType_->GetSpan(), U"@constructor"), classType(classType_)
{
    SetAccess(SymbolAccess::public_);
    SetParent(classType);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(classType->AddPointer(span_));
    AddMember(thisParam);
    ComputeName();
}

class ClassDefaultConstructorOperation : public Operation
{
public:
    ClassDefaultConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
    bool GenerateImplementation(ClassDefaultConstructor* defaultConstructor, ContainerScope* containerScope, BoundFunction* currentFunction, std::unique_ptr<Exception>& exception, const Span& span);
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

ClassDefaultConstructorOperation::ClassDefaultConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 1, boundCompileUnit_)
{
}

void ClassDefaultConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || !type->RemovePointer(span)->PlainType(span)->IsClassTypeSymbol()) return;
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type->BaseType());
    if (classType->IsStatic())
    {
        exception.reset(new Exception(GetModule(), "cannot create an instance of a static class", span, classType->GetSpan()));
        return;
    }
    if (classType->DefaultConstructor())
    {
        viableFunctions.Insert(classType->DefaultConstructor());
        return;
    }
    if (classType->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        GetBoundCompileUnit().FinalizeBinding(static_cast<ClassTemplateSpecializationSymbol*>(classType));
    }
    FunctionSymbol* function = functionMap[classType->TypeId()];
    if (!function)
    {
        std::unique_ptr<ClassDefaultConstructor> defaultConstructor(new ClassDefaultConstructor(classType, span));
        function = defaultConstructor.get();
        function->SetModule(GetModule());
        function->SetParent(classType);
        function->SetLinkOnceOdrLinkage();
        function->SetInline();
        functionMap[classType->TypeId()] = function;
        defaultConstructor->SetCompileUnit(GetBoundCompileUnit().GetCompileUnitNode());
        defaultConstructor->SetModule(GetModule());
        if (GenerateImplementation(defaultConstructor.get(), containerScope, currentFunction, exception, span))
        {
            functions.push_back(std::unique_ptr<FunctionSymbol>(defaultConstructor.release()));
        }
        else
        {
            functionMap[classType->TypeId()] = nullptr;
            return;
        }
    }
    viableFunctions.Insert(function);
}

bool ClassDefaultConstructorOperation::GenerateImplementation(ClassDefaultConstructor* defaultConstructor, ContainerScope* containerScope, BoundFunction* currentFunction, 
    std::unique_ptr<Exception>& exception, const Span& span)
{
    ClassTypeSymbol* classType = defaultConstructor->ClassType();
    try
    {
        bool nothrow = true;
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(GetModule(), defaultConstructor));
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(new BoundCompoundStatement(GetModule(), span)));
        if (classType->StaticConstructor())
        {
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::unique_ptr<BoundExpression>(new BoundFunctionCall(GetModule(), span,
                classType->StaticConstructor())))));
            if (!classType->StaticConstructor()->DontThrow()) nothrow = false;
        }
        if (classType->BaseClass())
        {
            std::vector<FunctionScopeLookup> baseConstructorCallLookups;
            baseConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            baseConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->BaseClass()->GetContainerScope()));
            baseConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> baseConstructorCallArguments;
            ParameterSymbol* thisParam = defaultConstructor->Parameters()[0];
            ArgumentMatch argumentMatch;
            FunctionSymbol* thisToBaseConversion = GetBoundCompileUnit().GetConversion(thisParam->GetType(), classType->BaseClass()->AddPointer(span), containerScope, currentFunction, span, argumentMatch);
            if (!thisToBaseConversion)
            {
                throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
            }
            BoundExpression* baseClassPointerConversion = new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thisParam)), thisToBaseConversion);
            baseConstructorCallArguments.push_back(std::unique_ptr<BoundExpression>(baseClassPointerConversion));
            std::unique_ptr<BoundFunctionCall> baseConstructorCall = ResolveOverload(U"@constructor", containerScope, baseConstructorCallLookups, baseConstructorCallArguments, GetBoundCompileUnit(),
                boundFunction.get(), span);
            if (!baseConstructorCall->GetFunctionSymbol()->DontThrow()) nothrow = false;
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::move(baseConstructorCall))));
        }
        if (classType->IsPolymorphic())
        {
            ParameterSymbol* thisParam = defaultConstructor->Parameters()[0];
            BoundExpression* classPtr = nullptr;
            ClassTypeSymbol* vmtPtrHolderClass = classType->VmtPtrHolderClass();
            if (vmtPtrHolderClass == classType)
            {
                classPtr = new BoundParameter(GetModule(), span, thisParam);
            }
            else
            {
                ArgumentMatch argumentMatch;
                FunctionSymbol* thisToHolderConversion = GetBoundCompileUnit().GetConversion(thisParam->GetType(), vmtPtrHolderClass->AddPointer(span), containerScope, currentFunction, span, argumentMatch);
                if (!thisToHolderConversion)
                {
                    throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
                }
                classPtr = new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thisParam)), thisToHolderConversion);
            }
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundSetVmtPtrStatement(GetModule(), std::unique_ptr<BoundExpression>(classPtr), classType)));
        }
        int n = classType->MemberVariables().size();
        for (int i = 0; i < n; ++i)
        {
            MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
            std::vector<FunctionScopeLookup> memberConstructorCallLookups;
            memberConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            memberConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
            memberConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> memberConstructorCallArguments;
            BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(GetModule(), span, memberVariableSymbol);
            boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, defaultConstructor->GetThisParam())));
            memberConstructorCallArguments.push_back(std::unique_ptr<BoundExpression>(
                new BoundAddressOfExpression(GetModule(), std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span))));
            std::unique_ptr<BoundFunctionCall> memberConstructorCall = ResolveOverload(U"@constructor", containerScope, memberConstructorCallLookups, memberConstructorCallArguments,
                GetBoundCompileUnit(), boundFunction.get(), span);
            if (!memberConstructorCall->GetFunctionSymbol()->DontThrow()) nothrow = false;
            boundFunction->MoveTemporaryDestructorCallsTo(*memberConstructorCall);
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::move(memberConstructorCall))));
        }
        GetBoundCompileUnit().AddBoundNode(std::move(boundFunction));
        if (nothrow)
        {
            defaultConstructor->SetNothrow();
        }
    }
    catch (const Exception& ex)
    {
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        exception.reset(new Exception(GetModule(), "cannot create default constructor for class '" + ToUtf8(classType->FullName()) + "'. Reason: " + ex.Message(), span, references));
        return false;
    }
    return  true;
}

class ClassCopyConstructor : public ConstructorSymbol
{
public:
    ClassCopyConstructor(ClassTypeSymbol* classType_, const Span& span_);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    bool IsGeneratedFunction() const override { return true; }
    ClassTypeSymbol* ClassType() { return classType; }
    const char* ClassName() const override { return "ClassCopyConstructor"; }
private:
    ClassTypeSymbol* classType;
};

ClassCopyConstructor::ClassCopyConstructor(ClassTypeSymbol* classType_, const Span& span_) :
    ConstructorSymbol(classType_->GetSpan(), U"@constructor"), classType(classType_)
{
    SetAccess(SymbolAccess::public_);
    SetParent(classType);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(classType->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(classType->AddConst(span_)->AddLvalueReference(span_));
    AddMember(thatParam);
    ComputeName();
}

class ClassCopyConstructorOperation : public Operation
{
public:
    ClassCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
    bool GenerateImplementation(ClassCopyConstructor* copyConstructor, ContainerScope* containerScope, BoundFunction* currentFunction, std::unique_ptr<Exception>& exception, const Span& span);
    void AddFunction(std::unique_ptr<FunctionSymbol>&& function) { functions.push_back(std::move(function)); }
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

ClassCopyConstructorOperation::ClassCopyConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 2, boundCompileUnit_)
{
}

void ClassCopyConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || !type->RemovePointer(span)->PlainType(span)->IsClassTypeSymbol()) return;
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type->BaseType());
    if (classType->IsStatic())
    {
        exception.reset(new Exception(GetModule(), "cannot copy an instance of a static class", span, classType->GetSpan()));
        return;
    }
    TypeSymbol* rightType = arguments[1]->GetType()->PlainType(span);
    bool typesEqual = TypesEqual(rightType, classType);
    bool bindToRvalueRef = arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference);
    bool conversionFunctionExists = false;
    if (!typesEqual)
    {
        ArgumentMatch argumentMatch;
        FunctionSymbol* conversion = GetBoundCompileUnit().GetConversion(rightType, classType, containerScope, currentFunction, span, argumentMatch);
        if (conversion && conversion->GetSymbolType() == SymbolType::conversionFunctionSymbol)
        {
            conversionFunctionExists = true;
            if (conversion->ReturnsClassInterfaceOrClassDelegateByValue())
            {
                bindToRvalueRef = true; // in this case class move constructor will match
            }
        }
    }
    if (!TypesEqual(arguments[1]->GetType(), classType->AddRvalueReference(span)) && !bindToRvalueRef && (typesEqual || conversionFunctionExists))
    {
        if (classType->CopyConstructor())
        {
            viableFunctions.Insert(classType->CopyConstructor());
            return;
        }
        if (GetBoundCompileUnit().HasCopyConstructorFor(classType->TypeId()))
        {
            viableFunctions.Insert(GetBoundCompileUnit().GetCopyConstructorFor(classType->TypeId()));
            return;
        }
        if (classType->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            GetBoundCompileUnit().FinalizeBinding(static_cast<ClassTemplateSpecializationSymbol*>(classType));
        }
        FunctionSymbol* function = functionMap[classType->TypeId()];
        if (!function)
        {
            std::unique_ptr<ClassCopyConstructor> copyConstructor(new ClassCopyConstructor(classType, span));
            function = copyConstructor.get();
            function->SetModule(GetModule());
            function->SetParent(classType);
            function->SetLinkOnceOdrLinkage();
            function->SetInline();
            functionMap[classType->TypeId()] = function;
            copyConstructor->SetCompileUnit(GetBoundCompileUnit().GetCompileUnitNode());
            copyConstructor->SetModule(GetModule());
            if (GenerateImplementation(copyConstructor.get(), containerScope, currentFunction, exception, span))
            {
                GetBoundCompileUnit().AddCopyConstructorToMap(classType->TypeId(), copyConstructor.get());
                functions.push_back(std::unique_ptr<FunctionSymbol>(copyConstructor.release()));
            }
            else
            {
                functionMap[classType->TypeId()] = nullptr;
                return;
            }
        }
        viableFunctions.Insert(function);
    }
}

bool ClassCopyConstructorOperation::GenerateImplementation(ClassCopyConstructor* copyConstructor, ContainerScope* containerScope, BoundFunction* currentFunction, 
    std::unique_ptr<Exception>& exception, const Span& span)
{
    ClassTypeSymbol* classType = copyConstructor->ClassType();
    try
    {
        bool nothrow = true;
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(GetModule(), copyConstructor));
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(new BoundCompoundStatement(GetModule(), span)));
        if (classType->StaticConstructor())
        {
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::unique_ptr<BoundExpression>(new BoundFunctionCall(GetModule(), span,
                classType->StaticConstructor())))));
            if (!classType->StaticConstructor()->DontThrow()) nothrow = false;
        }
        if (classType->BaseClass())
        {
            std::vector<FunctionScopeLookup> baseConstructorCallLookups;
            baseConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            baseConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->BaseClass()->GetContainerScope()));
            baseConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> baseConstructorCallArguments;
            ParameterSymbol* thisParam = copyConstructor->Parameters()[0];
            ArgumentMatch argumentMatch;
            FunctionSymbol* thisToBaseConversion = GetBoundCompileUnit().GetConversion(thisParam->GetType(), classType->BaseClass()->AddPointer(span), containerScope, currentFunction, span, argumentMatch);
            if (!thisToBaseConversion)
            {
                throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
            }
            BoundExpression* baseClassPointerConversion = new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thisParam)), thisToBaseConversion);
            baseConstructorCallArguments.push_back(std::unique_ptr<BoundExpression>(baseClassPointerConversion));
            ParameterSymbol* thatParam = copyConstructor->Parameters()[1];
            FunctionSymbol* thatToBaseConversion = GetBoundCompileUnit().GetConversion(thatParam->GetType(), classType->BaseClass()->AddConst(span)->AddLvalueReference(span), containerScope, 
                currentFunction, span, argumentMatch);
            if (!thatToBaseConversion)
            {
                throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
            }
            BoundExpression* thatArgumentConversion = new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thatParam)), thatToBaseConversion);
            baseConstructorCallArguments.push_back(std::unique_ptr<BoundExpression>(thatArgumentConversion));
            std::unique_ptr<BoundFunctionCall> baseConstructorCall = ResolveOverload(U"@constructor", containerScope, baseConstructorCallLookups, baseConstructorCallArguments, GetBoundCompileUnit(),
                boundFunction.get(), span);
            if (!baseConstructorCall->GetFunctionSymbol()->DontThrow()) nothrow = false;
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::move(baseConstructorCall))));
        }
        if (classType->IsPolymorphic())
        {
            ParameterSymbol* thisParam = copyConstructor->Parameters()[0];
            BoundExpression* classPtr = nullptr;
            ClassTypeSymbol* vmtPtrHolderClass = classType->VmtPtrHolderClass();
            if (vmtPtrHolderClass == classType)
            {
                classPtr = new BoundParameter(GetModule(), span, thisParam);
            }
            else
            {
                ArgumentMatch argumentMatch;
                FunctionSymbol* thisToHolderConversion = GetBoundCompileUnit().GetConversion(thisParam->GetType(), vmtPtrHolderClass->AddPointer(span), containerScope, currentFunction, span, argumentMatch);
                if (!thisToHolderConversion)
                {
                    throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
                }
                classPtr = new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thisParam)), thisToHolderConversion);
            }
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundSetVmtPtrStatement(GetModule(), std::unique_ptr<BoundExpression>(classPtr), classType)));
        }
        int n = classType->MemberVariables().size();
        for (int i = 0; i < n; ++i)
        {
            MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
            std::vector<FunctionScopeLookup> memberConstructorCallLookups;
            memberConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            memberConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
            memberConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> memberConstructorCallArguments;
            BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(GetModule(), span, memberVariableSymbol);
            boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, copyConstructor->GetThisParam())));
            memberConstructorCallArguments.push_back(std::unique_ptr<BoundExpression>(
                new BoundAddressOfExpression(GetModule(), std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span))));
            ParameterSymbol* thatParam = copyConstructor->Parameters()[1];
            BoundMemberVariable* thatBoundMemberVariable = new BoundMemberVariable(GetModule(), span, memberVariableSymbol);
            thatBoundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(
                new BoundReferenceToPointerExpression(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thatParam)), thatParam->GetType()->BaseType()->AddPointer(span))));
            memberConstructorCallArguments.push_back(std::unique_ptr<BoundExpression>(thatBoundMemberVariable));
            std::unique_ptr<BoundFunctionCall> memberConstructorCall = ResolveOverload(U"@constructor", containerScope, memberConstructorCallLookups, memberConstructorCallArguments,
                GetBoundCompileUnit(), boundFunction.get(), span);
            if (!memberConstructorCall->GetFunctionSymbol()->DontThrow()) nothrow = false;
            boundFunction->MoveTemporaryDestructorCallsTo(*memberConstructorCall);
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::move(memberConstructorCall))));
        }
        GetBoundCompileUnit().AddBoundNode(std::move(boundFunction));
        if (nothrow)
        {
            copyConstructor->SetNothrow();
        }
    }
    catch (const Exception& ex)
    {
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        exception.reset(new Exception(GetModule(), "cannot create copy constructor for class '" + ToUtf8(classType->FullName()) + "'. Reason: " + ex.Message(), span, references));
        return false;
    }
    return true;
}

class ClassMoveConstructor : public ConstructorSymbol
{
public:
    ClassMoveConstructor(ClassTypeSymbol* classType_, const Span& span_);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    bool IsGeneratedFunction() const override { return true; }
    ClassTypeSymbol* ClassType() { return classType; }
    const char* ClassName() const override { return "ClassMoveConstructor"; }
private:
    ClassTypeSymbol* classType;
};

ClassMoveConstructor::ClassMoveConstructor(ClassTypeSymbol* classType_, const Span& span_) :
    ConstructorSymbol(classType_->GetSpan(), U"@constructor"), classType(classType_)
{
    SetAccess(SymbolAccess::public_);
    SetParent(classType);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(classType->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(classType->AddRvalueReference(span_));
    AddMember(thatParam);
    ComputeName();
}

class ClassMoveConstructorOperation : public Operation
{
public:
    ClassMoveConstructorOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
    bool GenerateImplementation(ClassMoveConstructor* moveConstructor, ContainerScope* containerScope, BoundFunction* currentFunction, std::unique_ptr<Exception>& exception, const Span& span);
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

ClassMoveConstructorOperation::ClassMoveConstructorOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"@constructor", 2, boundCompileUnit_)
{
}

void ClassMoveConstructorOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || !type->RemovePointer(span)->PlainType(span)->IsClassTypeSymbol()) return;
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type->BaseType());
    if (classType->IsStatic())
    {
        exception.reset(new Exception(GetModule(), "cannot move an instance of a static class", span, classType->GetSpan()));
        return;
    }
    TypeSymbol* rightType = arguments[1]->GetType()->PlainType(span);
    bool bindToRvalueRef = arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference);
    bool typesEqual = TypesEqual(arguments[1]->GetType(), classType->AddRvalueReference(span));
    if (!typesEqual)
    {
        ArgumentMatch argumentMatch;
        FunctionSymbol* conversion = GetBoundCompileUnit().GetConversion(rightType, classType, containerScope, currentFunction, span, argumentMatch);
        if (conversion && conversion->GetSymbolType() == SymbolType::conversionFunctionSymbol)
        {
            if (conversion->ReturnsClassInterfaceOrClassDelegateByValue())
            {
                bindToRvalueRef = true;
            }
        }
    }
    if (typesEqual || bindToRvalueRef)
    {
        if (classType->MoveConstructor())
        {
            viableFunctions.Insert(classType->MoveConstructor());
            return;
        }
        if (classType->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            GetBoundCompileUnit().FinalizeBinding(static_cast<ClassTemplateSpecializationSymbol*>(classType));
        }
        FunctionSymbol* function = functionMap[classType->TypeId()];
        if (!function)
        {
            std::unique_ptr<ClassMoveConstructor> moveConstructor(new ClassMoveConstructor(classType, span));
            function = moveConstructor.get();
            function->SetModule(GetModule());
            function->SetParent(classType);
            function->SetLinkOnceOdrLinkage();
            function->SetInline();
            functionMap[classType->TypeId()] = function;
            moveConstructor->SetCompileUnit(GetBoundCompileUnit().GetCompileUnitNode());
            moveConstructor->SetModule(GetModule());
            if (GenerateImplementation(moveConstructor.get(), containerScope, currentFunction, exception, span))
            {
                functions.push_back(std::unique_ptr<FunctionSymbol>(moveConstructor.release()));
            }
            else
            {
                functionMap[classType->TypeId()] = nullptr;
                return;
            }
        }
        viableFunctions.Insert(function);
    }
}

bool ClassMoveConstructorOperation::GenerateImplementation(ClassMoveConstructor* moveConstructor, ContainerScope* containerScope, BoundFunction* currentFunction, 
    std::unique_ptr<Exception>& exception, const Span& span)
{
    ClassTypeSymbol* classType = moveConstructor->ClassType();
    try
    {
        bool nothrow = true;
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(GetModule(), moveConstructor));
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(new BoundCompoundStatement(GetModule(), span)));
        if (classType->StaticConstructor())
        {
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::unique_ptr<BoundExpression>(new BoundFunctionCall(GetModule(), span,
                classType->StaticConstructor())))));
            if (!classType->StaticConstructor()->DontThrow()) nothrow = false;
        }
        if (classType->BaseClass())
        {
            std::vector<FunctionScopeLookup> baseConstructorCallLookups;
            baseConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            baseConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->BaseClass()->GetContainerScope()));
            baseConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> baseConstructorCallArguments;
            ParameterSymbol* thisParam = moveConstructor->Parameters()[0];
            ArgumentMatch argumentMatch;
            FunctionSymbol* thisToBaseConversion = GetBoundCompileUnit().GetConversion(thisParam->GetType(), classType->BaseClass()->AddPointer(span), containerScope, currentFunction, span, argumentMatch);
            if (!thisToBaseConversion)
            {
                throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
            }
            std::unique_ptr<BoundExpression> baseClassPointerConversion(new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thisParam)), thisToBaseConversion));
            baseConstructorCallArguments.push_back(std::move(baseClassPointerConversion));
            ParameterSymbol* thatParam = moveConstructor->Parameters()[1];
            FunctionSymbol* thatToBaseConversion = GetBoundCompileUnit().GetConversion(thatParam->GetType(), classType->BaseClass()->AddRvalueReference(span), containerScope, currentFunction, span, argumentMatch);
            if (!thatToBaseConversion)
            {
                throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
            }
            std::unique_ptr<BoundExpression> thatArgumentConversion(new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thatParam)), thatToBaseConversion));
            baseConstructorCallArguments.push_back(std::move(thatArgumentConversion));
            std::unique_ptr<BoundFunctionCall> baseConstructorCall = ResolveOverload(U"@constructor", containerScope, baseConstructorCallLookups, baseConstructorCallArguments, GetBoundCompileUnit(),
                boundFunction.get(), span);
            if (!baseConstructorCall->GetFunctionSymbol()->DontThrow()) nothrow = false;
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::move(baseConstructorCall))));
        }
        if (classType->IsPolymorphic())
        {
            ParameterSymbol* thisParam = moveConstructor->Parameters()[0];
            BoundExpression* classPtr = nullptr;
            ClassTypeSymbol* vmtPtrHolderClass = classType->VmtPtrHolderClass();
            if (vmtPtrHolderClass == classType)
            {
                classPtr = new BoundParameter(GetModule(), span, thisParam);
            }
            else
            {
                ArgumentMatch argumentMatch;
                FunctionSymbol* thisToHolderConversion = GetBoundCompileUnit().GetConversion(thisParam->GetType(), vmtPtrHolderClass->AddPointer(span), containerScope, currentFunction, span, argumentMatch);
                if (!thisToHolderConversion)
                {
                    throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
                }
                classPtr = new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thisParam)), thisToHolderConversion);
            }
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundSetVmtPtrStatement(GetModule(), std::unique_ptr<BoundExpression>(classPtr), classType)));
        }
        int n = classType->MemberVariables().size();
        for (int i = 0; i < n; ++i)
        {
            MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
            std::vector<FunctionScopeLookup> memberConstructorCallLookups;
            memberConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            memberConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
            memberConstructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> memberConstructorCallArguments;
            BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(GetModule(), span, memberVariableSymbol);
            boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, moveConstructor->GetThisParam())));
            memberConstructorCallArguments.push_back(std::unique_ptr<BoundExpression>(
                new BoundAddressOfExpression(GetModule(), std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span))));
            ParameterSymbol* thatParam = moveConstructor->Parameters()[1];
            std::unique_ptr<BoundMemberVariable> thatBoundMemberVariable(new BoundMemberVariable(GetModule(), span, memberVariableSymbol));
            thatBoundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(
                new BoundReferenceToPointerExpression(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thatParam)), thatParam->GetType()->BaseType()->AddPointer(span))));
            std::vector<FunctionScopeLookup> rvalueLookups;
            rvalueLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            rvalueLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            std::vector<std::unique_ptr<BoundExpression>> rvalueArguments;
            rvalueArguments.push_back(std::move(thatBoundMemberVariable));
            std::unique_ptr<BoundFunctionCall> rvalueMemberCall = ResolveOverload(U"System.Rvalue", containerScope, rvalueLookups, rvalueArguments, GetBoundCompileUnit(), boundFunction.get(), span);
            memberConstructorCallArguments.push_back(std::move(rvalueMemberCall));
            std::unique_ptr<BoundFunctionCall> memberConstructorCall = ResolveOverload(U"@constructor", containerScope, memberConstructorCallLookups, memberConstructorCallArguments,
                GetBoundCompileUnit(), boundFunction.get(), span);
            if (!memberConstructorCall->GetFunctionSymbol()->DontThrow()) nothrow = false;
            boundFunction->MoveTemporaryDestructorCallsTo(*memberConstructorCall);
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::move(memberConstructorCall))));
        }
        GetBoundCompileUnit().AddBoundNode(std::move(boundFunction));
        if (nothrow)
        {
            moveConstructor->SetNothrow();
        }
    }
    catch (const Exception& ex)
    {
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        exception.reset(new Exception(GetModule(), "cannot create move constructor for class '" + ToUtf8(classType->FullName()) + "'. Reason: " + ex.Message(), span, references));
        return false;
    }
    return true;
}

class ClassCopyAssignment : public MemberFunctionSymbol
{
public:
    ClassCopyAssignment(ClassTypeSymbol* classType_, TypeSymbol* voidType_, const Span& span_);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    bool IsGeneratedFunction() const override { return true; }
    ClassTypeSymbol* ClassType() { return classType; }
    const char* ClassName() const override { return "ClassCopyAssignment"; }
private:
    ClassTypeSymbol* classType;
};

ClassCopyAssignment::ClassCopyAssignment(ClassTypeSymbol* classType_, TypeSymbol* voidType_, const Span& span_) :
    MemberFunctionSymbol(classType_->GetSpan(), U"operator="), classType(classType_)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    SetParent(classType);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(classType->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(classType->AddConst(span_)->AddLvalueReference(span_));
    AddMember(thatParam);
    SetReturnType(voidType_);
    ComputeName();
}

class ClassCopyAssignmentOperation : public Operation
{
public:
    ClassCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
    bool GenerateImplementation(ClassCopyAssignment* copyAssignment, ContainerScope* containerScope, BoundFunction* currentFunction, std::unique_ptr<Exception>& exception, const Span& span);
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

ClassCopyAssignmentOperation::ClassCopyAssignmentOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator=", 2, boundCompileUnit_)
{
}

void ClassCopyAssignmentOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || !type->RemovePointer(span)->PlainType(span)->IsClassTypeSymbol()) return;
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type->BaseType());
    if (classType->IsStatic())
    {
        exception.reset(new Exception(GetModule(), "cannot assign an instance of a static class", span, classType->GetSpan()));
        return;
    }
    TypeSymbol* rightType = arguments[1]->GetType()->PlainType(span);
    bool bindToRvalueRef = arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference);
    bool conversionFunctionExists = false;
    bool typesEqual = TypesEqual(rightType, classType);
    if (!typesEqual)
    {
        ArgumentMatch argumentMatch;
        FunctionSymbol* conversion = GetBoundCompileUnit().GetConversion(rightType, classType, containerScope, currentFunction, span, argumentMatch);
        if (conversion && conversion->GetSymbolType() == SymbolType::conversionFunctionSymbol)
        {
            conversionFunctionExists = true;
            if (conversion->ReturnsClassInterfaceOrClassDelegateByValue())
            {
                bindToRvalueRef = true; // in this case class move constructor will match
            }
        }
    }
    if (!TypesEqual(arguments[1]->GetType(), classType->AddRvalueReference(span)) && !bindToRvalueRef && (TypesEqual(rightType, classType) || conversionFunctionExists))
    {
        if (classType->CopyAssignment())
        {
            viableFunctions.Insert(classType->CopyAssignment());
            return;
        }
        if (classType->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            GetBoundCompileUnit().FinalizeBinding(static_cast<ClassTemplateSpecializationSymbol*>(classType));
        }
        FunctionSymbol* function = functionMap[classType->TypeId()];
        if (!function)
        {
            std::unique_ptr<ClassCopyAssignment> copyAssignment(new ClassCopyAssignment(classType, GetBoundCompileUnit().GetSymbolTable().GetTypeByName(U"void"), span));
            function = copyAssignment.get();
            function->SetModule(GetModule());
            function->SetParent(classType);
            function->SetLinkOnceOdrLinkage();
            function->SetInline();
            functionMap[classType->TypeId()] = function;
            copyAssignment->SetCompileUnit(GetBoundCompileUnit().GetCompileUnitNode());
            copyAssignment->SetModule(GetModule());
            if (GenerateImplementation(copyAssignment.get(), containerScope, currentFunction, exception, span))
            {
                functions.push_back(std::unique_ptr<FunctionSymbol>(copyAssignment.release())); // todo
            }
            else
            {
                functionMap[classType->TypeId()] = nullptr;
                return;
            }
        }
        viableFunctions.Insert(function);
    }
}

bool ClassCopyAssignmentOperation::GenerateImplementation(ClassCopyAssignment* copyAssignment, ContainerScope* containerScope, BoundFunction* currentFunction, 
    std::unique_ptr<Exception>& exception, const Span& span)
{
    ClassTypeSymbol* classType = copyAssignment->ClassType();
    try
    {
        bool nothrow = true;
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(GetModule(), copyAssignment));
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(new BoundCompoundStatement(GetModule(), span)));
        if (classType->BaseClass())
        {
            std::vector<FunctionScopeLookup> baseAssignmentCallLookups;
            baseAssignmentCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            baseAssignmentCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->BaseClass()->GetContainerScope()));
            baseAssignmentCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> baseAssignmentCallArguments;
            ParameterSymbol* thisParam = copyAssignment->Parameters()[0];
            ArgumentMatch argumentMatch;
            FunctionSymbol* thisToBaseConversion = GetBoundCompileUnit().GetConversion(thisParam->GetType(), classType->BaseClass()->AddPointer(span), containerScope, currentFunction, span, argumentMatch);
            if (!thisToBaseConversion)
            {
                throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
            }
            BoundExpression* baseClassPointerConversion = new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thisParam)), thisToBaseConversion);
            baseAssignmentCallArguments.push_back(std::unique_ptr<BoundExpression>(baseClassPointerConversion));
            ParameterSymbol* thatParam = copyAssignment->Parameters()[1];
            FunctionSymbol* thatToBaseConversion = GetBoundCompileUnit().GetConversion(thatParam->GetType(), classType->BaseClass()->AddConst(span)->AddLvalueReference(span), containerScope, 
                currentFunction, span, argumentMatch);
            if (!thatToBaseConversion)
            {
                throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
            }
            BoundExpression* thatArgumentConversion = new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thatParam)), thatToBaseConversion);
            baseAssignmentCallArguments.push_back(std::unique_ptr<BoundExpression>(thatArgumentConversion));
            std::unique_ptr<BoundFunctionCall> baseAssignmentCall = ResolveOverload(U"operator=", containerScope, baseAssignmentCallLookups, baseAssignmentCallArguments, GetBoundCompileUnit(),
                boundFunction.get(), span);
            if (!baseAssignmentCall->GetFunctionSymbol()->DontThrow()) nothrow = false;
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::move(baseAssignmentCall))));
        }
        int n = classType->MemberVariables().size();
        for (int i = 0; i < n; ++i)
        {
            MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
            std::vector<FunctionScopeLookup> memberAssignmentCallLookups;
            memberAssignmentCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            memberAssignmentCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
            memberAssignmentCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> memberAssignmentCallArguments;
            BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(GetModule(), span, memberVariableSymbol);
            boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, copyAssignment->GetThisParam())));
            memberAssignmentCallArguments.push_back(std::unique_ptr<BoundExpression>(
                new BoundAddressOfExpression(GetModule(), std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span))));
            ParameterSymbol* thatParam = copyAssignment->Parameters()[1];
            BoundMemberVariable* thatBoundMemberVariable = new BoundMemberVariable(GetModule(), span, memberVariableSymbol);
            thatBoundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(
                new BoundReferenceToPointerExpression(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thatParam)), thatParam->GetType()->BaseType()->AddPointer(span))));
            memberAssignmentCallArguments.push_back(std::unique_ptr<BoundExpression>(thatBoundMemberVariable));
            std::unique_ptr<BoundFunctionCall> memberAssignmentCall = ResolveOverload(U"operator=", containerScope, memberAssignmentCallLookups, memberAssignmentCallArguments,
                GetBoundCompileUnit(), boundFunction.get(), span);
            if (!memberAssignmentCall->GetFunctionSymbol()->DontThrow()) nothrow = false;
            boundFunction->MoveTemporaryDestructorCallsTo(*memberAssignmentCall);
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::move(memberAssignmentCall))));
        }
        GetBoundCompileUnit().AddBoundNode(std::move(boundFunction));
        if (nothrow)
        {
            copyAssignment->SetNothrow();
        }
    }
    catch (const Exception& ex)
    {
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        exception.reset(new Exception(GetModule(), "cannot create copy assignment for class '" + ToUtf8(classType->FullName()) + "'. Reason: " + ex.Message(), span, references));
        return false;
    }
    return true;
}

class ClassMoveAssignment : public MemberFunctionSymbol
{
public:
    ClassMoveAssignment(ClassTypeSymbol* classType_, TypeSymbol* voidType_, const Span& span_);
    SymbolAccess DeclaredAccess() const override { return SymbolAccess::public_; }
    bool IsGeneratedFunction() const override { return true; }
    ClassTypeSymbol* ClassType() { return classType; }
    const char* ClassName() const override { return "ClassMoveAssignment"; }
private:
    ClassTypeSymbol* classType;
};

ClassMoveAssignment::ClassMoveAssignment(ClassTypeSymbol* classType_, TypeSymbol* voidType_, const Span& span_) :
    MemberFunctionSymbol(classType_->GetSpan(), U"operator="), classType(classType_)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    SetParent(classType);
    ParameterSymbol* thisParam = new ParameterSymbol(span_, U"this");
    thisParam->SetType(classType->AddPointer(span_));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(span_, U"that");
    thatParam->SetType(classType->AddRvalueReference(span_));
    AddMember(thatParam);
    SetReturnType(voidType_);
    ComputeName();
}

class ClassMoveAssignmentOperation : public Operation
{
public:
    ClassMoveAssignmentOperation(BoundCompileUnit& boundCompileUnit_);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span) override;
    bool GenerateImplementation(ClassMoveAssignment* moveAssignment, ContainerScope* containerScope, BoundFunction* currentFunction, std::unique_ptr<Exception>& exception, const Span& span);
private:
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functions;
};

ClassMoveAssignmentOperation::ClassMoveAssignmentOperation(BoundCompileUnit& boundCompileUnit_) : Operation(U"operator=", 2, boundCompileUnit_)
{
}

void ClassMoveAssignmentOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    TypeSymbol* type = arguments[0]->GetType();
    if (type->PointerCount() != 1 || !type->RemovePointer(span)->PlainType(span)->IsClassTypeSymbol()) return;
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type->BaseType());
    if (classType->IsStatic())
    {
        exception.reset(new Exception(GetModule(), "cannot assign an instance of a static class", span, classType->GetSpan()));
        return;
    }
    TypeSymbol* rightType = arguments[1]->GetType()->PlainType(span);
    bool bindToRvalueRef = arguments[1]->GetFlag(BoundExpressionFlags::bindToRvalueReference);
    bool typesEqual = TypesEqual(arguments[1]->GetType(), classType->AddRvalueReference(span));
    if (!typesEqual)
    {
        ArgumentMatch argumentMatch;
        FunctionSymbol* conversion = GetBoundCompileUnit().GetConversion(rightType, classType, containerScope, currentFunction, span, argumentMatch);
        if (conversion && conversion->GetSymbolType() == SymbolType::conversionFunctionSymbol)
        {
            if (conversion->ReturnsClassInterfaceOrClassDelegateByValue())
            {
                bindToRvalueRef = true;
            }
        }
    }
    if (typesEqual || bindToRvalueRef)
    {
        if (classType->MoveAssignment())
        {
            viableFunctions.Insert(classType->MoveAssignment());
            return;
        }
        if (classType->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            GetBoundCompileUnit().FinalizeBinding(static_cast<ClassTemplateSpecializationSymbol*>(classType));
        }
        FunctionSymbol* function = functionMap[classType->TypeId()];
        if (!function)
        {
            std::unique_ptr<ClassMoveAssignment> moveAssignment(new ClassMoveAssignment(classType, GetBoundCompileUnit().GetSymbolTable().GetTypeByName(U"void"), span));
            function = moveAssignment.get();
            function->SetModule(GetModule());
            function->SetParent(classType);
            function->SetLinkOnceOdrLinkage();
            function->SetInline();
            functionMap[classType->TypeId()] = function;
            moveAssignment->SetCompileUnit(GetBoundCompileUnit().GetCompileUnitNode());
            moveAssignment->SetModule(GetModule());
            if (GenerateImplementation(moveAssignment.get(), containerScope, currentFunction, exception, span))
            {
                functions.push_back(std::unique_ptr<FunctionSymbol>(moveAssignment.release()));
            }
            else
            {
                functionMap[classType->TypeId()] = nullptr;
                return;
            }
        }
        viableFunctions.Insert(function);
    }
}

bool ClassMoveAssignmentOperation::GenerateImplementation(ClassMoveAssignment* moveAssignment, ContainerScope* containerScope, BoundFunction* currentFunction, 
    std::unique_ptr<Exception>& exception, const Span& span)
{
    ClassTypeSymbol* classType = moveAssignment->ClassType();
    try
    {
        bool nothrow = true;
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(GetModule(), moveAssignment));
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(new BoundCompoundStatement(GetModule(), span)));
        if (classType->BaseClass())
        {
            std::vector<FunctionScopeLookup> baseAssignmentCallLookups;
            baseAssignmentCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            baseAssignmentCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->BaseClass()->GetContainerScope()));
            baseAssignmentCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> baseAssignmentCallArguments;
            ParameterSymbol* thisParam = moveAssignment->Parameters()[0];
            ArgumentMatch argumentMatch;
            FunctionSymbol* thisToBaseConversion = GetBoundCompileUnit().GetConversion(thisParam->GetType(), classType->BaseClass()->AddPointer(span), containerScope, currentFunction, span, argumentMatch);
            if (!thisToBaseConversion)
            {
                throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
            }
            std::unique_ptr<BoundExpression> baseClassPointerConversion(new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thisParam)), thisToBaseConversion));
            baseAssignmentCallArguments.push_back(std::move(baseClassPointerConversion));
            ParameterSymbol* thatParam = moveAssignment->Parameters()[1];
            FunctionSymbol* thatToBaseConversion = GetBoundCompileUnit().GetConversion(thatParam->GetType(), classType->BaseClass()->AddRvalueReference(span), containerScope, currentFunction, span, argumentMatch);
            if (!thatToBaseConversion)
            {
                throw Exception(GetModule(), "base class conversion not found", span, classType->GetSpan());
            }
            std::unique_ptr<BoundExpression> thatArgumentConversion(new BoundConversion(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thatParam)), thatToBaseConversion));
            baseAssignmentCallArguments.push_back(std::move(thatArgumentConversion));
            std::unique_ptr<BoundFunctionCall> baseAssignmentCall = ResolveOverload(U"operator=", containerScope, baseAssignmentCallLookups, baseAssignmentCallArguments, GetBoundCompileUnit(),
                boundFunction.get(), span);
            if (!baseAssignmentCall->GetFunctionSymbol()->DontThrow()) nothrow = false;
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::move(baseAssignmentCall))));
        }
        int n = classType->MemberVariables().size();
        for (int i = 0; i < n; ++i)
        {
            MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
            std::unique_ptr<BoundMemberVariable> boundMemberVariable(new BoundMemberVariable(GetModule(), span, memberVariableSymbol));
            boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, moveAssignment->GetThisParam())));
            ParameterSymbol* thatParam = moveAssignment->Parameters()[1];
            std::unique_ptr<BoundMemberVariable> thatBoundMemberVariable(new BoundMemberVariable(GetModule(), span, memberVariableSymbol));
            thatBoundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(
                new BoundReferenceToPointerExpression(GetModule(), std::unique_ptr<BoundExpression>(new BoundParameter(GetModule(), span, thatParam)), thatParam->GetType()->BaseType()->AddPointer(span))));
            std::vector<FunctionScopeLookup> swapLookups;
            swapLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            swapLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            std::vector<std::unique_ptr<BoundExpression>> swapArguments;
            swapArguments.push_back(std::move(boundMemberVariable));
            swapArguments.push_back(std::move(thatBoundMemberVariable));
            std::unique_ptr<BoundFunctionCall> swapMemberCall = ResolveOverload(U"System.Swap", containerScope, swapLookups, swapArguments, GetBoundCompileUnit(), boundFunction.get(), span);
            if (!swapMemberCall->GetFunctionSymbol()->DontThrow()) nothrow = false;
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(GetModule(), std::move(swapMemberCall))));
        }
        GetBoundCompileUnit().AddBoundNode(std::move(boundFunction));
        if (nothrow)
        {
            moveAssignment->SetNothrow();
        }
    }
    catch (const Exception& ex)
    {
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        exception.reset(new Exception(GetModule(), "cannot create move assignment for class '" + ToUtf8(classType->FullName()) + "'. Reason: " + ex.Message(), span, references));
        return false;
    }
    return true;
}

void GenerateDestructorImplementation(BoundClass* boundClass, DestructorSymbol* destructorSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, BoundFunction* currentFunction, 
    const Span& span)
{
    Module* module = &boundCompileUnit.GetModule();
    ClassTypeSymbol* classType = boundClass->GetClassTypeSymbol();
    try
    {
        std::unique_ptr<BoundFunction> boundFunction(new BoundFunction(module, destructorSymbol));
        boundFunction->SetBody(std::unique_ptr<BoundCompoundStatement>(new BoundCompoundStatement(module, span)));
        if (classType->IsPolymorphic())
        {
            ParameterSymbol* thisParam = destructorSymbol->Parameters()[0];
            BoundExpression* classPtr = nullptr;
            ClassTypeSymbol* vmtPtrHolderClass = classType->VmtPtrHolderClass();
            if (vmtPtrHolderClass == classType)
            {
                classPtr = new BoundParameter(module, span, thisParam);
            }
            else
            {
                ArgumentMatch argumentMatch;
                FunctionSymbol* thisToHolderConversion = boundCompileUnit.GetConversion(thisParam->GetType(), vmtPtrHolderClass->AddPointer(span), containerScope, currentFunction, span, argumentMatch);
                if (!thisToHolderConversion)
                {
                    throw Exception(module, "base class conversion not found", span, classType->GetSpan());
                }
                classPtr = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)), thisToHolderConversion);
            }
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundSetVmtPtrStatement(module, std::unique_ptr<BoundExpression>(classPtr), classType)));
        }
        int n = classType->MemberVariables().size();
        for (int i = n - 1; i >= 0; --i)
        {
            MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
            if (memberVariableSymbol->GetType()->HasNontrivialDestructor())
            {
                std::vector<FunctionScopeLookup> memberDestructorCallLookups;
                memberDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                memberDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->ClassInterfaceOrNsScope()));
                memberDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                std::vector<std::unique_ptr<BoundExpression>> memberDestructorCallArguments;
                BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(module, span, memberVariableSymbol);
                boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(module, span, destructorSymbol->GetThisParam())));
                memberDestructorCallArguments.push_back(std::unique_ptr<BoundExpression>(
                    new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span))));
                std::unique_ptr<BoundFunctionCall> memberDestructorCall = ResolveOverload(U"@destructor", containerScope, memberDestructorCallLookups, memberDestructorCallArguments,
                    boundCompileUnit, boundFunction.get(), span);
                boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(memberDestructorCall))));
            }
        }
        if (classType->BaseClass() && classType->BaseClass()->HasNontrivialDestructor())
        {
            std::vector<FunctionScopeLookup> baseDestructorCallLookups;
            baseDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            baseDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->BaseClass()->GetContainerScope()));
            baseDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> baseDestructorCallArguments;
            ParameterSymbol* thisParam = destructorSymbol->Parameters()[0];
            ArgumentMatch argumentMatch;
            FunctionSymbol* thisToBaseConversion = boundCompileUnit.GetConversion(thisParam->GetType(), classType->BaseClass()->AddPointer(span), containerScope, currentFunction, span, argumentMatch);
            if (!thisToBaseConversion)
            {
                throw Exception(module, "base class conversion not found", span, classType->GetSpan());
            }
            BoundExpression* baseClassPointerConversion = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)), thisToBaseConversion);
            baseDestructorCallArguments.push_back(std::unique_ptr<BoundExpression>(baseClassPointerConversion));
            std::unique_ptr<BoundFunctionCall> baseDestructorCall = ResolveOverload(U"@destructor", containerScope, baseDestructorCallLookups, baseDestructorCallArguments, boundCompileUnit,
                boundFunction.get(), span);
            boundFunction->Body()->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(baseDestructorCall))));
        }
        boundClass->AddMember(std::move(boundFunction));
    }
    catch (const Exception& ex)
    {
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception(module, "cannot create destructor for class '" + ToUtf8(classType->FullName()) + "'. Reason: " + ex.Message(), span, references);
    }
}

BoundExpression* MakeExitEntryPtr(BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, const Span& span)
{
    Symbol* symbol = containerScope->Lookup(U"System.ExitEntry", ScopeLookup::this_and_base_and_parent);
    if (symbol)
    {
        if (symbol->IsTypeSymbol())
        {
            if (symbol->GetSymbolType() == SymbolType::classGroupTypeSymbol)
            {
                ClassGroupTypeSymbol* classGroupSymbol = static_cast<ClassGroupTypeSymbol*>(symbol);
                symbol = classGroupSymbol->GetClass(0);
            }
            TypeSymbol* exitEntryType = static_cast<TypeSymbol*>(symbol);
            SymbolCreatorVisitor symbolCreatorVisitor(boundCompileUnit.GetSymbolTable());
            GlobalVariableNode globalVariableNode(span, Specifiers::private_, new DotNode(span, new IdentifierNode(span, U"System"), new IdentifierNode(span, U"ExitEntry")),
                new IdentifierNode(span, U"exit@entry@" + ToUtf32(std::to_string(boundCompileUnit.GetNextExitEntryIndex()))), boundCompileUnit.GetCompileUnitNode());
            globalVariableNode.Accept(symbolCreatorVisitor);
            TypeBinder typeBinder(boundCompileUnit);
            typeBinder.SetContainerScope(containerScope);
            globalVariableNode.Accept(typeBinder);
            BoundGlobalVariable* exitEntryGlobalVariable = static_cast<BoundGlobalVariable*>(typeBinder.GetBoundGlobalVariable()->Clone());
            return new BoundAddressOfExpression(&boundCompileUnit.GetModule(), std::unique_ptr<BoundExpression>(exitEntryGlobalVariable), exitEntryType->AddPointer(span));
        }
        else
        {
            throw Exception(GetRootModuleForCurrentThread(), "System.ExitEntry expected to denote a type", span);
        }
    }
    else
    {
        throw Exception(GetRootModuleForCurrentThread(), "System.ExitEntry symbol not found", span);
    }
}

void GenerateStaticClassInitialization(StaticConstructorSymbol* staticConstructorSymbol, StaticConstructorNode* staticConstructorNode, BoundCompileUnit& boundCompileUnit, 
    BoundCompoundStatement* boundCompoundStatement, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder, const Span& span)
{
    Module* module = &boundCompileUnit.GetModule();
    Symbol* parent = staticConstructorSymbol->Parent();
    Assert(parent->GetSymbolType() == SymbolType::classTypeSymbol || parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type symbol expected");
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(parent);
    try
    {
        Assert(classType->InitializedVar(), "initialized variable expected");
        std::unique_ptr<BoundIfStatement> ifStatement(new BoundIfStatement(module, span, std::unique_ptr<BoundExpression>(new BoundMemberVariable(module, span, classType->InitializedVar())),
            std::unique_ptr<BoundStatement>(new BoundReturnStatement(module, std::unique_ptr<BoundFunctionCall>(nullptr), span)), std::unique_ptr<BoundStatement>(nullptr)));
        boundCompoundStatement->AddStatement(std::move(ifStatement));
        if (GetBackEnd() == BackEnd::llvm)
        {
            IdentifierNode staticInitCriticalSection(span, U"System.Runtime.StaticInitCriticalSection");
            TypeSymbol* staticInitCriticalSectionClassType = ResolveType(&staticInitCriticalSection, boundCompileUnit, containerScope);
            std::vector<FunctionScopeLookup> constructorLookups;
            constructorLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            constructorLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, staticInitCriticalSectionClassType->ClassInterfaceOrNsScope()));
            constructorLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> constructorArguments;
            constructorArguments.push_back(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(
                new BoundLocalVariable(module, span, staticConstructorSymbol->CreateTemporary(staticInitCriticalSectionClassType, span))),
                staticInitCriticalSectionClassType->AddPointer(span))));
            constructorArguments.push_back(std::unique_ptr<BoundExpression>(new BoundLiteral(module, std::unique_ptr<Value>(new UuidValue(span,
                boundCompileUnit.Install(classType->TypeId()))), boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))));
            std::unique_ptr<BoundConstructionStatement> constructionStatement(new BoundConstructionStatement(module, ResolveOverload(U"@constructor", containerScope, constructorLookups, constructorArguments,
                boundCompileUnit, boundFunction, span)));
            boundCompoundStatement->AddStatement(std::move(constructionStatement));
            std::unique_ptr<BoundIfStatement> ifStatement2(new BoundIfStatement(module, span, std::unique_ptr<BoundExpression>(new BoundMemberVariable(module, span, classType->InitializedVar())),
                std::unique_ptr<BoundStatement>(new BoundReturnStatement(module, std::unique_ptr<BoundFunctionCall>(nullptr), span)), std::unique_ptr<BoundStatement>(nullptr)));
            boundCompoundStatement->AddStatement(std::move(ifStatement2));
        }
        std::vector<FunctionScopeLookup> assignmentLookups;
        assignmentLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
        assignmentLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
        std::vector<std::unique_ptr<BoundExpression>> assignmentArguments;
        assignmentArguments.push_back(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(new BoundMemberVariable(module, span, classType->InitializedVar())),
            classType->InitializedVar()->GetType()->AddPointer(span))));
        assignmentArguments.push_back(std::unique_ptr<BoundExpression>(new BoundLiteral(module, std::unique_ptr<Value>(new BoolValue(span, true)),
            boundCompileUnit.GetSymbolTable().GetTypeByName(U"bool"))));
        std::unique_ptr<BoundAssignmentStatement> assignmentStatement(new BoundAssignmentStatement(module, ResolveOverload(U"operator=", containerScope, assignmentLookups, assignmentArguments,
            boundCompileUnit, boundFunction, span)));
        boundCompoundStatement->AddStatement(std::move(assignmentStatement));
        ClassTypeSymbol* baseClass = classType->BaseClass();
        if (baseClass)
        {
            if (baseClass->StaticConstructor())
            {
                boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::unique_ptr<BoundExpression>(
                    new BoundFunctionCall(module, span, baseClass->StaticConstructor())))));
            }
        }
        std::unordered_map<std::u32string, MemberInitializerNode*> memberInitializerMap;
        int n = staticConstructorNode->Initializers().Count();
        for (int i = 0; i < n; ++i)
        {
            InitializerNode* initializer = staticConstructorNode->Initializers()[i];
            if (initializer->GetNodeType() == NodeType::thisInitializerNode)
            {
                throw Exception(module, "static constructor cannot have 'this' initializers", initializer->GetSpan());
            }
            else if (initializer->GetNodeType() == NodeType::baseInitializerNode)
            {
                throw Exception(module, "static constructor cannot have 'base' initializers", initializer->GetSpan());
            }
            else if (initializer->GetNodeType() == NodeType::memberInitializerNode)
            {
                MemberInitializerNode* memberInitializer = static_cast<MemberInitializerNode*>(initializer);
                std::u32string memberName = memberInitializer->MemberId()->Str();
                auto it = memberInitializerMap.find(memberName);
                if (it != memberInitializerMap.cend())
                {
                    throw Exception(module, "already has initializer for member variable '" + ToUtf8(memberName) + "'", initializer->GetSpan());
                }
                memberInitializerMap[memberName] = memberInitializer;
            }
        }
        int nm = classType->StaticMemberVariables().size();
        for (int i = 0; i < nm; ++i)
        {
            MemberVariableSymbol* memberVariableSymbol = classType->StaticMemberVariables()[i];
            auto it = memberInitializerMap.find(memberVariableSymbol->Name());
            if (it != memberInitializerMap.cend())
            {
                MemberInitializerNode* memberInitializer = it->second;
                memberInitializerMap.erase(memberInitializer->MemberId()->Str());
                std::vector<FunctionScopeLookup> lookups;
                lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->GetContainerScope()));
                lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                std::vector<std::unique_ptr<BoundExpression>> arguments;
                BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(module, span, memberVariableSymbol);
                std::unique_ptr<BoundExpression> addrOfBoundMemberVariable(new BoundAddressOfExpression(module,
                    std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span)));
                std::unique_ptr<BoundExpression> addrOfBoundMemberVariable2;
                ClassTypeSymbol* memberVariableClassTypeWithDestructor = nullptr;
                if (memberVariableSymbol->GetType()->HasNontrivialDestructor())
                {
                    addrOfBoundMemberVariable2.reset(addrOfBoundMemberVariable->Clone());
                    memberVariableClassTypeWithDestructor = static_cast<ClassTypeSymbol*>(memberVariableSymbol->GetType()->BaseType());
                }
                arguments.push_back(std::move(addrOfBoundMemberVariable));
                int n = memberInitializer->Arguments().Count();
                for (int i = 0; i < n; ++i)
                {
                    Node* argumentNode = memberInitializer->Arguments()[i];
                    std::unique_ptr<BoundExpression> argument = BindExpression(argumentNode, boundCompileUnit, boundFunction, containerScope, statementBinder);
                    arguments.push_back(std::move(argument));
                }
                std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, lookups, arguments, boundCompileUnit, boundFunction, 
                    span);
                boundFunction->MoveTemporaryDestructorCallsTo(*constructorCall);
                boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(constructorCall))));
                if (memberVariableClassTypeWithDestructor)
                {
                    if (GetBackEnd() == BackEnd::llvm)
                    {
                        std::vector<FunctionScopeLookup> enqueueLookups;
                        enqueueLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                        enqueueLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                        std::vector<std::unique_ptr<BoundExpression>> enqueueArguments;
                        enqueueArguments.push_back(std::unique_ptr<BoundExpression>(new BoundBitCast(module, std::unique_ptr<BoundExpression>(new BoundFunctionPtr(module, span,
                            memberVariableClassTypeWithDestructor->Destructor(), boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))),
                            boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))));
                        enqueueArguments.push_back(std::unique_ptr<BoundExpression>(new BoundBitCast(module, std::move(addrOfBoundMemberVariable2),
                            boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))));
                        const char32_t* enqueueDestructorFunction = U"RtEnqueueDestruction";
                        std::unique_ptr<BoundFunctionCall> enqueueDestructorCall = ResolveOverload(enqueueDestructorFunction, containerScope, enqueueLookups, enqueueArguments, boundCompileUnit,
                            boundFunction, span);
                        boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(enqueueDestructorCall))));
                    }
                    else if (GetBackEnd() == BackEnd::cmsx)
                    {
                        std::vector<FunctionScopeLookup> atExitLookups;
                        atExitLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                        atExitLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                        std::vector<std::unique_ptr<BoundExpression>> atExitArguments;
                        atExitArguments.push_back(std::unique_ptr<BoundExpression>(MakeExitEntryPtr(boundCompileUnit, containerScope, span)));
                        atExitArguments.push_back(std::unique_ptr<BoundExpression>(new BoundBitCast(module, std::unique_ptr<BoundExpression>(new BoundFunctionPtr(module, span,
                            memberVariableClassTypeWithDestructor->Destructor(), boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))),
                            boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))));
                        atExitArguments.push_back(std::unique_ptr<BoundExpression>(new BoundBitCast(module, std::move(addrOfBoundMemberVariable2),
                            boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))));
                        const char32_t* atExitFunction = U"at_exit";
                        std::unique_ptr<BoundFunctionCall> atExitCall = ResolveOverload(atExitFunction, containerScope, atExitLookups, atExitArguments, boundCompileUnit,
                            boundFunction, span);
                        boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(atExitCall))));
                    }
                }
            }
            else
            {
                std::vector<FunctionScopeLookup> lookups;
                lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->GetContainerScope()));
                lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                std::vector<std::unique_ptr<BoundExpression>> arguments;
                BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(module, span, memberVariableSymbol);
                std::unique_ptr<BoundExpression> addrOfBoundMemberVariable(
                    new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span)));
                std::unique_ptr<BoundExpression> addrOfBoundMemberVariable2;
                ClassTypeSymbol* memberVariableClassTypeWithDestructor = nullptr;
                if (memberVariableSymbol->GetType()->HasNontrivialDestructor())
                {
                    addrOfBoundMemberVariable2.reset(addrOfBoundMemberVariable->Clone());
                    memberVariableClassTypeWithDestructor = static_cast<ClassTypeSymbol*>(memberVariableSymbol->GetType()->BaseType());
                }
                arguments.push_back(std::move(addrOfBoundMemberVariable));
                std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, lookups, arguments, boundCompileUnit, boundFunction,
                    span);
                boundFunction->MoveTemporaryDestructorCallsTo(*constructorCall);
                boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(constructorCall))));
                if (memberVariableClassTypeWithDestructor)
                {
                    if (GetBackEnd() == BackEnd::llvm)
                    {
                        std::vector<FunctionScopeLookup> enqueueLookups;
                        enqueueLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                        enqueueLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                        std::vector<std::unique_ptr<BoundExpression>> enqueueArguments;
                        enqueueArguments.push_back(std::unique_ptr<BoundExpression>(new BoundBitCast(module, std::unique_ptr<BoundExpression>(new BoundFunctionPtr(module, span,
                            memberVariableClassTypeWithDestructor->Destructor(), boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))),
                            boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))));
                        enqueueArguments.push_back(std::unique_ptr<BoundExpression>(new BoundBitCast(module, std::move(addrOfBoundMemberVariable2),
                            boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))));
                        const char32_t* enqueueDestructorFunction = U"RtEnqueueDestruction";
                        std::unique_ptr<BoundFunctionCall> enqueueDestructorCall = ResolveOverload(enqueueDestructorFunction, containerScope, enqueueLookups, enqueueArguments, boundCompileUnit,
                            boundFunction, span);
                        boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(enqueueDestructorCall))));
                    }
                    else if (GetBackEnd() == BackEnd::cmsx)
                    {
                        std::vector<FunctionScopeLookup> atExitLookups;
                        atExitLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                        atExitLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                        std::vector<std::unique_ptr<BoundExpression>> atExitArguments;
                        atExitArguments.push_back(std::unique_ptr<BoundExpression>(MakeExitEntryPtr(boundCompileUnit, containerScope, span)));
                        atExitArguments.push_back(std::unique_ptr<BoundExpression>(new BoundBitCast(module, std::unique_ptr<BoundExpression>(new BoundFunctionPtr(module, span,
                            memberVariableClassTypeWithDestructor->Destructor(), boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))),
                            boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))));
                        atExitArguments.push_back(std::unique_ptr<BoundExpression>(new BoundBitCast(module, std::move(addrOfBoundMemberVariable2),
                            boundCompileUnit.GetSymbolTable().GetTypeByName(U"void")->AddPointer(span))));
                        const char32_t* atExitFunction = U"at_exit";
                        std::unique_ptr<BoundFunctionCall> atExitCall = ResolveOverload(atExitFunction, containerScope, atExitLookups, atExitArguments, boundCompileUnit,
                            boundFunction, span);
                        boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(atExitCall))));
                    }
                }
            }
        }
    }
    catch (const Exception& ex)
    {
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception(module, "could not generate static initialization for class '" + ToUtf8(classType->FullName()) + "'. Reason: " + ex.Message(), staticConstructorNode->GetSpan(), references);
    }
}

void GenerateClassInitialization(ConstructorSymbol* constructorSymbol, ConstructorNode* constructorNode, BoundCompoundStatement* boundCompoundStatement, BoundFunction* boundFunction, 
    BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, StatementBinder* statementBinder, bool generateDefault, const Span& span)
{
    Module* module = &boundCompileUnit.GetModule();
    Symbol* parent = constructorSymbol->Parent();
    Assert(parent->GetSymbolType() == SymbolType::classTypeSymbol || parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type symbol expected");
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(parent);
    if (generateDefault)
    {
        if (classType->IsStatic())
        {
            throw Exception(module, "cannot create default initialization for class '" + ToUtf8(classType->FullName()) + "'. Reason: class is static", span);
        }
    }
    try
    { 
        if (classType->StaticConstructor())
        {
            boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::unique_ptr<BoundExpression>(new BoundFunctionCall(module, span,
                classType->StaticConstructor())))));
        }
        ParameterSymbol* thisParam = constructorSymbol->GetThisParam();
        Assert(thisParam, "this parameter expected");
        ThisInitializerNode* thisInitializer = nullptr;
        BaseInitializerNode* baseInitializer = nullptr;
        std::unordered_map<std::u32string, MemberInitializerNode*> memberInitializerMap;
        int ni = constructorNode->Initializers().Count();
        for (int i = 0; i < ni; ++i)
        {
            InitializerNode* initializer = constructorNode->Initializers()[i];
            if (initializer->GetNodeType() == NodeType::thisInitializerNode)
            {
                if (thisInitializer)
                {
                    throw Exception(module, "already has 'this' initializer", initializer->GetSpan());
                }
                else if (baseInitializer)
                {
                    throw Exception(module, "cannot have both 'this' and 'base' initializer", initializer->GetSpan());
                }
                thisInitializer = static_cast<ThisInitializerNode*>(initializer);
            }
            else if (initializer->GetNodeType() == NodeType::baseInitializerNode)
            {
                if (baseInitializer)
                {
                    throw Exception(module, "already has 'base' initializer", initializer->GetSpan());
                }
                else if (thisInitializer)
                {
                    throw Exception(module, "cannot have both 'this' and 'base' initializer", initializer->GetSpan());
                }
                baseInitializer = static_cast<BaseInitializerNode*>(initializer);
            }
            else if (initializer->GetNodeType() == NodeType::memberInitializerNode)
            {
                MemberInitializerNode* memberInitializer = static_cast<MemberInitializerNode*>(initializer);
                std::u32string memberName = memberInitializer->MemberId()->Str();
                auto it = memberInitializerMap.find(memberName);
                if (it != memberInitializerMap.cend())
                {
                    throw Exception(module, "already has initializer for member variable '" + ToUtf8(memberName) + "'", initializer->GetSpan());
                }
                memberInitializerMap[memberName] = memberInitializer;
            }
        }
        if (thisInitializer)
        {
            std::vector<FunctionScopeLookup> lookups;
            lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->GetContainerScope()));
            lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> arguments;
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)));
            int n = thisInitializer->Arguments().Count();
            for (int i = 0; i < n; ++i)
            {
                Node* argumentNode = thisInitializer->Arguments()[i];
                std::unique_ptr<BoundExpression> argument = BindExpression(argumentNode, boundCompileUnit, boundFunction, containerScope, statementBinder);
                arguments.push_back(std::move(argument));
            }
            std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, lookups, arguments, boundCompileUnit, boundFunction, span);
            boundFunction->MoveTemporaryDestructorCallsTo(*constructorCall);
            boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(constructorCall))));
        }
        else if (baseInitializer)
        {
            if (!classType->BaseClass())
            {
                throw Exception(module, "class '" + ToUtf8(classType->FullName()) + "' does not have a base class", constructorNode->GetSpan(), classType->GetSpan());
            }
            std::vector<FunctionScopeLookup> lookups;
            lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->BaseClass()->GetContainerScope()));
            lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> arguments;
            ArgumentMatch argumentMatch;
            FunctionSymbol* thisToBaseConversion = boundCompileUnit.GetConversion(thisParam->GetType(), classType->BaseClass()->AddPointer(span), containerScope, boundFunction,
                span, argumentMatch);
            if (!thisToBaseConversion)
            {
                throw Exception(module, "base class conversion not found", constructorNode->GetSpan(), classType->GetSpan());
            }
            BoundExpression* baseClassPointerConversion = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)), thisToBaseConversion);
            arguments.push_back(std::unique_ptr<BoundExpression>(baseClassPointerConversion));
            int n = baseInitializer->Arguments().Count();
            for (int i = 0; i < n; ++i)
            {
                Node* argumentNode = baseInitializer->Arguments()[i];
                std::unique_ptr<BoundExpression> argument = BindExpression(argumentNode, boundCompileUnit, boundFunction, containerScope, statementBinder);
                arguments.push_back(std::move(argument));
            }
            std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, lookups, arguments, boundCompileUnit, boundFunction, span);
            boundFunction->MoveTemporaryDestructorCallsTo(*constructorCall);
            boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(constructorCall))));
        }
        else if (classType->BaseClass())
        {
            std::vector<FunctionScopeLookup> lookups;
            lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->BaseClass()->GetContainerScope()));
            lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> arguments;
            ArgumentMatch argumentMatch;
            FunctionSymbol* thisToBaseConversion = boundCompileUnit.GetConversion(thisParam->GetType(), classType->BaseClass()->AddPointer(span), containerScope, boundFunction,
                span, argumentMatch);
            if (!thisToBaseConversion)
            {
                throw Exception(module, "base class conversion not found", constructorNode->GetSpan(), classType->GetSpan());
            }
            BoundExpression* baseClassPointerConversion = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)), thisToBaseConversion);
            arguments.push_back(std::unique_ptr<BoundExpression>(baseClassPointerConversion));
            bool copyConstructor = constructorSymbol->IsCopyConstructor();
            if (copyConstructor)
            {
                ParameterSymbol* thatParam = constructorSymbol->Parameters()[1];
                ArgumentMatch argumentMatch;
                FunctionSymbol* thatToBaseConversion = boundCompileUnit.GetConversion(thatParam->GetType(), 
                    classType->BaseClass()->AddConst(span)->AddLvalueReference(span), containerScope, boundFunction, span, argumentMatch);
                if (!thatToBaseConversion)
                {
                    throw Exception(module, "base class conversion not found", constructorNode->GetSpan(), classType->GetSpan());
                }
                BoundExpression* baseClassReferenceConversion = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thatParam)), thatToBaseConversion);
                arguments.push_back(std::unique_ptr<BoundExpression>(baseClassReferenceConversion));
            }
            std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, lookups, arguments, boundCompileUnit, boundFunction,
                span);
            boundFunction->MoveTemporaryDestructorCallsTo(*constructorCall);
            boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(constructorCall))));
        }
        if (classType->IsPolymorphic() && !thisInitializer)
        {
            BoundExpression* classPtr = nullptr;
            ClassTypeSymbol* vmtPtrHolderClass = classType->VmtPtrHolderClass();
            if (vmtPtrHolderClass == classType)
            {
                classPtr = new BoundParameter(module, span, thisParam);
            }
            else
            {
                ArgumentMatch argumentMatch;
                FunctionSymbol* thisToHolderConversion = boundCompileUnit.GetConversion(thisParam->GetType(), vmtPtrHolderClass->AddPointer(span), containerScope, boundFunction,
                    span, argumentMatch);
                if (!thisToHolderConversion)
                {
                    throw Exception(module, "base class conversion not found", constructorNode->GetSpan(), classType->GetSpan());
                }
                classPtr = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)), thisToHolderConversion);
            }
            boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundSetVmtPtrStatement(module, std::unique_ptr<BoundExpression>(classPtr), classType)));
        }
        int nm = classType->MemberVariables().size();
        for (int i = 0; i < nm; ++i)
        {
            MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
            auto it = memberInitializerMap.find(memberVariableSymbol->Name());
            if (it != memberInitializerMap.cend())
            {
                MemberInitializerNode* memberInitializer = it->second;
                memberInitializerMap.erase(memberInitializer->MemberId()->Str());
                std::vector<FunctionScopeLookup> lookups;
                lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->GetContainerScope()));
                lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                std::vector<std::unique_ptr<BoundExpression>> arguments;
                BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(module, span, memberVariableSymbol);
                boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)));
                arguments.push_back(std::unique_ptr<BoundExpression>(
                    new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span))));
                int n = memberInitializer->Arguments().Count();
                for (int i = 0; i < n; ++i)
                {
                    Node* argumentNode = memberInitializer->Arguments()[i];
                    std::unique_ptr<BoundExpression> argument = BindExpression(argumentNode, boundCompileUnit, boundFunction, containerScope, statementBinder);
                    arguments.push_back(std::move(argument));
                }
                std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, lookups, arguments, boundCompileUnit, boundFunction, span);
                boundFunction->MoveTemporaryDestructorCallsTo(*constructorCall);
                boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(constructorCall))));
            }
            else if (!thisInitializer)
            {
                if (constructorSymbol->IsCopyConstructor())
                {
                    std::vector<FunctionScopeLookup> lookups;
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->GetContainerScope()));
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                    std::vector<std::unique_ptr<BoundExpression>> arguments;
                    BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(module, span, memberVariableSymbol);
                    boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)));
                    arguments.push_back(std::unique_ptr<BoundExpression>(
                        new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span))));
                    CloneContext cloneContext;
                    DotNode thatMemberVarNode(span, constructorNode->Parameters()[0]->Clone(cloneContext),
                        new IdentifierNode(span, memberVariableSymbol->Name()));
                    std::unique_ptr<BoundExpression> thatMemberVarArgument = BindExpression(&thatMemberVarNode, boundCompileUnit, boundFunction, containerScope, statementBinder);
                    arguments.push_back(std::move(thatMemberVarArgument));
                    std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, lookups, arguments, boundCompileUnit, boundFunction,
                        span);
                    boundFunction->MoveTemporaryDestructorCallsTo(*constructorCall);
                    boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(constructorCall))));
                }
                else if (constructorSymbol->IsMoveConstructor())
                {
                    std::vector<FunctionScopeLookup> lookups;
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->GetContainerScope()));
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                    std::vector<std::unique_ptr<BoundExpression>> arguments;
                    BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(module, span, memberVariableSymbol);
                    boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)));
                    arguments.push_back(std::unique_ptr<BoundExpression>(
                        new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span))));
                    ParameterSymbol* thatParam = constructorSymbol->Parameters()[1];
                    std::unique_ptr<BoundMemberVariable> thatBoundMemberVariable(new BoundMemberVariable(module, span, memberVariableSymbol));
                    thatBoundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(
                        new BoundReferenceToPointerExpression(module, std::unique_ptr<BoundExpression>(
                            new BoundParameter(module, span, thatParam)), thatParam->GetType()->BaseType()->AddPointer(span))));
                    std::vector<FunctionScopeLookup> rvalueLookups;
                    rvalueLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                    rvalueLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                    std::vector<std::unique_ptr<BoundExpression>> rvalueArguments;
                    rvalueArguments.push_back(std::move(thatBoundMemberVariable));
                    std::unique_ptr<BoundFunctionCall> rvalueMemberCall = ResolveOverload(U"System.Rvalue", containerScope, rvalueLookups, rvalueArguments, boundCompileUnit, boundFunction, 
                        span);
                    arguments.push_back(std::move(rvalueMemberCall));
                    std::unique_ptr<BoundFunctionCall> memberConstructorCall = ResolveOverload(U"@constructor", containerScope, lookups, arguments, boundCompileUnit, boundFunction, 
                        span);
                    boundFunction->MoveTemporaryDestructorCallsTo(*memberConstructorCall);
                    boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(memberConstructorCall))));
                }
                else
                {
                    std::vector<FunctionScopeLookup> lookups;
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->GetContainerScope()));
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                    std::vector<std::unique_ptr<BoundExpression>> arguments;
                    BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(module, span, memberVariableSymbol);
                    boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)));
                    arguments.push_back(std::unique_ptr<BoundExpression>(
                        new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span))));
                    std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, lookups, arguments, boundCompileUnit, boundFunction, 
                        span);
                    boundFunction->MoveTemporaryDestructorCallsTo(*constructorCall);
                    boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(constructorCall))));
                }
            }
        }
        if (!memberInitializerMap.empty())
        {
            MemberInitializerNode* initializer = memberInitializerMap.begin()->second;
            throw Exception(module, "no member variable found for initializer named '" + ToUtf8(initializer->MemberId()->Str()) + "'", initializer->GetSpan(), classType->GetSpan());
        }
    }
    catch (const Exception& ex)
    {
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception(module, "could not generate initialization for class '" + ToUtf8(classType->FullName()) + "'. Reason: " + ex.Message(), constructorNode->GetSpan(), references);
    }
}

void GenerateClassAssignment(MemberFunctionSymbol* assignmentFunctionSymbol, MemberFunctionNode* assignmentNode, BoundCompoundStatement* boundCompoundStatement, BoundFunction* boundFunction,
    BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, StatementBinder* statementBinder, bool generateDefault, const Span& span)
{
    Module* module = &boundCompileUnit.GetModule();
    Symbol* parent = assignmentFunctionSymbol->Parent();
    Assert(parent->GetSymbolType() == SymbolType::classTypeSymbol || parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type symbol expected");
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(parent);
    if (generateDefault)
    {
        if (classType->IsStatic())
        {
            throw Exception(module, "cannot create default assigment for class '" + ToUtf8(classType->FullName()) + "'. Reason: class is static", span);
        }
    }
    try
    {
        ParameterSymbol* thisParam = assignmentFunctionSymbol->GetThisParam();
        Assert(thisParam, "this parameter expected");
        if (assignmentFunctionSymbol->IsCopyAssignment())
        {
            if (classType->BaseClass())
            {
                std::vector<FunctionScopeLookup> lookups;
                lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->BaseClass()->GetContainerScope()));
                lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                std::vector<std::unique_ptr<BoundExpression>> arguments;
                ArgumentMatch argumentMatch;
                FunctionSymbol* thisToBaseConversion = boundCompileUnit.GetConversion(thisParam->GetType(), classType->BaseClass()->AddPointer(span), containerScope, boundFunction,
                    span, argumentMatch);
                if (!thisToBaseConversion)
                {
                    throw Exception(module, "base class conversion not found", assignmentNode->GetSpan(), classType->GetSpan());
                }
                BoundExpression* baseClassPointerConversion = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)), thisToBaseConversion);
                arguments.push_back(std::unique_ptr<BoundExpression>(baseClassPointerConversion));
                ParameterSymbol* thatParam = assignmentFunctionSymbol->Parameters()[1];
                FunctionSymbol* thatToBaseConversion = boundCompileUnit.GetConversion(thatParam->GetType(),
                    classType->BaseClass()->AddConst(span)->AddLvalueReference(span), containerScope, boundFunction, span, argumentMatch);
                if (!thatToBaseConversion)
                {
                    throw Exception(module, "base class conversion not found", assignmentNode->GetSpan(), classType->GetSpan());
                }
                BoundExpression* baseClassReferenceConversion = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thatParam)), thatToBaseConversion);
                arguments.push_back(std::unique_ptr<BoundExpression>(baseClassReferenceConversion));
                std::unique_ptr<BoundFunctionCall> assignmentCall = ResolveOverload(U"operator=", containerScope, lookups, arguments, boundCompileUnit, boundFunction,
                    span);
                boundFunction->MoveTemporaryDestructorCallsTo(*assignmentCall);
                boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(assignmentCall))));
            }
            if (generateDefault)
            {
                int n = classType->MemberVariables().size();
                for (int i = 0; i < n; ++i)
                {
                    MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
                    std::vector<FunctionScopeLookup> lookups;
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->GetContainerScope()));
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                    std::vector<std::unique_ptr<BoundExpression>> arguments;
                    BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(module, span, memberVariableSymbol);
                    boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)));
                    arguments.push_back(std::unique_ptr<BoundExpression>(
                        new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span))));
                    CloneContext cloneContext;
                    DotNode thatMemberVarNode(span, assignmentNode->Parameters()[0]->Clone(cloneContext),
                        new IdentifierNode(span, memberVariableSymbol->Name()));
                    std::unique_ptr<BoundExpression> thatMemberVarArgument = BindExpression(&thatMemberVarNode, boundCompileUnit, boundFunction, containerScope, statementBinder);
                    arguments.push_back(std::move(thatMemberVarArgument));
                    std::unique_ptr<BoundFunctionCall> assignmentCall = ResolveOverload(U"operator=", containerScope, lookups, arguments, boundCompileUnit, boundFunction,
                        span);
                    boundFunction->MoveTemporaryDestructorCallsTo(*assignmentCall);
                    boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(assignmentCall))));
                }
            }
        }
        else if (assignmentFunctionSymbol->IsMoveAssignment())
        {
            if (classType->BaseClass())
            {
                std::vector<FunctionScopeLookup> lookups;
                lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->BaseClass()->GetContainerScope()));
                lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                std::vector<std::unique_ptr<BoundExpression>> arguments;
                ArgumentMatch argumentMatch;
                FunctionSymbol* thisToBaseConversion = boundCompileUnit.GetConversion(thisParam->GetType(), classType->BaseClass()->AddPointer(span), containerScope, 
                    boundFunction, span, argumentMatch);
                if (!thisToBaseConversion)
                {
                    throw Exception(module, "base class conversion not found", assignmentNode->GetSpan(), classType->GetSpan());
                }
                BoundExpression* baseClassPointerConversion = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)), thisToBaseConversion);
                arguments.push_back(std::unique_ptr<BoundExpression>(baseClassPointerConversion));
                ParameterSymbol* thatParam = assignmentFunctionSymbol->Parameters()[1];
                FunctionSymbol* thatToBaseConversion = boundCompileUnit.GetConversion(thatParam->GetType(),
                    classType->BaseClass()->AddRvalueReference(span), containerScope, boundFunction, span, argumentMatch);
                if (!thatToBaseConversion)
                {
                    throw Exception(module, "base class conversion not found", assignmentNode->GetSpan(), classType->GetSpan());
                }
                BoundExpression* baseClassReferenceConversion = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thatParam)), thatToBaseConversion);
                arguments.push_back(std::unique_ptr<BoundExpression>(baseClassReferenceConversion));
                std::unique_ptr<BoundFunctionCall> assignmentCall = ResolveOverload(U"operator=", containerScope, lookups, arguments, boundCompileUnit, boundFunction,
                    span);
                boundFunction->MoveTemporaryDestructorCallsTo(*assignmentCall);
                boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(assignmentCall))));
            }
            if (generateDefault)
            {
                int n = classType->MemberVariables().size();
                for (int i = 0; i < n; ++i)
                {
                    MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
                    std::vector<FunctionScopeLookup> lookups;
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->GetContainerScope()));
                    lookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                    std::vector<std::unique_ptr<BoundExpression>> arguments;
                    BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(module, span, memberVariableSymbol);
                    boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)));
                    arguments.push_back(std::unique_ptr<BoundExpression>(boundMemberVariable));
                    BoundMemberVariable* thatBoundMemberVariable = new BoundMemberVariable(module, span, memberVariableSymbol);
                    ParameterSymbol* thatParam = assignmentFunctionSymbol->Parameters()[1];
                    TypeSymbol* thatPtrType = thatParam->GetType()->RemoveReference(span)->AddPointer(span);
                    thatBoundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundReferenceToPointerExpression(module,
                        std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thatParam)), thatPtrType)));
                    arguments.push_back(std::unique_ptr<BoundExpression>(thatBoundMemberVariable));
                    std::unique_ptr<BoundFunctionCall> swapCall = ResolveOverload(U"System.Swap", containerScope, lookups, arguments, boundCompileUnit, boundFunction, span);
                    boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(swapCall))));
                }
            }
        }
    }
    catch (const Exception& ex)
    {
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception(module, "could not generate assignment for class '" + ToUtf8(classType->FullName()) + "'. Reason: " + ex.Message(), assignmentNode->GetSpan(), references);
    }
}

void GenerateClassTermination(DestructorSymbol* destructorSymbol, DestructorNode* destructorNode, BoundCompoundStatement* boundCompoundStatement, BoundFunction* boundFunction,
    BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, StatementBinder* statementBinder, const Span& span)
{
    Module* module = &boundCompileUnit.GetModule();
    Symbol* parent = destructorSymbol->Parent();
    Assert(parent->GetSymbolType() == SymbolType::classTypeSymbol || parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type symbol expected");
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(parent);
    try
    {
        ParameterSymbol* thisParam = destructorSymbol->GetThisParam();
        Assert(thisParam, "this parameter expected");
        if (classType->IsPolymorphic())
        {
            ParameterSymbol* thisParam = destructorSymbol->Parameters()[0];
            BoundExpression* classPtr = nullptr;
            ClassTypeSymbol* vmtPtrHolderClass = classType->VmtPtrHolderClass();
            if (vmtPtrHolderClass == classType)
            {
                classPtr = new BoundParameter(module, span, thisParam);
            }
            else
            {
                ArgumentMatch argumentMatch;
                FunctionSymbol* thisToHolderConversion = boundCompileUnit.GetConversion(thisParam->GetType(), vmtPtrHolderClass->AddPointer(span), containerScope, boundFunction,
                    span, argumentMatch);
                if (!thisToHolderConversion)
                {
                    throw Exception(module, "base class conversion not found", destructorNode->GetSpan(), classType->GetSpan());
                }
                classPtr = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)), thisToHolderConversion);
            }
            boundCompoundStatement->InsertStatementToFront(std::unique_ptr<BoundStatement>(new BoundSetVmtPtrStatement(module, std::unique_ptr<BoundExpression>(classPtr), classType)));
        }
        int n = classType->MemberVariables().size();
        for (int i = n - 1; i >= 0; --i)
        {
            MemberVariableSymbol* memberVariableSymbol = classType->MemberVariables()[i];
            if (memberVariableSymbol->GetType()->HasNontrivialDestructor())
            {
                std::vector<FunctionScopeLookup> memberDestructorCallLookups;
                memberDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
                memberDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, memberVariableSymbol->GetType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
                memberDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
                std::vector<std::unique_ptr<BoundExpression>> memberDestructorCallArguments;
                BoundMemberVariable* boundMemberVariable = new BoundMemberVariable(module, span, memberVariableSymbol);
                boundMemberVariable->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(module, span, destructorSymbol->GetThisParam())));
                memberDestructorCallArguments.push_back(std::unique_ptr<BoundExpression>(
                    new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(boundMemberVariable), boundMemberVariable->GetType()->AddPointer(span))));
                std::unique_ptr<BoundFunctionCall> memberDestructorCall = ResolveOverload(U"@destructor", containerScope, memberDestructorCallLookups, memberDestructorCallArguments,
                    boundCompileUnit, boundFunction, span);
                boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(memberDestructorCall))));
            }
        }
        if (classType->BaseClass() && classType->BaseClass()->HasNontrivialDestructor())
        {
            std::vector<FunctionScopeLookup> baseDestructorCallLookups;
            baseDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            baseDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->BaseClass()->GetContainerScope()));
            baseDestructorCallLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
            std::vector<std::unique_ptr<BoundExpression>> baseDestructorCallArguments;
            ArgumentMatch argumentMatch;
            FunctionSymbol* thisToBaseConversion = boundCompileUnit.GetConversion(thisParam->GetType(), classType->BaseClass()->AddPointer(span), containerScope, boundFunction,
                span, argumentMatch);
            if (!thisToBaseConversion)
            {
                throw Exception(module, "base class conversion not found", destructorNode->GetSpan(), classType->GetSpan());
            }
            BoundExpression* baseClassPointerConversion = new BoundConversion(module, std::unique_ptr<BoundExpression>(new BoundParameter(module, span, thisParam)), thisToBaseConversion);
            baseDestructorCallArguments.push_back(std::unique_ptr<BoundExpression>(baseClassPointerConversion));
            std::unique_ptr<BoundFunctionCall> baseDestructorCall = ResolveOverload(U"@destructor", containerScope, baseDestructorCallLookups, baseDestructorCallArguments, boundCompileUnit,
                boundFunction, span);
            boundCompoundStatement->AddStatement(std::unique_ptr<BoundStatement>(new BoundExpressionStatement(module, std::move(baseDestructorCall))));
        }
    }
    catch (const Exception& ex)
    {
        std::vector<Span> references;
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception(module, "could not generate termination for class '" + ToUtf8(classType->FullName()) + "'. Reason: " + ex.Message(), destructorNode->GetSpan(), references);
    }
}

Operation::Operation(const std::u32string& groupName_, int arity_, BoundCompileUnit& boundCompileUnit_) : 
    groupName(groupName_), arity(arity_), boundCompileUnit(boundCompileUnit_), module(&boundCompileUnit.GetModule())
{
}

Operation::~Operation()
{
}

SymbolTable* Operation::GetSymbolTable()
{
    return &boundCompileUnit.GetSymbolTable();
}

BoundCompileUnit& Operation::GetBoundCompileUnit()
{
    return boundCompileUnit;
}

void ArityOperation::Add(Operation* operation)
{
    operations.push_back(operation);
}

void ArityOperation::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    for (Operation* operation : operations)
    {
        operation->CollectViableFunctions(containerScope, arguments, currentFunction, viableFunctions, exception, span);
    }
}

void OperationGroup::Add(Operation* operation)
{
    int arity = operation->Arity();
    if (arity >= arityOperations.size())
    {
        arityOperations.resize(arity + 1);
    }
    ArityOperation* arityOperation = arityOperations[arity].get();
    if (!arityOperation)
    {
        arityOperation = new ArityOperation();
        arityOperations[arity].reset(arityOperation);
    }
    arityOperation->Add(operation);
}

void OperationGroup::CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
    ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    int arity = arguments.size();
    if (arity < arityOperations.size())
    {
        ArityOperation* arityOperation = arityOperations[arity].get();
        if (arityOperation)
        {
            arityOperation->CollectViableFunctions(containerScope, arguments, currentFunction, viableFunctions, exception, span);
        }
    }
}

OperationRepository::OperationRepository(BoundCompileUnit& boundCompileUnit_) : boundCompileUnit(boundCompileUnit_), copyConstructorOperation(nullptr)
{
    Add(new LvalueReferenceCopyConstructorOperation(boundCompileUnit));
    Add(new LvalueReferenceCopyAssignmentOperation(boundCompileUnit));
    Add(new LvalueReferenceMoveAssignmentOperation(boundCompileUnit));
    Add(new LvalueReferenceReturnOperation(boundCompileUnit));
    Add(new RvalueReferenceCopyConstructorOperation(boundCompileUnit));
    Add(new RvalueReferenceCopyAssignmentOperation(boundCompileUnit));
    Add(new RvalueReferenceReturnOperation(boundCompileUnit));
    Add(new PointerDefaultConstructorOperation(boundCompileUnit));
    Add(new PointerCopyConstructorOperation(boundCompileUnit));
    Add(new PointerMoveConstructorOperation(boundCompileUnit));
    Add(new PointerCopyAssignmentOperation(boundCompileUnit));
    Add(new PointerMoveAssignmentOperation(boundCompileUnit));
    Add(new PointerReturnOperation(boundCompileUnit));
    Add(new PointerPlusOffsetOperation(boundCompileUnit));
    Add(new OffsetPlusPointerOperation(boundCompileUnit));
    Add(new PointerMinusOffsetOperation(boundCompileUnit));
    Add(new PointerMinusPointerOperation(boundCompileUnit));
    Add(new PointerEqualOperation(boundCompileUnit));
    Add(new PointerLessOperation(boundCompileUnit));
    Add(new PointerArrowOperation(boundCompileUnit));
    Add(new ClassDefaultConstructorOperation(boundCompileUnit));
    copyConstructorOperation = new ClassCopyConstructorOperation(boundCompileUnit);
    Add(copyConstructorOperation);
    Add(new ClassMoveConstructorOperation(boundCompileUnit));
    Add(new ClassCopyAssignmentOperation(boundCompileUnit));
    Add(new ClassMoveAssignmentOperation(boundCompileUnit));
    Add(new ArrayDefaultConstructorOperation(boundCompileUnit));
    Add(new ArrayCopyConstructorOperation(boundCompileUnit));
    Add(new ArrayMoveConstructorOperation(boundCompileUnit));
    Add(new ArrayCopyAssignmentOperation(boundCompileUnit));
    Add(new ArrayMoveAssignmentOperation(boundCompileUnit));
    Add(new ArrayElementAccessOperation(boundCompileUnit));
    Add(new InterfaceDefaultConstructorOperation(boundCompileUnit));
    Add(new InterfaceCopyConstructorOperation(boundCompileUnit));
    Add(new InterfaceMoveConstructorOperation(boundCompileUnit));
    Add(new InterfaceCopyAssignmentOperation(boundCompileUnit));
    Add(new InterfaceMoveAssignmentOperation(boundCompileUnit));
}

void OperationRepository::Add(Operation* operation)
{
    OperationGroup* group = nullptr;
    auto it = operationGroupMap.find(operation->GroupName());
    if (it != operationGroupMap.cend())
    {
        group = it->second;
    }
    else
    {
        group = new OperationGroup();
        operationGroupMap.insert(std::make_pair(operation->GroupName(), group));
        operationGroups.push_back(std::unique_ptr<OperationGroup>(group));
    }
    group->Add(operation);
    operations.push_back(std::unique_ptr<Operation>(operation));
}

void OperationRepository::CollectViableFunctions(const std::u32string& groupName, ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, 
    BoundFunction* currentFunction, ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span)
{
    auto it = operationGroupMap.find(groupName);
    if (it != operationGroupMap.cend())
    {
        OperationGroup* operationGroup = it->second;
        operationGroup->CollectViableFunctions(containerScope, arguments, currentFunction, viableFunctions, exception, span);
    }
}

void OperationRepository::GenerateCopyConstructorFor(ClassTypeSymbol* classTypeSymbol, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span)
{
    if (boundCompileUnit.HasCopyConstructorFor(classTypeSymbol->TypeId())) return;
    std::unique_ptr<ClassCopyConstructor> copyConstructor(new ClassCopyConstructor(classTypeSymbol, span));
    copyConstructor->SetCompileUnit(boundCompileUnit.GetCompileUnitNode());
    copyConstructor->SetModule(&boundCompileUnit.GetModule());
    ClassCopyConstructorOperation* copyConstructorOp = static_cast<ClassCopyConstructorOperation*>(copyConstructorOperation);
    std::unique_ptr<Exception> exception;
    if (copyConstructorOp->GenerateImplementation(copyConstructor.get(), containerScope, currentFunction, exception, span))
    {
        copyConstructor->SetModule(&boundCompileUnit.GetModule());
        copyConstructor->SetParent(classTypeSymbol);
        copyConstructor->SetLinkOnceOdrLinkage();
        copyConstructor->SetInline();
        boundCompileUnit.AddCopyConstructorFor(classTypeSymbol->TypeId(), std::move(copyConstructor));
    }
    else
    {
        if (exception)
        {
            throw *exception;
        }
        else
        {
            throw Exception(classTypeSymbol->GetModule(), "could not generate copy constructor for class '" + ToUtf8(classTypeSymbol->FullName()) + "'", span);
        }
    }
}

void OperationRepository::GenerateCopyConstructorFor(InterfaceTypeSymbol* interfaceTypeSymbol, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span)
{
    if (boundCompileUnit.HasCopyConstructorFor(interfaceTypeSymbol->TypeId())) return;
    std::unique_ptr<InterfaceTypeCopyConstructor> copyConstructor(new InterfaceTypeCopyConstructor(interfaceTypeSymbol, span));
    boundCompileUnit.GetSymbolTable().SetFunctionIdFor(copyConstructor.get());
    copyConstructor->SetCompileUnit(boundCompileUnit.GetCompileUnitNode());
    copyConstructor->SetModule(&boundCompileUnit.GetModule());
    copyConstructor->SetParent(interfaceTypeSymbol);
    boundCompileUnit.AddCopyConstructorFor(interfaceTypeSymbol->TypeId(), std::move(copyConstructor));
}

} } // namespace cmajor::binder
