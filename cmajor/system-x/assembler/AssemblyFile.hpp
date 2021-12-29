// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_ASSEMBLER_ASSEMBLY_FILE_INCLUDED
#define CMSX_ASSEMBLER_ASSEMBLY_FILE_INCLUDED
#include <system-x/assembler/Instruction.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <fstream>
#include <string>

namespace cmsx::assembler {

using soulng::util::CodeFormatter;

enum class AssemblyObjectKind : int
{
    function, structure, externObjects, linkOnceObjects
};

enum class AssemblySectionKind : int
{
    link, code, data, debug
};

enum class FunctionPart : int
{
    prologue, body, epilogue
};

class CMSX_ASSEMBLER_API AssemblyObject 
{
public:
    AssemblyObject(AssemblyObjectKind kind_);
    virtual ~AssemblyObject();
    virtual void Write(CodeFormatter& formatter) = 0;
private:
    AssemblyObjectKind kind;
};

class CMSX_ASSEMBLER_API AssemblyFunction : public AssemblyObject
{
public:
    AssemblyFunction(const std::string& name_);
    AssemblyFunction(const AssemblyFunction&) = delete;
    AssemblyFunction& operator=(const AssemblyFunction&) = delete;
    void SetActiveFunctionPart(FunctionPart functionPart);
    void AddInstruction(Instruction* inst);
    void Write(CodeFormatter& formatter) override;
private:
    std::string name;
    FunctionPart activeFunctionPart;
    std::vector<std::unique_ptr<Instruction>> prologue;
    std::vector<std::unique_ptr<Instruction>> body;
    std::vector<std::unique_ptr<Instruction>> epilogue;
};

class CMSX_ASSEMBLER_API AssemblyStruct : public AssemblyObject
{
public:
    AssemblyStruct(const std::string& name_);
    AssemblyStruct(const AssemblyStruct&) = delete;
    AssemblyStruct& operator=(const AssemblyStruct&) = delete;
    void AddInstruction(Instruction* inst);
    void Write(CodeFormatter& formatter) override;
private:
    std::string name;
    std::vector<std::unique_ptr<Instruction>> content;
};

class CMSX_ASSEMBLER_API AssemblyExternObject : public AssemblyObject
{
public:
    AssemblyExternObject();
    AssemblyExternObject(const AssemblyExternObject&) = delete;
    AssemblyExternObject& operator=(const AssemblyExternObject&) = delete;
    void AddExternSymbol(GlobalSymbol* externSymbol);
    void Write(CodeFormatter& formatter) override;
private:
    std::vector<std::unique_ptr<Instruction>> content;
};

class CMSX_ASSEMBLER_API AssemblyLinkOnceObject : public AssemblyObject
{
public:
    AssemblyLinkOnceObject();
    AssemblyLinkOnceObject(const AssemblyLinkOnceObject&) = delete;
    AssemblyLinkOnceObject& operator=(const AssemblyLinkOnceObject&) = delete;
    void AddLinkOnceSymbol(GlobalSymbol* linkOnceSymbol);
    void Write(CodeFormatter& formatter) override;
private:
    std::vector<std::unique_ptr<Instruction>> content;
};

class CMSX_ASSEMBLER_API AssemblySection
{
public:
    AssemblySection(AssemblySectionKind kind_);
    AssemblySection(const AssemblySection&) = delete;
    AssemblySection& operator=(const AssemblySection&) = delete;
    AssemblyExternObject* GetOrCreateExternObject();
    AssemblyLinkOnceObject* GetOrCreateLinkOnceObject();
    AssemblyFunction* CreateFunction(const std::string& name);
    AssemblyStruct* CreateStructure(const std::string& name);
    void Write(CodeFormatter& formatter);
private:
    AssemblySectionKind kind;
    std::vector<std::unique_ptr<AssemblyObject>> objects;
    AssemblyExternObject* externObject;
    AssemblyLinkOnceObject* linkOnceObject;
};

class CMSX_ASSEMBLER_API AssemblyFile
{
public:
    AssemblyFile(const std::string& filePath_);
    AssemblySection* GetLinkSection();
    AssemblySection* GetCodeSection();
    AssemblySection* GetDataSection();
    AssemblySection* GetDebugSection();
    AssemblyFunction* CreateFunction(const std::string& name);
    void Write();
private:
    std::string filePath;
    std::ofstream file;
    CodeFormatter formatter;
    std::unique_ptr<AssemblySection> linkSection;
    std::unique_ptr<AssemblySection> codeSection;
    std::unique_ptr<AssemblySection> dataSection;
    std::unique_ptr<AssemblySection> debugSection;
};

} // namespace cmsx::assembler

#endif // CMSX_ASSEMBLER_ASSEMBLY_FILE_INCLUDED
