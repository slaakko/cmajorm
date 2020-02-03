// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/symbols/Warning.hpp>
#include <cmajor/symbols/DebugFlags.hpp>
#include <sngcm/ast/Project.hpp>
#include <sngcm/ast/AstReader.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Time.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;
using namespace soulng::util;

class SystemModuleSet
{
public:
    static void Init();
    static void Done();
    static SystemModuleSet& Instance() { Assert(instance, "system module set not initialized"); return *instance; }
    bool IsSystemModule(const std::u32string& moduleName) const;
private:
    static std::unique_ptr<SystemModuleSet> instance;
    std::unordered_set<std::u32string> systemModuleNames;
    SystemModuleSet();
};

std::unique_ptr<SystemModuleSet> SystemModuleSet::instance;

void SystemModuleSet::Init()
{
    instance.reset(new SystemModuleSet());
}

void SystemModuleSet::Done()
{
    instance.reset();
}

SystemModuleSet::SystemModuleSet()
{
    systemModuleNames.insert(U"System.Core");
    systemModuleNames.insert(U"System.Runtime");
    systemModuleNames.insert(U"System.Base");
    systemModuleNames.insert(U"System.Lex");
    systemModuleNames.insert(U"System.Parsing");
    systemModuleNames.insert(U"System.Net.Sockets");
    systemModuleNames.insert(U"System.Net.Http");
    systemModuleNames.insert(U"System.Json");
    systemModuleNames.insert(U"System.Xml");
    systemModuleNames.insert(U"System.Dom");
    systemModuleNames.insert(U"System.Numerics.Multiprecision");
    systemModuleNames.insert(U"System.IO.Compression");
    systemModuleNames.insert(U"System.RegularExpressions");
    systemModuleNames.insert(U"System.XPath");
    systemModuleNames.insert(U"System.Init");
    systemModuleNames.insert(U"System");
    systemModuleNames.insert(U"System.Install");
    systemModuleNames.insert(U"System.Windows");
    systemModuleNames.insert(U"System.Windows.Install");
}

bool SystemModuleSet::IsSystemModule(const std::u32string& moduleName) const
{
    auto it = systemModuleNames.find(moduleName);
    if (it != systemModuleNames.cend())
    {
        return true;
    }
    return false;
}

bool IsSystemModule(const std::u32string& moduleName) 
{
    return SystemModuleSet::Instance().IsSystemModule(moduleName);
}

const char* cmajorModuleTag = "CMM";

ModuleTag::ModuleTag()
{
    bytes[0] = cmajorModuleTag[0];
    bytes[1] = cmajorModuleTag[1];
    bytes[2] = cmajorModuleTag[2];
    bytes[3] = currentModuleFormat;
}

void ModuleTag::Write(SymbolWriter& writer)
{
    writer.GetBinaryWriter().Write(bytes[0]);
    writer.GetBinaryWriter().Write(bytes[1]);
    writer.GetBinaryWriter().Write(bytes[2]);
    writer.GetBinaryWriter().Write(bytes[3]);
}

void ModuleTag::Read(SymbolReader& reader)
{
    bytes[0] = reader.GetBinaryReader().ReadByte();
    bytes[1] = reader.GetBinaryReader().ReadByte();
    bytes[2] = reader.GetBinaryReader().ReadByte();
    bytes[3] = reader.GetBinaryReader().ReadByte();
}

std::string ModuleFlagStr(ModuleFlags flags)
{
    std::string s;
    if ((flags & ModuleFlags::system) != ModuleFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("system");
    }
    if ((flags & ModuleFlags::core) != ModuleFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("core");
    }
    return s;
}

ModuleDependency::ModuleDependency(Module* module_) : module(module_)
{
}

void ModuleDependency::AddReferencedModule(Module* referencedModule)
{
    if (std::find(referencedModules.cbegin(), referencedModules.cend(), referencedModule) == referencedModules.cend())
    {
        referencedModules.push_back(referencedModule);
    }
}

void ModuleDependency::Dump(CodeFormatter& formatter)
{
    formatter.IncIndent();
    int n = referencedModules.size();
    for (int i = 0; i < n; ++i)
    {
        Module* referencedModule = referencedModules[i];
        formatter.WriteLine(ToUtf8(referencedModule->Name()));
        referencedModule->GetModuleDependency().Dump(formatter);
    }
    formatter.DecIndent();
}

int16_t FileTable::RegisterFilePath(const std::string& filePath)
{
    int16_t fileId = filePaths.size();
    filePaths.push_back(filePath);
    return fileId;
}

std::string FileTable::GetFilePath(int16_t fileId) const
{
    if (fileId >= 0 && fileId < filePaths.size())
    {
        return filePaths[fileId];
    }
    return std::string();
}

void FileTable::Write(BinaryWriter& writer, bool systemModule)
{
    uint32_t n = filePaths.size();
    writer.WriteULEB128UInt(n);
    std::string cmajorRoot;
    if (systemModule)
    {
        cmajorRoot = GetFullPath(CmajorRootDir());
        if (!EndsWith(cmajorRoot, "/"))
        {
            cmajorRoot.append("/");
        }
    }
    for (uint32_t i = 0; i < n; ++i)
    {
        std::string filePath = GetFullPath(filePaths[i]);
        if (systemModule)
        {
            if (filePath.find(cmajorRoot, 0) == 0)
            {
                filePath = filePath.substr(cmajorRoot.size());
            }
        }
        writer.Write(filePath);
    }
}

