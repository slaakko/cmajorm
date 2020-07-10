// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DIType.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <stdexcept>

namespace cmajor { namespace debug {

using namespace soulng::unicode;

DIType::DIType(Kind kind_) : kind(kind_), id(), name(), project(nullptr)
{
}

DIType::~DIType()
{
}

void DIType::Write(soulng::util::BinaryWriter& writer)
{
    writer.Write(id);
    writer.Write(name);
}

void DIType::Read(soulng::util::BinaryReader& reader)
{
    reader.ReadUuid(id);
    name = reader.ReadUtf8String();
}

void DIType::SetId(const boost::uuids::uuid& id_)
{
    id = id_;
}

void DIType::SetName(const std::string& name_)
{
    name = name_;
}

std::string DIType::KindStr(Kind kind)
{
    switch (kind)
    {
        case Kind::primitiveType:
        {
            return "primitive";
        }
        case Kind::enumType:
        {
            return "enum";
        }
        case Kind::classType:
        {
            return "class";
        }
        case Kind::specializationType:
        {
            return "specialization";
        }
        case Kind::delegateType:
        {
            return "delegate";
        }
        case Kind::classDelegateType:
        {
            return "classDelegate";
        }
        case Kind::interfaceType:
        {
            return "interface";
        }
        case Kind::templateParameter:
        {
            return "templateParameter";
        }
        case Kind::constType:
        {
            return "const";
        }
        case Kind::referenceType:
        {
            return "reference";
        }
        case Kind::pointerType:
        {
            return "pointer";
        }
        case Kind::arrayType:
        {
            return "array";
        }
    }
    return "";
}

Scope* DIType::GetScope()
{
    return nullptr;
}

std::unique_ptr<JsonValue> DIType::ToJson() const
{
    JsonObject* jsonObject = new JsonObject();
    jsonObject->AddField(U"kind", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(KindStr(kind)))));
    jsonObject->AddField(U"id", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(boost::uuids::to_string(id)))));
    jsonObject->AddField(U"name", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(name))));
    return std::unique_ptr<JsonValue>(jsonObject);
}

DITypeRef::DITypeRef(DIType* type_) : type(type_)
{
}

std::unique_ptr<JsonValue> DITypeRef::ToJson()
{
    JsonObject* jsonObject = new JsonObject();
    jsonObject->AddField(U"name", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(type->Name()))));
    jsonObject->AddField(U"id", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(boost::uuids::to_string(type->Id())))));
    return std::unique_ptr<JsonValue>(jsonObject);
}

DIPrimitiveType::DIPrimitiveType() : DIType(DIType::Kind::primitiveType), kind(Kind::none)
{
}

void DIPrimitiveType::Write(soulng::util::BinaryWriter& writer)
{
    DIType::Write(writer);
    writer.Write(static_cast<int8_t>(kind));
}

void DIPrimitiveType::Read(soulng::util::BinaryReader& reader)
{
    DIType::Read(reader);
    kind = static_cast<Kind>(reader.ReadSByte());
}

std::string DIPrimitiveType::PrimitiveTypeKindStr(Kind kind)
{
    switch (kind)
    {
        case Kind::boolType: return "bool";
        case Kind::sbyteType: return "sbyte";
        case Kind::byteType: return "byte";
        case Kind::shortType: return "short";
        case Kind::ushortType: return "ushort";
        case Kind::intType: return "int";
        case Kind::uintType: return "uint";
        case Kind::longType: return "long";
        case Kind::ulongType: return "ulong";
        case Kind::floatType: return "float";
        case Kind::doubleType: return "double";
        case Kind::charType: return "char";
        case Kind::wcharType: return "wchar";
        case Kind::ucharType: return "uchar";
        case Kind::voidType: return "void";
    }
    return "none";
}

std::unique_ptr<JsonValue> DIPrimitiveType::ToJson() const
{
    std::unique_ptr<JsonValue> value = DIType::ToJson();
    if (value->Type() == JsonValueType::object)
    {
        JsonObject* jsonObject = static_cast<JsonObject*>(value.get());
        jsonObject->AddField(U"primitiveType", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(PrimitiveTypeKindStr(kind)))));
    }
    return value;
}

