// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/EnumSymbol.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/symbols/Module.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

EnumTypeSymbol::EnumTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : 
    TypeSymbol(SymbolType::enumTypeSymbol, span_, sourceModuleId_, name_), underlyingType()
{
}

void EnumTypeSymbol::Write(SymbolWriter& writer)
{
    TypeSymbol::Write(writer);
    writer.GetBinaryWriter().Write(underlyingType->TypeId());
}

void EnumTypeSymbol::Read(SymbolReader& reader)
{
    TypeSymbol::Read(reader);
    boost::uuids::uuid underlyingTypeId;
    reader.GetBinaryReader().ReadUuid(underlyingTypeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, underlyingTypeId, 0);
}

void EnumTypeSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    Assert(index == 0, "invalid emplace type index");
    underlyingType = typeSymbol;
}

std::string EnumTypeSymbol::Syntax() const
{
    std::string syntax = GetSpecifierStr();
    if (!syntax.empty())
    {
        syntax.append(1, ' ');
    }
    syntax.append("enum ");
    syntax.append(ToUtf8(DocName()));
    syntax.append(1, ';');
    return syntax;
}

void EnumTypeSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject() && Access() == SymbolAccess::public_)
    {
        collector->AddEnumeratedType(this);
    }
}

void EnumTypeSymbol::CollectMembers(SymbolCollector* collector)
{
    TypeSymbol::Accept(collector);
}

void EnumTypeSymbol::Dump(CodeFormatter& formatter)
{
    formatter.WriteLine(ToUtf8(Name()));
    formatter.WriteLine("full name: " + ToUtf8(FullNameWithSpecifiers()));
    formatter.WriteLine("underlying type: " + ToUtf8(underlyingType->FullName()));
    formatter.WriteLine("typeid: " + boost::uuids::to_string(TypeId()));
    formatter.WriteLine("enumeration constants:");
    formatter.IncIndent();
    SymbolCollector collector;
    TypeSymbol::Accept(&collector);
    for (EnumConstantSymbol* enumConstant : collector.EnumerationConstants())
    {
        formatter.WriteLine();
        enumConstant->Dump(formatter);
    }
    formatter.DecIndent();
}

void EnumTypeSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be static", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be virtual", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be override", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be abstract", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be inline", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be explicit", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be external", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be suppressed", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be default", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be constexpr", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be cdecl", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be nothrow", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be throw", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be new", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be const", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception("enumerated type cannot be unit_test", GetSpan(), SourceModuleId());
    }
}

std::vector<EnumConstantSymbol*> EnumTypeSymbol::GetEnumConstants()
{
    SymbolCollector collector;
    TypeSymbol::Accept(&collector);
    return collector.EnumerationConstants();
}

ValueType EnumTypeSymbol::GetValueType() const
{
    return underlyingType->GetValueType();
}

Value* EnumTypeSymbol::MakeValue() const
{
    return underlyingType->MakeValue();
}

std::u32string EnumTypeSymbol::Id() const
{
    return MangledName();
}

void* EnumTypeSymbol::CreateDIType(Emitter& emitter)
{
    uint64_t sizeInBits = SizeInBits(emitter);
    uint32_t alignInBits = AlignmentInBits(emitter);
    std::vector<void*> elements;
    std::vector<EnumConstantSymbol*> enumConstants = GetEnumConstants();
    for (EnumConstantSymbol* enumConstant : enumConstants)
    {
        int64_t value = 0;
        if (underlyingType->IsUnsignedType())
        {
            Value* val = enumConstant->GetValue()->As(GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"ulong"), false, GetSpan(), SourceModuleId(), true);
            if (val)
            {
                ULongValue* ulongValue = static_cast<ULongValue*>(val);
                value = static_cast<int64_t>(ulongValue->GetValue());
            }
        }
        else
        {
            Value* val = enumConstant->GetValue()->As(GetRootModuleForCurrentThread()->GetSymbolTable().GetTypeByName(U"long"), false, GetSpan(), SourceModuleId(), true);
            if (val)
            {
                LongValue* longValue = static_cast<LongValue*>(val);
                value = longValue->GetValue();
            }
        }
        elements.push_back(emitter.CreateDITypeForEnumConstant(ToUtf8(enumConstant->Name()), value));
    }
    return emitter.CreateDITypeForEnumType(ToUtf8(Name()), ToUtf8(MangledName()), GetSpan(), SourceModuleId(), elements, sizeInBits, alignInBits, underlyingType->GetDIType(emitter));
}

