// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppi/CompileUnit.hpp>
#include <cmajor/cmcppi/Function.hpp>
#include <fstream>

namespace cmcppi {

CompileUnit::CompileUnit(const std::string& filePath_) : filePath(filePath_), context()
{
}

void CompileUnit::SetId(const std::string& id_)
{
    id = id_;
}

void CompileUnit::SetSourceFilePath(const std::string& sourceFilePath_)
{
    sourceFilePath = sourceFilePath_;
}

void CompileUnit::Write()
{
    std::ofstream file(filePath);
    CodeFormatter formatter(file);
    formatter.SetIndentSize(8);
    formatter.WriteLine("// " + sourceFilePath);
    formatter.WriteLine();
    context.GetTypeRepository().Write(formatter);
}

Function* CompileUnit::GetOrInsertFunction(const std::string& name, FunctionType* type)
{
    auto it = functionMap.find(name);
    if (it != functionMap.cend())
    {
        return it->second;
    }
    else
    {
        Function* function = new Function(name, type, context);
        functions.push_back(std::unique_ptr<Function>(function));
        functionMap[name] = function;
        return function;
    }
}

} // namespace cmcppi
