// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_INFO_DIVARIABLE_INCLUDED
#define CMAJOR_DEBUG_INFO_DIVARIABLE_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <soulng/util/BinaryWriter.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <memory>

namespace cmajor { namespace debug {

class DEBUG_API DIVariable
{
public:
    DIVariable();
    virtual ~DIVariable();
    void Write(soulng::util::BinaryWriter& writer);
    void Read(soulng::util::BinaryReader& reader);
    const std::string& Name() const { return name; }
    void SetName(const std::string& name_);
    const std::string& IrName() const { return irName; }
    void SetIrName(const std::string& irName_);
    const boost::uuids::uuid& TypeId() const { return typeId; }
    void SetTypeId(const boost::uuids::uuid& typeId_);
private:
    std::string name;
    std::string irName;
    boost::uuids::uuid typeId;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_INFO_DIVARIABLE_INCLUDED