void EnumTypeSymbol::Check()
{
    TypeSymbol::Check();
    if (!underlyingType)
    {
        throw SymbolCheckException( "enumerated type symbol has no underlying type", GetSpan(), SourceModuleId());
    }
}

EnumConstantSymbol::EnumConstantSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : 
    Symbol(SymbolType::enumConstantSymbol, span_, sourceModuleId_, name_), evaluating(false)
{
}

void EnumConstantSymbol::SetValue(Value* value_)
{
    value.reset(value_);
}

void EnumConstantSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject())
    {
        collector->AddEnumerationConstant(this);
    }
}

void EnumConstantSymbol::Dump(CodeFormatter& formatter)
{
    formatter.WriteLine(ToUtf8(Name()));
    formatter.WriteLine("value: " + value->ToString());
}

void EnumConstantSymbol::Write(SymbolWriter& writer)
{
    Symbol::Write(writer);
    WriteValue(value.get(), writer.GetBinaryWriter());
    writer.GetBinaryWriter().Write(strValue);
}

void EnumConstantSymbol::Read(SymbolReader& reader)
{
    Symbol::Read(reader);
    value = ReadValue(reader.GetBinaryReader(), GetSpan(), SourceModuleId());
    strValue = reader.GetBinaryReader().ReadUtf32String();
}

EnumTypeDefaultConstructor::EnumTypeDefaultConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::enumTypeDefaultConstructor, span_, sourceModuleId_, name_), underlyingTypeDefaultConstructor(nullptr)
{
}

EnumTypeDefaultConstructor::EnumTypeDefaultConstructor(EnumTypeSymbol* enumType_) : 
    FunctionSymbol(SymbolType::enumTypeDefaultConstructor, enumType_->GetSpan(), enumType_->SourceModuleId(), U"enumTypeDefaultConstructor"), underlyingTypeDefaultConstructor(nullptr)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"this");
    thisParam->SetType(enumType_->AddPointer(Span(), boost::uuids::nil_uuid()));
    AddMember(thisParam);
    ComputeName();
    TypeSymbol* underlyingType = enumType_->UnderlyingType();
    Assert(underlyingType->IsBasicTypeSymbol(), "basic type expected");
    BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(underlyingType);
    underlyingTypeDefaultConstructor = basicTypeSymbol->DefaultConstructor();
}

void EnumTypeDefaultConstructor::Write(SymbolWriter& writer) 
{
    FunctionSymbol::Write(writer);
    Assert(underlyingTypeDefaultConstructor, "underlying default constructor not set");
    const boost::uuids::uuid& defaultConstructorId = underlyingTypeDefaultConstructor->FunctionId();
    writer.GetBinaryWriter().Write(defaultConstructorId);
}

void EnumTypeDefaultConstructor::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid defaultConstructorId;
    reader.GetBinaryReader().ReadUuid(defaultConstructorId);
    reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, defaultConstructorId, 0);
}

void EnumTypeDefaultConstructor::EmplaceFunction(FunctionSymbol* functionSymbol, int index)
{
    if (index == 0)
    {
        underlyingTypeDefaultConstructor = functionSymbol;
    }
    else
    {
        Assert(false, "invalid emplace function index");
    }
}

void EnumTypeDefaultConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    Assert(underlyingTypeDefaultConstructor, "underlying default constructor not set");
    underlyingTypeDefaultConstructor->GenerateCall(emitter, genObjects, flags, span, moduleId);
}

void EnumTypeDefaultConstructor::Check()
{
    FunctionSymbol::Check();
    if (!underlyingTypeDefaultConstructor)
    {
        throw SymbolCheckException("enumerated type default constructor has no underlying type default constructor", GetSpan(), SourceModuleId());
    }
}

EnumTypeCopyConstructor::EnumTypeCopyConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::enumTypeCopyConstructor, span_, sourceModuleId_, name_), underlyingTypeCopyConstructor(nullptr)
{
}

EnumTypeCopyConstructor::EnumTypeCopyConstructor(EnumTypeSymbol* enumType_) : 
    FunctionSymbol(SymbolType::enumTypeCopyConstructor, enumType_->GetSpan(), enumType_->SourceModuleId(), U"enumTypeCopyConstructor"), underlyingTypeCopyConstructor(nullptr)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"this");
    thisParam->SetType(enumType_->AddPointer(Span(), boost::uuids::nil_uuid()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"that");
    thatParam->SetType(enumType_);
    AddMember(thatParam);
    ComputeName();
    TypeSymbol* underlyingType = enumType_->UnderlyingType();
    Assert(underlyingType->IsBasicTypeSymbol(), "basic type expected");
    BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(underlyingType);
    underlyingTypeCopyConstructor = basicTypeSymbol->CopyConstructor();
}