void FileTable::Read(BinaryReader& reader, bool systemModule)
{
    filePaths.clear();
    std::string cmajorRoot;
    if (systemModule)
    {
        cmajorRoot = GetFullPath(CmajorRootDir());
        if (!EndsWith(cmajorRoot, "/"))
        {
            cmajorRoot.append("/");
        }
    }
    uint32_t n = reader.ReadULEB128UInt();
    for (uint32_t i = 0; i < n; ++i)
    {
        if (systemModule)
        {
            std::string filePath = reader.ReadUtf8String();
            filePath = Path::Combine(cmajorRoot, filePath);
            filePaths.push_back(std::move(filePath));
        }
        else
        {
            std::string filePath = reader.ReadUtf8String();
            filePaths.push_back(std::move(filePath));
        }
    }
}

void FileTable::Dump(CodeFormatter& formatter)
{
    if (!IsEmpty())
    {
        formatter.WriteLine("source file paths:");
        formatter.IncIndent();
        int n = filePaths.size();
        for (int i = 0; i < n; ++i)
        {
            formatter.WriteLine(std::to_string(i) + " : " + filePaths[i]);
        }
        formatter.DecIndent();
    }
}

void Visit(std::vector<Module*>& finishReadOrder, Module* module, std::unordered_set<Module*>& visited, std::unordered_set<Module*>& tempVisit,
    std::unordered_map<Module*, ModuleDependency*>& dependencyMap, const Module* rootModule)
{
    if (tempVisit.find(module) == tempVisit.cend())
    {
        if (visited.find(module) == visited.cend())
        {
            tempVisit.insert(module);
            auto i = dependencyMap.find(module);
            if (i != dependencyMap.cend())
            {
                ModuleDependency* dependency = i->second;
                for (Module* dependentAssembly : dependency->ReferencedModules())
                {
                    Visit(finishReadOrder, dependentAssembly, visited, tempVisit, dependencyMap, rootModule);
                }
                tempVisit.erase(module);
                visited.insert(module);
                finishReadOrder.push_back(module);
            }
            else
            {
                throw std::runtime_error("module '" + ToUtf8(module->Name()) + "' not found in dependencies of module '" + ToUtf8(rootModule->Name()) + "'");
            }
        }
    }
    else
    {
        throw std::runtime_error("circular module dependency '" + ToUtf8(module->Name()) + "' detected in dependencies of module '" + ToUtf8(rootModule->Name()) + "'");
    }
}

std::vector<Module*> CreateFinishReadOrder(std::vector<Module*>& modules, std::unordered_map<Module*, ModuleDependency*>& dependencyMap, const Module* rootModule)
{
    std::vector<Module*> finishReadOrder;
    std::unordered_set<Module*> visited;
    std::unordered_set<Module*> tempVisit;
    for (Module* module : modules)
    {
        if (visited.find(module) == visited.cend())
        {
            Visit(finishReadOrder, module, visited, tempVisit, dependencyMap, rootModule);
        }
    }
    return finishReadOrder;
}

void FinishReads(Module* rootModule, std::vector<Module*>& finishReadOrder, bool all)
{
#ifdef MODULE_READING_DEBUG
    LogMessage(rootModule->LogStreamId(), "FinishReads: begin " + ToUtf8(rootModule->Name()), rootModule->DebugLogIndent());
    rootModule->IncDebugLogIndent();
#endif 
    int n = finishReadOrder.size() - 1;
    if (all)
    {
        n = finishReadOrder.size();
    }
    for (int i = 0; i < n; ++i)
    {
        Module* module = finishReadOrder[i];
        if (!module->HasSymbolTable())
        {
#ifdef MODULE_READING_DEBUG
            LogMessage(rootModule->LogStreamId(), "FinishReads: reading " + ToUtf8(module->Name()), rootModule->DebugLogIndent());
#endif 
            module->CreateSymbolTable();
            std::vector<TypeOrConceptRequest> typeAndConceptRequests;
            std::vector<FunctionRequest> functionRequests;
            std::vector<ClassTemplateSpecializationSymbol*> classTemplateSpecializations;
            std::vector<FunctionSymbol*> conversions;
            std::vector<ArrayTypeSymbol*> arrayTypes;
            std::vector<DerivedTypeSymbol*> derivedTypes;
            SymbolReader reader(module->FilePathReadFrom());
            reader.SetModule(module);
            reader.SetArrayTypesTarget(&arrayTypes);
            reader.SetDerivedTypesTarget(&derivedTypes);
            reader.SetClassTemplateSpecializationTarget(&classTemplateSpecializations);
            reader.SetTypeAndConceptRequestTarget(&typeAndConceptRequests);
            reader.SetFunctionRequestTarget(&functionRequests);
            reader.SetConversionsTarget(&conversions);
            reader.GetBinaryReader().Skip(module->SymbolTablePos());
            reader.SetRootModule(rootModule);
            module->GetSymbolTable().Read(reader);
            for (Module* referencedModule : module->ReferencedModules())
            {
                module->GetSymbolTable().Import(referencedModule->GetSymbolTable());
            }
            module->GetSymbolTable().FinishRead(arrayTypes, derivedTypes, classTemplateSpecializations, typeAndConceptRequests, functionRequests, conversions);
            module->SetImmutable();
#ifdef MODULE_CHECKING
            module->Check();
#endif
            rootModule->GetSymbolTable().Import(module->GetSymbolTable());
        }
        else
        {
#ifdef MODULE_READING_DEBUG
            LogMessage(rootModule->LogStreamId(), "FinishReads: " + ToUtf8(module->Name()) + " in cache", rootModule->DebugLogIndent());
#endif 
            rootModule->GetSymbolTable().Import(module->GetSymbolTable());
        }
    }
#ifdef MODULE_READING_DEBUG
    rootModule->DecDebugLogIndent();
    LogMessage(rootModule->LogStreamId(), "FinishReads: end " + ToUtf8(rootModule->Name()), rootModule->DebugLogIndent());
#endif 
}

