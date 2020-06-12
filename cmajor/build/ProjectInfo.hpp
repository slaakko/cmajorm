#ifndef HPP_23BDA85CC3068B14723007EFBFB1E9CAC65E844F
#define HPP_23BDA85CC3068B14723007EFBFB1E9CAC65E844F
#include <soulng/util/Json.hpp>
#include <cmajor/build/BuildApi.hpp>
#include <string>
#include <vector>
#include <memory>

struct BUILD_API SourceFileInfo
{
    SourceFileInfo();
    SourceFileInfo(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string fileId;
    std::string fileHash;
    std::string filePath;
    std::string content;
    std::string info;
};

struct BUILD_API ProjectInfo
{
    ProjectInfo();
    ProjectInfo(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string projectId;
    std::string projectHash;
    std::string projectFilePath;
    std::string projectName;
    std::vector<SourceFileInfo> fileInfos;
    std::vector<std::string> dependsOnProjects;
    std::string target;
    std::string info;
    std::string error;
};

struct BUILD_API ProjectUpToDate
{
    ProjectUpToDate();
    ProjectUpToDate(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string projectId;
    bool upToDate;
    std::string info;
    std::string error;
};

struct BUILD_API ProjectUpdated
{
    ProjectUpdated();
    ProjectUpdated(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string projectId;
    bool ok;
    std::string info;
    std::string error;
};

struct BUILD_API ProjectToBuild
{
    ProjectToBuild();
    ProjectToBuild(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string projectId;
    bool sendBuildOutput;
    std::string config;
    std::string toolChain;
};

struct BUILD_API ProjectBuilt
{
    ProjectBuilt();
    ProjectBuilt(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string projectId;
    bool ok;
    std::string info;
    std::string error;
};

struct BUILD_API MessageLine
{
    MessageLine();
    MessageLine(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string line;
};

struct BUILD_API MessageShown
{
    MessageShown();
    MessageShown(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string error;
};

struct BUILD_API Error
{
    Error();
    Error(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string info;
    std::string error;
};

struct BUILD_API CloseConnection
{
    CloseConnection();
    CloseConnection(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string info;
    std::string error;
};

#endif // HPP_23BDA85CC3068B14723007EFBFB1E9CAC65E844F
