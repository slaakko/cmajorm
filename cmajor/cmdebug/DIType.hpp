// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_DITYPE_INCLUDED
#define CMAJOR_DEBUG_DITYPE_INCLUDED
#include <cmajor/cmdebug/DIEnumConstant.hpp>
#include <cmajor/cmdebug/DIVariable.hpp>
#include <soulng/util/Json.hpp>
#include <vector>
#include <unordered_map>

namespace cmajor { namespace debug {

using namespace soulng::util;

class DEBUG_API Scope
{
public:
    virtual ~Scope();
    virtual DIVariable* GetVariable(const std::string& name) const = 0;
    virtual std::string Name() const = 0;
};

class DEBUG_API DIType
{
public:
    enum class Kind : int8_t
    {
        primitiveType,
        enumType,
        classType,
        specializationType,
        delegateType,
        classDelegateType,
        interfaceType,
        templateParameter,
        constType,
        referenceType,
        pointerType,
        arrayType
    };
    DIType(Kind kind_);
    virtual ~DIType();
    virtual void Write(soulng::util::BinaryWriter& writer);
    virtual void Read(soulng::util::BinaryReader& reader);
    Kind GetKind() const { return kind; }
    const boost::uuids::uuid& Id() const { return id; }
    void SetId(const boost::uuids::uuid& id_);
    const std::string& Name() const { return name; }
    void SetName(const std::string& name_);
    static std::string KindStr(Kind kind);
    virtual Scope* GetScope();
    Project* GetProject() const { return project; }
    void SetProject(Project* project_) { project = project_; }
    virtual std::unique_ptr<JsonValue> ToJson() const;
private:
    Kind kind;
    boost::uuids::uuid id;
    std::string name;
    Project* project;
};

class DEBUG_API DITypeRef
{
public:
    DITypeRef(DIType* type_);
    std::unique_ptr<JsonValue> ToJson();
private:
    DIType* type;
};

class DEBUG_API DIPrimitiveType : public DIType
{
public:
    enum class Kind : int8_t
    {
        none, boolType, sbyteType, byteType, shortType, ushortType, intType, uintType, longType, ulongType, floatType, doubleType, charType, wcharType, ucharType, voidType
    };
    DIPrimitiveType();
    void Write(soulng::util::BinaryWriter& writer) override;
    void Read(soulng::util::BinaryReader& reader) override;
    void SetPrimitiveTypeKind(Kind kind_) { kind = kind_; }
    Kind GetPrimitiveTypeKind() const { return kind; }
    static std::string PrimitiveTypeKindStr(Kind kind);
    std::unique_ptr<JsonValue> ToJson() const override;
private:
    Kind kind;
};

class DEBUG_API DIEnumType : public DIType
{
public:
    DIEnumType();
    void SetUnderlyingTypeId(const boost::uuids::uuid& underlyingTypeId_);
    const boost::uuids::uuid& UnderlyingTypeId() const { return underlyingTypeId; }
    const std::vector<DIEnumConstant>& EnumConstants() const { return enumConstants; }
    void AddEnumConstant(DIEnumConstant&& enumConstant);
    void Write(soulng::util::BinaryWriter& writer) override;
    void Read(soulng::util::BinaryReader& reader) override;
    std::unique_ptr<JsonValue> ToJson() const override;
private:
    boost::uuids::uuid underlyingTypeId;
    std::vector<DIEnumConstant> enumConstants;
};

class DEBUG_API DITemplateParameter : public DIType
{
public:
    DITemplateParameter();
};

class DIClassType;

class DEBUG_API ClassScope : public Scope
{
public:
    ClassScope(DIClassType* classType_);
    ClassScope(const ClassScope&) = delete;
    ClassScope(ClassScope&&) = delete;
    ClassScope& operator=(const ClassScope&) = delete;
    ClassScope& operator=(ClassScope&&) = delete;
    std::string Name() const override;
    void AddMemberVariable(DIVariable* memberVariable);
    DIVariable* GetVariable(const std::string& name) const override;
private:
    DIClassType* classType;
    std::unordered_map<std::string, DIVariable*> memberVariableMap;
};

class DEBUG_API DIClassType : public DIType
{
public:
    DIClassType();
    DIClassType(Kind kind);
    DIClassType(const DIClassType&) = delete;
    DIClassType(DIClassType&&) = delete;
    DIClassType& operator=(const DIClassType&) = delete;
    DIClassType& operator=(DIClassType&&) = delete;
    DIType* BaseClassType() const;
    void SetBaseClassId(const boost::uuids::uuid& baseClassId_);
    const boost::uuids::uuid& BaseClassId() const { return baseClassId; }
    void AddTemplateParameter(DITemplateParameter* templateParameter);
    const std::vector<std::unique_ptr<DITemplateParameter>>& TemplateParameters() const { return templateParameters; }
    void AddMemberVariable(DIVariable* memberVariable);
    const std::vector<std::unique_ptr<DIVariable>>& MemberVariables() const { return memberVariables; }
    void SetPolymorphic() { polymorphic = true; }
    bool IsPolymorphic() const { return polymorphic; }
    void SetVmtPtrIndex(int32_t vmtPtrIndex_) { vmtPtrIndex = vmtPtrIndex_; }
    int32_t VmtPtrIndex() const { return vmtPtrIndex; }
    void SetIrName(const std::string& irName_);
    const std::string& IrName() const { return irName; }
    void SetVmtVariableName(const std::string& vmtVariableName_);
    const std::string& VmtVariableName() const { return vmtVariableName; }
    void Write(soulng::util::BinaryWriter& writer) override;
    void Read(soulng::util::BinaryReader& reader) override;
    std::unique_ptr<JsonValue> ToJson() const override;
    Scope* GetScope() override { return &scope; }
private:
    ClassScope scope;
    boost::uuids::uuid baseClassId;
    std::vector<std::unique_ptr<DITemplateParameter>> templateParameters;
    std::vector<std::unique_ptr<DIVariable>> memberVariables;
    bool polymorphic;
    int32_t vmtPtrIndex;
    std::string irName;
    std::string vmtVariableName;
};

class DEBUG_API DIClassTemplateSpecializationType : public DIClassType
{
public:
    DIClassTemplateSpecializationType();
    void SetPrimaryTypeId(const boost::uuids::uuid& primaryTypeId_);
    const boost::uuids::uuid& PrimaryTypeId() const { return primaryTypeId; }
    void Write(soulng::util::BinaryWriter& writer) override;
    void Read(soulng::util::BinaryReader& reader) override;
    std::unique_ptr<JsonValue> ToJson() const override;
private:
    boost::uuids::uuid primaryTypeId;
};

class DEBUG_API DIDelegateType : public DIType
{
public:
    DIDelegateType();
};

class DEBUG_API DIClassDelegateType : public DIType
{
public:
    DIClassDelegateType();
};

class DEBUG_API DIInterfaceType : public DIType
{
public:
    DIInterfaceType();
};

class DEBUG_API DIConstType : public DIType
{
public:
    DIConstType();
    void SetBaseTypeId(const boost::uuids::uuid& baseTypeId_);
    const boost::uuids::uuid& BaseTypeId() const { return baseTypeId; }
    DIType* BaseType() const;
    void Write(soulng::util::BinaryWriter& writer) override;
    void Read(soulng::util::BinaryReader& reader) override;
    std::unique_ptr<JsonValue> ToJson() const override;
private:
    boost::uuids::uuid baseTypeId;
};

class DEBUG_API DIReferenceType : public DIType
{
public:
    DIReferenceType();
    void SetBaseTypeId(const boost::uuids::uuid& baseTypeId_);
    const boost::uuids::uuid& BaseTypeId() const { return baseTypeId; }
    DIType* BaseType() const;
    void Write(soulng::util::BinaryWriter& writer) override;
    void Read(soulng::util::BinaryReader& reader) override;
    std::unique_ptr<JsonValue> ToJson() const override;
private:
    boost::uuids::uuid baseTypeId;
};

class DEBUG_API DIPointerType : public DIType
{
public:
    DIPointerType();
    void SetPointedTypeId(const boost::uuids::uuid& pointedTypeId_);
    const boost::uuids::uuid& PointedTypeId() const { return pointedTypeId; }
    DIType* PointedToType() const;
    void Write(soulng::util::BinaryWriter& writer) override;
    void Read(soulng::util::BinaryReader& reader) override;
    std::unique_ptr<JsonValue> ToJson() const override;
private:
    boost::uuids::uuid pointedTypeId;
};

class DEBUG_API DIArrayType : public DIType
{
public:
    DIArrayType();
    void SetElementTypeId(const boost::uuids::uuid& elementTypeId_);
    const boost::uuids::uuid& ElementTypeId() const { return elementTypeId; }
    DIType* ElementType() const;
    int64_t Size() const { return size; }
    void SetSize(int64_t size_) { size = size_; }
    void Write(soulng::util::BinaryWriter& writer) override;
    void Read(soulng::util::BinaryReader& reader) override;
    std::unique_ptr<JsonValue> ToJson() const override;
private:
    boost::uuids::uuid elementTypeId;
    int64_t size;
};

DEBUG_API DIType* MakePointerType(DIType* pointedToType);
DEBUG_API DIType* MakeReferenceType(DIType* referredToType);
DEBUG_API DIType* MakeConstType(DIType* baseType);

DEBUG_API void WriteType(soulng::util::BinaryWriter& writer, DIType* type);
DEBUG_API std::unique_ptr<DIType> ReadType(soulng::util::BinaryReader& reader, Project* project);

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_DITYPE_INCLUDED