void Import(sngcm::ast::Target target, Module* rootModule, Module* module, const std::vector<std::string>& references, std::unordered_set<std::string>& importSet, std::vector<Module*>& modules,
    std::unordered_map<std::string, ModuleDependency*>& moduleDependencyMap, std::unordered_map<std::string, Module*>& readMap)
{
#ifdef MODULE_READING_DEBUG
    LogMessage(rootModule->LogStreamId(), "Import: begin " + ToUtf8(module->Name()), rootModule->DebugLogIndent());
    rootModule->IncDebugLogIndent();
#endif 
    for (const std::string& reference : references)
    {
        if (importSet.find(reference) == importSet.cend())
        {
            importSet.insert(reference);
#ifdef MODULE_READING_DEBUG
            LogMessage(rootModule->LogStreamId(), "Import: reference: " + reference, rootModule->DebugLogIndent());
#endif 
            std::string config = GetConfig();
            boost::filesystem::path mfn = boost::filesystem::path(reference).filename();
            boost::filesystem::path mfp;
            std::string searchedDirectories;
            if (!rootModule->IsSystemModule())
            {
                sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::llvm;
                if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
                {
                    backend = sngcm::ast::BackEnd::cmsx;
                }
                mfp = CmajorSystemLibDir(config, backend);
                searchedDirectories.append("\n").append(mfp.generic_string());
                mfp /= mfn;
                if (!boost::filesystem::exists(mfp))
                {
                    mfp = reference;
                    if (!boost::filesystem::exists(mfp))
                    {
                        boost::filesystem::path mrd = mfp;
                        mrd.remove_filename();
                        searchedDirectories.append("\n").append(mrd.generic_string());
                        throw std::runtime_error("Could not find module reference '" + mfn.generic_string() + "'.\nDirectories searched:\n" + searchedDirectories);
                    }
                }
            }
            else
            {
                mfp = reference;
                if (!boost::filesystem::exists(mfp))
                {
                    boost::filesystem::path mrd = mfp;
                    mrd.remove_filename();
                    searchedDirectories.append("\n").append(mrd.generic_string());
                    throw std::runtime_error("Could not find module reference '" + mfn.generic_string() + "'.\nDirectories searched:\n" + searchedDirectories);
                }
            }
            std::string moduleFilePath = GetFullPath(mfp.generic_string());
            Module* referencedModule = GetModuleFromModuleCache(moduleFilePath);
            rootModule->AllRefModules().push_back(referencedModule);
            readMap[moduleFilePath] = referencedModule;
            importSet.insert(moduleFilePath);
            SymbolReader reader(moduleFilePath);
            referencedModule->ReadHeader(target, reader, rootModule, importSet, modules, moduleDependencyMap, readMap);
            module->AddReferencedModule(referencedModule);
            Import(target, rootModule, module, referencedModule->ReferenceFilePaths(), importSet, modules, moduleDependencyMap, readMap);
        }
        else
        {
            std::string config = GetConfig();
            boost::filesystem::path mfn = boost::filesystem::path(reference).filename();
            boost::filesystem::path mfp;
            std::string searchedDirectories;
            if (!rootModule->IsSystemModule())
            {
                sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::llvm;
                if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
                {
                    backend = sngcm::ast::BackEnd::cmsx;
                }
                mfp = CmajorSystemLibDir(config, backend);
                mfp /= mfn;
                if (!boost::filesystem::exists(mfp))
                {
                    mfp = reference;
                    if (!boost::filesystem::exists(mfp))
                    {
                        boost::filesystem::path mrd = mfp;
                        mrd.remove_filename();
                        searchedDirectories.append("\n").append(mrd.generic_string());
                        throw std::runtime_error("Could not find module reference '" + mfn.generic_string() + "'.\nDirectories searched:\n" + searchedDirectories);
                    }
                }
            }
            else
            {
                mfp = reference;
                if (!boost::filesystem::exists(mfp))
                {
                    boost::filesystem::path mrd = mfp;
                    mrd.remove_filename();
                    searchedDirectories.append("\n").append(mrd.generic_string());
                    throw std::runtime_error("Could not find module reference '" + mfn.generic_string() + "'.\nDirectories searched:\n" + searchedDirectories);
                }
            }
            std::string moduleFilePath = GetFullPath(mfp.generic_string());
            auto it = readMap.find(moduleFilePath);
            if (it != readMap.cend())
            {
                Module* referencedModule = it->second;
                if (rootModule->IsSystemModule() ||
                    ((target == sngcm::ast::Target::program || target == sngcm::ast::Target::library || target == sngcm::ast::Target::unitTest) && referencedModule->Name() != U"System" ||
                    (target == sngcm::ast::Target::winguiapp || target == sngcm::ast::Target::winapp || target == sngcm::ast::Target::winlib) && referencedModule->Name() != U"System.Windows"))
                {
                    module->AddReferencedModule(referencedModule);
                }
#ifdef MODULE_READING_DEBUG
                LogMessage(rootModule->LogStreamId(), "Import: " + ToUtf8(module->Name()) + " references " + ToUtf8(referencedModule->Name()), rootModule->DebugLogIndent());
#endif
            }
            else
            {
                throw std::runtime_error("module file path '" + moduleFilePath + "' not found from module read map for module '" + ToUtf8(rootModule->Name()) + "'");
            }
        }
    }
#ifdef MODULE_READING_DEBUG
    rootModule->DecDebugLogIndent();
    LogMessage(rootModule->LogStreamId(), "Import: end " + ToUtf8(module->Name()), rootModule->DebugLogIndent());
#endif 
}