void EnumTypeCopyConstructor::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    Assert(underlyingTypeCopyConstructor, "underlying copy constructor not set");
    const boost::uuids::uuid& copyConstructorId = underlyingTypeCopyConstructor->FunctionId();
    writer.GetBinaryWriter().Write(copyConstructorId);
}

void EnumTypeCopyConstructor::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid copyConstructorId;
    reader.GetBinaryReader().ReadUuid(copyConstructorId);
    reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, copyConstructorId, 0);
}

void EnumTypeCopyConstructor::EmplaceFunction(FunctionSymbol* functionSymbol, int index)
{
    if (index == 0)
    {
        underlyingTypeCopyConstructor = functionSymbol;
    }
    else
    {
        Assert(false, "invalid emplace function index");
    }
}

void EnumTypeCopyConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    Assert(underlyingTypeCopyConstructor, "underlying copy constructor not set");
    underlyingTypeCopyConstructor->GenerateCall(emitter, genObjects, flags, span, moduleId);
}

void EnumTypeCopyConstructor::Check()
{
    FunctionSymbol::Check();
    if (!underlyingTypeCopyConstructor)
    {
        throw SymbolCheckException("enumerated type copy constructor has no underlying type copy constructor", GetSpan(), SourceModuleId());
    }
}

EnumTypeMoveConstructor::EnumTypeMoveConstructor(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::enumTypeMoveConstructor, span_, sourceModuleId_, name_), underlyingTypeMoveConstructor(nullptr)
{
}

EnumTypeMoveConstructor::EnumTypeMoveConstructor(EnumTypeSymbol* enumType_) : 
    FunctionSymbol(SymbolType::enumTypeMoveConstructor, enumType_->GetSpan(), enumType_->SourceModuleId(), U"enumTypeMoveConstructor"), underlyingTypeMoveConstructor(nullptr)
{
    SetGroupName(U"@constructor");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"this");
    thisParam->SetType(enumType_->AddPointer(Span(), boost::uuids::nil_uuid()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"that");
    thatParam->SetType(enumType_->AddRvalueReference(Span(), boost::uuids::nil_uuid()));
    AddMember(thatParam);
    ComputeName();
    TypeSymbol* underlyingType = enumType_->UnderlyingType();
    Assert(underlyingType->IsBasicTypeSymbol(), "basic type expected");
    BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(underlyingType);
    underlyingTypeMoveConstructor = basicTypeSymbol->MoveConstructor();
}

void EnumTypeMoveConstructor::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    Assert(underlyingTypeMoveConstructor, "underlying move constructor not set");
    const boost::uuids::uuid& moveConstructorId = underlyingTypeMoveConstructor->FunctionId();
    writer.GetBinaryWriter().Write(moveConstructorId);
}

void EnumTypeMoveConstructor::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid moveConstructorId;
    reader.GetBinaryReader().ReadUuid(moveConstructorId);
    reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, moveConstructorId, 0);
}

void EnumTypeMoveConstructor::EmplaceFunction(FunctionSymbol* functionSymbol, int index)
{
    if (index == 0)
    {
        underlyingTypeMoveConstructor = functionSymbol;
    }
    else
    {
        Assert(false, "invalid emplace function index");
    }
}

void EnumTypeMoveConstructor::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    Assert(underlyingTypeMoveConstructor, "underlying move constructor not set");
    underlyingTypeMoveConstructor->GenerateCall(emitter, genObjects, flags, span, moduleId);
}

void EnumTypeMoveConstructor::Check()
{
    FunctionSymbol::Check();
    if (!underlyingTypeMoveConstructor)
    {
        throw SymbolCheckException("enumerated type move constructor has no underlying type move constructor", GetSpan(), SourceModuleId());
    }
}

EnumTypeCopyAssignment::EnumTypeCopyAssignment(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::enumTypeCopyAssignment, span_, sourceModuleId_, name_), underlyingTypeCopyAssignment(nullptr)
{
}

