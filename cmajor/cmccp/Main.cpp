#include <cmajor/build/InitDone.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/EditModuleCache.hpp>
#include <sngxml/xpath/InitDone.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <cmajor/build/Build.hpp>
#include <cmajor/binder/TypeBinding.hpp>
#include <cmajor/symbols/Sources.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowparser/CompileUnit.hpp>
#include <sngcm/ast/Project.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Time.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngxml::xpath::Init();
        sngcm::ast::Init();
        cmajor::symbols::Init();
        cmajor::build::Init();
    }
    ~InitDone()
    {
        cmajor::build::Done();
        cmajor::symbols::Done();
        sngcm::ast::Done();
        sngxml::xpath::Done();
        soulng::util::Done();
    }
};

using namespace soulng::util;
using namespace soulng::unicode;
using namespace cmajor::symbols;
using namespace cmajor::build;
using namespace sngcm::ast;

void PrintHelp()
{

}

int main(int argc, const char** argv)
{
    InitDone initDone;
    SetReadProjectFunction(cmajor::build::ReadProject);
    SetTypeBindingFunction(cmajor::binder::BindTypes);
    try
    {
        std::vector<std::string> cmFiles;
        std::vector<std::string> cmpFiles;
        bool modules = false;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (soulng::util::StartsWith(arg, "--"))
            {
                if (arg == "--help")
                {
                    PrintHelp();
                    return 1;
                }
                else if (arg == "--module")
                {
                    modules = true;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (soulng::util::StartsWith(arg, "-"))
            {
                std::string options = arg.substr(1);
                for (char o : options)
                {
                    switch (o)
                    {
                        case 'h': 
                        {
                            PrintHelp();
                            return 1;
                        }
                        case 'm':
                        {
                            modules = true;
                            break;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else
            {
                if (EndsWith(arg, ".cm"))
                {
                    cmFiles.push_back(arg);
                }
                else if (EndsWith(arg, ".cmp"))
                {
                    cmpFiles.push_back(arg);
                }
                else
                {
                    throw std::runtime_error("unknown file extension '" + Path::GetExtension(arg) + "' (not .cm or .cmp)");
                }
            }
        }
        for (int i = 0; i < cmFiles.size(); ++i)
        {
            Source source(cmFiles[i]);
            source.Read();
            boost::uuids::uuid moduleId = boost::uuids::nil_uuid();
            source.Parse(moduleId, i);
        }
        if (modules)
        {
            if (cmpFiles.empty())
            {
                throw std::runtime_error("no projects given");
            }
            for (int i = 0; i < cmpFiles.size(); ++i)
            {
                LoadEditModuleResult result = EditModuleCache::Instance().LoadEditModule(cmpFiles[i], "cpp", "debug");
                if (result.ok)
                {
                    std::chrono::steady_clock::duration loadingDuration = result.startParsing - result.startLoading;
                    std::string loadingDurationStr = DurationStr(std::chrono::duration_cast<std::chrono::nanoseconds>(loadingDuration));
                    std::chrono::steady_clock::duration parsingDuration = result.endParsing - result.startParsing;
                    std::string parsingDurationStr = DurationStr(std::chrono::duration_cast<std::chrono::nanoseconds>(parsingDuration));
                    std::chrono::steady_clock::duration addSymbolsDuration = result.endLoading - result.endParsing;
                    std::string addSymbolsDurationStr = DurationStr(std::chrono::duration_cast<std::chrono::nanoseconds>(addSymbolsDuration));
                    std::cout << cmpFiles[i] << " : " << "loading: " << loadingDurationStr << ", parsing: " << parsingDurationStr << ", add symbols: " << addSymbolsDurationStr << std::endl;
                    Module* editModule = EditModuleCache::Instance().GetEditModule(cmpFiles[i], "cpp", "debug");
                    if (editModule)
                    {
                        Sources* sources = editModule->GetSources();
                        std::string filePath = GetFullPath("C:\\work\\cmajorm\\cmajor\\system\\System.Base\\Algorithm.cm");
                        int index = sources->GetSourceIndex(filePath);
                        if (index != -1)
                        {
                            Source* source = sources->GetSource(index);
                            std::vector<Symbol*> matches = source->LookupSymbolsBeginningWith(U"System.Collections.List");
                        }
                    }
                }
                else
                {
                    std::cout << cmpFiles[i] << "error: " << result.error << std::endl;
                }
            }
        }
        else
        {
            for (int i = 0; i < cmpFiles.size(); ++i)
            {
                std::unique_ptr<Project> project = ReadProject(cmpFiles[i]);
                Sources sources(project->SourceFilePaths());
                ParseResult parseResult = sources.Parse(nullptr);
                if (parseResult.ok)
                {
                    std::chrono::steady_clock::duration duration = parseResult.end - parseResult.start;
                    std::string durationStr = DurationStr(std::chrono::duration_cast<std::chrono::nanoseconds>(duration));
                    std::cout << ToUtf8(project->Name()) << " | number of errors: " << parseResult.numberOfErrors << " | " << durationStr << std::endl;
                }
                else
                {
                    std::cout << ToUtf8(project->Name()) << " | " << "error: " << parseResult.error << std::endl;
                }
            }
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}