void ImportModulesWithReferences(sngcm::ast::Target target,
    Module* rootModule, Module* module, const std::vector<std::string>& references, std::unordered_set<std::string>& importSet, std::vector<Module*>& modules,
    std::unordered_map<std::string, ModuleDependency*>& moduleDependencyMap, std::unordered_map<std::string, Module*>& readMap)
{
#ifdef MODULE_READING_DEBUG
    LogMessage(rootModule->LogStreamId(), "ImportModulesWithReferences: begin " + ToUtf8(module->Name()), rootModule->DebugLogIndent());
    rootModule->IncDebugLogIndent();
#endif 
    std::vector<std::string> allReferences = references;
    if (!rootModule->IsSystemModule() && !GetGlobalFlag(GlobalFlags::profile))
    {
        sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::llvm;
        if (GetBackEnd() == cmajor::symbols::BackEnd::cmsx)
        {
            backend = sngcm::ast::BackEnd::cmsx;
        }
        if (target == sngcm::ast::Target::winguiapp || target == sngcm::ast::Target::winapp || target == sngcm::ast::Target::winlib)
        {
            allReferences.push_back(CmajorSystemWindowsModuleFilePath(GetConfig()));
        }
        else
        {
            allReferences.push_back(CmajorSystemModuleFilePath(GetConfig(), backend));
        }
    }
    Import(target, rootModule, module, allReferences, importSet, modules, moduleDependencyMap, readMap);
#ifdef MODULE_READING_DEBUG
    rootModule->DecDebugLogIndent();
    LogMessage(rootModule->LogStreamId(), "ImportModulesWithReferences: end " + ToUtf8(module->Name()), rootModule->DebugLogIndent());
#endif 
}

void ImportModules(sngcm::ast::Target target, Module* rootModule, Module* module, std::unordered_set<std::string>& importSet, std::vector<Module*>& modules,
    std::unordered_map<std::string, ModuleDependency*>& dependencyMap, std::unordered_map<std::string, Module*>& readMap)
{
#ifdef MODULE_READING_DEBUG
    LogMessage(rootModule->LogStreamId(), "ImportModules: begin " + ToUtf8(module->Name()), rootModule->DebugLogIndent());
    rootModule->IncDebugLogIndent();
#endif 
    ImportModulesWithReferences(target, rootModule, module, module->ReferenceFilePaths(), importSet, modules, dependencyMap, readMap);
#ifdef MODULE_READING_DEBUG
    rootModule->DecDebugLogIndent();
    LogMessage(rootModule->LogStreamId(), "ImportModules: end " + ToUtf8(module->Name()), rootModule->DebugLogIndent());
#endif 
}

Module::Module() : 
    format(currentModuleFormat), flags(ModuleFlags::none), name(), originalFilePath(), filePathReadFrom(), referenceFilePaths(), moduleDependency(this), symbolTablePos(0), 
    symbolTable(nullptr), directoryPath(), libraryFilePaths(), moduleIdMap(), logStreamId(0), headerRead(false), systemCoreModule(nullptr), debugLogIndent(0), index(-1),
    buildStartMs(0), buildStopMs(0), preparing(false)
{
}

Module::Module(const std::string& filePath)  :
    format(currentModuleFormat), flags(ModuleFlags::none), name(), originalFilePath(), filePathReadFrom(), referenceFilePaths(), moduleDependency(this), symbolTablePos(0), 
    symbolTable(new SymbolTable(this)), directoryPath(), libraryFilePaths(), moduleIdMap(), logStreamId(0), headerRead(false), systemCoreModule(nullptr), debugLogIndent(0),
    index(-1), buildStartMs(0), buildStopMs(0), preparing(false)
{
    SymbolReader reader(filePath);
    ModuleTag expectedTag;
    ModuleTag tag;
    tag.Read(reader);
    for (int i = 0; i < 3; ++i)
    {
        if (tag.bytes[i] != expectedTag.bytes[i])
        {
            throw std::runtime_error("Invalid Cmajor module tag read from file '" + reader.GetBinaryReader().FileName() + "', please rebuild module from sources");
        }
    }
    if (tag.bytes[3] != expectedTag.bytes[3])
    {
        throw std::runtime_error("Cmajor module format version mismatch reading from file '" + reader.GetBinaryReader().FileName() +
            "': format " + std::string(1, expectedTag.bytes[3]) + " expected, format " + std::string(1, tag.bytes[3]) + " read, please rebuild module from sources");
    }
    flags = ModuleFlags(reader.GetBinaryReader().ReadByte());
    name = reader.GetBinaryReader().ReadUtf32String();
    std::unordered_set<std::string> importSet;
    Module* rootModule = this;
    std::vector<Module*> modules;
    std::unordered_map<std::string, ModuleDependency*> moduleDependencyMap;
    std::unordered_map<std::string, Module*> readMap;
    if (SystemModuleSet::Instance().IsSystemModule(name)) SetSystemModule();
    SymbolReader reader2(filePath);
    ReadHeader(sngcm::ast::Target::library, reader2, rootModule, importSet, modules, moduleDependencyMap, readMap);
    moduleDependencyMap[originalFilePath] = &moduleDependency;
    std::unordered_map<Module*, ModuleDependency*> dependencyMap;
    for (const auto& p : moduleDependencyMap)
    {
        dependencyMap[p.second->GetModule()] = p.second;
    }
    std::vector<Module*> finishReadOrder = CreateFinishReadOrder(modules, dependencyMap, rootModule);
    if (!fileTable.IsEmpty())
    {
#ifdef _WIN32
        if (GetBackEnd() == BackEnd::cmsx)
        {
            libraryFilePath = GetFullPath(boost::filesystem::path(originalFilePath).replace_extension(".a").generic_string());
        }
        else if (GetBackEnd() == BackEnd::llvm)
        {
            libraryFilePath = GetFullPath(boost::filesystem::path(originalFilePath).replace_extension(".lib").generic_string());
        }
#else
        libraryFilePath = GetFullPath(boost::filesystem::path(originalFilePath).replace_extension(".a").generic_string());
#endif
    }
    for (Module* module : finishReadOrder)
    {
        if (!module->LibraryFilePath().empty() && !module->fileTable.IsEmpty())
        {
            libraryFilePaths.push_back(module->LibraryFilePath());
        }
    }
    FinishReads(rootModule, finishReadOrder, true);
}

