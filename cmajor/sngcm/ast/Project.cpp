// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Project.hpp>
#include <cmajor/cmtoolchain/ToolChains.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Sha1.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

namespace sngcm { namespace ast {

using namespace soulng::util;
using namespace soulng::unicode;

ModuleVersionTagVerifier* moduleVersionTagVerifier = nullptr;

void SetModuleVersionTagVerifier(ModuleVersionTagVerifier* verifier)
{
    moduleVersionTagVerifier = verifier;
}

std::string TargetStr(Target target)
{
    switch (target)
    {
        case Target::program: return "program";
        case Target::library: return "library";
        case Target::winapp : return "winapp";
        case Target::winlib: return "winlib";
        case Target::winguiapp: return "winguiapp";
        case Target::unitTest: return "unitTest";
    }
    return "library";
}

Target ParseTarget(const std::string& targetStr)
{
    if (targetStr == "program")
    {
        return Target::program;
    }
    else if (targetStr == "library")
    {
        return Target::library;
    }
    else if (targetStr == "winapp")
    {
        return Target::winapp;
    }
    else if (targetStr == "winlib")
    {
        return Target::winlib;
    }
    else if (targetStr == "winguiapp")
    {
        return Target::winguiapp;
    }
    else if (targetStr == "unitTest")
    {
        return Target::unitTest;
    }
    return Target::program;
}

std::string CmajorRootDir()
{
    char* e = getenv("CMAJOR_ROOT");
    if (e == nullptr || !*e)
    {
        throw std::runtime_error("please set 'CMAJOR_ROOT' environment variable to contain /path/to/cmajor directory.");
    }
    return std::string(e);
}

std::string CmajorSystemLibDir(const std::string& config, BackEnd backend, const std::string& toolChain, SystemDirKind systemDirKind)
{
    if (backend == BackEnd::llvm)
    {
        boost::filesystem::path sld(CmajorRootDir());
        if (systemDirKind == SystemDirKind::repository)
        {
            sld /= "repository";
        }
        sld /= "system";
        sld /= "lib";
        sld /= config;
        return GetFullPath(sld.generic_string());
    }
    else if (backend == BackEnd::cmsx)
    {
        boost::filesystem::path sld(CmajorRootDir());
        sld /= "projects";
        sld /= "cmsx";
        sld /= "system";
        sld /= "lib";
        sld /= config;
        return GetFullPath(sld.generic_string());
    }
    else if (backend == BackEnd::cppcm)
    {
        boost::filesystem::path sld(CmajorRootDir());
        if (systemDirKind == SystemDirKind::repository)
        {
            sld /= "repository";
        }
        sld /= "system";
        sld /= "lib";
        sld /= "cpp";
        sld /= toolChain;
        sld /= config;
        return GetFullPath(sld.generic_string());
    }
    else
    {
        return std::string();
    }
}

std::string CmajorResourceDir()
{
    boost::filesystem::path rd(CmajorRootDir());
    rd /= "res";
    return GetFullPath(rd.generic_string());
}

std::string CmajorLogFileDir()
{
    std::string cmajorRooDir = CmajorRootDir();
    std::string logFileDir = Path::Combine(cmajorRooDir, "log");
    boost::filesystem::create_directories(logFileDir);
    return logFileDir;
}

std::string outDir;

void SetOutDir(const std::string& outDir_)
{
    outDir = outDir_;
}

const std::string& OutDir()
{
    return outDir;
}

std::string CmajorSystemModuleFilePath(const std::string& config, BackEnd backend, const std::string& toolChain, SystemDirKind systemDirKind)
{
    boost::filesystem::path smfp(CmajorSystemLibDir(config, backend, toolChain, systemDirKind));
    smfp /= "System.cmm";
    return GetFullPath(smfp.generic_string());
}

std::string CmajorSystemWindowsModuleFilePath(const std::string& config, const std::string& toolChain, SystemDirKind systemDirKind)
{
    boost::filesystem::path smfp(CmajorSystemLibDir(config, BackEnd::llvm, toolChain, systemDirKind));
    smfp /= "System.Windows.cmm";
    return GetFullPath(smfp.generic_string());
}

ProjectDeclaration::ProjectDeclaration(ProjectDeclarationType declarationType_) : declarationType(declarationType_)
{
}

ProjectDeclaration::~ProjectDeclaration()
{
}

ReferenceDeclaration::ReferenceDeclaration(const std::string& filePath_) : ProjectDeclaration(ProjectDeclarationType::referenceDeclaration), filePath(filePath_)
{
}

void ReferenceDeclaration::Write(CodeFormatter& formatter)
{
    formatter.WriteLine("reference <" + filePath + ">;");
}

SourceFileDeclaration::SourceFileDeclaration(const std::string& filePath_) : ProjectDeclaration(ProjectDeclarationType::sourceFileDeclaration), filePath(filePath_)
{
}

void SourceFileDeclaration::Write(CodeFormatter& formatter)
{
    formatter.WriteLine("source <" + filePath + ">;");
}

ResourceFileDeclaration::ResourceFileDeclaration(const std::string& filePath_) : ProjectDeclaration(ProjectDeclarationType::resourceFileDeclaration), filePath(filePath_)
{
}

void ResourceFileDeclaration::Write(CodeFormatter& formatter)
{
    formatter.WriteLine("resource <" + filePath + ">;");
}

TextFileDeclaration::TextFileDeclaration(const std::string& filePath_) : ProjectDeclaration(ProjectDeclarationType::textFileDeclaration), filePath(filePath_)
{
}

void TextFileDeclaration::Write(CodeFormatter& formatter)
{
    formatter.WriteLine("text <" + filePath + ">;");
}

TargetDeclaration::TargetDeclaration(Target target_) : ProjectDeclaration(ProjectDeclarationType::targetDeclaration), target(target_)
{
}

void TargetDeclaration::Write(CodeFormatter& formatter)
{
    formatter.WriteLine("target=" + TargetStr(target) + ";");
}

Project::Project(const std::u32string& name_, const std::string& filePath_, const std::string& config_, BackEnd backend_, const std::string& toolChain_,
    SystemDirKind systemDirKind) :
    backend(backend_), name(name_), filePath(filePath_), config(config_), target(Target::program), sourceBasePath(filePath), outdirBasePath(filePath),
    isSystemProject(false), logStreamId(0), built(false), toolChain(toolChain_)
{
    std::string platform = GetPlatform();
    if (!outDir.empty())
    {
        sourceBasePath.remove_filename();
        outdirBasePath = outDir;
        outdirBasePath /= ToUtf8(name);
    }
    else
    {
        sourceBasePath.remove_filename();
        outdirBasePath = sourceBasePath;
    }
    systemLibDir = CmajorSystemLibDir(config, backend, toolChain, systemDirKind);
    boost::filesystem::path mfp(filePath);
    boost::filesystem::path fn = mfp.filename();
    mfp.remove_filename();
    if (!outDir.empty())
    {
        mfp = outdirBasePath;
    }
    mfp /= "lib";
    if (backend == BackEnd::cppcm)
    {
        mfp /= "cpp";
        mfp /= toolChain;
    }
    mfp /= config;
    mfp /= fn;
    mfp.replace_extension(".cmm");
    moduleFilePath = GetFullPath(mfp.generic_string());
    boost::filesystem::path lfp(mfp);
#ifdef _WIN32
    if (backend == BackEnd::cmsx)
    {
        lfp.replace_extension(".a");
    }
    else if (backend == BackEnd::llvm)
    {
        lfp.replace_extension(".lib");
    }
    else if (backend == BackEnd::cppcm)
    {
        const Tool& libraryManagerTool = GetLibraryManagerTool(platform, toolChain);
        lfp.replace_extension(libraryManagerTool.outputFileExtension);
    }
#else
    if (backend == BackEnd::cppcm)
    {
        const Tool& libraryManagerTool = GetLibraryManagerTool(platform, toolChain);
        lfp.replace_extension(libraryManagerTool.outputFileExtension);
    }
    else
    {
        lfp.replace_extension(".a");
    }
#endif
    libraryFilePath = GetFullPath(lfp.generic_string());
    boost::filesystem::path efp(filePath);
    efp.remove_filename();
    if (!outDir.empty())
    {
        efp = outdirBasePath;
    }
    efp /= "bin";
    if (backend == BackEnd::cppcm)
    {
        efp /= "cpp";
        efp /= toolChain;
    }
    efp /= config;
    efp /= fn;
#ifdef _WIN32
    if (backend == BackEnd::cmsx)
    {
        efp.replace_extension();
    }
    else if (backend == BackEnd::llvm)
    {
        efp.replace_extension(".exe");
    }
    else if (backend == BackEnd::cppcm)
    {
        const Tool& linkerTool = GetLinkerTool(platform, toolChain);
        efp.replace_extension(linkerTool.outputFileExtension);
    }
#else
    if (backend == BackEnd::cppcm)
    {
        const Tool& linkerTool = GetLinkerTool(platform, toolChain);
        efp.replace_extension(linkerTool.outputFileExtension);
    }
    else
    {
        efp.replace_extension();
    }
#endif
    executableFilePath = GetFullPath(efp.generic_string());
}

void Project::AddDeclaration(ProjectDeclaration* declaration)
{
    declarations.push_back(std::unique_ptr<ProjectDeclaration>(declaration));
}

void Project::ResolveDeclarations()
{
    for (const std::unique_ptr<ProjectDeclaration>& declaration : declarations)
    {
        switch (declaration->GetDeclarationType())
        {
            case ProjectDeclarationType::referenceDeclaration:
            {
                ReferenceDeclaration* reference = static_cast<ReferenceDeclaration*>(declaration.get());
                boost::filesystem::path rp(reference->FilePath());
                relativeReferencedProjectFilePaths.push_back(rp.generic_string());
                if (rp.is_absolute())
                {
                    referencedProjectFilePaths.push_back(GetFullPath(rp.generic_string()));
                }
                else
                {
                    boost::filesystem::path ar = sourceBasePath / rp;
                    referencedProjectFilePaths.push_back(GetFullPath(ar.generic_string()));
                }
                boost::filesystem::path fn = rp.filename();
                rp.remove_filename();
                if (rp.is_relative())
                {
                    rp = systemLibDir / rp;
                }
                rp /= fn;
                if (rp.extension() == ".cmp" || rp.extension() == ".cmproj")
                {
                    rp.replace_extension(".cmm");
                }
                if (rp.extension() != ".cmm")
                {
                    throw std::runtime_error("invalid reference path extension '" + rp.generic_string() + "' (not .cmp, .cmproj or .cmm)");
                }
                if (!boost::filesystem::exists(rp))
                {
                    rp = reference->FilePath();
                    rp.remove_filename();
                    if (rp.is_relative())
                    {
                        rp = outdirBasePath / rp;
                    }
                    rp /= "lib";
                    if (backend == BackEnd::cppcm)
                    {
                        rp /= "cpp";
                        rp /= toolChain;
                    }
                    rp /= config;
                    rp /= fn;
                    if (rp.extension() == ".cmp" || rp.extension() == ".cmproj")
                    {
                        rp.replace_extension(".cmm");
                    }
                    if (rp.extension() != ".cmm")
                    {
                        throw std::runtime_error("invalid reference path extension '" + rp.generic_string() + "' (not .cmp, .cmproj or .cmm)");
                    }
                }
                std::string referencePath = GetFullPath(rp.generic_string());
                if (std::find(references.cbegin(), references.cend(), referencePath) == references.cend())
                {
                    references.push_back(referencePath);
                }
                break;
            }
            case ProjectDeclarationType::sourceFileDeclaration:
            {
                SourceFileDeclaration* sourceFileDeclaration = static_cast<SourceFileDeclaration*>(declaration.get());
                boost::filesystem::path sfp(sourceFileDeclaration->FilePath());
                relativeSourceFilePaths.push_back(sfp.generic_string());
                if (sfp.is_relative())
                {
                    sfp = sourceBasePath / sfp;
                }
                if (sfp.extension() != ".cm")
                {
                    throw std::runtime_error("invalid source file extension '" + sfp.generic_string() + "' (not .cm)");
                }
                if (!boost::filesystem::exists(sfp))
                {
                    throw std::runtime_error("source file path '" + GetFullPath(sfp.generic_string()) + "' not found");
                }
                std::string sourceFilePath = GetFullPath(sfp.generic_string());
                if (std::find(sourceFilePaths.cbegin(), sourceFilePaths.cend(), sourceFilePath) == sourceFilePaths.cend() && sourceFilePath != excludeSourceFilePath)
                {
                    sourceFilePaths.push_back(sourceFilePath);
                }
                break;
            }
            case ProjectDeclarationType::resourceFileDeclaration:
            {
                ResourceFileDeclaration* resourceFileDeclaration = static_cast<ResourceFileDeclaration*>(declaration.get());
                boost::filesystem::path rfp(resourceFileDeclaration->FilePath());
                relativeResourceFilePaths.push_back(rfp.generic_string());
                if (rfp.is_relative())
                {
                    rfp = sourceBasePath / rfp;
                }
                if (rfp.extension() != ".xml")
                {
                    throw std::runtime_error("invalid resource file extension '" + rfp.generic_string() + "' (not .xml)");
                }
                if (!boost::filesystem::exists(rfp))
                {
                    throw std::runtime_error("resource file path '" + GetFullPath(rfp.generic_string()) + "' not found");
                }
                std::string resourceFilePath = GetFullPath(rfp.generic_string());
                if (std::find(resourceFilePaths.cbegin(), resourceFilePaths.cend(), resourceFilePath) == resourceFilePaths.cend())
                {
                    resourceFilePaths.push_back(resourceFilePath);
                }
                break;
            }
            case ProjectDeclarationType::targetDeclaration:
            {
                TargetDeclaration* targetDeclaration = static_cast<TargetDeclaration*>(declaration.get());
                target = targetDeclaration->GetTarget();
                break;
            }
            case ProjectDeclarationType::textFileDeclaration:
            {
                TextFileDeclaration* textFileDeclaration = static_cast<TextFileDeclaration*>(declaration.get());
                boost::filesystem::path tfp(textFileDeclaration->FilePath());
                relativeTextFilePaths.push_back(tfp.generic_string());
                if (tfp.is_relative())
                {
                    tfp = sourceBasePath / tfp;
                }
                std::string textFilePath = GetFullPath(tfp.generic_string());
                textFilePaths.push_back(textFilePath);
                break;
            }
            default:
            {
                throw std::runtime_error("unknown project declaration");
            }
        }
    }
}

void Project::Write(const std::string& projectFilePath)
{
    std::ofstream projectFile(projectFilePath);
    CodeFormatter formatter(projectFile);
    formatter.WriteLine("project " + ToUtf8(Name()) + ";");
    for (const std::unique_ptr<ProjectDeclaration>& declaration : declarations)
    {
        declaration->Write(formatter);
    }
}

bool Project::DependsOn(Project* that) const
{
    return std::find(references.cbegin(), references.cend(), that->moduleFilePath) != references.cend();
}

void Project::AddDependsOnProjects(Project* dependsOnProject)
{
    dependsOn.push_back(dependsOnProject);
}

void Project::SetModuleFilePath(const std::string& moduleFilePath_)
{
    moduleFilePath = moduleFilePath_;
}

void Project::SetLibraryFilePath(const std::string& libraryFilePath_)
{
    libraryFilePath = libraryFilePath_;
}

bool Project::IsUpToDate(const std::string& systemModuleFilePath) const
{
    if (!boost::filesystem::exists(moduleFilePath))
    {
        return false;
    }
    for (const std::string& sourceFilePath : sourceFilePaths)
    {
        if (boost::filesystem::last_write_time(sourceFilePath) > boost::filesystem::last_write_time(moduleFilePath))
        {
            return false;
        }
    }
    for (const std::string& resourceFilePath : resourceFilePaths)
    {
        if (boost::filesystem::last_write_time(resourceFilePath) > boost::filesystem::last_write_time(moduleFilePath))
        {
            return false;
        }
    }
    for (const std::string& referenceFilePath : references)
    {
        if (boost::filesystem::last_write_time(referenceFilePath) > boost::filesystem::last_write_time(moduleFilePath))
        {
            return false;
        }
    }
    if (!systemModuleFilePath.empty() && !IsSystemProject() && boost::filesystem::exists(systemModuleFilePath))
    {
        if (boost::filesystem::last_write_time(systemModuleFilePath) > boost::filesystem::last_write_time(moduleFilePath))
        {
            return false;
        }
    }
    return true;
}

bool Project::Ready() const
{
    for (Project* dependOn : dependsOn)
    {
        if (!dependOn->Built())
        {
            return false;
        }
    }
    return true;
}

void Project::SetExcludeSourceFilePath(const std::string& excludeSourceFilePath_)
{
    excludeSourceFilePath = excludeSourceFilePath_;
}

std::string Project::Id() const
{
    std::string id = "project_";
    id.append(ToUtf8(name)).append(1, '_').append(GetSha1MessageDigest(filePath));
    return id;
}

void Project::AddDependsOnId(const std::string& dependsOnId)
{
    dependsOnIds.push_back(dependsOnId);
}

} } // namespace sngcm::ast
