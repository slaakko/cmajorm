// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_COMPILE_UNIT_INCLUDED
#define CMAJOR_CMSXI_COMPILE_UNIT_INCLUDED
#include <cmajor/cmsxi/Function.hpp>
#include <cmajor/cmsxi/Metadata.hpp>
#include <cmajor/cmsxi/Context.hpp>

namespace cmsxi {

class CMSXI_API CompileUnit
{
public:
    CompileUnit(const std::string& filePath_);
    CompileUnit(const CompileUnit&) = delete;
    CompileUnit& operator=(const CompileUnit&) = delete;
    Function* GetOrInsertFunction(const std::string& name, FunctionType* type);
    void SetId(const std::string& id_);
    const std::string& SourceFilePath() const { return sourceFilePath; }
    void SetSourceFilePath(const std::string& sourceFilePath_);
    void Write();
    Context* GetContext() { return &context; }
private:
    Context context;
    MDStructRef* cu;
    std::string id;
    std::string sourceFilePath;
    std::string filePath;
    std::vector<std::unique_ptr<Function>> functions;
    std::unordered_map<std::string, Function*> functionMap;
};

} // namespace cmsxi

#endif // CMAJOR_CMSXI_COMPILE_UNIT_INCLUDED