Module::Module(const std::u32string& name_, const std::string& filePath_, sngcm::ast::Target target) :
    format(currentModuleFormat), flags(ModuleFlags::none), name(name_), originalFilePath(filePath_), filePathReadFrom(), referenceFilePaths(), moduleDependency(this), symbolTablePos(0),
    symbolTable(new SymbolTable(this)), directoryPath(), libraryFilePaths(), moduleIdMap(), logStreamId(0), headerRead(false), systemCoreModule(nullptr), debugLogIndent(0),
    index(-1), buildStartMs(0), buildStopMs(0), preparing(false)
{
    if (SystemModuleSet::Instance().IsSystemModule(name))
    {
        SetSystemModule();
    }
    if (name == U"System.Core")
    {
        SetCore();
    }
}

void Module::PrepareForCompilation(const std::vector<std::string>& references, sngcm::ast::Target target)
{
    boost::filesystem::path mfd = originalFilePath;
    mfd.remove_filename();
    boost::filesystem::create_directories(mfd);
    SetDirectoryPath(GetFullPath(mfd.generic_string()));
    if (name == U"System.Core")
    {
        InitCoreSymbolTable(*symbolTable);
    }
    std::unordered_set<std::string> importSet;
    Module* rootModule = this;
    rootModule->RegisterFileTable(&fileTable, rootModule);
    std::unordered_map<std::string, ModuleDependency*> moduleDependencyMap;
    std::unordered_map<std::string, Module*> readMap;
    std::vector<Module*> modules;
    ImportModulesWithReferences(target, rootModule, rootModule, references, importSet, modules, moduleDependencyMap, readMap);
    modules.push_back(this);
    moduleDependencyMap[originalFilePath] = &moduleDependency;
    std::unordered_map<Module*, ModuleDependency*> dependencyMap;
    for (const auto& p : moduleDependencyMap)
    {
        dependencyMap[p.second->GetModule()] = p.second;
    }
    std::vector<Module*> finishReadOrder = CreateFinishReadOrder(modules, dependencyMap, rootModule);
    if (!this->fileTable.IsEmpty())
    {
#ifdef _WIN32
        if (GetBackEnd() == BackEnd::cmsx)
        {
            libraryFilePath = GetFullPath(boost::filesystem::path(originalFilePath).replace_extension(".a").generic_string());
        }
        else if (GetBackEnd() == BackEnd::llvm)
        {
            libraryFilePath = GetFullPath(boost::filesystem::path(originalFilePath).replace_extension(".lib").generic_string());
        }
#else
        libraryFilePath = GetFullPath(boost::filesystem::path(originalFilePath).replace_extension(".a").generic_string());
#endif
    }
    for (Module* module : finishReadOrder)
    {
        if (!module->LibraryFilePath().empty())
        {
            libraryFilePaths.push_back(module->LibraryFilePath());
        }
    }
    FinishReads(rootModule, finishReadOrder, false);
}

void Module::CreateSymbolTable()
{
    symbolTable.reset(new SymbolTable(this));
}

void Module::RegisterFileTable(FileTable* fileTable, Module* module)
{
    int16_t moduleId = fileTables.size();
    fileTables.push_back(fileTable);
    moduleIdMap[module] = moduleId;
}

void Module::SetLexers(std::vector<std::unique_ptr<CmajorLexer>>&& lexers_)
{
    lexers = std::move(lexers_);
    for (const auto& lexer : lexers)
    {
        lexerVec.push_back(lexer.get());
    }
}

std::vector<soulng::lexer::Lexer*>* Module::GetLexers() 
{
    return &lexerVec;
}

std::string Module::GetFilePath(int32_t fileIndex) const
{
    if (fileIndex == -1)
    {
        return std::string();
    }
    int16_t moduleId = sngcm::ast::GetModuleId(fileIndex);
    int16_t fileId = sngcm::ast::GetFileId(fileIndex);
    if (moduleId >= 0 && moduleId < fileTables.size())
    {
        FileTable* fileTable = fileTables[moduleId];
        return fileTable->GetFilePath(fileId);
    }
    return std::string();
}

std::u32string Module::GetErrorLines(const Span& span) const
{
    if (span.fileIndex >= 0)
    {
        int16_t moduleId = sngcm::ast::GetModuleId(span.fileIndex);
        if (moduleId == 0 && GetFlag(ModuleFlags::compiling))
        {
            int16_t fileId = sngcm::ast::GetFileId(span.fileIndex);
            if (fileId < lexers.size())
            {
                return lexers[fileId]->ErrorLines(span);
            }
        }
        else
        {
            std::string filePath = GetFilePath(span.fileIndex);
            if (filePath.empty())
            {
                return std::u32string();
            }
            std::u32string content = ToUtf32(soulng::util::ReadFile(filePath));
            return soulng::lexer::GetErrorLines(content.c_str(), content.c_str() + content.length(), span);
        }
    }
    return std::u32string();
}

void Module::GetColumns(const Span& span, int32_t& startCol, int32_t& endCol) const
{
    if (span.fileIndex >= 0)
    {
        int16_t moduleId = sngcm::ast::GetModuleId(span.fileIndex);
        if (moduleId == 0 && GetFlag(ModuleFlags::compiling))
        {
            int16_t fileId = sngcm::ast::GetFileId(span.fileIndex);
            if (fileId < lexers.size())
            {
                return lexers[fileId]->GetColumns(span, startCol, endCol);
            }
        }
        else
        {
            std::string filePath = GetFilePath(span.fileIndex);
            if (filePath.empty())
            {
                return;
            }
            std::u32string content = ToUtf32(soulng::util::ReadFile(filePath));
            return soulng::lexer::GetColumns(content.c_str(), content.c_str() + content.length(), span, startCol, endCol);
        }
    }
}

