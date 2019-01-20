// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcm/Compiler.hpp>
#include <cmajor/Build/Build.hpp>
#include <cmajor/ast/InitDone.hpp>
#include <cmajor/parsing/InitDone.hpp>
#include <cmajor/parsing/Exception.hpp>
#include <cmajor/util/InitDone.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/Log.hpp>
#include <cmajor/util/System.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/dom/Parser.hpp>
#include <cmajor/dom/CharacterData.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <mutex>
#include <stdexcept>

bool initialized = false;

using namespace cmajor::unicode;

class CompileData
{
public:
    static void Init();
    static void Done();
    static CompileData& Instance() { return *instance; }
    int SetCompileResult(const std::u16string& compileResult);
    int GetCompileResultLength(int compileResultHandle);
    int GetCompileResult(int compileResultHandle, char16_t* buf, int length);
private:
    static std::unique_ptr<CompileData> instance;
    std::mutex mtx;
    std::vector<std::unique_ptr<std::u16string>> compileResults;
};

std::unique_ptr<CompileData> CompileData::instance;

void CompileData::Init()
{
    instance.reset(new CompileData());
}

void CompileData::Done()
{
    instance.reset();
}

int CompileData::SetCompileResult(const std::u16string& compileResult)
{
    std::lock_guard<std::mutex> lock(mtx);
    int compileResultHandle = compileResults.size();
    compileResults.push_back(std::unique_ptr<std::u16string>(new std::u16string(compileResult)));
    return compileResultHandle;
}

int CompileData::GetCompileResultLength(int compileResultHandle)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (compileResultHandle < 0 || compileResultHandle >= compileResults.size()) return -1;
    if (!compileResults[compileResultHandle]) return -1;
    const std::u16string& compileResult = *compileResults[compileResultHandle];
    return compileResult.length();
}

int CompileData::GetCompileResult(int compileResultHandle, char16_t* buf, int size)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (compileResultHandle < 0 || compileResultHandle >= compileResults.size()) return -1;
    if (!compileResults[compileResultHandle]) return -1;
    const std::u16string& compileResult = *compileResults[compileResultHandle];
    int n = compileResult.length();
    if (size <= n) return -1;
    for (int i = 0; i < n; ++i)
    {
        buf[i] = compileResult[i];
    }
    for (int i = n; i < size; ++i)
    {
        buf[i] = u'\0';
    }
    compileResults[compileResultHandle].reset();
    return 0;
}

extern "C" void Init()
{
    cmajor::ast::Init();
    cmajor::symbols::Init();
    cmajor::parsing::Init();
    cmajor::util::Init();
    CompileData::Init();
    cmajor::util::SetLogMode(cmajor::util::LogMode::queue); 
    cmajor::util::DisableConsoleWindow();
    initialized = true;
}

extern "C" void Done()
{
    initialized = false;
    CompileData::Done();
    cmajor::util::Done();
    cmajor::parsing::Done();
    cmajor::symbols::Done();
    cmajor::ast::Done();
}

std::unique_ptr<cmajor::dom::Element> SpanElement(cmajor::symbols::Module* module, const cmajor::parsing::Span& span)
{
    std::unique_ptr<cmajor::dom::Element> spanElement(new cmajor::dom::Element(U"span"));
    if (span.Valid() && module)
    {
        std::string fileName = module->GetFilePath(span.FileIndex());
        if (fileName.empty()) return std::unique_ptr<cmajor::dom::Element>();
        spanElement->SetAttribute(U"file", ToUtf32(fileName));
        spanElement->SetAttribute(U"line", ToUtf32(std::to_string(span.LineNumber())));
        MappedInputFile file(fileName);
        std::string s(file.Begin(), file.End());
        std::u32string t(ToUtf32(s));
        std::u32string text = cmajor::parsing::GetErrorLines(&t[0], &t[0] + t.length(), span);
        int32_t startCol = 0;
        int32_t endCol = 0;
        GetColumns(&t[0], &t[0] + t.length(), span, startCol, endCol);
        spanElement->SetAttribute(U"startCol", ToUtf32(std::to_string(startCol)));
        spanElement->SetAttribute(U"endCol", ToUtf32(std::to_string(endCol)));
        spanElement->SetAttribute(U"text", text);
    }
    else
    {
        spanElement.reset();
    }
    return spanElement;
}

