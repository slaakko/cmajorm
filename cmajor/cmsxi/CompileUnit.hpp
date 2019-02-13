// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMSXI_COMPILE_UNIT_INCLUDED
#define CMAJOR_CMSXI_COMPILE_UNIT_INCLUDED
#include <cmajor/cmsxi/Function.hpp>

namespace cmsxi {

class CMSXI_API CompileUnit
{
public:
    CompileUnit(const std::string& filePath_);
    CompileUnit(const CompileUnit&) = delete;
    CompileUnit& operator=(const CompileUnit&) = delete;
    Function* GetOrInsertFunction(const std::string& name, FunctionType* type);
    void Write(Context& context);
private:
    std::string filePath;
    std::vector<std::unique_ptr<Function>> functions;
    std::unordered_map<std::string, Function*> functionMap;
};

} // namespace cmsxi

#endif // CMAJOR_CMSXI_COMPILE_UNIT_INCLUDED
