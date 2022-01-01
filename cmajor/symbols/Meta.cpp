// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Meta.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

IntrinsicFunction::IntrinsicFunction()
{
}

IntrinsicFunction::~IntrinsicFunction()
{
}

std::unique_ptr<Value> IntrinsicFunction::Evaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId)
{
    if (arguments.size() != Arity())
    {
        throw Exception("wrong number of parameters for intrinsic " + std::string(GroupName()), span, moduleId);
    }
    if (templateArguments.size() != NumberOfTypeParameters())
    {
        throw Exception("wrong number of template type arguments for intrinsic " + std::string(GroupName()), span, moduleId);
    }
    return DoEvaluate(arguments, templateArguments, span, moduleId);
}

FunctionSymbol* CreateIntrinsic(IntrinsicFunction* intrinsic, SymbolTable& symbolTable, ContainerSymbol* parent)
{
    FunctionSymbol* fun = new FunctionSymbol(Span(), boost::uuids::nil_uuid(), ToUtf32(intrinsic->GroupName()));
    fun->SetModule(symbolTable.GetModule());
    fun->SetGroupName(ToUtf32(intrinsic->GroupName()));
    fun->SetIntrinsic(intrinsic);
    fun->SetAccess(SymbolAccess::public_);
    fun->SetReturnType(intrinsic->ReturnType(symbolTable));
    fun->SetParent(parent);
    int n = intrinsic->NumberOfTypeParameters();
    for (int i = 0; i < n; ++i)
    {
        std::u32string p = U"T";
        if (i > 0)
        {
            p.append(ToUtf32(std::to_string(i)));
        }
        TemplateParameterSymbol* s = new TemplateParameterSymbol(Span(), boost::uuids::nil_uuid(), p);
        symbolTable.SetTypeIdFor(s);
        s->SetModule(symbolTable.GetModule());
        fun->AddMember(s);
    }
    fun->ComputeName();
    return fun;
}

class TypePredicate : public IntrinsicFunction
{
public:
    TypePredicate();
    int Arity() const override { return 0; }
    int NumberOfTypeParameters() const override { return 1; }
    TypeSymbol* ReturnType(SymbolTable& symbolTable) const override { return symbolTable.GetTypeByName(U"bool"); }
};

TypePredicate::TypePredicate() : IntrinsicFunction()
{
}

class IsIntegralTypePredicate : public TypePredicate
{
public:
    IsIntegralTypePredicate();
    const char* GroupName() const override { return "IsIntegralType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsIntegralType()));
    }
};

IsIntegralTypePredicate::IsIntegralTypePredicate() : TypePredicate()
{
}

class IsSignedTypePredicate : public TypePredicate
{
public:
    IsSignedTypePredicate();
    const char* GroupName() const override { return "IsSignedType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsIntegralType() && !type->IsUnsignedType()));
    }
};

IsSignedTypePredicate::IsSignedTypePredicate() : TypePredicate()
{
}

class IsUnsignedTypePredicate : public TypePredicate
{
public:
    IsUnsignedTypePredicate();
    const char* GroupName() const override { return "IsUnsignedType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsUnsignedType()));
    }
};

IsUnsignedTypePredicate::IsUnsignedTypePredicate() : TypePredicate()
{
}

class IsFloatingPointTypePredicate : public TypePredicate
{
public:
    IsFloatingPointTypePredicate();
    const char* GroupName() const override { return "IsFloatingPointType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsFloatingPointType()));
    }
};

IsFloatingPointTypePredicate::IsFloatingPointTypePredicate() : TypePredicate()
{
}

class IsBasicTypePredicate : public TypePredicate
{
public:
    IsBasicTypePredicate();
    const char* GroupName() const override { return "IsBasicType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsBasicTypeSymbol()));
    }
};

IsBasicTypePredicate::IsBasicTypePredicate() : TypePredicate()
{
}

class IsBoolTypePredicate : public TypePredicate
{
public:
    IsBoolTypePredicate();
    const char* GroupName() const override { return "IsBoolType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::boolTypeSymbol));
    }
};

IsBoolTypePredicate::IsBoolTypePredicate() : TypePredicate()
{
}

class IsSByteTypePredicate : public TypePredicate
{
public:
    IsSByteTypePredicate();
    const char* GroupName() const override { return "IsSByteType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::sbyteTypeSymbol));
    }
};

IsSByteTypePredicate::IsSByteTypePredicate() : TypePredicate()
{
}

class IsByteTypePredicate : public TypePredicate
{
public:
    IsByteTypePredicate();
    const char* GroupName() const override { return "IsByteType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::byteTypeSymbol));
    }
};

IsByteTypePredicate::IsByteTypePredicate() : TypePredicate()
{
}

class IsShortTypePredicate : public TypePredicate
{
public:
    IsShortTypePredicate();
    const char* GroupName() const override { return "IsShortType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::shortTypeSymbol));
    }
};

