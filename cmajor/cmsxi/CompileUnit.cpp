// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxi/CompileUnit.hpp>
#include <cmajor/cmsxi/Context.hpp>
#include <fstream>

namespace cmsxi {

CompileUnit::CompileUnit(const std::string& filePath_) : filePath(filePath_)
{
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
        Function* function = new Function(name, type);
        functions.push_back(std::unique_ptr<Function>(function));
        functionMap[name] = function;
        return function;
    }
}

void CompileUnit::Write(Context& context)
{
    std::ofstream file(filePath);
    CodeFormatter formatter(file);
    formatter.SetIndentSize(8);
    context.GetTypeRepository().Write(formatter);
    context.GetDataRepository().Write(context, formatter);
    bool first = true;
    for (const auto& f : functions)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.WriteLine();
        }
        f->Write(formatter, context);
    }
}

} // namespace cmsxi