DIEnumType::DIEnumType() : DIType(Kind::enumType), underlyingTypeId(boost::uuids::nil_uuid())
{
}

void DIEnumType::SetUnderlyingTypeId(const boost::uuids::uuid& underlyingTypeId_)
{
    underlyingTypeId = underlyingTypeId_;
}

void DIEnumType::AddEnumConstant(DIEnumConstant&& enumConstant)
{
    enumConstants.push_back(std::move(enumConstant));
}

void DIEnumType::Write(soulng::util::BinaryWriter& writer)
{
    DIType::Write(writer);
    writer.Write(underlyingTypeId);
    int32_t n = enumConstants.size();
    writer.Write(n);
    for (int32_t i = 0; i < n; ++i)
    {
        DIEnumConstant& enumConstant = enumConstants[i];
        enumConstant.Write(writer);
    }
}

void DIEnumType::Read(soulng::util::BinaryReader& reader)
{
    DIType::Read(reader);
    reader.ReadUuid(underlyingTypeId);
    int32_t n = reader.ReadInt();
    for (int32_t i = 0; i < n; ++i)
    {
        DIEnumConstant enumConstant;
        enumConstant.Read(reader);
        enumConstants.push_back(std::move(enumConstant));
    }
}

std::unique_ptr<JsonValue> DIEnumType::ToJson() const
{
    std::unique_ptr<JsonValue> value = DIType::ToJson();
    if (value->Type() == JsonValueType::object)
    {
        JsonObject* jsonObject = static_cast<JsonObject*>(value.get());
        jsonObject->AddField(U"underlyingTypeId", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(boost::uuids::to_string(underlyingTypeId)))));
    }
    return value;
}

DITemplateParameter::DITemplateParameter() : DIType(Kind::templateParameter)
{
}

ClassScope::ClassScope(DIClassType* classType_) : classType(classType_)
{
}

std::string ClassScope::Name() const
{
    return classType->Name();
}

void ClassScope::AddMemberVariable(DIVariable* memberVariable)
{
    memberVariableMap[memberVariable->Name()] = memberVariable;
}