IsShortTypePredicate::IsShortTypePredicate() : TypePredicate()
{
}

class IsUShortTypePredicate : public TypePredicate
{
public:
    IsUShortTypePredicate();
    const char* GroupName() const override { return "IsUShortType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::ushortTypeSymbol));
    }
};

IsUShortTypePredicate::IsUShortTypePredicate() : TypePredicate()
{
}

class IsIntTypePredicate : public TypePredicate
{
public:
    IsIntTypePredicate();
    const char* GroupName() const override { return "IsIntType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::intTypeSymbol));
    }
};

IsIntTypePredicate::IsIntTypePredicate() : TypePredicate()
{
}

class IsUIntTypePredicate : public TypePredicate
{
public:
    IsUIntTypePredicate();
    const char* GroupName() const override { return "IsUIntType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::uintTypeSymbol));
    }
};

IsUIntTypePredicate::IsUIntTypePredicate() : TypePredicate()
{
}

class IsLongTypePredicate : public TypePredicate
{
public:
    IsLongTypePredicate();
    const char* GroupName() const override { return "IsLongType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::longTypeSymbol));
    }
};

IsLongTypePredicate::IsLongTypePredicate() : TypePredicate()
{
}

class IsULongTypePredicate : public TypePredicate
{
public:
    IsULongTypePredicate();
    const char* GroupName() const override { return "IsULongType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::ulongTypeSymbol));
    }
};

IsULongTypePredicate::IsULongTypePredicate() : TypePredicate()
{
}

class IsFloatTypePredicate : public TypePredicate
{
public:
    IsFloatTypePredicate();
    const char* GroupName() const override { return "IsFloatType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::floatTypeSymbol));
    }
};

IsFloatTypePredicate::IsFloatTypePredicate() : TypePredicate()
{
}

class IsDoubleTypePredicate : public TypePredicate
{
public:
    IsDoubleTypePredicate();
    const char* GroupName() const override { return "IsDoubleType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::doubleTypeSymbol));
    }
};

IsDoubleTypePredicate::IsDoubleTypePredicate() : TypePredicate()
{
}

class IsCharTypePredicate : public TypePredicate
{
public:
    IsCharTypePredicate();
    const char* GroupName() const override { return "IsCharType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::charTypeSymbol));
    }
};

IsCharTypePredicate::IsCharTypePredicate() : TypePredicate()
{
}

class IsWCharTypePredicate : public TypePredicate
{
public:
    IsWCharTypePredicate();
    const char* GroupName() const override { return "IsWCharType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::wcharTypeSymbol));
    }
};

IsWCharTypePredicate::IsWCharTypePredicate() : TypePredicate()
{
}

class IsUCharTypePredicate : public TypePredicate
{
public:
    IsUCharTypePredicate();
    const char* GroupName() const override { return "IsUCharType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::ucharTypeSymbol));
    }
};

IsUCharTypePredicate::IsUCharTypePredicate() : TypePredicate()
{
}

class IsVoidTypePredicate : public TypePredicate
{
public:
    IsVoidTypePredicate();
    const char* GroupName() const override { return "IsVoidType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::voidTypeSymbol));
    }
};

IsVoidTypePredicate::IsVoidTypePredicate() : TypePredicate()
{
}

class IsClassTypePredicate : public TypePredicate
{
public:
    IsClassTypePredicate();
    const char* GroupName() const override { return "IsClassType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsClassTypeSymbol()));
    }
};

IsClassTypePredicate::IsClassTypePredicate() : TypePredicate()
{
}

class IsPolymorphicTypePredicate : public TypePredicate
{
public:
    IsPolymorphicTypePredicate();
    const char* GroupName() const override { return "IsPolymorphicType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsPolymorphicType()));
    }
};

IsPolymorphicTypePredicate::IsPolymorphicTypePredicate() : TypePredicate()
{
}

class IsInterfaceTypePredicate : public TypePredicate
{
public:
    IsInterfaceTypePredicate();
    const char* GroupName() const override { return "IsInterfaceType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->GetSymbolType() == SymbolType::interfaceTypeSymbol));
    }
};

IsInterfaceTypePredicate::IsInterfaceTypePredicate() : TypePredicate()
{
}

class IsDelegateTypePredicate : public TypePredicate
{
public:
    IsDelegateTypePredicate();
    const char* GroupName() const override { return "IsDelegateType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsDelegateType()));
    }
};

IsDelegateTypePredicate::IsDelegateTypePredicate() : TypePredicate()
{
}

class IsClassDelegateTypePredicate : public TypePredicate
{
public:
    IsClassDelegateTypePredicate();
    const char* GroupName() const override { return "IsClassDelegateType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsClassDelegateType()));
    }
};

IsClassDelegateTypePredicate::IsClassDelegateTypePredicate() : TypePredicate()
{
}

class IsEnumeratedTypePredicate : public TypePredicate
{
public:
    IsEnumeratedTypePredicate();
    const char* GroupName() const override { return "IsEnumeratedType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsEnumeratedType()));
    }
};

