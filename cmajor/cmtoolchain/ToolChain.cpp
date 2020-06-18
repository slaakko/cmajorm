#include "ToolChain.hpp"
#include <sngjson/json/JsonImport.hpp>
#include <sngjson/json/JsonExport.hpp>
#include <utility>

Configuration::Configuration() : name(), outputDirectory(), args()
{
}

std::unique_ptr<soulng::util::JsonValue> Configuration::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(name);
    object->AddField(U"name", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(outputDirectory);
    object->AddField(U"outputDirectory", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(args);
    object->AddField(U"args", std::move(fieldValue2));
    return object;
}

Configuration::Configuration(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "name", name);
    sngjson::json::FromJson(value, "outputDirectory", outputDirectory);
    sngjson::json::FromJson(value, "args", args);
}

Tool::Tool() : name(), commandName(), outputFileExtension(), debugInformationFileExtension(), assemblyFileExtension(), configurations()
{
}

std::unique_ptr<soulng::util::JsonValue> Tool::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(name);
    object->AddField(U"name", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(commandName);
    object->AddField(U"commandName", std::move(fieldValue1));
    std::unique_ptr<soulng::util::JsonValue> fieldValue2 = sngjson::json::ToJson(outputFileExtension);
    object->AddField(U"outputFileExtension", std::move(fieldValue2));
    std::unique_ptr<soulng::util::JsonValue> fieldValue3 = sngjson::json::ToJson(debugInformationFileExtension);
    object->AddField(U"debugInformationFileExtension", std::move(fieldValue3));
    std::unique_ptr<soulng::util::JsonValue> fieldValue4 = sngjson::json::ToJson(assemblyFileExtension);
    object->AddField(U"assemblyFileExtension", std::move(fieldValue4));
    std::unique_ptr<soulng::util::JsonValue> fieldValue5 = sngjson::json::ToJson(configurations);
    object->AddField(U"configurations", std::move(fieldValue5));
    return object;
}

Tool::Tool(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "name", name);
    sngjson::json::FromJson(value, "commandName", commandName);
    sngjson::json::FromJson(value, "outputFileExtension", outputFileExtension);
    sngjson::json::FromJson(value, "debugInformationFileExtension", debugInformationFileExtension);
    sngjson::json::FromJson(value, "assemblyFileExtension", assemblyFileExtension);
    sngjson::json::FromJson(value, "configurations", configurations);
}

ToolChain::ToolChain() : name(), tools()
{
}

std::unique_ptr<soulng::util::JsonValue> ToolChain::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(name);
    object->AddField(U"name", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(tools);
    object->AddField(U"tools", std::move(fieldValue1));
    return object;
}

ToolChain::ToolChain(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "name", name);
    sngjson::json::FromJson(value, "tools", tools);
}

Platform::Platform() : name(), toolChains()
{
}

std::unique_ptr<soulng::util::JsonValue> Platform::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(name);
    object->AddField(U"name", std::move(fieldValue0));
    std::unique_ptr<soulng::util::JsonValue> fieldValue1 = sngjson::json::ToJson(toolChains);
    object->AddField(U"toolChains", std::move(fieldValue1));
    return object;
}

Platform::Platform(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "name", name);
    sngjson::json::FromJson(value, "toolChains", toolChains);
}

Platforms::Platforms() : platforms()
{
}

std::unique_ptr<soulng::util::JsonValue> Platforms::ToJson() const
{
    std::unique_ptr<soulng::util::JsonObject> object(new soulng::util::JsonObject());
    std::unique_ptr<soulng::util::JsonValue> fieldValue0 = sngjson::json::ToJson(platforms);
    object->AddField(U"platforms", std::move(fieldValue0));
    return object;
}

Platforms::Platforms(soulng::util::JsonValue* value)
{
    sngjson::json::FromJson(value, "platforms", platforms);
}

