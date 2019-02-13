// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsystemx/CompileUnit.hpp>
#include <fstream>

namespace cmsystemx {

CompileUnit::CompileUnit(const std::string& assemblyFilePath_) : assemblyFilePath(assemblyFilePath_)
{
}

void CompileUnit::Write(Context& context)
{
    std::ofstream assemblyFile(assemblyFilePath);
    for (const auto& fun : functions)
    {
        fun->WriteDeclaration(assemblyFile);
    }
    assemblyFile << std::endl;
    for (const auto& fun : functions)
    {
        fun->WriteDefinition(context, assemblyFile);
    }
}

Function* CompileUnit::GetOrInsertFunction(Context& context, const std::string& mangledName, FunctionType* functionType)
{
    auto it = functionMap.find(mangledName);
    if (it != functionMap.cend())
    {
        return it->second;
    }
    else
    {
        Function* function = new Function(context, mangledName, functionType);
        functions.push_back(std::unique_ptr<Function>(function));
        functionMap[mangledName] = function;
        return function;
    }
}

} // namespace cmsystemx
