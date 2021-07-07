// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Sources.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowparser/CompileUnit.hpp>
#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <thread>
#include <mutex>
#include <sstream>

namespace cmajor { namespace symbols {

using namespace soulng::util;
using namespace soulng::unicode;

TypeBindingFunction typeBindingFunction;

void SetTypeBindingFunction(TypeBindingFunction typeBindingFunc)
{
    typeBindingFunction = typeBindingFunc;
}

bool IsValidCCSymbol(Symbol* symbol, Module* module, FunctionSymbol* fromFunction)
{
    AccessCheckFunction hasAccess = GetAccessCheckFunction();
    switch (symbol->GetSymbolType())
    {
        case SymbolType::functionGroupSymbol:
        {
            FunctionGroupSymbol* group = static_cast<FunctionGroupSymbol*>(symbol);
            if (group->IsValidCCFunctionGroup(fromFunction))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        case SymbolType::classGroupTypeSymbol:
        {
            ClassGroupTypeSymbol* group = static_cast<ClassGroupTypeSymbol*>(symbol);
            return group->IsValidCCClassGroup(module, fromFunction);
        }
        case SymbolType::templateParameterSymbol:
        case SymbolType::boundTemplateParameterSymbol:
        {
            return false;
        }
        case SymbolType::memberVariableSymbol:
        {
            return hasAccess(fromFunction, symbol);
        }
        default:
        {
            return !StartsWith(symbol->Name(), U"@");
        }
    }
    return true;
}

Source::Source(const std::string& filePath_) : filePath(filePath_), cursorScope(nullptr), cursorContainer(nullptr), synchronized(false)
{
}

void Source::Read()
{
    std::string str = ReadFile(filePath);
    content = ToUtf32(str);
}

void Source::Parse(const boost::uuids::uuid& moduleId, int index)
{
    errors.clear();
    CmajorNothrowLexer lexer(Start(), End(), FilePath(), index);
    boost::uuids::uuid mid = moduleId;
    NothrowParsingContext parsingContext;
    std::unique_ptr<CompileUnitNode> parsedCompileUnit = NothrowCompileUnitParser::Parse(lexer, &mid, &parsingContext);
    std::vector<std::unique_ptr<std::exception>> parsingErrors = lexer.Errors();
    if (!parsingErrors.empty())
    {
        CmajorNothrowLexer lexer(Start(), End(), FilePath(), index);
        lexer.SetFlag(LexerFlags::synchronize);
        NothrowParsingContext parsingContext;
        parsedCompileUnit = NothrowCompileUnitParser::Parse(lexer, &mid, &parsingContext);
        parsingErrors = lexer.Errors();
        synchronized = lexer.GetFlag(LexerFlags::synchronizedAtLeastOnce);
    }
    else
    {
        synchronized = lexer.GetFlag(LexerFlags::synchronizedAtLeastOnce);
    }
    for (const std::unique_ptr<std::exception>& ex : parsingErrors)
    {
        errors.push_back(ex->what());
    }
    compileUnit = std::move(parsedCompileUnit);
}

void Source::SetContent(const std::u32string& content_)
{
    content = content_;
}

void Source::AddSymbol(Symbol* symbol)
{
    symbols.push_back(symbol);
}

void Source::AddSymbols(Module* module)
{
    try
    {
        if (compileUnit)
        {
            symbols.clear();
            aliasNodes.clear();
            namespaceImports.clear(); 
            cursorContainer = nullptr;
            SymbolTable& symbolTable = module->GetSymbolTable();
            symbolTable.ResetMainFunctionSymbol();
            SymbolCreatorVisitor visitor(symbolTable);
            visitor.SetEditMode();
            visitor.SetSource(this);
            compileUnit->Accept(visitor);
            aliasNodes = symbolTable.AliasNodes();
            namespaceImports = symbolTable.NamespaceImports();
            cursorContainer = symbolTable.CursorContainer();
        }
    }
    catch (const Exception& ex)
    {
        errors.push_back(ex.Message());
    }
    catch (const std::exception& ex)
    {
        errors.push_back(ex.what());
    }
    catch (...)
    { 
        errors.push_back("unknown error occurred");
    }
}

void Source::GetScopes(Module* module)
{
    try
    {
        cursorScope = nullptr;
        fileScope.reset(new FileScope());
        SymbolTable& symbolTable = module->GetSymbolTable();
        if (cursorContainer)
        {
            cursorScope = cursorContainer->GetContainerScope();
        }
        else
        {
            cursorScope = symbolTable.GlobalNs().GetContainerScope();
        }
        for (AliasNode* aliasNode : aliasNodes)
        {
            try
            {
                fileScope->InstallAlias(symbolTable.GlobalNs().GetContainerScope(), aliasNode);
            }
            catch (const Exception& ex)
            {
                errors.push_back(ex.Message());
            }
        }
        for (NamespaceImportNode* namespaceImport : namespaceImports)
        {
            try
            {
                fileScope->InstallNamespaceImport(symbolTable.GlobalNs().GetContainerScope(), namespaceImport);
            }
            catch (const Exception& ex)
            {
                errors.push_back(ex.Message());
            }
        }
    }
    catch (const Exception& ex)
    {
        errors.push_back(ex.Message());
    }
    catch (const std::exception& ex)
    {
        errors.push_back(ex.what());
    }
    catch (...)
    {
        errors.push_back("unknown error occurred");
    }
}

void Source::RemoveSymbols()
{
    int n = symbols.size();
    for (int i = n - 1; i >= 0; --i)
    {
        std::unique_ptr<Symbol> symbol = symbols[i]->RemoveFromParent();
    }
    symbols.clear();
}

void Source::BindTypes(Module* module)
{
    try
    {
        if (typeBindingFunction && compileUnit.get())
        {
            std::vector<std::string> e = typeBindingFunction(module, compileUnit.get());
            errors.insert(errors.end(), e.cbegin(), e.cend());
        }
    }
    catch (const Exception& ex)
    {
        errors.push_back(ex.Message());
    }
    catch (const std::exception& ex)
    {
        errors.push_back(ex.what());
    }
    catch (...)
    {
        errors.push_back("unknown error occurred");
    }
}

std::vector<CCSymbolEntry> Source::LookupSymbolsBeginningWith(const std::u32string& prefix)
{
    if (!cursorScope || !fileScope) return std::vector<CCSymbolEntry>();
    std::vector<CCSymbolEntry> matches = cursorScope->LookupBeginWith(prefix, ScopeLookup::this_and_base_and_parent);
    std::vector<CCSymbolEntry> m = fileScope->LookupBeginWith(prefix);
    AddMatches(matches, m);
    return matches;
}

std::string Source::GetCCList(Module* module, const std::string& ccText)
{
    ccSymbols.clear();
    FunctionSymbol* fromFunction = nullptr;
    if (cursorContainer)
    {
        fromFunction = cursorContainer->FunctionNoThrow();
    }
    std::u32string prefix = ToUtf32(ccText);
    std::vector<CCSymbolEntry> ccSymbolEntries = LookupSymbolsBeginningWith(prefix);
    sngxml::dom::Document ccListDoc;
    sngxml::dom::Element* ccListElement = new sngxml::dom::Element(U"ccList");
    ccListDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(ccListElement));
    int symbolIndex = 0;
    for (const CCSymbolEntry& ccSymbolEntry : ccSymbolEntries)
    {
        Symbol* symbol = ccSymbolEntry.symbol;
        int ccPrefixLength = ccSymbolEntry.ccPrefixLen;
        const std::u32string& replacement = ccSymbolEntry.replacement;
        if (IsValidCCSymbol(symbol, module, fromFunction))
        {
            sngxml::dom::Element* ccElement = symbol->ToCCElement(ccPrefixLength, replacement, symbolIndex++);
            ccListElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(ccElement));
            ccSymbols.push_back(symbol);
        }
    }
    std::stringstream s;
    CodeFormatter formatter(s);
    ccListDoc.Write(formatter);
    return s.str();
}

std::string Source::GetSymbolList(int symbolIndex)
{
    if (symbolIndex >= 0 && symbolIndex < ccSymbols.size())
    {
        return std::string("foo");
    }
    else
    {
        throw std::runtime_error("invalid symbol index");
    }
}

struct ParserData
{
    ParserData(bool& stop_, std::list<int>& indexQueue_, std::vector<std::exception_ptr>& exceptions_, Sources& sources_, const boost::uuids::uuid& moduleId_) :
        stop(stop_), indexQueue(indexQueue_), exceptions(exceptions_), sources(sources_), moduleId(moduleId_)
    {
    }
    bool& stop;
    std::list<int>& indexQueue;
    std::vector<std::exception_ptr>& exceptions;
    Sources& sources;
    boost::uuids::uuid moduleId;
    std::mutex mtx;
};

void DoParseSource(ParserData* parserData)
{
    int index = -1;
    try
    {
        while (!parserData->stop)
        {
            {
                std::lock_guard<std::mutex> lock(parserData->mtx);
                if (parserData->indexQueue.empty()) return;
                index = parserData->indexQueue.front();
                parserData->indexQueue.pop_front();
            }
            Source* source = parserData->sources.GetSource(index);
            source->Read();
            source->Parse(parserData->moduleId, index);
        }
    }
    catch (...)
    {
        if (index != -1)
        {
            parserData->exceptions[index] = std::current_exception();
            parserData->stop = true;
        }
    }
}

Sources::Sources(const std::vector<std::string>& filePaths)
{
    int n = filePaths.size();
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<Source> source(new Source(filePaths[i]));
        sources.push_back(std::move(source));
    }
    MakeSourceIndexMap();
}

