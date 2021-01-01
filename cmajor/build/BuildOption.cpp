// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/BuildOption.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/cmtoolchain/ToolChains.hpp>

namespace cmajor { namespace build {

BuildOptions buildOptions = BuildOptions::none;
std::string buildConfig = "debug";
std::string buildToolChain = "";

std::string BoolStr(bool value)
{
    if (value)
    {
        return "true";
    }
    else
    {
        return "false";
    }
}

bool GetBuildOption(BuildOptions option)
{
    return(buildOptions & option) != BuildOptions::none;
}

void BuildOptionSetter::SetOption(BuildOptions option)
{
    buildOptions = buildOptions | option;
    if (option == BuildOptions::verbose)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::verbose);
    }
    else if (option == BuildOptions::debug)
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::printDebugMessages);
    }
}

void BuildOptionSetter::SetConfigOption(const std::string& config)
{
    buildConfig = config;
    if (config == "release")
    {
        cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::release);
    }
}

void BuildOptionSetter::SetToolChainOption(const std::string& toolChain)
{
    buildToolChain = toolChain;
    SetToolChain(toolChain);
}

} } // namespace cmajor::build;

