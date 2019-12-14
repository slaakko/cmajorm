// =================================
// Copyright (c) 2019 Seppo Laakko
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

IntrinsicFunction::IntrinsicFunction(Module* module_) : module(module_)
{
}

IntrinsicFunction::~IntrinsicFunction()
{
}

std::unique_ptr<Value> IntrinsicFunction::Evaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span)
{
    if (arguments.size() != Arity())
    {
        throw Exception(module, "wrong number of parameters for intrinsic " + std::string(GroupName()), span);
    }
    if (templateArguments.size() != NumberOfTypeParameters())
    {
        throw Exception(module, "wrong number of template type arguments for intrinsic " + std::string(GroupName()), span);
    }
    return DoEvaluate(arguments, templateArguments, span);
}

FunctionSymbol* CreateIntrinsic(IntrinsicFunction* intrinsic, SymbolTable& symbolTable, ContainerSymbol* parent)
{
    FunctionSymbol* fun = new FunctionSymbol(Span(), ToUtf32(intrinsic->GroupName()));
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
        TemplateParameterSymbol* s = new TemplateParameterSymbol(Span(), p);
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
    TypePredicate(Module* module_);
    int Arity() const override { return 0; }
    int NumberOfTypeParameters() const override { return 1; }
    TypeSymbol* ReturnType(SymbolTable& symbolTable) const override { return symbolTable.GetTypeByName(U"bool"); }
};

TypePredicate::TypePredicate(Module* module_) : IntrinsicFunction(module_)
{
}

class IsIntegralTypePredicate : public TypePredicate
{
public:
    IsIntegralTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsIntegralType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsIntegralType()));
    }
};

IsIntegralTypePredicate::IsIntegralTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsSignedTypePredicate : public TypePredicate
{
public:
    IsSignedTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsSignedType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsIntegralType() && !type->IsUnsignedType()));
    }
};

IsSignedTypePredicate::IsSignedTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsUnsignedTypePredicate : public TypePredicate
{
public:
    IsUnsignedTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsUnsignedType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsUnsignedType()));
    }
};

IsUnsignedTypePredicate::IsUnsignedTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsFloatingPointTypePredicate : public TypePredicate
{
public:
    IsFloatingPointTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsFloatingPointType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsFloatingPointType()));
    }
};

IsFloatingPointTypePredicate::IsFloatingPointTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsBasicTypePredicate : public TypePredicate
{
public:
    IsBasicTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsBasicType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsBasicTypeSymbol()));
    }
};

IsBasicTypePredicate::IsBasicTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsBoolTypePredicate : public TypePredicate
{
public:
    IsBoolTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsBoolType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::boolTypeSymbol));
    }
};

IsBoolTypePredicate::IsBoolTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsSByteTypePredicate : public TypePredicate
{
public:
    IsSByteTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsSByteType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::sbyteTypeSymbol));
    }
};

IsSByteTypePredicate::IsSByteTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsByteTypePredicate : public TypePredicate
{
public:
    IsByteTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsByteType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::byteTypeSymbol));
    }
};

IsByteTypePredicate::IsByteTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsShortTypePredicate : public TypePredicate
{
public:
    IsShortTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsShortType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::shortTypeSymbol));
    }
};

IsShortTypePredicate::IsShortTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsUShortTypePredicate : public TypePredicate
{
public:
    IsUShortTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsUShortType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::ushortTypeSymbol));
    }
};

IsUShortTypePredicate::IsUShortTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsIntTypePredicate : public TypePredicate
{
public:
    IsIntTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsIntType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::intTypeSymbol));
    }
};

IsIntTypePredicate::IsIntTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsUIntTypePredicate : public TypePredicate
{
public:
    IsUIntTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsUIntType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::uintTypeSymbol));
    }
};

IsUIntTypePredicate::IsUIntTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsLongTypePredicate : public TypePredicate
{
public:
    IsLongTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsLongType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::longTypeSymbol));
    }
};

IsLongTypePredicate::IsLongTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsULongTypePredicate : public TypePredicate
{
public:
    IsULongTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsULongType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::ulongTypeSymbol));
    }
};

IsULongTypePredicate::IsULongTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsFloatTypePredicate : public TypePredicate
{
public:
    IsFloatTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsFloatType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::floatTypeSymbol));
    }
};

IsFloatTypePredicate::IsFloatTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsDoubleTypePredicate : public TypePredicate
{
public:
    IsDoubleTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsDoubleType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::doubleTypeSymbol));
    }
};

IsDoubleTypePredicate::IsDoubleTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsCharTypePredicate : public TypePredicate
{
public:
    IsCharTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsCharType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::charTypeSymbol));
    }
};

IsCharTypePredicate::IsCharTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsWCharTypePredicate : public TypePredicate
{
public:
    IsWCharTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsWCharType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::wcharTypeSymbol));
    }
};

IsWCharTypePredicate::IsWCharTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsUCharTypePredicate : public TypePredicate
{
public:
    IsUCharTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsUCharType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::ucharTypeSymbol));
    }
};

IsUCharTypePredicate::IsUCharTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsVoidTypePredicate : public TypePredicate
{
public:
    IsVoidTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsVoidType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::voidTypeSymbol));
    }
};

IsVoidTypePredicate::IsVoidTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsClassTypePredicate : public TypePredicate
{
public:
    IsClassTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsClassType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsClassTypeSymbol()));
    }
};