int Sources::GetSourceIndex(const std::string& filePath)
{
    auto it = sourceIndexMap.find(filePath);
    if (it != sourceIndexMap.cend())
    {
        return it->second;
    }
    else
    {
        return -1;
    }
}

void Sources::MakeSourceIndexMap()
{
    sourceIndexMap.clear();
    int n = sources.size();
    for (int i = 0; i < n; ++i)
    {
        Source* source = sources[i].get();
        sourceIndexMap[source->FilePath()] = i;
    }
}

ParseResult Sources::Parse(Module* module)
{
    ParseResult result;
    result.start = std::chrono::steady_clock::now();
    try
    {
        bool stop = false;
        std::list<int> indexQueue;
        std::vector<std::exception_ptr> exceptions;
        int n = Count();
        exceptions.resize(n);
        for (int i = 0; i < n; ++i)
        {
            indexQueue.push_back(i);
        }
        boost::uuids::uuid moduleId = boost::uuids::nil_uuid();
        if (module)
        {
            moduleId = module->Id();
        }
        ParserData parserData(stop, indexQueue, exceptions, *this, moduleId);
        std::vector<std::thread> threads;
        int numThreads = std::thread::hardware_concurrency();
        if (numThreads <= 0)
        {
            numThreads = 1;
        }
        for (int i = 0; i < numThreads; ++i)
        {
            threads.push_back(std::thread(DoParseSource, &parserData));
            if (parserData.stop) break;
        }
        int numStartedThreads = threads.size();
        for (int i = 0; i < numStartedThreads; ++i)
        {
            if (threads[i].joinable())
            {
                threads[i].join();
            }
        }
        for (int i = 0; i < n; ++i)
        {
            if (exceptions[i])
            {
                std::rethrow_exception(exceptions[i]);
            }
        }
    }
    catch (const Exception& ex)
    {
        result.ok = false;
        result.error = StringStr(ex.Message());
    }
    catch (const std::exception& ex)
    {
        result.ok = false;
        result.error = StringStr(ex.what());
    }
    catch (...)
    {
        result.ok = false;
        result.error = "unknown error occurred";
    }
    result.end = std::chrono::steady_clock::now();
    return result;
}

