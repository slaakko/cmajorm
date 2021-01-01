// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMCPPI_COMPILE_UNIT_INCLUDED
#define CMAJOR_CMCPPI_COMPILE_UNIT_INCLUDED
#include <cmajor/cmcppi/Context.hpp>

namespace cmcppi {

class CMCPPI_API CompileUnit
{
public:
    CompileUnit(const std::string& filePath_);
    CompileUnit(const CompileUnit&) = delete;
    CompileUnit& operator=(const CompileUnit&) = delete;
    void SetId(const std::string& id_);
    void SetSourceFilePath(const std::string& sourceFilePath_);
    void Write();
    Context* GetContext() { return &context; }
    Function* GetOrInsertFunction(const std::string& name, FunctionType* type);
private:
    Context context;
    std::string id;
    std::string sourceFilePath;
    std::string filePath;
    std::vector<std::unique_ptr<Function>> functions;
    std::unordered_map<std::string, Function*> functionMap;
};

} // namespace cmcppi

#endif // CMAJOR_CMCPPI_COMPILE_UNIT_INCLUDED