IsClassTypePredicate::IsClassTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsPolymorphicTypePredicate : public TypePredicate
{
public:
    IsPolymorphicTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsPolymorphicType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsPolymorphicType()));
    }
};

IsPolymorphicTypePredicate::IsPolymorphicTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsInterfaceTypePredicate : public TypePredicate
{
public:
    IsInterfaceTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsInterfaceType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->GetSymbolType() == SymbolType::interfaceTypeSymbol));
    }
};

IsInterfaceTypePredicate::IsInterfaceTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsDelegateTypePredicate : public TypePredicate
{
public:
    IsDelegateTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsDelegateType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsDelegateType()));
    }
};

IsDelegateTypePredicate::IsDelegateTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsClassDelegateTypePredicate : public TypePredicate
{
public:
    IsClassDelegateTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsClassDelegateType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsClassDelegateType()));
    }
};

IsClassDelegateTypePredicate::IsClassDelegateTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsEnumeratedTypePredicate : public TypePredicate
{
public:
    IsEnumeratedTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsEnumeratedType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsEnumeratedType()));
    }
};

IsEnumeratedTypePredicate::IsEnumeratedTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsConstTypePredicate : public TypePredicate
{
public:
    IsConstTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsConstType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsConstType()));
    }
};

IsConstTypePredicate::IsConstTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsReferenceTypePredicate : public TypePredicate
{
public:
    IsReferenceTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsReferenceType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsReferenceType()));
    }
};

IsReferenceTypePredicate::IsReferenceTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsLvalueReferenceTypePredicate : public TypePredicate
{
public:
    IsLvalueReferenceTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsLvalueReferenceType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsLvalueReferenceType()));
    }
};

IsLvalueReferenceTypePredicate::IsLvalueReferenceTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsRvalueReferenceTypePredicate : public TypePredicate
{
public:
    IsRvalueReferenceTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsRvalueReferenceType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsRvalueReferenceType()));
    }
};

IsRvalueReferenceTypePredicate::IsRvalueReferenceTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsArrayTypePredicate : public TypePredicate
{
public:
    IsArrayTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsArrayType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsArrayType()));
    }
};

IsArrayTypePredicate::IsArrayTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsPointerTypePredicate : public TypePredicate
{
public:
    IsPointerTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsPointerType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsPointerType()));
    }
};

IsPointerTypePredicate::IsPointerTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class IsGenericPtrTypePredicate : public TypePredicate
{
public:
    IsGenericPtrTypePredicate(Module* module_);
    const char* GroupName() const override { return "IsGenericPtrType"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new BoolValue(span, type->IsVoidPtrType()));
    }
};

IsGenericPtrTypePredicate::IsGenericPtrTypePredicate(Module* module_) : TypePredicate(module_)
{
}

class PointerCountIntrinsicFunction : public IntrinsicFunction
{
public:
    PointerCountIntrinsicFunction(Module* module_);
    int Arity() const override { return 0; }
    int NumberOfTypeParameters() const override { return 1; }
    TypeSymbol* ReturnType(SymbolTable& symbolTable) const override { return symbolTable.GetTypeByName(U"int"); }
    const char* GroupName() const override { return "PointerCount"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        return std::unique_ptr<Value>(new IntValue(span, type->PointerCount()));
    }
};

PointerCountIntrinsicFunction::PointerCountIntrinsicFunction(Module* module_) : IntrinsicFunction(module_)
{
}

class ArrayLengthIntrinsicFunction : public IntrinsicFunction
{
public:
    ArrayLengthIntrinsicFunction(Module* module_);
    int Arity() const override { return 0; }
    int NumberOfTypeParameters() const override { return 1; }
    TypeSymbol* ReturnType(SymbolTable& symbolTable) const override { return symbolTable.GetTypeByName(U"long"); }
    const char* GroupName() const override { return "ArrayLength"; }
    std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) override
    {
        TypeSymbol* type = templateArguments.front();
        if (type->IsArrayType())
        {
            ArrayTypeSymbol* arrayType = static_cast<ArrayTypeSymbol*>(type);
            return std::unique_ptr<Value>(new LongValue(span, arrayType->Size()));
        }
        return std::unique_ptr<Value>(new LongValue(span, 0));
    }
};

ArrayLengthIntrinsicFunction::ArrayLengthIntrinsicFunction(Module* module_) : IntrinsicFunction(module_)
{
}

void MetaInit(SymbolTable& symbolTable)
{
    Module* module = symbolTable.GetModule();
    symbolTable.BeginNamespace(U"System.Meta", Span());
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsIntegralTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsSignedTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsUnsignedTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsFloatingPointTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsBasicTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsBoolTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsSByteTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsByteTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsShortTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsUShortTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsIntTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsUIntTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsLongTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsULongTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsFloatTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsDoubleTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsCharTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsWCharTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsUCharTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsVoidTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsClassTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsPolymorphicTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsInterfaceTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsDelegateTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsClassDelegateTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsEnumeratedTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsConstTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsReferenceTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsLvalueReferenceTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsRvalueReferenceTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsArrayTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsPointerTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new IsGenericPtrTypePredicate(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new PointerCountIntrinsicFunction(module), symbolTable, symbolTable.Container()));
    symbolTable.Container()->AddMember(CreateIntrinsic(new ArrayLengthIntrinsicFunction(module), symbolTable, symbolTable.Container()));
    symbolTable.EndNamespace(); 
}

} } // namespace cmajor::symbols