void Module::Write(SymbolWriter& writer)
{
    ModuleTag tag;
    tag.Write(writer);
    writer.GetBinaryWriter().Write(static_cast<uint8_t>(flags & ~(ModuleFlags::root | ModuleFlags::immutable | ModuleFlags::compiling)));
    writer.GetBinaryWriter().Write(name);
    writer.GetBinaryWriter().Write(originalFilePath);
    uint32_t nr = referencedModules.size();
    writer.GetBinaryWriter().WriteULEB128UInt(nr);
    for (uint32_t i = 0; i < nr; ++i)
    {
        Module* referencedModule = referencedModules[i];
        writer.GetBinaryWriter().Write(referencedModule->OriginalFilePath());
    }
    fileTable.Write(writer.GetBinaryWriter(), IsSystemModule());
    uint32_t efn = exportedFunctions.size();
    writer.GetBinaryWriter().WriteULEB128UInt(efn);
    for (uint32_t i = 0; i < efn; ++i)
    {
        writer.GetBinaryWriter().Write(exportedFunctions[i]);
    }
    uint32_t edn = exportedData.size();
    writer.GetBinaryWriter().WriteULEB128UInt(edn);
    for (uint32_t i = 0; i < edn; ++i)
    {
        writer.GetBinaryWriter().Write(exportedData[i]);
    }
    symbolTable->Write(writer);
}

void Module::AddReferencedModule(Module* referencedModule)
{
    moduleDependency.AddReferencedModule(referencedModule);
    if (std::find(referencedModules.cbegin(), referencedModules.cend(), referencedModule) == referencedModules.cend())
    {
        referencedModules.push_back(referencedModule);
    }
}

void Module::ReadHeader(sngcm::ast::Target target, SymbolReader& reader, Module* rootModule, std::unordered_set<std::string>& importSet, std::vector<Module*>& modules,
    std::unordered_map<std::string, ModuleDependency*>& dependencyMap, std::unordered_map<std::string, Module*>& readMap)
{
    if (headerRead)
    {
#ifdef MODULE_READING_DEBUG
        LogMessage(rootModule->LogStreamId(), "ReadHeader: cached begin " + ToUtf8(name), rootModule->DebugLogIndent());
        rootModule->IncDebugLogIndent();
#endif 
        rootModule->RegisterFileTable(&fileTable, this);
        if (dependencyMap.find(originalFilePath) == dependencyMap.cend())
        {
            modules.push_back(this);
            dependencyMap[originalFilePath] = &moduleDependency;
        }
        for (const std::string& exportedFunction : exportedFunctions)
        {
            rootModule->allExportedFunctions.push_back(exportedFunction);
            for (const std::string& data : exportedData)
            {
                rootModule->allExportedData.push_back(data);
            }
        }
        ImportModules(target, rootModule, this, importSet, modules, dependencyMap, readMap); 
#ifdef MODULE_READING_DEBUG
        rootModule->DecDebugLogIndent();
        LogMessage(rootModule->LogStreamId(), "ReadHeader: cached end " + ToUtf8(name), rootModule->DebugLogIndent());
#endif 
        return;
    }
    headerRead = true;
    ModuleTag expectedTag;
    ModuleTag tag;
    tag.Read(reader);
    for (int i = 0; i < 3; ++i)
    {
        if (tag.bytes[i] != expectedTag.bytes[i])
        {
            throw std::runtime_error("Invalid Cmajor module tag read from file '" + reader.GetBinaryReader().FileName() + "', please rebuild module from sources");
        }
    }
    if (tag.bytes[3] != expectedTag.bytes[3])
    {
        throw std::runtime_error("Cmajor module format version mismatch reading from file '" + reader.GetBinaryReader().FileName() + 
            "': format " + std::string(1, expectedTag.bytes[3]) + " expected, format " + std::string(1, tag.bytes[3]) + " read, please rebuild module from sources");
    }
    rootModule->RegisterFileTable(&fileTable, this);
    flags = ModuleFlags(reader.GetBinaryReader().ReadByte());
    name = reader.GetBinaryReader().ReadUtf32String();
#ifdef MODULE_READING_DEBUG
    LogMessage(rootModule->LogStreamId(), "ReadHeader: read begin " + ToUtf8(name), rootModule->DebugLogIndent());
    rootModule->IncDebugLogIndent();
#endif 
    originalFilePath = reader.GetBinaryReader().ReadUtf8String();
    if (dependencyMap.find(originalFilePath) == dependencyMap.cend())
    {
        modules.push_back(this);
        dependencyMap[originalFilePath] = &moduleDependency;
    }
    filePathReadFrom = GetFullPath(reader.GetBinaryReader().FileName());
    referenceFilePaths.clear();
    uint32_t nr = reader.GetBinaryReader().ReadULEB128UInt();
    for (uint32_t i = 0; i < nr; ++i)
    {
        referenceFilePaths.push_back(reader.GetBinaryReader().ReadUtf8String());
    }
    fileTable.Read(reader.GetBinaryReader(), IsSystemModule());
    if (!fileTable.IsEmpty())
    {
#ifdef _WIN32
        if (GetBackEnd() == BackEnd::cmsx)
        {
            libraryFilePath = GetFullPath(boost::filesystem::path(filePathReadFrom).replace_extension(".a").generic_string());
        }
        else if (GetBackEnd() == BackEnd::llvm)
        {
            libraryFilePath = GetFullPath(boost::filesystem::path(filePathReadFrom).replace_extension(".lib").generic_string());
        }
#else
        libraryFilePath = GetFullPath(boost::filesystem::path(filePathReadFrom).replace_extension(".a").generic_string());
#endif
    }
    exportedFunctions.clear();
    uint32_t efn = reader.GetBinaryReader().ReadULEB128UInt();
    for (uint32_t i = 0; i < efn; ++i)
    {
        exportedFunctions.push_back(reader.GetBinaryReader().ReadUtf8String());
    }
    for (const std::string& exportedFunction : exportedFunctions)
    {
        rootModule->allExportedFunctions.push_back(exportedFunction);
    }
    exportedData.clear();
    uint32_t edn = reader.GetBinaryReader().ReadULEB128UInt();
    for (uint32_t i = 0; i < edn; ++i)
    {
        exportedData.push_back(reader.GetBinaryReader().ReadUtf8String());
    }
    for (const std::string& data : exportedData)
    {
        rootModule->allExportedData.push_back(data);
    }
    CheckUpToDate();
    symbolTablePos = reader.GetBinaryReader().Pos();
    ImportModules(target, rootModule, this, importSet, modules, dependencyMap, readMap); 
#ifdef MODULE_READING_DEBUG
    rootModule->DecDebugLogIndent();
    LogMessage(rootModule->LogStreamId(), "ReadHeader: read end " + ToUtf8(name), rootModule->DebugLogIndent());
#endif 
}

