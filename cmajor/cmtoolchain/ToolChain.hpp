#ifndef HPP_87DFBD1E06DDC83F99A0E744C366852A5D970885
#define HPP_87DFBD1E06DDC83F99A0E744C366852A5D970885
#include <soulng/util/Json.hpp>
#include <cmajor/cmtoolchain/ToolChainApi.hpp>
#include <string>
#include <vector>
#include <memory>

struct CMTOOLCHAIN_API Tool
{
    Tool();
    Tool(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string name;
    std::string commandName;
    std::string outputFileExtension;
    std::string debugInformationFileExtension;
    std::string assemblyFileExtension;
    std::vector<std::string> args;
};

struct CMTOOLCHAIN_API ToolChain
{
    ToolChain();
    ToolChain(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::string name;
    std::vector<Tool> tools;
};

struct CMTOOLCHAIN_API ToolChains
{
    ToolChains();
    ToolChains(soulng::util::JsonValue* value);
    std::unique_ptr<soulng::util::JsonValue> ToJson() const;
    std::vector<ToolChain> toolChains;
};

#endif // HPP_87DFBD1E06DDC83F99A0E744C366852A5D970885
