// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_DIVARIABLE_INCLUDED
#define CMAJOR_DEBUG_DIVARIABLE_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <soulng/util/BinaryWriter.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <soulng/util/Json.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <memory>

namespace cmajor { namespace debug {

using namespace soulng::util;

class Project;
class DIType;

class DEBUG_API DIVariable
{
public:
    enum class Kind : int8_t
    {
        localVariable, memberVariable
    };
    DIVariable(Kind kind_);
    virtual ~DIVariable();
    void SetInitLineNumber(int32_t initLineNumber_);
    int32_t GetInitLineNumber() const { return initLineNumber; }
    Kind GetKind() const { return kind; }
    static std::string KindStr(Kind kind);
    void SetProject(Project* project_) { project = project_; }
    void Write(soulng::util::BinaryWriter& writer);
    void Read(soulng::util::BinaryReader& reader);
    const std::string& Name() const { return name; }
    void SetName(const std::string& name_);
    const std::string& IrName() const { return irName; }
    void SetIrName(const std::string& irName_);
    const boost::uuids::uuid& TypeId() const { return typeId; }
    void SetTypeId(const boost::uuids::uuid& typeId_);
    DIType* GetType() const;
    std::unique_ptr<JsonValue> ToJson() const;
private:
    Kind kind;
    int32_t initLineNumber;
    std::string name;
    std::string irName;
    boost::uuids::uuid typeId;
    Project* project;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_DIVARIABLE_INCLUDED
