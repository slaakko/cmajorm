// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdebug/DIEnumConstant.hpp>

namespace cmajor { namespace debug {

DIEnumConstant::DIEnumConstant() : name(), value(), strValue()
{
}

void DIEnumConstant::Write(soulng::util::BinaryWriter& writer)
{
    writer.Write(name);
    writer.Write(value);
    writer.Write(strValue);
}

void DIEnumConstant::Read(soulng::util::BinaryReader& reader)
{
    name = reader.ReadUtf8String();
    value = reader.ReadUtf8String();
    strValue = reader.ReadUtf8String();
}

void DIEnumConstant::SetName(const std::string& name_)
{
    name = name_;
}

void DIEnumConstant::SetValue(const std::string& value_)
{
    value = value_;
}

void DIEnumConstant::SetStrValue(const std::string& strValue_)
{
    strValue = strValue_;
}

} } // namespace cmajor::debug