EnumTypeCopyAssignment::EnumTypeCopyAssignment(EnumTypeSymbol* enumType_, TypeSymbol* voidType_) :
    FunctionSymbol(SymbolType::enumTypeCopyAssignment, enumType_->GetSpan(), enumType_->SourceModuleId(), U"enumTypeCopyAssignment"), underlyingTypeCopyAssignment(nullptr)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"this");
    thisParam->SetType(enumType_->AddPointer(Span(), boost::uuids::nil_uuid()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"that");
    thatParam->SetType(enumType_);
    AddMember(thatParam);
    SetReturnType(voidType_);
    ComputeName();
    TypeSymbol* underlyingType = enumType_->UnderlyingType();
    Assert(underlyingType->IsBasicTypeSymbol(), "basic type expected");
    BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(underlyingType);
    underlyingTypeCopyAssignment = basicTypeSymbol->CopyAssignment();
}

void EnumTypeCopyAssignment::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    Assert(underlyingTypeCopyAssignment, "underlying copy assignment not set");
    const boost::uuids::uuid& copyAssignmentId = underlyingTypeCopyAssignment->FunctionId();
    writer.GetBinaryWriter().Write(copyAssignmentId);
}

void EnumTypeCopyAssignment::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid copyAssignmentId;
    reader.GetBinaryReader().ReadUuid(copyAssignmentId);
    reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, copyAssignmentId, 0);
}

void EnumTypeCopyAssignment::EmplaceFunction(FunctionSymbol* functionSymbol, int index)
{
    if (index == 0)
    {
        underlyingTypeCopyAssignment = functionSymbol;
    }
    else
    {
        Assert(false, "invalid emplace function index");
    }
}

void EnumTypeCopyAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    Assert(underlyingTypeCopyAssignment, "underlying copy assignment not set");
    underlyingTypeCopyAssignment->GenerateCall(emitter, genObjects, flags, span, moduleId);
}

void EnumTypeCopyAssignment::Check()
{
    FunctionSymbol::Check();
    if (!underlyingTypeCopyAssignment)
    {
        throw SymbolCheckException("enumerated type copy assignment has no underlying type copy assignment", GetSpan(), SourceModuleId());
    }
}

EnumTypeMoveAssignment::EnumTypeMoveAssignment(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::enumTypeMoveAssignment, span_, sourceModuleId_, name_), underlyingTypeMoveAssignment(nullptr)
{
}

EnumTypeMoveAssignment::EnumTypeMoveAssignment(EnumTypeSymbol* enumType_, TypeSymbol* voidType_) : 
    FunctionSymbol(SymbolType::enumTypeMoveAssignment, enumType_->GetSpan(), enumType_->SourceModuleId(), U"enumTypeMoveAssignment"), underlyingTypeMoveAssignment(nullptr)
{
    SetGroupName(U"operator=");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* thisParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"this");
    thisParam->SetType(enumType_->AddPointer(Span(), boost::uuids::nil_uuid()));
    AddMember(thisParam);
    ParameterSymbol* thatParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"that");
    thatParam->SetType(enumType_->AddRvalueReference(Span(), boost::uuids::nil_uuid()));
    AddMember(thatParam);
    SetReturnType(voidType_);
    ComputeName();
    TypeSymbol* underlyingType = enumType_->UnderlyingType();
    Assert(underlyingType->IsBasicTypeSymbol(), "basic type expected");
    BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(underlyingType);
    underlyingTypeMoveAssignment = basicTypeSymbol->MoveAssignment();
}

void EnumTypeMoveAssignment::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    Assert(underlyingTypeMoveAssignment, "underlying move assignment not set");
    const boost::uuids::uuid& moveAssignmentId = underlyingTypeMoveAssignment->FunctionId();
    writer.GetBinaryWriter().Write(moveAssignmentId);
}

void EnumTypeMoveAssignment::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid moveAssignmentId;
    reader.GetBinaryReader().ReadUuid(moveAssignmentId);
    reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, moveAssignmentId, 0);
}

void EnumTypeMoveAssignment::EmplaceFunction(FunctionSymbol* functionSymbol, int index)
{
    if (index == 0)
    {
        underlyingTypeMoveAssignment = functionSymbol;
    }
    else
    {
        Assert(false, "invalid emplace function index");
    }
}

void EnumTypeMoveAssignment::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    Assert(underlyingTypeMoveAssignment, "underlying move assignment not set");
    underlyingTypeMoveAssignment->GenerateCall(emitter, genObjects, flags, span, moduleId);
}