void AddWarningsTo(cmajor::dom::Element* diagnosticsElement, cmajor::symbols::Module* module)
{
    if (!module) return;
    if (!module->WarningCollection().Warnings().empty())
    {
        for (const cmajor::symbols::Warning& warning : module->WarningCollection().Warnings())
        {
            std::unique_ptr<cmajor::dom::Element> diagnosticElement(new cmajor::dom::Element(U"diagnostic"));
            diagnosticElement->SetAttribute(U"category", U"warning");
            diagnosticElement->SetAttribute(U"message", ToUtf32(warning.Message()));
            diagnosticElement->SetAttribute(U"project", warning.Project());
            std::unique_ptr<cmajor::dom::Element> spanElement = SpanElement(module, warning.Defined());
            if (spanElement)
            {
                diagnosticElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(spanElement.release()));
            }
            diagnosticsElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(diagnosticElement.release()));
            for (const cmajor::parsing::Span& span : warning.References())
            {
                if (!span.Valid()) continue;
                std::unique_ptr<cmajor::dom::Element> diagnosticElement(new cmajor::dom::Element(U"diagnostic"));
                diagnosticElement->SetAttribute(U"category", U"info");
                diagnosticElement->SetAttribute(U"message", ToUtf32("see reference to"));
                std::unique_ptr<cmajor::dom::Element> spanElement = SpanElement(module, span);
                if (spanElement)
                {
                    diagnosticElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(spanElement.release()));
                    diagnosticsElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(diagnosticElement.release()));
                }
            }
        }
    }
}

const char* version = "3.2.0";

