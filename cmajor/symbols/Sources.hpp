// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_SOURCES_INCLUDED
#define CMAJOR_SYMBOLS_SOURCES_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <sngcm/ast/CompileUnit.hpp>
#include <chrono>
#include <memory>
#include <map>
#include <string>
#include <vector>

namespace cmajor { namespace symbols {

using namespace sngcm::ast;

class Module;
class Symbol;
class ContainerSymbol;
class ContainerScope;
class FileScope;
struct CCSymbolEntry;

enum class CCContext
{
    genericContext, emptyContext, globalContext, classContext, statementContext
};

SYMBOLS_API CCContext GetCCContext(const std::u32string& cursorLine, const std::string& filePath, int index, const std::vector<int>& globalRuleContext);
SYMBOLS_API std::vector<CCSymbolEntry> GetKeywordEntries(CCContext ccContext, const std::u32string& prefix);

using TypeBindingFunction = std::vector<std::string> (*)(Module* module, CompileUnitNode* compileUnit);

SYMBOLS_API void SetTypeBindingFunction(TypeBindingFunction typeBindingFunc);

class SYMBOLS_API Source
{
public:
    Source(const std::string& filePath_);
    const std::string& FilePath() const { return filePath; }
    void Read();
    void SetContent(const std::u32string& content_);
    const std::u32string& Content() const { return content; }
    const char32_t* Start() const { return content.c_str(); }
    const char32_t* End() const { return content.c_str() + content.size(); }
    void Parse(const boost::uuids::uuid& moduleId, int index);
    const std::vector<std::string>& Errors() const { return errors; }
    bool Synchronized() const { return synchronized; }
    ContainerSymbol* CursorContainer() const { return cursorContainer; }
    CompileUnitNode* CompileUnit() { return compileUnit.get(); }
    void AddSymbol(Symbol* symbol);
    void AddSymbols(Module* module);
    void RemoveSymbols();
    void GetScopes(Module* module);
    void BindTypes(Module* module);
    std::vector<CCSymbolEntry> LookupSymbolsBeginningWith(const std::u32string& prefix);
    std::string GetCCList(Module* module, const std::u32string& ccText, const std::u32string& cursorLine, int index, const std::vector<int>& ruleContext);
    std::string GetParamHelpList(int symbolIndex);
    const std::vector<int>& RuleContext() const { return ruleContext; }
private:
    void SetRuleContext(const std::vector<int>& rc_);
    void SetRuleContext();
    std::string filePath;
    std::u32string content;
    std::vector<std::string> errors;
    bool synchronized;
    std::unique_ptr<CompileUnitNode> compileUnit;
    std::vector<Symbol*> symbols;
    std::vector<AliasNode*> aliasNodes;
    std::vector<NamespaceImportNode*> namespaceImports;
    ContainerSymbol* cursorContainer;
    ContainerScope* cursorScope;
    std::unique_ptr<FileScope> fileScope;
    std::vector<Symbol*> ccSymbols;
    std::vector<int> rc;
    std::vector<int> ruleContext;
};

struct SYMBOLS_API ParseResult
{
    ParseResult() : ok(true), numberOfErrors(0), synchronized(false), start(), end() {}
    bool ok;
    std::string error;
    int numberOfErrors;
    bool synchronized;
    std::string cursorContainer;
    std::vector<int> ruleContext;
    std::vector<std::string> errors;
    std::chrono::steady_clock::time_point start;
    std::chrono::steady_clock::time_point end;
};

class SYMBOLS_API Sources
{
public:
    Sources(const std::vector<std::string>& filePaths);
    Sources(const Sources&) = delete;
    Sources(Sources&&) = delete;
    Sources& operator=(const Sources&) = delete;
    Sources& operator=(Sources&&) = delete;
    int Count() const { return sources.size(); }
    Source* GetSource(int i) { return sources[i].get(); }
    int GetSourceIndex(const std::string& filePath);
    void MakeSourceIndexMap();
    ParseResult Parse(Module* module);
    void AddSymbols(Module* module);
    void GetScopes(Module* module);
    void BindTypes(Module* module);
    int GetNumberOfErrors();
    bool Synchronized();
    ParseResult ParseSource(Module* module, const std::string& sourceFilePath, const std::u32string& sourceCode);
    std::string GetCCList(Module* module, const std::string& sourceFilePath, const std::u32string& ccText, const std::u32string& cursorLine, const std::vector<int>& ruleContext);
    std::string GetParamHelpList(Module* module, const std::string& sourceFilePath, int symbolIndex);
private:
    std::vector<std::unique_ptr<Source>> sources;
    std::map<std::string, int> sourceIndexMap;
};

SYMBOLS_API void InitSources();
SYMBOLS_API void DoneSources();

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_SOURCES_INCLUDED
