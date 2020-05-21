#include <sngjson/json/JsonSchemaLexer.hpp>
#include <sngjson/json/JsonSchemaParser.hpp>
#include <sngjson/json/JsonSchema.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Unicode.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace soulng::util;
using namespace soulng::unicode;

int main(int argc, const char** argv)
{
    try
    {
        bool verbose = false;
        std::vector<std::string> fileNames;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (StartsWith(arg, "--"))
            {
                if (arg == "--verbose")
                {
                    verbose = true;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (StartsWith(arg, "-"))
            {
                std::string options = arg.substr(1);
                for (char o : options)
                {
                    switch (o)
                    {
                        case 'v': verbose = true; break;
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else
            {
                fileNames.push_back(GetFullPath(arg));
            }
        }
        int index = 0;
        for (const std::string& fileName : fileNames)
        {
            if (verbose)
            {
                std::cout << "> " << fileName << std::endl;
            }
            std::u32string content = ToUtf32(ReadFile(fileName));
            JsonSchemaLexer lexer(content, fileName, index);
            sngjson::json::Context context;
            JsonSchemaParser::Parse(lexer, &context);
            std::string hppFilePath = Path::ChangeExtension(fileName, ".hpp");
            std::string cppFilePath = Path::ChangeExtension(fileName, ".cpp");
            context.Write(hppFilePath, cppFilePath);
            ++index;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