void Sources::AddSymbols(Module* module)
{
    for (int i = 0; i < sources.size(); ++i)
    {
        Source* source = GetSource(i);
        source->AddSymbols(module);
    }
}

void Sources::GetScopes(Module* module)
{
    for (int i = 0; i < sources.size(); ++i)
    {
        Source* source = GetSource(i);
        source->GetScopes(module);
    }
}

void Sources::BindTypes(Module* module)
{
    for (int i = 0; i < sources.size(); ++i)
    {
        Source* source = GetSource(i);
        source->BindTypes(module);
    }
}

int Sources::GetNumberOfErrors()
{
    int numberOfErrors = 0;
    for (int i = 0; i < sources.size(); ++i)
    {
        Source* source = GetSource(i);
        numberOfErrors += source->Errors().size();
    }
    return numberOfErrors;
}

bool Sources::Synchronized()
{
    bool synchronized = false;
    for (int i = 0; i < sources.size(); ++i)
    {
        Source* source = GetSource(i);
        if (source->Synchronized())
        {
            synchronized = true;
            break;
        }
    }
    return synchronized;
}

ParseResult Sources::ParseSource(Module* module, const std::string& sourceFilePath, const std::u32string& sourceCode)
{
    ParseResult result;
    result.ok = true;
    result.start = std::chrono::steady_clock::now();
    try
    {
        int index = GetSourceIndex(sourceFilePath);
        if (index == -1)
        {
            result.ok = false;
            result.error = "source file path '" + sourceFilePath + "' not found";
            return result;
        }
        bool moveSource = false;
        Source* src = sources[index].get();
        if (index < sources.size() - 1)
        {
            moveSource = true;
            for (int i = sources.size() - 1; i >= 0; --i)
            {
                Source* s = GetSource(i);
                s->RemoveSymbols();
            }
        }
        else
        {
            src->RemoveSymbols();
        }
        if (moveSource)
        {
            std::unique_ptr<Source> source = std::move(sources[index]);
            sources.erase(sources.begin() + index);
            sources.push_back(std::move(source));
            MakeSourceIndexMap();
        }
        src->SetContent(sourceCode);
        src->Parse(module->Id(), sources.size());
        if (moveSource)
        {
            for (int i = 0; i < sources.size(); ++i)
            {
                Source* s = GetSource(i);
                s->AddSymbols(module);
            }
        }
        else
        {
            src->AddSymbols(module);
        }
        if (moveSource)
        {
            for (int i = 0; i < sources.size(); ++i)
            {
                Source* s = GetSource(i);
                s->GetScopes(module);
            }
        }
        else
        {
            src->GetScopes(module);
        }
        if (moveSource)
        {
            for (int i = 0; i < sources.size(); ++i)
            {
                Source* s = GetSource(i);
                s->BindTypes(module);
            }
        }
        else
        {
            src->BindTypes(module);
        }
        result.numberOfErrors = src->Errors().size();
        for (const std::string& error : src->Errors())
        {
            result.errors.push_back(StringStr(error));
        }
        result.synchronized = src->Synchronized();
        if (src->CursorContainer())
        {
            result.cursorContainer = ToUtf8(src->CursorContainer()->FullName());
        }
    }
    catch (const Exception& ex)
    {
        result.ok = false;
        result.error = StringStr(ex.Message());
        MakeSourceIndexMap();
    }
    catch (const std::exception& ex)
    {
        result.ok = false;
        result.error = StringStr(ex.what());
        MakeSourceIndexMap();
    }
    catch (...)
    {
        result.ok = false;
        result.error = "unknown error occurred";
        MakeSourceIndexMap();
    }
    result.end = std::chrono::steady_clock::now();
    return result;
}

std::string Sources::GetCCList(Module* module, const std::string& sourceFilePath, const std::string& ccText)
{
    int index = GetSourceIndex(sourceFilePath);
    if (index == -1)
    {
        throw std::runtime_error("source file path '" + sourceFilePath + "' not found");
    }
    Source* source = GetSource(index);
    return source->GetCCList(module, ccText);
}

std::string Sources::GetSymbolList(Module* module, const std::string& sourceFilePath, int symbolIndex)
{
    int index = GetSourceIndex(sourceFilePath);
    if (index == -1)
    {
        throw std::runtime_error("source file path '" + sourceFilePath + "' not found");
    }
    Source* source = GetSource(index);
    return source->GetSymbolList(symbolIndex);
}

} } // namespace cmajor::symbols
