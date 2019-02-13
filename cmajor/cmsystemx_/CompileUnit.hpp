// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsystemx/Function.hpp>
#include <string>

namespace cmsystemx {

class CMSYSTEMX_API CompileUnit
{
public:
    CompileUnit(const std::string& assemblyFilePath_);
    CompileUnit(const CompileUnit&) = delete;
    CompileUnit& operator=(const CompileUnit&) = delete;
    const std::string& AssemblyFilePath() const { return assemblyFilePath; }
    void Write(Context& context);
    Function* GetOrInsertFunction(Context& context, const std::string& mangledName, FunctionType* functionType);
private:
    std::string assemblyFilePath;
    std::vector<std::unique_ptr<Function>> functions;
    std::unordered_map<std::string, Function*> functionMap;
};

} // namespace cmsystemx
