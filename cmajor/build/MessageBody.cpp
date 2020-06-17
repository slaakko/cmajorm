#include "D:/work/cmajorm/cmajor/build/MessageBody.hpp"
#include <sngjson/json/JsonImport.hpp>
#include <sngjson/json/JsonExport.hpp>
#include <utility>

SourceFileInfo::SourceFileInfo() : fileId(), fileHash(), filePath(), content(), info()
{
}

std::unique_ptr<soulng::util::JsonValue> SourceFileInfo::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(fileId);
    object->AddField(U"fileId", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(fileHash);
    object->AddField(U"fileHash", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(filePath);
    object->AddField(U"filePath", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(content);
    object->AddField(U"content", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(info);
    object->AddField(U"info", std::move(fieldValue4));
    return object;
}

SourceFileInfo::SourceFileInfo(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "fileId", fileId);
    sngjson::json::FromJson(value, "fileHash", fileHash);
    sngjson::json::FromJson(value, "filePath", filePath);
    sngjson::json::FromJson(value, "content", content);
    sngjson::json::FromJson(value, "info", info);
}

ProjectInfo::ProjectInfo() : projectId(), projectHash(), projectFilePath(), projectName(), fileInfos(), dependsOnProjects(), target(), info(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> ProjectInfo::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(projectId);
    object->AddField(U"projectId", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(projectHash);
    object->AddField(U"projectHash", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(projectFilePath);
    object->AddField(U"projectFilePath", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(projectName);
    object->AddField(U"projectName", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(fileInfos);
    object->AddField(U"fileInfos", std::move(fieldValue4));
    std::unique_ptr<soulng::util::JsonValue> fieldValue5 = sngjson::json::ToJson(dependsOnProjects);
    object->AddField(U"dependsOnProjects", std::move(fieldValue5));
    std::unique_ptr<soulng::util::JsonValue> fieldValue6 = sngjson::json::ToJson(target);
    object->AddField(U"target", std::move(fieldValue6));
    std::unique_ptr<soulng::util::JsonValue> fieldValue7 = sngjson::json::ToJson(info);
    object->AddField(U"info", std::move(fieldValue7));
    std::unique_ptr<soulng::util::JsonValue> fieldValue8 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue8));
    return object;
}

ProjectInfo::ProjectInfo(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "projectId", projectId);
    sngjson::json::FromJson(value, "projectHash", projectHash);
    sngjson::json::FromJson(value, "projectFilePath", projectFilePath);
    sngjson::json::FromJson(value, "projectName", projectName);
    sngjson::json::FromJson(value, "fileInfos", fileInfos);
    sngjson::json::FromJson(value, "dependsOnProjects", dependsOnProjects);
    sngjson::json::FromJson(value, "target", target);
    sngjson::json::FromJson(value, "info", info);
    sngjson::json::FromJson(value, "error", error);
}

ProjectUpToDate::ProjectUpToDate() : projectId(), upToDate(), info(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> ProjectUpToDate::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(projectId);
    object->AddField(U"projectId", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(upToDate);
    object->AddField(U"upToDate", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(info);
    object->AddField(U"info", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue3));
    return object;
}

ProjectUpToDate::ProjectUpToDate(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "projectId", projectId);
    sngjson::json::FromJson(value, "upToDate", upToDate);
    sngjson::json::FromJson(value, "info", info);
    sngjson::json::FromJson(value, "error", error);
}

ProjectUpdated::ProjectUpdated() : projectId(), ok(), info(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> ProjectUpdated::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(projectId);
    object->AddField(U"projectId", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(ok);
    object->AddField(U"ok", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(info);
    object->AddField(U"info", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue3));
    return object;
}

ProjectUpdated::ProjectUpdated(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "projectId", projectId);
    sngjson::json::FromJson(value, "ok", ok);
    sngjson::json::FromJson(value, "info", info);
    sngjson::json::FromJson(value, "error", error);
}

ProjectToBuild::ProjectToBuild() : projectId(), projectHash(), only(), rebuild(), sendBuildOutput(), config(), toolChain()
{
}

std::unique_ptr<soulng::util::JsonValue> ProjectToBuild::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(projectId);
    object->AddField(U"projectId", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(projectHash);
    object->AddField(U"projectHash", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(only);
    object->AddField(U"only", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(rebuild);
    object->AddField(U"rebuild", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(sendBuildOutput);
    object->AddField(U"sendBuildOutput", std::move(fieldValue4));
    std::unique_ptr<soulng::util::JsonValue> fieldValue5 = sngjson::json::ToJson(config);
    object->AddField(U"config", std::move(fieldValue5));
    std::unique_ptr<soulng::util::JsonValue> fieldValue6 = sngjson::json::ToJson(toolChain);
    object->AddField(U"toolChain", std::move(fieldValue6));
    return object;
}

ProjectToBuild::ProjectToBuild(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "projectId", projectId);
    sngjson::json::FromJson(value, "projectHash", projectHash);
    sngjson::json::FromJson(value, "only", only);
    sngjson::json::FromJson(value, "rebuild", rebuild);
    sngjson::json::FromJson(value, "sendBuildOutput", sendBuildOutput);
    sngjson::json::FromJson(value, "config", config);
    sngjson::json::FromJson(value, "toolChain", toolChain);
}

ProjectBuilt::ProjectBuilt() : projectId(), ok(), info(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> ProjectBuilt::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(projectId);
    object->AddField(U"projectId", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(ok);
    object->AddField(U"ok", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(info);
    object->AddField(U"info", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue3));
    return object;
}

ProjectBuilt::ProjectBuilt(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "projectId", projectId);
    sngjson::json::FromJson(value, "ok", ok);
    sngjson::json::FromJson(value, "info", info);
    sngjson::json::FromJson(value, "error", error);
}

MessageLine::MessageLine() : line()
{
}

std::unique_ptr<soulng::util::JsonValue> MessageLine::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(line);
    object->AddField(U"line", std::move(fieldValue0));
    return object;
}

MessageLine::MessageLine(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "line", line);
}

MessageShown::MessageShown() : error()
{
}

std::unique_ptr<soulng::util::JsonValue> MessageShown::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue0));
    return object;
}

MessageShown::MessageShown(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "error", error);
}

Error::Error() : info(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> Error::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(info);
    object->AddField(U"info", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue1));
    return object;
}

Error::Error(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "info", info);
    sngjson::json::FromJson(value, "error", error);
}

CloseConnection::CloseConnection() : info(), error()
{
}

std::unique_ptr<soulng::util::JsonValue> CloseConnection::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(info);
    object->AddField(U"info", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(error);
    object->AddField(U"error", std::move(fieldValue1));
    return object;
}

CloseConnection::CloseConnection(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "info", info);
    sngjson::json::FromJson(value, "error", error);
}