DIVariable* ClassScope::GetVariable(const std::string& name) const
{
    auto it = memberVariableMap.find(name);
    if (it != memberVariableMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

DIClassType::DIClassType() : DIType(Kind::classType), scope(this), baseClassId(boost::uuids::nil_uuid()), polymorphic(false), vmtPtrIndex(-1)
{
}

DIClassType::DIClassType(Kind kind) : DIType(kind), scope(this), baseClassId(boost::uuids::nil_uuid()), polymorphic(false), vmtPtrIndex(-1)
{
}

DIType* DIClassType::BaseClassType() const
{
    return GetProject()->GetType(baseClassId);
}

void DIClassType::SetBaseClassId(const boost::uuids::uuid& baseClassId_)
{
    baseClassId = baseClassId_;
}

void DIClassType::AddTemplateParameter(DITemplateParameter* templateParameter)
{
    templateParameters.push_back(std::unique_ptr<DITemplateParameter>(templateParameter));
}

void DIClassType::AddMemberVariable(DIVariable* memberVariable)
{
    scope.AddMemberVariable(memberVariable);
    memberVariables.push_back(std::unique_ptr<DIVariable>(memberVariable));
}

void DIClassType::SetIrName(const std::string& irName_)
{
    irName = irName_;
}

void DIClassType::SetVmtVariableName(const std::string& vmtVariableName_)
{
    vmtVariableName = vmtVariableName_;
}

void DIClassType::Write(soulng::util::BinaryWriter& writer)
{
    DIType::Write(writer);
    writer.Write(baseClassId);
    int32_t ntp = templateParameters.size();
    writer.Write(ntp);
    for (int32_t i = 0; i < ntp; ++i)
    {
        DITemplateParameter* templateParameter = templateParameters[i].get();
        templateParameter->Write(writer);
    }
    int32_t nmv = memberVariables.size();
    writer.Write(nmv);
    for (int32_t i = 0; i < nmv; ++i)
    {
        DIVariable* memberVariable = memberVariables[i].get();
        memberVariable->Write(writer);
    }
    writer.Write(polymorphic);
    if (polymorphic)
    {
        writer.Write(vmtPtrIndex);
        writer.Write(irName);
        writer.Write(vmtVariableName);
    }
}

void DIClassType::Read(soulng::util::BinaryReader& reader)
{
    DIType::Read(reader);
    reader.ReadUuid(baseClassId);
    int32_t ntp = reader.ReadInt();
    for (int32_t i = 0; i < ntp; ++i)
    {
        DITemplateParameter* templateParameter = new DITemplateParameter();
        templateParameter->SetProject(GetProject());
        templateParameter->Read(reader);
        templateParameters.push_back(std::unique_ptr<DITemplateParameter>(templateParameter));
    }
    int32_t nmv = reader.ReadInt();
    for (int32_t i = 0; i < nmv; ++i)
    {
        DIVariable* memberVariable = new DIVariable();
        memberVariable->SetProject(GetProject());
        memberVariable->Read(reader);
        AddMemberVariable(memberVariable);
    }
    polymorphic = reader.ReadBool();
    if (polymorphic)
    {
        vmtPtrIndex = reader.ReadInt();
        irName = reader.ReadUtf8String();
        vmtVariableName = reader.ReadUtf8String();
    }
}

std::unique_ptr<JsonValue> DIClassType::ToJson() const
{
    std::unique_ptr<JsonValue> value = DIType::ToJson();
    if (value->Type() == JsonValueType::object)
    {
        JsonObject* jsonObject = static_cast<JsonObject*>(value.get());
        jsonObject->AddField(U"baseClassId", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(boost::uuids::to_string(baseClassId)))));
        JsonArray* templateParameterArray = new JsonArray();
        for (const auto& templateParameter : templateParameters)
        {
            templateParameterArray->AddItem(templateParameter->ToJson());
        }
        jsonObject->AddField(U"templateParameters", std::unique_ptr<JsonValue>(templateParameterArray));
        JsonArray* memberVariableArray = new JsonArray();
        for (const auto& memberVariable : memberVariables)
        {
            memberVariableArray->AddItem(memberVariable->ToJson());
        }
        jsonObject->AddField(U"memberVariables", std::unique_ptr<JsonValue>(memberVariableArray));
        jsonObject->AddField(U"polymorphic", std::unique_ptr<JsonValue>(new JsonBool(polymorphic)));
        if (polymorphic)
        {
            jsonObject->AddField(U"vmtPtrIndex", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(std::to_string(vmtPtrIndex)))));
            jsonObject->AddField(U"irName", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(irName))));
            jsonObject->AddField(U"vmtVariableName", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(vmtVariableName))));
        }
    }
    return value;
}

DIClassTemplateSpecializationType::DIClassTemplateSpecializationType() : DIClassType(Kind::specializationType), primaryTypeId(boost::uuids::nil_uuid())
{
}

void DIClassTemplateSpecializationType::SetPrimaryTypeId(const boost::uuids::uuid& primaryTypeId_)
{
    primaryTypeId = primaryTypeId_;
}

void DIClassTemplateSpecializationType::Write(soulng::util::BinaryWriter& writer)
{
    DIClassType::Write(writer);
    writer.Write(primaryTypeId);
}

void DIClassTemplateSpecializationType::Read(soulng::util::BinaryReader& reader)
{
    DIClassType::Read(reader);
    reader.ReadUuid(primaryTypeId);
}

std::unique_ptr<JsonValue> DIClassTemplateSpecializationType::ToJson() const
{
    std::unique_ptr<JsonValue> value = DIClassType::ToJson();
    if (value->Type() == JsonValueType::object)
    {
        JsonObject* jsonObject = static_cast<JsonObject*>(value.get());
        jsonObject->AddField(U"primaryTypeId", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(boost::uuids::to_string(primaryTypeId)))));
    }
    return value;
}

