// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppi/Data.hpp>
#include <cmajor/cmcppi/Context.hpp>
#include <unordered_map>
#include <unordered_set>
#include <set>

namespace cmcppi {

GlobalVariable::GlobalVariable(Type* type_, const std::string& name_) : type(type_), name(name_), initializer(nullptr), linkOnce(false), stringPtr(false)
{
}

std::string GlobalVariable::Name(Context& context)
{
    return name;
}

Type* GlobalVariable::GetType(Context& context)
{
    if (stringPtr)
    {
        return type;
    }
    else
    {
        return context.GetPtrType(type);
    }
}

void GlobalVariable::Write(Context& context, CodeFormatter& formatter)
{
    if (linkOnce)
    {
        formatter.Write("inline ");
    }
    if (!initializer)
    {
        formatter.Write("extern ");
    }
    if (type->IsArrayType())
    {
        ArrayType* arrayType = static_cast<ArrayType*>(type);
        if (initializer)
        {
            formatter.Write(arrayType->ElementType()->Name());
        }
        else
        {
            formatter.Write(arrayType->Name());
        }
    }
    else
    {
        formatter.Write(type->Name());
    }
    formatter.Write(" ");
    formatter.Write("__global_" + name);
    if (initializer)
    {
        if (type->IsArrayType())
        {
            ArrayType* arrayType = static_cast<ArrayType*>(type);
            if (arrayType->Size() != 0)
            {
                formatter.Write("[" + std::to_string(arrayType->Size()) + "]");
            }
            else
            {
                formatter.Write("[]");
            }
        }
    }
    if (initializer)
    {
        formatter.Write(" = ");
        if (stringPtr)
        {
            if (type->IsPtrType())
            {
                PtrType* ptrType = static_cast<PtrType*>(type);
                Type* baseType = ptrType->BaseType();
                if (baseType->Id() == charTypeId)
                {
                    formatter.Write("(char8_t*)");
                }
                else if (baseType->Id() == wcharTypeId)
                {
                    formatter.Write("(char16_t*)");
                }
                else if (baseType->Id() == ucharTypeId)
                {
                    formatter.Write("(char32_t*)");
                }
            }
            formatter.Write(initializer->Name(context));
        }
        else if (initializer->IsAggregateValue() || initializer->IsStringValue() || initializer->IsWStringValue() || initializer->IsUStringValue())
        {
            formatter.Write(initializer->Name(context));
        }
        else
        {
            formatter.Write(initializer->Name(context));
        }
    }
    else if (linkOnce)
    {
        throw std::runtime_error("global variable '" + name + "' with inline linkage has no initializer");
    }
    formatter.WriteLine(";");
    if (stringPtr)
    {
        formatter.Write(type->Name());
        formatter.Write(" ");
        formatter.Write(name + " = __global_" + name);
    }
    else
    {
        formatter.Write("inline ");
        formatter.Write(context.GetPtrType(type)->Name());
        formatter.Write(" ");
        formatter.Write(name + " = &__global_" + name);
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
    GlobalVariable* globalVariable = new GlobalVariable(context.GetPtrType(context.GetCharType()), "string" + std::to_string(nextStringId++) + "_" + compileUnitId);
    globalVariable->SetInitializer(context.GetStringValue(globalVariable->GetType(context), stringValue));
    globalVariable->SetStringPtr();
    globalVariableDefinitions.push_back(std::unique_ptr<GlobalVariable>(globalVariable));
    return globalVariable;
}

GlobalVariable* DataRepository::CreateGlobalWStringPtr(Context& context, const std::u16string& stringValue)
{
    GlobalVariable* globalVariable = new GlobalVariable(context.GetPtrType(context.GetWCharType()), "wstring" + std::to_string(nextStringId++) + "_" + compileUnitId);
    globalVariable->SetInitializer(context.GetWStringValue(globalVariable->GetType(context), stringValue));
    globalVariable->SetStringPtr();
    globalVariableDefinitions.push_back(std::unique_ptr<GlobalVariable>(globalVariable));
    return globalVariable;
}

GlobalVariable* DataRepository::CreateGlobalUStringPtr(Context& context, const std::u32string& stringValue)
{
    GlobalVariable* globalVariable = new GlobalVariable(context.GetPtrType(context.GetUCharType()), "ustring" + std::to_string(nextStringId++) + "_" + compileUnitId);
    globalVariable->SetInitializer(context.GetUStringValue(globalVariable->GetType(context), stringValue));
    globalVariable->SetStringPtr();
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
    formatter.WriteLine("extern \"C\" {");
    formatter.WriteLine();
    std::vector<GlobalVariable*> dataOrder = CreateDataOrder(globalVariableDefinitions, context);
    for (GlobalVariable* globalVariable : dataOrder)
    {
        globalVariable->Write(context, formatter);
        formatter.WriteLine(";");
    }
    formatter.WriteLine();
    formatter.WriteLine("} // extern \"C\"");
}

void DataRepository::SetCompileUnitId(const std::string& compileUnitId_)
{
    compileUnitId = compileUnitId_;
}

} // namespace cmcppi