extern "C" int Compile(const char16_t* compileXmlRequest)
{
    if (!initialized) return -1;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    cmajor::symbols::ResetGlobalFlags();
    cmajor::symbols::SetCompilerVersion(version);
    cmajor::symbols::Module* module = nullptr;
    bool noDebugInfo = false;
    std::unique_ptr<cmajor::symbols::Module> rootModule;
    std::vector<std::unique_ptr<cmajor::symbols::Module>> rootModules;
    cmajor::dom::Document compileResultDoc;
    std::unique_ptr<cmajor::dom::Element> compileResultElement(new cmajor::dom::Element(U"compileResult"));
    std::unique_ptr<cmajor::dom::Element> diagnosticsElement(new cmajor::dom::Element(U"diagnostics"));
    try
    {
        std::u32string compileRequest = ToUtf32(compileXmlRequest);
        std::unique_ptr<cmajor::dom::Document> compileRequestDoc = cmajor::dom::ParseDocument(compileRequest, "compileRequest");
        cmajor::dom::Element* compileRequestElement = compileRequestDoc->DocumentElement();
        std::string filePath = ToUtf8(compileRequestElement->GetAttribute(U"filePath"));
        std::string config = ToUtf8(compileRequestElement->GetAttribute(U"config"));
        if (config == "release")
        {
            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::release);
        }
        bool verbose = compileRequestElement->GetAttribute(U"verbose") == U"true";
        if (verbose)
        {
            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::verbose);
        }
        bool clean = compileRequestElement->GetAttribute(U"clean") == U"true";
        if (clean)
        {
            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::clean);
        }
        bool time = compileRequestElement->GetAttribute(U"time") == U"true";
        bool strictNothrow = compileRequestElement->GetAttribute(U"strict-nothrow") == U"true";
        if (strictNothrow)
        {
            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::strictNothrow);
        }
        bool emitLlvm = compileRequestElement->GetAttribute(U"emit-llvm") == U"true";
        if (emitLlvm)
        {
            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::emitLlvm);
        }
        bool emitOptLlvm = compileRequestElement->GetAttribute(U"emit-opt-llvm") == U"true";
        if (emitOptLlvm)
        {
            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::emitOptLlvm);
        }
        bool linkWithDebugRuntime = compileRequestElement->GetAttribute(U"link-with-debug-runtime") == U"true";
        if (linkWithDebugRuntime)
        {
            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::linkWithDebugRuntime);
        }
        bool linkUsingMsLink = compileRequestElement->GetAttribute(U"link-using-ms-link") == U"true";
        if (linkUsingMsLink)
        {
            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::linkUsingMsLink);
        }
        bool singleThreadedCompile = compileRequestElement->GetAttribute(U"single-threaded-compile") == U"true";
        if (singleThreadedCompile)
        {
            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::singleThreadedCompile); 
        }
        std::string optimizationLevelStr = ToUtf8(compileRequestElement->GetAttribute(U"optimization-level"));
        if (!optimizationLevelStr.empty())
        {
            int optimizationLevel = boost::lexical_cast<int>(optimizationLevelStr);
            cmajor::symbols::SetOptimizationLevel(optimizationLevel);
        }
        std::string buildThreads = ToUtf8(compileRequestElement->GetAttribute(U"build-threads"));
        if (!buildThreads.empty())
        {
            int numBuildThreads = boost::lexical_cast<int>(buildThreads);
            cmajor::symbols::SetNumBuildThreads(numBuildThreads);
        }
        if (cmajor::symbols::GetGlobalFlag(cmajor::symbols::GlobalFlags::verbose))
        {
            LogMessage(-1, "Cmajor compiler version " + std::string(version) + " for Windows x64");
        }
        if (!cmajor::symbols::GetGlobalFlag(cmajor::symbols::GlobalFlags::release) && !noDebugInfo)
        {
            cmajor::symbols::SetGlobalFlag(cmajor::symbols::GlobalFlags::generateDebugInfo);
        }
        if (Path::GetExtension(filePath) == ".cms")
        {
            cmajor::build::BuildSolution(GetFullPath(filePath), rootModules);
        }
        else if (Path::GetExtension(filePath) == ".cmp")
        {
            module = rootModule.get();
            cmajor::build::BuildProject(GetFullPath(filePath), rootModule);
        }
        else
        {
            throw std::runtime_error("invalid 'filePath' parameter: not .cms or .cmp");
        }
        compileResultElement->SetAttribute(U"success", U"true");
        if (time)
        {
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            auto dur = end - start;
            long long totalSecs = std::chrono::duration_cast<std::chrono::seconds>(dur).count() + 1;
            int hours = static_cast<int>(totalSecs / 3600);
            int mins = static_cast<int>((totalSecs / 60) % 60);
            int secs = static_cast<int>(totalSecs % 60);
            std::stringstream timeStream;
            timeStream <<
                (hours > 0 ? std::to_string(hours) + " hour" + ((hours != 1) ? "s " : " ") : "") <<
                (mins > 0 ? std::to_string(mins) + " minute" + ((mins != 1) ? "s " : " ") : "") <<
                secs << " second" << ((secs != 1) ? "s" : "");
            LogMessage(-1, timeStream.str());
        }
    }
    catch (const cmajor::parsing::ParsingException& ex)
    {
        LogMessage(-1, ex.what());
        module = static_cast<cmajor::symbols::Module*>(ex.Module());
        compileResultElement->SetAttribute(U"success", U"false");
        std::unique_ptr<cmajor::dom::Element> diagnosticElement(new cmajor::dom::Element(U"diagnostic"));
        diagnosticElement->SetAttribute(U"category", U"error");
        diagnosticElement->SetAttribute(U"message", ToUtf32(ex.Message()));
        diagnosticElement->SetAttribute(U"tool", U"cmc");
        diagnosticElement->SetAttribute(U"project", ToUtf32(ex.Project()));
        std::unique_ptr<cmajor::dom::Element> spanElement = SpanElement(module, ex.GetSpan());
        if (spanElement)
        {
            diagnosticElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(spanElement.release()));
        }
        diagnosticsElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(diagnosticElement.release()));
    }
    catch (const cmajor::symbols::Exception& ex)
    {
        LogMessage(-1, ex.What());
        module = ex.GetModule();
        compileResultElement->SetAttribute(U"success", U"false");
        std::unique_ptr<cmajor::dom::Element> diagnosticElement(new cmajor::dom::Element(U"diagnostic"));
        diagnosticElement->SetAttribute(U"category", U"error");
        diagnosticElement->SetAttribute(U"message", ToUtf32(ex.Message()));
        diagnosticElement->SetAttribute(U"tool", ex.GetModule()->GetCurrentToolName());
        diagnosticElement->SetAttribute(U"project", ex.GetModule()->GetCurrentProjectName());
        std::unique_ptr<cmajor::dom::Element> spanElement = SpanElement(ex.GetModule(), ex.Defined());
        if (spanElement)
        {
            diagnosticElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(spanElement.release()));
        }
        diagnosticsElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(diagnosticElement.release()));
        for (const cmajor::parsing::Span& span : ex.References())
        {
            if (!span.Valid()) continue;
            std::unique_ptr<cmajor::dom::Element> diagnosticElement(new cmajor::dom::Element(U"diagnostic"));
            diagnosticElement->SetAttribute(U"category", U"info");
            diagnosticElement->SetAttribute(U"message", ToUtf32("see reference to"));
            std::unique_ptr<cmajor::dom::Element> spanElement = SpanElement(ex.GetModule(), span);
            if (spanElement)
            {
                diagnosticElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(spanElement.release()));
                diagnosticsElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(diagnosticElement.release()));
            }
        }
    }
    catch (const std::exception& ex)
    {
        LogMessage(-1, ex.what());
        compileResultElement->SetAttribute(U"success", U"false");
        std::unique_ptr<cmajor::dom::Element> diagnosticElement(new cmajor::dom::Element(U"diagnostic"));
        diagnosticElement->SetAttribute(U"category", U"error");
        diagnosticElement->SetAttribute(U"message", ToUtf32(ex.what()));
        diagnosticsElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(diagnosticElement.release()));
    }
    if (!module)
    {
        if (rootModule)
        {
            module = rootModule.get();
        }
        else
        {
            for (const auto& m : rootModules)
            {
                if (m)
                {
                    module = m.get();
                }
            }
        }
    }
    AddWarningsTo(diagnosticsElement.get(), module);
    compileResultElement->AppendChild(std::unique_ptr<cmajor::dom::Node>(diagnosticsElement.release()));
    compileResultDoc.AppendChild(std::unique_ptr<cmajor::dom::Node>(compileResultElement.release()));
    std::ostringstream strStream;
    CodeFormatter formatter(strStream);
    formatter.SetIndentSize(1);
    compileResultDoc.Write(formatter);
    return CompileData::Instance().SetCompileResult(ToUtf16(strStream.str()));
}

extern "C" int GetCompileResultLength(int compileResultHandle)
{
    if (!initialized) return -1;
    return CompileData::Instance().GetCompileResultLength(compileResultHandle);
}

extern "C" int GetCompileResult(int compileResultHandle, char16_t* buf, int size)
{
    if (!initialized) return -1;
    return CompileData::Instance().GetCompileResult(compileResultHandle, buf, size);
}

extern "C" void StopBuild()
{
    cmajor::build::StopBuild();
}

extern "C" int WaitForLogMessage()
{
    if (!initialized) return -1;
    return cmajor::util::WaitForLogMessage();
}

extern "C" int FetchLogMessage(char16_t* buf, int size)
{
    if (!initialized) return -1;
    return cmajor::util::FetchLogMessage(buf, size);
}

extern "C" void StartLog()
{
    cmajor::util::StartLog();
}

extern "C" void EndLog()
{
    cmajor::util::EndLog();
}

extern "C" void ResetModuleCache()
{
    cmajor::symbols::ResetModuleCache();
}

extern "C" void SetUseModuleCache(bool useModuleCache_)
{
    cmajor::symbols::SetUseModuleCache(useModuleCache_);
}
