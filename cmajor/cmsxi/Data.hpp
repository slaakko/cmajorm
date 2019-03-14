// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_DATA_INCLUDED
#define CMAJOR_CMSXI_DATA_INCLUDED
#include <cmajor/cmsxi/Type.hpp>

namespace cmsxi {

class CMSXI_API GlobalVariable : public Value
{
public:
    GlobalVariable(Type* type_, const std::string& name_);
    std::string Name(Context& context) override;
    Type* GetType(Context& context) override;
    void SetInitializer(ConstantValue* initializer_) { initializer = initializer_; }
    void Write(Context& context, CodeFormatter& formatter);
    void SetLinkOnce() { linkOnce = true; }
private:
    Type* type;
    std::string name;
    ConstantValue* initializer;
    bool linkOnce;
};

class CMSXI_API DataRepository
{
public:
    DataRepository();
    DataRepository(const DataRepository&) = delete;
    DataRepository& operator=(const DataRepository&) = delete;
    GlobalVariable* GetOrInsertGlobal(const std::string& name, Type* type);
    GlobalVariable* CreateGlobalStringPtr(Context& context, const std::string& stringValue);
    void Write(Context& context, CodeFormatter& formatter);
    void SetCompileUnitId(const std::string& compileUnitId_);
private:
    std::vector<std::unique_ptr<GlobalVariable>> globalVariableDefinitions;
    std::unordered_map<std::string, GlobalVariable*> globalVariableMap;
    int nextStringId;
    std::string compileUnitId;
};

} // namespace cmsxi

#endif // CMAJOR_CMSXI_DATA_INCLUDED
