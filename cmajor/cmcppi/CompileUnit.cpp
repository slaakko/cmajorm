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
    formatter.SetIndentSize(4);
    formatter.WriteLine("// " + sourceFilePath);
    formatter.WriteLine();
    context.GetTypeRepository().Write(formatter);
    formatter.WriteLine();
    if (!functions.empty())
    {
        formatter.WriteLine("extern \"C\" {");
        formatter.WriteLine();
        for (const auto& f : functions)
        {
            f->WriteDeclaration(formatter, context);
        }
        formatter.WriteLine();
        formatter.WriteLine("} // extern \"C\"");
        formatter.WriteLine();
    }
    context.GetDataRepository().Write(context, formatter);
    if (!functions.empty())
    {
        bool first = true;
        for (const auto& f : functions)
        {
            if (f->BasicBlocks().empty())
            {
                continue;
            }
            formatter.WriteLine();
            if (first)
            {
                formatter.WriteLine("extern \"C\" {");
                formatter.WriteLine();
                first = false;
            }
            f->Write(formatter, context);
        }
        formatter.WriteLine();
        formatter.WriteLine("} // extern \"C\"");
    }
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
