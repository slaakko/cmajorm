// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/AssemblyFile.hpp>
#include <soulng/util/TextUtils.hpp>

namespace cmsx::assembler {

using namespace soulng::util;

AssemblyObject::AssemblyObject(AssemblyObjectKind kind_) : kind(kind_)
{
}

AssemblyObject::~AssemblyObject()
{
}

AssemblyFunction::AssemblyFunction(const std::string& name_) : AssemblyObject(AssemblyObjectKind::function), name(name_), activeFunctionPart(FunctionPart::body)
{
}

void AssemblyFunction::SetActiveFunctionPart(FunctionPart functionPart)
{
    activeFunctionPart = functionPart;
}

void AssemblyFunction::AddInstruction(Instruction* inst)
{
    switch (activeFunctionPart)
    {
        case FunctionPart::prologue: prologue.push_back(std::unique_ptr<Instruction>(inst)); break;
        case FunctionPart::body: body.push_back(std::unique_ptr<Instruction>(inst)); break;
        case FunctionPart::epilogue: epilogue.push_back(std::unique_ptr<Instruction>(inst)); break;
    }
}

void AssemblyFunction::Write(CodeFormatter& formatter)
{
    formatter.WriteLine(name + " FUNC");
    formatter.IncIndent();
    for (const auto& instruction : prologue)
    {
        instruction->Write(formatter);
    }
    for (const auto& instruction : body)
    {
        instruction->Write(formatter);
    }
    for (const auto& instruction : epilogue)
    {
        instruction->Write(formatter);
    }
    formatter.DecIndent();
    formatter.WriteLine(name + " ENDF");
}

AssemblyStruct::AssemblyStruct(const std::string& name_) : AssemblyObject(AssemblyObjectKind::structure), name(name_)
{
}

void AssemblyStruct::AddInstruction(Instruction* inst)
{
    content.push_back(std::unique_ptr<Instruction>(inst));
}

void AssemblyStruct::Write(CodeFormatter& formatter)
{
    formatter.WriteLine(name + " STRUCT");
    formatter.IncIndent();
    for (const auto& instruction : content)
    {
        instruction->Write(formatter);
    }
    formatter.DecIndent();
    formatter.WriteLine(name + " ENDS");
}

AssemblyExternObject::AssemblyExternObject() : AssemblyObject(AssemblyObjectKind::externObjects)
{
}

void AssemblyExternObject::AddExternSymbol(GlobalSymbol* externSymbol)
{
    content.push_back(std::unique_ptr<Instruction>(new Instruction(EXTERN)));
    content.back()->AddOperand(externSymbol);
}

void AssemblyExternObject::Write(CodeFormatter& formatter)
{
    formatter.IncIndent();
    for (const auto& inst : content)
    {
        inst->Write(formatter);
    }
    formatter.DecIndent();
}

AssemblyLinkOnceObject::AssemblyLinkOnceObject() : AssemblyObject(AssemblyObjectKind::linkOnceObjects)
{
}

void AssemblyLinkOnceObject::AddLinkOnceSymbol(GlobalSymbol* linkOnceSymbol)
{
    content.push_back(std::unique_ptr<Instruction>(new Instruction(LINKONCE)));
    content.back()->AddOperand(linkOnceSymbol);
}

void AssemblyLinkOnceObject::Write(CodeFormatter& formatter)
{
    formatter.IncIndent();
    for (const auto& inst : content)
    {
        inst->Write(formatter);
    }
    formatter.DecIndent();
}

AssemblyDebugInfo::AssemblyDebugInfo() : AssemblyObject(AssemblyObjectKind::debugInfo)
{
}

void AssemblyDebugInfo::AddInstruction(Instruction* inst)
{
    content.push_back(std::unique_ptr<Instruction>(inst));
}

void AssemblyDebugInfo::Write(CodeFormatter& formatter)
{
    formatter.IncIndent();
    for (const auto& inst : content)
    {
        inst->Write(formatter);
    }
    formatter.DecIndent();
}

AssemblySection::AssemblySection(AssemblySectionKind kind_) : kind(kind_), externObject(nullptr), linkOnceObject(nullptr)
{
}

AssemblyExternObject* AssemblySection::GetOrCreateExternObject()
{
    if (!externObject)
    {
        externObject = new AssemblyExternObject();
        objects.push_back(std::unique_ptr<AssemblyObject>(externObject));
    }
    return externObject;
}

AssemblyLinkOnceObject* AssemblySection::GetOrCreateLinkOnceObject()
{
    if (!linkOnceObject)
    {
        linkOnceObject = new AssemblyLinkOnceObject();
        objects.push_back(std::unique_ptr<AssemblyObject>(linkOnceObject));
    }
    return linkOnceObject;
}

AssemblyFunction* AssemblySection::CreateFunction(const std::string& name)
{
    AssemblyFunction* function = new AssemblyFunction(name);
    objects.push_back(std::unique_ptr<AssemblyObject>(function));
    return function;
}

AssemblyStruct* AssemblySection::CreateStructure(const std::string& name)
{
    AssemblyStruct* structure = new AssemblyStruct(name);
    objects.push_back(std::unique_ptr<AssemblyObject>(structure));
    return structure;
}

AssemblyDebugInfo* AssemblySection::CreateDebugInfo()
{
    AssemblyDebugInfo* debugInfo = new AssemblyDebugInfo();
    objects.push_back(std::unique_ptr<AssemblyObject>(debugInfo));
    return debugInfo;
}

void AssemblySection::Write(CodeFormatter& formatter)
{
    switch (kind)
    {
        case AssemblySectionKind::link:
        {
            formatter.IncIndent();
            formatter.WriteLine(".LINK");
            formatter.WriteLine();
            formatter.DecIndent();
            break;
        }
        case AssemblySectionKind::code:
        {
            formatter.IncIndent();
            formatter.WriteLine(".CODE");
            formatter.WriteLine();
            formatter.DecIndent();
            break;
        }
        case AssemblySectionKind::data:
        {
            formatter.IncIndent();
            formatter.WriteLine(".DATA");
            formatter.WriteLine();
            formatter.DecIndent();
            break;
        }
        case AssemblySectionKind::debug:
        {
            formatter.IncIndent();
            formatter.WriteLine(".DEBUG");
            formatter.WriteLine();
            formatter.DecIndent();
            break;
        }
    }
    bool first = true;
    for (const auto& object : objects)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.WriteLine();
        }
        object->Write(formatter);
    }
}

