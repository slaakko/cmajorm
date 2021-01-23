// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsxi/Data.hpp>
#include <cmajor/cmsxi/Context.hpp>
#include <unordered_map>
#include <set>

namespace cmsxi {

GlobalVariable::GlobalVariable(Type* type_, const std::string& name_) : type(type_), name(name_), initializer(nullptr), linkOnce(false)
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
    if (linkOnce)
    {
        formatter.Write(" once");
    }
    formatter.Write(" ");
    formatter.Write(name);
    if (initializer)
    {
        formatter.Write(" = ");
        if (initializer->IsAggregateValue() || initializer->IsStringValue())
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

DataRepository::DataRepository() : globalVariableDefinitions(), globalVariableMap(), nextStringId(0)
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

GlobalVariable* DataRepository::CreateGlobalStringPtr(Context& context, const std::string& stringValue)
{
    GlobalVariable* globalVariable = new GlobalVariable(context.GetByteType(), "string" + std::to_string(nextStringId++) + "_" + compileUnitId);
    globalVariable->SetInitializer(context.GetStringValue(globalVariable->GetType(context), stringValue));
    globalVariableDefinitions.push_back(std::unique_ptr<GlobalVariable>(globalVariable));
    return globalVariable;
}

void Visit(std::vector<GlobalVariable*>& order, GlobalVariable* variable, std::unordered_set<GlobalVariable*>& visited, std::unordered_set<GlobalVariable*>& tempVisit,
    const std::unordered_map<GlobalVariable*, std::set<GlobalVariable*>>& dependencies, Context& context)
{
    if (tempVisit.find(variable) == tempVisit.end())
    {
        if (visited.find(variable) == visited.end())
        {
            tempVisit.insert(variable);
            auto i = dependencies.find(variable);
            if (i != dependencies.end())
            {
                const std::set<GlobalVariable*>& dependsOn = i->second;
                for (GlobalVariable* var : dependsOn)
                {
                    Visit(order, var, visited, tempVisit, dependencies, context);
                }
                tempVisit.erase(variable);
                visited.insert(variable);
                order.push_back(variable);
            }
            else
            {
                tempVisit.erase(variable);
                visited.insert(variable);
                order.push_back(variable);
            }
        }
    }
    else
    {
        throw std::runtime_error("circular type dependency '" + variable->Name(context) + "' detected");
    }
}

std::vector<GlobalVariable*> CreateDataOrder(const std::vector<std::unique_ptr<GlobalVariable>>& globalVariables, Context& context)
{
    std::unordered_map<std::string, GlobalVariable*> nameMap;
    std::unordered_map<GlobalVariable*, std::set<GlobalVariable*>> dependencies;
    for (const std::unique_ptr<GlobalVariable>& globalVariable : globalVariables)
    {
        nameMap[globalVariable->Name(context)] = globalVariable.get();
    }
    for (const std::unique_ptr<GlobalVariable>& globalVariable : globalVariables)
    {
        ConstantValue* initializer = globalVariable->Initializer();
        if (initializer)
        {
            initializer->AddDependencies(globalVariable.get(), nameMap, dependencies, context);
        }
    }
    std::vector<GlobalVariable*> order;
    std::unordered_set<GlobalVariable*> visited;
    std::unordered_set<GlobalVariable*> tempVisit;
    for (const std::unique_ptr<GlobalVariable>& globalVariable : globalVariables)
    {
        if (visited.find(globalVariable.get()) == visited.end())
        {
            Visit(order, globalVariable.get(), visited, tempVisit, dependencies, context);
        }
    }
    return order;
}

void DataRepository::Write(Context& context, CodeFormatter& formatter)
{
    if (globalVariableDefinitions.empty()) return;
    formatter.WriteLine("data");
    formatter.WriteLine("{");
    formatter.IncIndent();
    std::vector<GlobalVariable*> dataOrder = CreateDataOrder(globalVariableDefinitions, context);
    for (const auto& globalVariable : dataOrder)
    {
        globalVariable->Write(context, formatter);
        formatter.WriteLine();
    }
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();
}

void DataRepository::SetCompileUnitId(const std::string& compileUnitId_)
{
    compileUnitId = compileUnitId_;
}

} // namespace cmsxi