DIDelegateType::DIDelegateType() : DIType(Kind::delegateType)
{
}

DIClassDelegateType::DIClassDelegateType() : DIType(Kind::classDelegateType)
{
}

DIInterfaceType::DIInterfaceType() : DIType(Kind::interfaceType)
{
}

DIConstType::DIConstType() : DIType(Kind::constType), baseTypeId(boost::uuids::nil_uuid())
{
}

DIType* DIConstType::BaseType() const
{
    return GetProject()->GetType(baseTypeId);
}

void DIConstType::SetBaseTypeId(const boost::uuids::uuid& baseTypeId_)
{
    baseTypeId = baseTypeId_;
}

void DIConstType::Write(soulng::util::BinaryWriter& writer)
{
    DIType::Write(writer);
    writer.Write(baseTypeId);
}

void DIConstType::Read(soulng::util::BinaryReader& reader)
{
    DIType::Read(reader);
    reader.ReadUuid(baseTypeId);
}

std::unique_ptr<JsonValue> DIConstType::ToJson() const
{
    std::unique_ptr<JsonValue> value = DIType::ToJson();
    if (value->Type() == JsonValueType::object)
    {
        JsonObject* jsonObject = static_cast<JsonObject*>(value.get());
        jsonObject->AddField(U"baseTypeId", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(boost::uuids::to_string(baseTypeId)))));
    }
    return value;
}

DIReferenceType::DIReferenceType() : DIType(Kind::referenceType), baseTypeId(boost::uuids::nil_uuid())
{
}

void DIReferenceType::SetBaseTypeId(const boost::uuids::uuid& baseTypeId_)
{
    baseTypeId = baseTypeId_;
}

DIType* DIReferenceType::BaseType() const
{
    return GetProject()->GetType(baseTypeId);
}

void DIReferenceType::Write(soulng::util::BinaryWriter& writer)
{
    DIType::Write(writer);
    writer.Write(baseTypeId);
}

void DIReferenceType::Read(soulng::util::BinaryReader& reader)
{
    DIType::Read(reader);
    reader.ReadUuid(baseTypeId);
}

std::unique_ptr<JsonValue> DIReferenceType::ToJson() const
{
    std::unique_ptr<JsonValue> value = DIType::ToJson();
    if (value->Type() == JsonValueType::object)
    {
        JsonObject* jsonObject = static_cast<JsonObject*>(value.get());
        jsonObject->AddField(U"baseTypeId", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(boost::uuids::to_string(baseTypeId)))));
    }
    return value;
}

DIPointerType::DIPointerType() : DIType(Kind::pointerType), pointedTypeId(boost::uuids::nil_uuid())
{
}

void DIPointerType::SetPointedTypeId(const boost::uuids::uuid& pointedTypeId_)
{
    pointedTypeId = pointedTypeId_;
}

DIType* DIPointerType::PointedToType() const
{
    return GetProject()->GetType(pointedTypeId);
}

void DIPointerType::Write(soulng::util::BinaryWriter& writer)
{
    DIType::Write(writer);
    writer.Write(pointedTypeId);
}

void DIPointerType::Read(soulng::util::BinaryReader& reader)
{
    DIType::Read(reader);
    reader.ReadUuid(pointedTypeId);
}

std::unique_ptr<JsonValue> DIPointerType::ToJson() const
{
    std::unique_ptr<JsonValue> value = DIType::ToJson();
    if (value->Type() == JsonValueType::object)
    {
        JsonObject* jsonObject = static_cast<JsonObject*>(value.get());
        jsonObject->AddField(U"pointedTypeId", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(boost::uuids::to_string(pointedTypeId)))));
    }
    return value;
}

DIArrayType::DIArrayType() : DIType(Kind::arrayType), elementTypeId(boost::uuids::nil_uuid()), size(-1)
{
}