AssemblyFile::AssemblyFile(const std::string& filePath_) : filePath(filePath_), file(filePath), formatter(file)
{
    formatter.SetIndentSize(8);
}

AssemblyFunction* AssemblyFile::CreateFunction(const std::string& name)
{
    return GetCodeSection()->CreateFunction(name);
}

AssemblySection* AssemblyFile::GetLinkSection()
{
    if (!linkSection)
    {
        linkSection.reset(new AssemblySection(AssemblySectionKind::link));
    }
    return linkSection.get();
}

AssemblySection* AssemblyFile::GetCodeSection()
{
    if (!codeSection)
    {
        codeSection.reset(new AssemblySection(AssemblySectionKind::code));
    }
    return codeSection.get();
}

AssemblySection* AssemblyFile::GetDataSection()
{
    if (!dataSection)
    {
        dataSection.reset(new AssemblySection(AssemblySectionKind::data));
    }
    return dataSection.get();
}

AssemblySection* AssemblyFile::GetDebugSection()
{
    if (!debugSection)
    {
        debugSection.reset(new AssemblySection(AssemblySectionKind::debug));
    }
    return debugSection.get();
}

void AssemblyFile::Write()
{
    if (linkSection)
    {
        linkSection->Write(formatter);
    }
    if (codeSection)
    {
        if (linkSection)
        {
            formatter.WriteLine();
        }
        codeSection->Write(formatter);
    }
    if (dataSection)
    {
        if (linkSection || codeSection)
        {
            formatter.WriteLine();
        }
        dataSection->Write(formatter);
    }
    if (debugSection)
    {
        if (linkSection || codeSection || dataSection)
        {
            formatter.WriteLine();
        }
        debugSection->Write(formatter);
    }
}

} // namespace cmsx::assembler