void Module::SetDirectoryPath(const std::string& directoryPath_)
{
    directoryPath = directoryPath_;
}

void Module::AddExportedFunction(const std::string& exportedFunction)
{
    exportedFunctions.push_back(exportedFunction);
}

void Module::AddExportedData(const std::string& data)
{
    std::lock_guard<std::recursive_mutex> lck(lock);
    exportedData.push_back(data);
}

void Module::Dump()
{
    CodeFormatter formatter(std::cout);
    formatter.WriteLine("========================");
    formatter.WriteLine("MODULE " + ToUtf8(name));
    formatter.WriteLine("========================");
    formatter.WriteLine();
    formatter.WriteLine("format: " + std::string(1, format));
    formatter.WriteLine("flags: " + ModuleFlagStr(flags));
    formatter.WriteLine("original file path: " + originalFilePath);
    formatter.WriteLine("file path read from: " + filePathReadFrom);
    if (!libraryFilePath.empty())
    {
        formatter.WriteLine("library file path: " + libraryFilePath);
    }
    int n = referenceFilePaths.size();
    if (n > 0)
    {
        formatter.WriteLine("reference file paths:");
        formatter.IncIndent();
        for (int i = 0; i < n; ++i)
        {
            formatter.WriteLine(referenceFilePaths[i]);
        }
        formatter.DecIndent();
    }
    fileTable.Dump(formatter);
    formatter.WriteLine("module dependencies:");
    formatter.IncIndent();
    formatter.WriteLine(ToUtf8(Name()));
    moduleDependency.Dump(formatter);
    formatter.DecIndent();
    SymbolCollector collector;
    symbolTable->GlobalNs().Accept(&collector);
    collector.SortByFullName();
    if (!collector.BasicTypes().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("BASIC TYPES");
        for (BasicTypeSymbol* basicType : collector.BasicTypes())
        {
            formatter.WriteLine();
            basicType->Dump(formatter);
        }
    }
    if (!collector.Classes().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("CLASSES");
        for (ClassTypeSymbol* class_ : collector.Classes())
        {
            formatter.WriteLine();
            class_->Dump(formatter);
        }
    }
    if (!collector.Interfaces().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("INTERFACES");
        for (InterfaceTypeSymbol* interface : collector.Interfaces())
        {
            formatter.WriteLine();
            interface->Dump(formatter);
        }
    }
    if (!collector.Functions().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("FUNCTIONS");
        for (FunctionSymbol* function : collector.Functions())
        {
            formatter.WriteLine();
            function->Dump(formatter);
        }
    }
    if (!collector.Typedefs().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("TYPEDEFS");
        for (TypedefSymbol* typedef_ : collector.Typedefs())
        {
            formatter.WriteLine();
            typedef_->Dump(formatter);
        }
    }
    if (!collector.Concepts().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("CONCEPTS");
        for (ConceptSymbol* concept_ : collector.Concepts())
        {
            formatter.WriteLine();
            concept_->Dump(formatter);
        }
    }
    if (!collector.Delegates().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("DELEGATES");
        for (DelegateTypeSymbol* delegate_ : collector.Delegates())
        {
            formatter.WriteLine();
            delegate_->Dump(formatter);
        }
    }
    if (!collector.ClassDelegates().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("CLASS DELEGATES");
        for (ClassDelegateTypeSymbol* classDelegate : collector.ClassDelegates())
        {
            formatter.WriteLine();
            classDelegate->Dump(formatter);
        }
    }
    if (!collector.Constants().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("CONSTANTS");
        for (ConstantSymbol* constant : collector.Constants())
        {
            formatter.WriteLine();
            constant->Dump(formatter);
        }
    }
    if (!collector.EnumeratedTypes().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("ENUMERATED TYPES");
        for (EnumTypeSymbol* enumeratedType : collector.EnumeratedTypes())
        {
            formatter.WriteLine();
            enumeratedType->Dump(formatter);
        }
    }
}

