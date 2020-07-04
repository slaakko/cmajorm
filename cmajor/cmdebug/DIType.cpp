// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DIType.hpp>
#include <stdexcept>

namespace cmajor { namespace debug {

DIType::DIType(Kind kind_) : kind(kind_), id(), name()
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

DIPrimitiveType::DIPrimitiveType() : DIType(Kind::primitiveType)
{
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

DITemplateParameter::DITemplateParameter() : DIType(Kind::templateParameter)
{
}

DIClassType::DIClassType() : DIType(Kind::classType), baseClassId(boost::uuids::nil_uuid())
{
}

DIClassType::DIClassType(Kind kind) : DIType(kind), baseClassId(boost::uuids::nil_uuid())
{
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
    memberVariables.push_back(std::unique_ptr<DIVariable>(memberVariable));
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
}

void DIClassType::Read(soulng::util::BinaryReader& reader)
{
    DIType::Read(reader);
    reader.ReadUuid(baseClassId);
    int32_t ntp = reader.ReadInt();
    for (int32_t i = 0; i < ntp; ++i)
    {
        DITemplateParameter* templateParameter = new DITemplateParameter();
        templateParameter->Read(reader);
        templateParameters.push_back(std::unique_ptr<DITemplateParameter>(templateParameter));
    }
    int32_t nmv = reader.ReadInt();
    for (int32_t i = 0; i < nmv; ++i)
    {
        DIVariable* memberVariable = new DIVariable();
        memberVariable->Read(reader);
        memberVariables.push_back(std::unique_ptr<DIVariable>(memberVariable));
    }
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

DIReferenceType::DIReferenceType() : DIType(Kind::referenceType), baseTypeId(boost::uuids::nil_uuid())
{
}

void DIReferenceType::SetBaseTypeId(const boost::uuids::uuid& baseTypeId_)
{
    baseTypeId = baseTypeId_;
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

DIPointerType::DIPointerType() : DIType(Kind::pointerType), pointedTypeId(boost::uuids::nil_uuid())
{
}

void DIPointerType::SetPointedTypeId(const boost::uuids::uuid& pointedTypeId_)
{
    pointedTypeId = pointedTypeId_;
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

DIArrayType::DIArrayType() : DIType(Kind::arrayType), elementTypeId(boost::uuids::nil_uuid()), size(-1)
{
}

void DIArrayType::SetElementTypeId(const boost::uuids::uuid& elementTypeId_)
{
    elementTypeId = elementTypeId_;
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

void WriteType(soulng::util::BinaryWriter& writer, DIType* type)
{
    writer.Write(static_cast<int8_t>(type->GetKind()));
    type->Write(writer);
}

std::unique_ptr<DIType> ReadType(soulng::util::BinaryReader& reader)
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
    type->Read(reader);
    return type;
}

} } // namespace cmajor::debug
