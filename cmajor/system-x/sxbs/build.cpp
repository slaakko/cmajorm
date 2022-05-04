// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/sxbs/build.hpp>
#include <system-x/sxbs/log.hpp>
#include <cmajor/build/Build.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/Unicode.hpp>
#include <thread>
#include <vector>

namespace sxbs {

void BuildDebug(const std::string& project, std::exception_ptr& ex)
{
    try
    {
        cmajor::symbols::ResetGlobalFlags();
        cmajor::symbols::SetBackEnd(cmajor::symbols::BackEnd::cmsx);
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::verbose);
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::rebuild);
        std::string ext = soulng::util::Path::GetExtension(project);
        if (ext == ".cmp")
        {
            std::set<std::string> builtModules;
            std::unique_ptr<cmajor::symbols::Module> rootModule;
            cmajor::build::BuildProject(project, rootModule, builtModules);
        }
        else if (ext == ".cms")
        {
            std::vector<std::unique_ptr<cmajor::symbols::Module>> rootModules;
            cmajor::build::BuildSolution(project, rootModules);
        }
        else
        {
            throw std::runtime_error("error building project '" + project + "': unknown extension");
        }
        ex = std::exception_ptr();
    }
    catch (...)
    {
        ex = std::current_exception();
    }
}

void BuildRelease(const std::string& project, std::exception_ptr& ex)
{
    try
    {
        cmajor::symbols::ResetGlobalFlags();
        cmajor::symbols::SetBackEnd(cmajor::symbols::BackEnd::cmsx);
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::verbose);
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::rebuild);
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::release);
        std::string ext = soulng::util::Path::GetExtension(project);
        if (ext == ".cmp")
        {
            std::set<std::string> builtModules;
            std::unique_ptr<cmajor::symbols::Module> rootModule;
            cmajor::build::BuildProject(project, rootModule, builtModules);
        }
        else if (ext == ".cms")
        {
            std::vector<std::unique_ptr<cmajor::symbols::Module>> rootModules;
            cmajor::build::BuildSolution(project, rootModules);
        }
        else
        {
            throw std::runtime_error("error building project '" + project + "': unknown extension");
        }
        ex = std::exception_ptr();
    }
    catch (...)
    {
        ex = std::current_exception();
    }
}

void Build(ServerThread* serverThread, const std::string& project, Config config)
{
    std::string fullProject = soulng::util::Path::Combine(soulng::util::Path::Combine(soulng::util::Path::Combine(soulng::unicode::CmajorRoot(), "system-x"), "projects"), project);
    std::vector<std::thread> threads;
    std::vector<std::exception_ptr> exceptions;
    std::exception_ptr logEx;
    std::exception_ptr buildDebugEx;
    std::exception_ptr buildReleaseEx;
    std::pair<ServerThread*, std::exception_ptr> logArg;
    threads.push_back(std::thread(SendLog, serverThread, std::ref(logEx)));
    if ((config & Config::debug) != Config::none)
    {
        if (!logEx)
        {
            BuildDebug(fullProject, buildDebugEx);
        }
    }
    if ((config & Config::release) != Config::none)
    {
        if (!buildDebugEx)
        {
            BuildRelease(fullProject, buildReleaseEx);
        }
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
    exceptions.push_back(std::move(logEx));
    exceptions.push_back(std::move(buildDebugEx));
    exceptions.push_back(std::move(buildReleaseEx));
    for (auto& ex : exceptions)
    {
        if (ex)
        {
            std::rethrow_exception(ex);
        }
    }
}

} // sxbs