void Module::CheckUpToDate()
{
    if (fileTable.IsEmpty()) return;
    boost::filesystem::path libDirPath = boost::filesystem::path(originalFilePath).parent_path();
    int n = fileTable.NumFilePaths();
    for (int i = 0; i < n; ++i)
    {
        boost::filesystem::path sfp(fileTable.GetFilePath(i));
        if (boost::filesystem::exists(sfp))
        {
            boost::filesystem::path objectFilePath;
#ifdef _WIN32
            if (GetBackEnd() == BackEnd::llvm)
            {
                objectFilePath = libDirPath / sfp.filename().replace_extension(".obj");
            }
            else if (GetBackEnd() == BackEnd::cmsx)
            {
                objectFilePath = libDirPath / sfp.filename().replace_extension(".o");
            }
#else
            objectFilePath = libDirPath / sfp.filename().replace_extension(".o");
#endif
            if (boost::filesystem::exists(objectFilePath))
            {
                if (boost::filesystem::last_write_time(sfp) > boost::filesystem::last_write_time(objectFilePath))
                {
                    Warning warning(name, "source file '" + GetFullPath(sfp.generic_string()) + "' is more recent than object file '" +
                        GetFullPath(objectFilePath.generic_string()) + "'");
                    warning.SetModule(GetRootModuleForCurrentThread());
                    bool found = false;
                    for (const Warning& prev : warnings.Warnings())
                    {
                        if (prev.Message() == warning.Message())
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        warnings.AddWarning(warning);
                    }
                }
            }
        }
    }
}

int16_t Module::GetModuleId(Module* module) 
{
    auto it = moduleIdMap.find(module);
    if (it != moduleIdMap.cend())
    {
        return it->second;
    }
    return -1;
}

void Module::SetCurrentProjectName(const std::u32string& currentProjectName_)
{
    if (IsRootModule())
    {
        currentProjectName = currentProjectName_;
    }
    else
    {
        throw std::runtime_error("cannot set current project name for a nonroot module");
    }
}

std::u32string Module::GetCurrentProjectName()
{
    if (IsRootModule())
    {
        return currentProjectName;
    }
    else
    {
        throw std::runtime_error("cannot get current project name for a nonroot module");
    }
}

void Module::SetCurrentToolName(const std::u32string& currentToolName_)
{
    if (IsRootModule())
    {
        currentToolName = currentToolName_;
    }
    else
    {
        throw std::runtime_error("cannot set current tool name for a nonroot module");
    }
}

std::u32string Module::GetCurrentToolName()
{
    if (IsRootModule())
    {
        return currentToolName;
    }
    else
    {
        throw std::runtime_error("cannot get current tool name for a nonroot module");
    }
}

CompileWarningCollection& Module::WarningCollection()
{
    if (IsRootModule())
    {
        return warnings;
    }
    else
    {
        throw std::runtime_error("cannot get warnings for a nonroot module");
    }
}

void Module::ClearDefines()
{
    if (IsRootModule())
    {
        defines = GetCommandLineDefines();
    }
    else
    {
        throw std::runtime_error("cannot clear defines for a nonroot module");
    }
}

void Module::DefineSymbol(const std::u32string& symbol)
{
    if (IsRootModule())
    {
        defines.insert(symbol);
    }
    else
    {
        throw std::runtime_error("cannot define a symbol for a nonroot module");
    }
}

bool Module::IsSymbolDefined(const std::u32string& symbol)
{
    if (IsRootModule())
    {
        auto it = defines.find(symbol);
        if (it != defines.cend())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        throw std::runtime_error("cannot get defined symbol for a nonroot module");
    }
}

Module* Module::GetSystemCoreModule()
{
    if (IsCore())
    {
        return this;
    }
    if (!systemCoreModule)
    {
        for (Module* referencedModule : referencedModules)
        {
            Module* systemCore = referencedModule->GetSystemCoreModule();
            if (systemCore)
            {
                systemCoreModule = systemCore;
                break;
            }
        }
    }
    return systemCoreModule;
}

void Module::Check()
{
    symbolTable->Check();
}

void Module::StartBuild()
{
    buildStartMs = CurrentMs();
}

void Module::StopBuild()
{
    buildStopMs = CurrentMs();
}

int Module::GetBuildTimeMs()
{
    return static_cast<int>(buildStopMs - buildStartMs);
}

#ifdef _WIN32
    __declspec(thread) Module* rootModule = nullptr;
#else
    __thread Module* rootModule = nullptr;
#endif

Module* GetRootModuleForCurrentThread()
{
    if (!rootModule)
    {
        throw std::runtime_error("root module for the current thread not set");
    }
    return rootModule;
}

void SetRootModuleForCurrentThread(Module* rootModule_)
{
    rootModule = rootModule_;
}

class SYMBOLS_API SystemModuleVersionTagVerifier : public sngcm::ast::ModuleVersionTagVerifier
{
public:
    void VerifyModuleVersionTag(const std::string& moduleFilePath) override;
};

void SystemModuleVersionTagVerifier::VerifyModuleVersionTag(const std::string& moduleFilePath)
{
    SymbolReader reader(moduleFilePath);
    ModuleTag expectedTag;
    ModuleTag tag;
    tag.Read(reader);
    for (int i = 0; i < 3; ++i)
    {
        if (tag.bytes[i] != expectedTag.bytes[i])
        {
            throw std::runtime_error("Invalid Cmajor module tag read from file '" + reader.GetBinaryReader().FileName() + "', please rebuild module from sources");
        }
    }
    if (tag.bytes[3] != expectedTag.bytes[3])
    {
        throw std::runtime_error("Cmajor module format version mismatch reading from file '" + reader.GetBinaryReader().FileName() +
            "': format " + std::string(1, expectedTag.bytes[3]) + " expected, format " + std::string(1, tag.bytes[3]) + " read, please rebuild module from sources");
    }
}

SystemModuleVersionTagVerifier verifier;

void InitModule()
{
    sngcm::ast::SetModuleVersionTagVerifier(&verifier);
    SystemModuleSet::Init();
}

void DoneModule()
{
    SystemModuleSet::Done();
}

} } // namespace cmajor::symbols
