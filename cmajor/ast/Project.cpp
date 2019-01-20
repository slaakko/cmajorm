// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ast/Project.hpp>
#include <cmajor/util/Path.hpp>
#include <boost/filesystem.hpp>

namespace cmajor { namespace ast {

using namespace cmajor::util;

std::string CmajorRootDir()
{
    char* e = getenv("CMAJOR_ROOT");
    if (e == nullptr || !*e)
    {
        throw std::runtime_error("please set 'CMAJOR_ROOT' environment variable to contain /path/to/cmajor directory.");
    }
    return std::string(e);
}

std::string CmajorSystemLibDir(const std::string& config)
{
    boost::filesystem::path sld(CmajorRootDir());
    sld /= "system";
    sld /= "lib";
    sld /= config;
    return GetFullPath(sld.generic_string());
}

std::string CmajorSystemModuleFilePath(const std::string& config)
{
    boost::filesystem::path smfp(CmajorSystemLibDir(config));
    smfp /= "System.cmm";
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

SourceFileDeclaration::SourceFileDeclaration(const std::string& filePath_) : ProjectDeclaration(ProjectDeclarationType::sourceFileDeclaration), filePath(filePath_)
{
}

TextFileDeclaration::TextFileDeclaration(const std::string& filePath_) : ProjectDeclaration(ProjectDeclarationType::textFileDeclaration), filePath(filePath_)
{
}

TargetDeclaration::TargetDeclaration(Target target_) : ProjectDeclaration(ProjectDeclarationType::targetDeclaration), target(target_)
{
}

Project::Project(const std::u32string& name_, const std::string& filePath_, const std::string& config_) :
    name(name_), filePath(filePath_), config(config_), target(Target::program), basePath(filePath), isSystemProject(false), logStreamId(0), built(false)
{
    basePath.remove_filename();
    systemLibDir = CmajorSystemLibDir(config);
    boost::filesystem::path mfp(filePath);
    boost::filesystem::path fn = mfp.filename();
    mfp.remove_filename();
    mfp /= "lib";
    mfp /= config;
    mfp /= fn;
    mfp.replace_extension(".cmm");
    moduleFilePath = GetFullPath(mfp.generic_string());
    boost::filesystem::path lfp(mfp);
#ifdef _WIN32
    lfp.replace_extension(".lib");
#else
    lfp.replace_extension(".a");
#endif
    libraryFilePath = GetFullPath(lfp.generic_string());
    boost::filesystem::path efp(filePath);
    efp.remove_filename();
    efp /= "bin";
    efp /= config;
    efp /= fn;
#ifdef _WIN32
    efp.replace_extension(".exe");
#else
    efp.replace_extension();
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
                    boost::filesystem::path ar = basePath / rp;
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
                        rp = basePath / rp;
                    }
                    rp /= "lib";
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
                    sfp = basePath / sfp;
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
                break;
            }
            default:
            {
                throw std::runtime_error("unknown project declaration");
            }
        }
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

} } // namespace cmajor::ast
