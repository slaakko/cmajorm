// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxi/Data.hpp>
#include <cmajor/cmsxi/Context.hpp>

namespace cmsxi {

GlobalVariable::GlobalVariable(Type* type_, const std::string& name_) : type(type_), name(name_), initializer(nullptr)
{
}

std::string GlobalVariable::Name(Context& context)
{
    return name;
}

Type* GlobalVariable::GetType(Context& context)
{
    return context.GetPtrType(type);
}

void GlobalVariable::Write(Context& context, CodeFormatter& formatter)
{
    formatter.Write(type->Name());
    formatter.Write(" ");
    formatter.Write(name);
    if (initializer)
    {
        formatter.Write(" = ");
        if (initializer->IsAggregateValue())
        {
            formatter.Write(initializer->Name(context));
        }
        else
        {
            formatter.Write(initializer->GetType(context)->Name());
            formatter.Write(" ");
            formatter.Write(initializer->Name(context));
        }
    }
    else
    {
        formatter.Write(";");
    }
}

DataRepository::DataRepository() : globalVariableDefinitions(), globalVariableMap()
{
}

GlobalVariable* DataRepository::GetOrInsertGlobal(const std::string& name, Type* type)
{
    auto it = globalVariableMap.find(name);
    if (it != globalVariableMap.cend())
    {
        return it->second;
    }
    GlobalVariable* globalVariable = new GlobalVariable(type, name);
    globalVariableDefinitions.push_back(std::unique_ptr<GlobalVariable>(globalVariable));
    globalVariableMap[name] = globalVariable;
    return globalVariable;
}

void DataRepository::Write(Context& context, CodeFormatter& formatter)
{
    if (globalVariableDefinitions.empty()) return;
    formatter.WriteLine("data");
    formatter.WriteLine("{");
    formatter.IncIndent();
    for (const auto& globalVariable : globalVariableDefinitions)
    {
        globalVariable->Write(context, formatter);
        formatter.WriteLine();
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();
}

} // namespace cmsxi