void EnumTypeMoveAssignment::Check()
{
    FunctionSymbol::Check();
    if (!underlyingTypeMoveAssignment)
    {
        throw SymbolCheckException( "enumerated type move assignment has no underlying type move assignment", GetSpan(), SourceModuleId());
    }
}

EnumTypeReturn::EnumTypeReturn(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::enumTypeReturn, span_, sourceModuleId_, name_), underlyingTypeReturn(nullptr)
{
}

EnumTypeReturn::EnumTypeReturn(EnumTypeSymbol* enumType_) : 
    FunctionSymbol(SymbolType::enumTypeReturn, enumType_->GetSpan(), enumType_->SourceModuleId(), U"enumTypeReturn"), underlyingTypeReturn(nullptr)
{
    SetGroupName(U"@return");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* valueParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"value");
    valueParam->SetType(enumType_);
    AddMember(valueParam);
    SetReturnType(enumType_);
    ComputeName();
    TypeSymbol* underlyingType = enumType_->UnderlyingType();
    Assert(underlyingType->IsBasicTypeSymbol(), "basic type expected");
    BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(underlyingType);
    underlyingTypeReturn = basicTypeSymbol->ReturnFun();
}

void EnumTypeReturn::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    Assert(underlyingTypeReturn, "underlying return not set");
    const boost::uuids::uuid& returnId = underlyingTypeReturn->FunctionId();
    writer.GetBinaryWriter().Write(returnId);
}

void EnumTypeReturn::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid returnId;
    reader.GetBinaryReader().ReadUuid(returnId);
    reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, returnId, 0);
}

void EnumTypeReturn::EmplaceFunction(FunctionSymbol* functionSymbol, int index)
{
    if (index == 0)
    {
        underlyingTypeReturn = functionSymbol;
    }
    else
    {
        Assert(false, "invalid emplace function index");
    }
}

void EnumTypeReturn::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    Assert(underlyingTypeReturn, "underlying return not set");
    underlyingTypeReturn->GenerateCall(emitter, genObjects, flags, span, moduleId);
}

void EnumTypeReturn::Check()
{
    FunctionSymbol::Check();
    if (!underlyingTypeReturn)
    {
        throw SymbolCheckException("enumerated type return has no underlying type return", GetSpan(), SourceModuleId());
    }
}

EnumTypeEqualityOp::EnumTypeEqualityOp(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::enumTypeEquality, span_, sourceModuleId_, name_), underlyingTypeEquality(nullptr)
{
}

EnumTypeEqualityOp::EnumTypeEqualityOp(EnumTypeSymbol* enumType_, TypeSymbol* boolType_) : 
    FunctionSymbol(SymbolType::enumTypeEquality, enumType_->GetSpan(), enumType_->SourceModuleId(), U"enumTypeEquality"), underlyingTypeEquality(nullptr)
{
    SetGroupName(U"operator==");
    SetAccess(SymbolAccess::public_);
    ParameterSymbol* leftParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"left");
    leftParam->SetType(enumType_);
    AddMember(leftParam);
    ParameterSymbol* rightParam = new ParameterSymbol(enumType_->GetSpan(), enumType_->SourceModuleId(), U"right");
    rightParam->SetType(enumType_);
    AddMember(rightParam);
    SetReturnType(boolType_);
    ComputeName();
    TypeSymbol* underlyingType = enumType_->UnderlyingType();
    Assert(underlyingType->IsBasicTypeSymbol(), "basic type expected");
    BasicTypeSymbol* basicTypeSymbol = static_cast<BasicTypeSymbol*>(underlyingType);
    underlyingTypeEquality = basicTypeSymbol->EqualityOp();
}

void EnumTypeEqualityOp::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    Assert(underlyingTypeEquality, "underlying equality not set");
    const boost::uuids::uuid& equalityId = underlyingTypeEquality->FunctionId();
    writer.GetBinaryWriter().Write(equalityId);
}

void EnumTypeEqualityOp::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid equalityId;
    reader.GetBinaryReader().ReadUuid(equalityId);
    reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, equalityId, 0);
}

void EnumTypeEqualityOp::EmplaceFunction(FunctionSymbol* functionSymbol, int index)
{
    if (index == 0)
    {
        underlyingTypeEquality = functionSymbol;
    }
    else
    {
        Assert(false, "invalid emplace function index");
    }
}

void EnumTypeEqualityOp::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    Assert(underlyingTypeEquality, "underlying equality not set");
    underlyingTypeEquality->GenerateCall(emitter, genObjects, flags, span, moduleId);
}