void DIArrayType::SetElementTypeId(const boost::uuids::uuid& elementTypeId_)
{
    elementTypeId = elementTypeId_;
}

DIType* DIArrayType::ElementType() const
{
    return GetProject()->GetType(elementTypeId);
}

void DIArrayType::Write(soulng::util::BinaryWriter& writer)
{
    DIType::Write(writer);
    writer.Write(elementTypeId);
    writer.Write(size);
}

void DIArrayType::Read(soulng::util::BinaryReader& reader)
{
    DIType::Read(reader);
    reader.ReadUuid(elementTypeId);
    size = reader.ReadLong();
}

std::unique_ptr<JsonValue> DIArrayType::ToJson() const
{
    std::unique_ptr<JsonValue> value = DIType::ToJson();
    if (value->Type() == JsonValueType::object)
    {
        JsonObject* jsonObject = static_cast<JsonObject*>(value.get());
        jsonObject->AddField(U"elementTypeId", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(boost::uuids::to_string(elementTypeId)))));
        jsonObject->AddField(U"size", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(std::to_string(size)))));
    }
    return value;
}

DIType* MakePointerType(DIType* pointedToType)
{
    DIPointerType* pointerType = new DIPointerType();
    pointerType->SetId(boost::uuids::random_generator()());
    pointerType->SetPointedTypeId(pointedToType->Id());
    pointerType->SetName(pointedToType->Name() + "*");
    pointerType->SetProject(pointedToType->GetProject());
    pointedToType->GetProject()->AddType(pointerType);
    return pointerType;
}

DIType* MakeReferenceType(DIType* referredToType)
{
    DIReferenceType* referenceType = new DIReferenceType();
    referenceType->SetId(boost::uuids::random_generator()());
    referenceType->SetBaseTypeId(referredToType->Id());
    referenceType->SetName(referredToType->Name() + "&");
    referenceType->SetProject(referredToType->GetProject());
    referredToType->GetProject()->AddType(referenceType);
    return referenceType;
}

DIType* MakeConstType(DIType* baseType)
{
    DIConstType* constType = new DIConstType();
    constType->SetId(boost::uuids::random_generator()());
    constType->SetBaseTypeId(baseType->Id());
    constType->SetName("const " + baseType->Name());
    constType->SetProject(baseType->GetProject());
    baseType->GetProject()->AddType(constType);
    return constType;
}

void WriteType(soulng::util::BinaryWriter& writer, DIType* type)
{
    writer.Write(static_cast<int8_t>(type->GetKind()));
    type->Write(writer);
}

std::unique_ptr<DIType> ReadType(soulng::util::BinaryReader& reader, Project* project)
{
    std::unique_ptr<DIType> type;
    int8_t k = reader.ReadSByte();
    DIType::Kind kind = static_cast<DIType::Kind>(k);
    switch (kind)
    {
        case DIType::Kind::primitiveType: type.reset(new DIPrimitiveType()); break;
        case DIType::Kind::enumType: type.reset(new DIEnumType()); break;
        case DIType::Kind::classType: type.reset(new DIClassType()); break;
        case DIType::Kind::specializationType: type.reset(new DIClassTemplateSpecializationType()); break;
        case DIType::Kind::delegateType: type.reset(new DIDelegateType()); break;
        case DIType::Kind::classDelegateType: type.reset(new DIClassDelegateType()); break;
        case DIType::Kind::interfaceType: type.reset(new DIInterfaceType()); break;
        case DIType::Kind::templateParameter: type.reset(new DITemplateParameter()); break;
        case DIType::Kind::constType: type.reset(new DIConstType()); break;
        case DIType::Kind::referenceType: type.reset(new DIReferenceType()); break;
        case DIType::Kind::pointerType: type.reset(new DIPointerType()); break;
        case DIType::Kind::arrayType: type.reset(new DIArrayType()); break;
        default: throw std::runtime_error("unknown DIType kind '" + std::to_string(k));
    }
    type->SetProject(project);
    type->Read(reader);
    return type;
}

} } // namespace cmajor::debug
