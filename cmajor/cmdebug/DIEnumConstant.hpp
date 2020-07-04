// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_INFO_DIENUMCONSTANT_INCLUDED
#define CMAJOR_DEBUG_INFO_DIENUMCONSTANT_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <soulng/util/BinaryWriter.hpp>
#include <soulng/util/BinaryReader.hpp>

namespace cmajor { namespace debug {

class DEBUG_API DIEnumConstant
{
public:
    DIEnumConstant();
    void Write(soulng::util::BinaryWriter& writer);
    void Read(soulng::util::BinaryReader& reader);
    void SetName(const std::string& name_);
    const std::string& Name() const { return name; }
    void SetValue(const std::string& value_);
    const std::string& Value() const { return value; }
    void SetStrValue(const std::string& strValue_);
    const std::string& StrValue() const { return strValue; }
private:
    std::string name;
    std::string value;
    std::string strValue;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_INFO_DIENUMCONSTANT_INCLUDED