void EnumTypeEqualityOp::Check()
{
    FunctionSymbol::Check();
    if (!underlyingTypeEquality)
    {
        throw SymbolCheckException("enumerated type equality operation has no underlying type equality operation", GetSpan(), SourceModuleId());
    }
}

EnumTypeToUnderlyingTypeConversion::EnumTypeToUnderlyingTypeConversion(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    FunctionSymbol(SymbolType::enumTypeToUnderlyingType, span_, sourceModuleId_, name_), sourceType(), targetType()
{
    SetGroupName(U"@conversion");
}

EnumTypeToUnderlyingTypeConversion::EnumTypeToUnderlyingTypeConversion(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_, TypeSymbol* sourceType_, TypeSymbol* targetType_) :
    FunctionSymbol(SymbolType::enumTypeToUnderlyingType, span_, sourceModuleId_, name_), sourceType(sourceType_), targetType(targetType_)
{
    SetConversion();
    SetGroupName(U"@conversion");
    SetConversionSourceType(sourceType->PlainType(GetSpan(), SourceModuleId())); 
    SetConversionTargetType(targetType->PlainType(GetSpan(), SourceModuleId())); 
}

void EnumTypeToUnderlyingTypeConversion::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(sourceType->TypeId());
    writer.GetBinaryWriter().Write(targetType->TypeId());
}

void EnumTypeToUnderlyingTypeConversion::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid sourceTypeId;
    reader.GetBinaryReader().ReadUuid(sourceTypeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, sourceTypeId, 1);
    boost::uuids::uuid targetTypeId;
    reader.GetBinaryReader().ReadUuid(targetTypeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, targetTypeId, 2);
}

void EnumTypeToUnderlyingTypeConversion::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        sourceType = typeSymbol;
    }
    else if (index == 2)
    {
        targetType = typeSymbol;
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void EnumTypeToUnderlyingTypeConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
}

void EnumTypeToUnderlyingTypeConversion::Check()
{
    FunctionSymbol::Check();
    if (!sourceType)
    {
        throw SymbolCheckException("enum type to underlying type conversion has no source type", GetSpan(), SourceModuleId());
    }
    if (!targetType)
    {
        throw SymbolCheckException("enum type to underlying type conversion has no target type", GetSpan(), SourceModuleId());
    }
}

UnderlyingTypeToEnumTypeConversion::UnderlyingTypeToEnumTypeConversion(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_)
    : FunctionSymbol(SymbolType::underlyingToEnumType, span_, sourceModuleId_, name_), sourceType(), targetType()
{
    SetGroupName(U"@conversion");
}

UnderlyingTypeToEnumTypeConversion::UnderlyingTypeToEnumTypeConversion(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_, TypeSymbol* sourceType_, TypeSymbol* targetType_)
    : FunctionSymbol(SymbolType::underlyingToEnumType, span_, sourceModuleId_, name_), sourceType(sourceType_), targetType(targetType_)
{
    SetConversion();
    SetGroupName(U"@conversion");
    SetConversionSourceType(sourceType->PlainType(GetSpan(), SourceModuleId())); 
    SetConversionTargetType(targetType->PlainType(GetSpan(), SourceModuleId())); 
}

void UnderlyingTypeToEnumTypeConversion::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(sourceType->TypeId());
    writer.GetBinaryWriter().Write(targetType->TypeId());
}

void UnderlyingTypeToEnumTypeConversion::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    boost::uuids::uuid sourceTypeId;
    reader.GetBinaryReader().ReadUuid(sourceTypeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, sourceTypeId, 1);
    boost::uuids::uuid targetTypeId;
    reader.GetBinaryReader().ReadUuid(targetTypeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, targetTypeId, 2);
}

void UnderlyingTypeToEnumTypeConversion::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 1)
    {
        sourceType = typeSymbol;
    }
    else if (index == 2)
    {
        targetType = typeSymbol;
    }
    else
    {
        FunctionSymbol::EmplaceType(typeSymbol, index);
    }
}

void UnderlyingTypeToEnumTypeConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
}

void UnderlyingTypeToEnumTypeConversion::Check()
{
    FunctionSymbol::Check();
    if (!sourceType)
    {
        throw SymbolCheckException("underlying type to enum type conversion has no source type", GetSpan(), SourceModuleId());
    }
    if (!targetType)
    {
        throw SymbolCheckException("underlying type to enum type conversion has no target type", GetSpan(), SourceModuleId());
    }
}

} } // namespace cmajor::symbols
