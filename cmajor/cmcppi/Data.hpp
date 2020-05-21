// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPI_DATA_INCLUDED
#define CMAJOR_CMCPPI_DATA_INCLUDED
#include <cmajor/cmcppi/Type.hpp>

namespace cmcppi {

class CMCPPI_API GlobalVariable : public Value
{
public:
    GlobalVariable(Type* type_, const std::string& name_);
    std::string Name(Context& context) override;
    Type* GetType(Context& context) override;
    ConstantValue* Initializer() const { return initializer; }
    void SetInitializer(ConstantValue* initializer_) { initializer = initializer_; }
    void Write(Context& context, CodeFormatter& formatter);
    void SetLinkOnce() { linkOnce = true; }
    void SetStringPtr() { stringPtr = true; }
private:
    Type* type;
    std::string name;
    ConstantValue* initializer;
    bool linkOnce;
    bool stringPtr;
};

class CMCPPI_API DataRepository
{
public:
    DataRepository();
    DataRepository(const DataRepository&) = delete;
    DataRepository& operator=(const DataRepository&) = delete;
    GlobalVariable* GetOrInsertGlobal(const std::string& name, Type* type);
    GlobalVariable* CreateGlobalStringPtr(Context& context, const std::string& stringValue);
    GlobalVariable* CreateGlobalWStringPtr(Context& context, const std::u16string& stringValue);
    GlobalVariable* CreateGlobalUStringPtr(Context& context, const std::u32string& stringValue);
    void Write(Context& context, CodeFormatter& formatter);
    void SetCompileUnitId(const std::string& compileUnitId_);
private:
    std::vector<std::unique_ptr<GlobalVariable>> globalVariableDefinitions;
    std::unordered_map<std::string, GlobalVariable*> globalVariableMap;
    int nextStringId;
    std::string compileUnitId;
};

} // namespace cmcppi

#endif // CMAJOR_CMCPPI_DATA_INCLUDED
