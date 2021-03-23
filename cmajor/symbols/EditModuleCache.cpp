// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/EditModuleCache.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Sha1.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

ReadProjectFunc readProjectFunction;

void SetReadProjectFunction(ReadProjectFunc readProjectFunc)
{
    readProjectFunction = readProjectFunc;
}

std::string MakeEditModuleKey(const std::string& projectFilePath, const std::string& backendStr, const std::string& configurationStr)
{
    std::string editModuleKey = projectFilePath;
    editModuleKey.append(":").append(backendStr).append(":").append(configurationStr);
    return editModuleKey;
}

bool toolChainsRead = false;

std::unique_ptr<EditModuleCache> EditModuleCache::instance;

void EditModuleCache::Init()
{
    instance.reset(new EditModuleCache());
}

void EditModuleCache::Done()
{
    instance.reset();
}

EditModuleCache::EditModuleCache()
{
}

LoadEditModuleResult EditModuleCache::LoadEditModule(const std::string& projectFilePath, const std::string& backendStr, const std::string& configurationStr)
{
    LoadEditModuleResult result;
    result.ok = true;
    result.error = std::string();
    result.startLoading = std::chrono::steady_clock::now();
    result.resultKind = LoadEditModuleResultKind::moduleLoaded;
    cmajor::symbols::ResetGlobalFlags();
    std::string projectFileContent = ReadFile(projectFilePath);
    std::string projectFileHash = GetSha1MessageDigest(projectFileContent);
    int index = editModules.size();
    std::string editModuleKey = MakeEditModuleKey(projectFilePath, backendStr, configurationStr);
    result.key = editModuleKey;
    auto it = editModuleMap.find(editModuleKey);
    if (it != editModuleMap.cend())
    {
        const std::pair<Module*, std::string>& moduleHashPair = it->second;
        const std::string& prevHash = moduleHashPair.second;
        if (prevHash == projectFileHash)
        {
            result.resultKind = LoadEditModuleResultKind::moduleUpToDate;
            result.endLoading = std::chrono::steady_clock::now();
            return result;
        }
        Module* module = moduleHashPair.first;
        index = module->Index();
        result.resultKind = LoadEditModuleResultKind::moduleReloaded;
    }
    if (!toolChainsRead)
    {
        toolChainsRead = true;
        ReadToolChains(GetGlobalFlag(cmajor::symbols::GlobalFlags::verbose));
    }
    ResetToolChain();
    sngcm::ast::BackEnd backend = sngcm::ast::BackEnd::llvm;
    if (backendStr == "cpp")
    {
        backend = sngcm::ast::BackEnd::cppcm;
        SetBackEnd(cmajor::symbols::BackEnd::cmcpp);
    }
    else if (backendStr == "llvm")
    {
        backend = sngcm::ast::BackEnd::llvm;
        SetBackEnd(cmajor::symbols::BackEnd::llvm);
    }
    sngcm::ast::Config config = sngcm::ast::Config::debug;
    if (configurationStr == "release")
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::release);
        config = sngcm::ast::Config::release;
    }
    if (!readProjectFunction)
    {
        result.ok = false;
        result.error = "read project function not set";
        return result;
    }
    std::unique_ptr<Project> project = readProjectFunction(projectFilePath);
    std::unique_ptr<Module> module(new Module(project->Name(), project->ModuleFilePath(), project->GetTarget()));
    module->SetIndex(index);
    module->SetRootModule();
    SetRootModuleForCurrentThread(module.get());
    module->SetLogStreamId(project->LogStreamId());
    module->SetCurrentProjectName(project->Name());
    module->SetCurrentToolName(U"cmccs");
    module->SetFlag(cmajor::symbols::ModuleFlags::compiling);
    PrepareModuleForCompilation(module.get(), project->References(), project->GetTarget());
    module->SetSources(new Sources(project->SourceFilePaths()));
    ParseResult parseResult = module->ParseSources();
    result.ok = parseResult.ok;
    result.numberOfErrors = parseResult.numberOfErrors;
    result.synchronized = parseResult.synchronized;
    result.error = parseResult.error;
    result.startParsing = parseResult.start;
    result.endParsing = parseResult.end;
    while (index >= editModules.size())
    {
        editModules.push_back(std::unique_ptr<Module>());
    }
    editModuleMap[editModuleKey] = std::make_pair(module.get(), projectFileHash);
    editModules[index].reset(module.release());
    result.endLoading = std::chrono::steady_clock::now();
    return result;
}

Module* EditModuleCache::GetEditModule(const std::string& projectFilePath, const std::string& backendStr, const std::string& configurationStr)
{
    std::string editModuleKey = MakeEditModuleKey(projectFilePath, backendStr, configurationStr);
    auto it = editModuleMap.find(editModuleKey);
    if (it != editModuleMap.cend())
    {
        return it->second.first;
    }
    else
    {
        return nullptr;
    }
}

void InitEditModuleCache()
{
    EditModuleCache::Init();
}

void DoneEditModuleCache()
{
    EditModuleCache::Done();
}

} } // namespace cmajor::symbols
