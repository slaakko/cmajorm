// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/Config.hpp>
#include <soulng/util/Path.hpp>
#include <boost/filesystem.hpp>
#include <stdexcept>

namespace cmajor { namespace service {

using namespace soulng::util;

std::string CmajorRootDir()
{
    char* e = getenv("CMAJOR_ROOT");
    if (e == nullptr || !*e)
    {
        throw std::runtime_error("please set 'CMAJOR_ROOT' environment variable to contain /path/to/cmajor directory.");
    }
    return GetFullPath(std::string(e));
}

std::string CmajorConfigDir()
{
    std::string configDir = GetFullPath(Path::Combine(CmajorRootDir(), "config"));
    boost::filesystem::create_directories(configDir);
    return configDir;
}

std::string PortMapConfigFilePath()
{
    return GetFullPath(Path::Combine(CmajorConfigDir(), "cmpm.config.xml"));
}

std::string CmajorLogDir()
{
    std::string logDir = Path::Combine(CmajorRootDir(), "log");
    boost::filesystem::create_directories(logDir);
    return logDir;
}

std::string CmajorProjectsDir()
{
    std::string projectsDir = Path::Combine(CmajorRootDir(), "projects");
    boost::filesystem::create_directories(projectsDir);
    return projectsDir;
}

std::string PortMapLogFilePath()
{
    return GetFullPath(Path::Combine(CmajorLogDir(), "cmpms.log"));
}

} } // namespace cmajor::service
