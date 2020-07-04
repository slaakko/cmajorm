// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DIVariable.hpp>

namespace cmajor { namespace debug {

DIVariable::DIVariable() : name(), typeId()
{
}

DIVariable::~DIVariable()
{
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

void DIVariable::Write(soulng::util::BinaryWriter& writer)
{
    writer.Write(name);
    writer.Write(irName);
    writer.Write(typeId);
}

void DIVariable::Read(soulng::util::BinaryReader& reader)
{
    name = reader.ReadUtf8String();
    irName = reader.ReadUtf8String();
    reader.ReadUuid(typeId);
}

} } // namespace cmajor::debug