IsEnumeratedTypePredicate::IsEnumeratedTypePredicate() : TypePredicate()
{
}

class IsConstTypePredicate : public TypePredicate
{
public:
    IsConstTypePredicate();
    const char* GroupName() const override { return "IsConstType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsConstType()));
    }
};

IsConstTypePredicate::IsConstTypePredicate() : TypePredicate()
{
}

class IsReferenceTypePredicate : public TypePredicate
{
public:
    IsReferenceTypePredicate();
    const char* GroupName() const override { return "IsReferenceType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsReferenceType()));
    }
};

IsReferenceTypePredicate::IsReferenceTypePredicate() : TypePredicate()
{
}

class IsLvalueReferenceTypePredicate : public TypePredicate
{
public:
    IsLvalueReferenceTypePredicate();
    const char* GroupName() const override { return "IsLvalueReferenceType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsLvalueReferenceType()));
    }
};

IsLvalueReferenceTypePredicate::IsLvalueReferenceTypePredicate() : TypePredicate()
{
}

class IsRvalueReferenceTypePredicate : public TypePredicate
{
public:
    IsRvalueReferenceTypePredicate();
    const char* GroupName() const override { return "IsRvalueReferenceType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsRvalueReferenceType()));
    }
};

IsRvalueReferenceTypePredicate::IsRvalueReferenceTypePredicate() : TypePredicate()
{
}

class IsArrayTypePredicate : public TypePredicate
{
public:
    IsArrayTypePredicate();
    const char* GroupName() const override { return "IsArrayType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsArrayType()));
    }
};

IsArrayTypePredicate::IsArrayTypePredicate() : TypePredicate()
{
}

class IsPointerTypePredicate : public TypePredicate
{
public:
    IsPointerTypePredicate();
    const char* GroupName() const override { return "IsPointerType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsPointerType()));
    }
};

IsPointerTypePredicate::IsPointerTypePredicate() : TypePredicate()
{
}

class IsGenericPtrTypePredicate : public TypePredicate
{
public:
    IsGenericPtrTypePredicate();
    const char* GroupName() const override { return "IsGenericPtrType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, moduleId, type->IsVoidPtrType()));
    }
};

IsGenericPtrTypePredicate::IsGenericPtrTypePredicate() : TypePredicate()
{
}

class PointerCountIntrinsicFunction : public IntrinsicFunction
{
public:
    PointerCountIntrinsicFunction();
    int Arity() const override { return 0; }
    int NumberOfTypeParameters() const override { return 1; }
    TypeSymbol* ReturnType(SymbolTable& symbolTable) const override { return symbolTable.GetTypeByName(U"int"); }
    const char* GroupName() const override { return "PointerCount"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new IntValue(span, moduleId, type->PointerCount()));
    }
};

PointerCountIntrinsicFunction::PointerCountIntrinsicFunction() : IntrinsicFunction()
{
}

class ArrayLengthIntrinsicFunction : public IntrinsicFunction
{
public:
    ArrayLengthIntrinsicFunction();
    int Arity() const override { return 0; }
    int NumberOfTypeParameters() const override { return 1; }
    TypeSymbol* ReturnType(SymbolTable& symbolTable) const override { return symbolTable.GetTypeByName(U"long"); }
    const char* GroupName() const override { return "ArrayLength"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span, const boost::uuids::uuid& moduleId) override
    {
        TypeSymbol* type = templateArguments.front();
        if (type->IsArrayType())
        {
            ArrayTypeSymbol* arrayType = static_cast<ArrayTypeSymbol*>(type);
            return std::unique_ptr<Value>(new LongValue(span, moduleId, arrayType->Size()));
        }
        return std::unique_ptr<Value>(new LongValue(span, moduleId, 0));
    }
};

ArrayLengthIntrinsicFunction::ArrayLengthIntrinsicFunction() : IntrinsicFunction()
{
}

void MetaInit(SymbolTable& symbolTable)
{
    symbolTable.BeginNamespace(U"System.Meta", Span(), boost::uuids::nil_uuid());
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsIntegralTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsSignedTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsUnsignedTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsFloatingPointTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsBasicTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsBoolTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsSByteTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsByteTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsShortTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsUShortTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsIntTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsUIntTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsLongTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsULongTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsFloatTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsDoubleTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsCharTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsWCharTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsUCharTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsVoidTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsClassTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsPolymorphicTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsInterfaceTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsDelegateTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsClassDelegateTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsEnumeratedTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsConstTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsReferenceTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsLvalueReferenceTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsRvalueReferenceTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsArrayTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsPointerTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsGenericPtrTypePredicate(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new PointerCountIntrinsicFunction(), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new ArrayLengthIntrinsicFunction(), symbolTable, symbolTable.Container()));
    symbolTable.EndNamespace(); 
}

} } // namespace cmajor::symbols
