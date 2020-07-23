// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DIVariable.hpp>
#include <cmajor/cmdebug/DebugInfo.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace debug {

using namespace soulng::unicode;

DIVariable::DIVariable(Kind kind_) : kind(kind_), initLineNumber(-1), name(), typeId(), project(nullptr)
{
}

DIVariable::~DIVariable()
{
}

void DIVariable::SetInitLineNumber(int32_t initLineNumber_)
{
    initLineNumber = initLineNumber_;
}

std::string DIVariable::KindStr(Kind kind)
{
    switch (kind)
    {
        case Kind::localVariable: return "local";
        case Kind::memberVariable: return "member";
    }
    return std::string();
}

void DIVariable::SetName(const std::string& name_)
{
    name = name_;
}

void DIVariable::SetIrName(const std::string& irName_)
{
    irName = irName_;
}

void DIVariable::SetTypeId(const boost::uuids::uuid& typeId_)
{
    typeId = typeId_;
}

DIType* DIVariable::GetType() const
{
    if (project)
    {
        return project->GetType(typeId);
    }
    else
    {
        throw std::runtime_error("internal error: project of variable not set");
    }
}

void DIVariable::Write(soulng::util::BinaryWriter& writer)
{
    writer.Write(static_cast<int8_t>(kind));
    writer.Write(initLineNumber);
    writer.Write(name);
    writer.Write(irName);
    writer.Write(typeId);
}

void DIVariable::Read(soulng::util::BinaryReader& reader)
{
    kind = static_cast<Kind>(reader.ReadSByte());
    initLineNumber = reader.ReadInt();
    name = reader.ReadUtf8String();
    irName = reader.ReadUtf8String();
    reader.ReadUuid(typeId);
}

std::unique_ptr<JsonValue> DIVariable::ToJson() const
{
    JsonObject* jsonObject = new JsonObject();
    jsonObject->AddField(U"name", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(name))));
    jsonObject->AddField(U"irName", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(irName))));
    jsonObject->AddField(U"typeId", std::unique_ptr<JsonValue>(new JsonString(ToUtf32(boost::uuids::to_string(typeId)))));
    return std::unique_ptr<JsonValue>(jsonObject);
}

} } // namespace cmajor::debug
