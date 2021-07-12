// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Sources.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <cmajor/symbols/KeywordSymbol.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowparser/CompileUnit.hpp>
#include <sngcm/cmnothrowparser/Statement.hpp>
#include <sngcm/cmnothrowparser/Class.hpp>
#include <sngcm/cmnothrowparser/NothrowParsingContext.hpp>
#include <sngcm/cmnothrowparser/Rules.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <algorithm>
#include <thread>
#include <mutex>
#include <sstream>

namespace cmajor { namespace symbols {

using namespace soulng::util;
using namespace soulng::unicode;

class SourceData
{
public:
    static void Init();
    static void Done();
    static SourceData& Instance() { return *instance; }
    const std::vector<int>& GlobalRuleIds() const { return globalRuleIds; }
    const std::vector<int>& DefininingIdRules() const { return definingIdRules; }
    const std::vector<int>& CompoundStatementRuleId() const { return compoundStatementRuleId; }
    const std::vector<int>& StatementRuleIds() const { return statementRuleIds; }
    const std::vector<int>& ClassRuleId() const { return classRuleId; }
private:
    SourceData();
    static std::unique_ptr<SourceData> instance;
    std::vector<int> globalRuleIds;
    std::vector<int> definingIdRules;
    std::vector<int> compoundStatementRuleId;
    std::vector<int> statementRuleIds;
    std::vector<int> classRuleId;
};

std::unique_ptr<SourceData> SourceData::instance;

void SourceData::Init()
{
    instance.reset(new SourceData());
}

void SourceData::Done()
{
    instance.reset();
}

SourceData::SourceData()
{
    globalRuleIds.push_back(NothrowCompileUnitParser_UsingDirectives);
    globalRuleIds.push_back(NothrowCompileUnitParser_NamespaceContent);
    definingIdRules.push_back(NothrowClassParser_DefiningClassId);
    definingIdRules.push_back(NothrowClassParser_DefiningMemberVariableId);
    definingIdRules.push_back(NothrowCompileUnitParser_DefiningNamespaceId);
    definingIdRules.push_back(NothrowConceptParser_DefiningConceptId);
    definingIdRules.push_back(NothrowDelegateParser_DefiningDelegateId);
    definingIdRules.push_back(NothrowDelegateParser_DefiningClassDelegateId);
    definingIdRules.push_back(NothrowEnumerationParser_DefiningEnumTypeId);
    definingIdRules.push_back(NothrowFunctionParser_FunctionGroupId);
    definingIdRules.push_back(NothrowGlobalVariableParser_DefininigGlobalVariableId);
    definingIdRules.push_back(NothrowInterfaceParser_DefiningInterfaceId);
    definingIdRules.push_back(NothrowInterfaceParser_InterfaceFunctionGroupId);
    definingIdRules.push_back(NothrowTypedefParser_DefiningTypedefId);
    definingIdRules.push_back(NothrowStatementParser_DefiningRangeForId);
    definingIdRules.push_back(NothrowStatementParser_DefiningLocalVariableId);
    compoundStatementRuleId.push_back(NothrowStatementParser_CompoundStatement);
    statementRuleIds.push_back(NothrowStatementParser_Statement);
    statementRuleIds.push_back(NothrowStatementParser_LabeledStatement);
    statementRuleIds.push_back(NothrowStatementParser_ControlStatement);
    statementRuleIds.push_back(NothrowStatementParser_CompoundStatement);
    statementRuleIds.push_back(NothrowStatementParser_ReturnStatement);
    statementRuleIds.push_back(NothrowStatementParser_IfStatement);
    statementRuleIds.push_back(NothrowStatementParser_WhileStatement);
    statementRuleIds.push_back(NothrowStatementParser_DoStatement);
    statementRuleIds.push_back(NothrowStatementParser_ForStatement);
    statementRuleIds.push_back(NothrowStatementParser_ForInitStatement);
    statementRuleIds.push_back(NothrowStatementParser_ForLoopStatementExpr);
    statementRuleIds.push_back(NothrowStatementParser_RangeForStatement);
    statementRuleIds.push_back(NothrowStatementParser_BreakStatement);
    statementRuleIds.push_back(NothrowStatementParser_ContinueStatement);
    statementRuleIds.push_back(NothrowStatementParser_GotoStatement);
    statementRuleIds.push_back(NothrowStatementParser_SwitchStatement);
    statementRuleIds.push_back(NothrowStatementParser_CaseStatement);
    statementRuleIds.push_back(NothrowStatementParser_DefaultStatement);
    statementRuleIds.push_back(NothrowStatementParser_GotoCaseStatement);
    statementRuleIds.push_back(NothrowStatementParser_GotoDefaultStatement);
    statementRuleIds.push_back(NothrowStatementParser_AssignmentStatementExpr);
    statementRuleIds.push_back(NothrowStatementParser_AssignmentStatement);
    statementRuleIds.push_back(NothrowStatementParser_ConstructionStatement);
    statementRuleIds.push_back(NothrowStatementParser_DeleteStatement);
    statementRuleIds.push_back(NothrowStatementParser_DestroyStatement);
    statementRuleIds.push_back(NothrowStatementParser_ExpressionStatement);
    statementRuleIds.push_back(NothrowStatementParser_EmptyStatement);
    statementRuleIds.push_back(NothrowStatementParser_ThrowStatement);
    statementRuleIds.push_back(NothrowStatementParser_TryStatement);
    statementRuleIds.push_back(NothrowStatementParser_Catch);
    statementRuleIds.push_back(NothrowStatementParser_AssertStatement);
    statementRuleIds.push_back(NothrowStatementParser_ConditionalCompilationStatement);
    classRuleId.push_back(NothrowClassParser_Class);
}

bool Find(const std::vector<int>& ruleContext, const std::vector<int>& ruleIds)
{
    int n = ruleContext.size();
    for (int i = n - 1; i >= 0; --i)
    {
        for (int r : ruleIds)
        {
            if (ruleContext[i] == r) return true;
        }
    }
    return false;
}

std::vector<int> GetLineRuleContext(const std::u32string& cursorLine, const std::string& filePath, int index, const boost::uuids::uuid& moduleId, const std::vector<int>& globalRuleContext)
{
    boost::uuids::uuid mid = moduleId;
    NothrowParsingContext parsingContext;
    CmajorNothrowLexer lexer(cursorLine.c_str(), cursorLine.c_str() + cursorLine.length(), filePath, index);
    lexer.SetFlag(LexerFlags::lcc);
    if (Find(globalRuleContext, SourceData::Instance().CompoundStatementRuleId()))
    {
        std::unique_ptr<sngcm::ast::StatementNode> statement = NothrowStatementParser::Parse(lexer, &mid, &parsingContext);
    }
    else if (Find(globalRuleContext, SourceData::Instance().ClassRuleId()))
    {
        std::unique_ptr<sngcm::ast::Node> member = NothrowClassMemberParser::Parse(lexer, &mid, &parsingContext);
    }
    else if (globalRuleContext.empty() || Find(globalRuleContext, SourceData::Instance().GlobalRuleIds()))
    {
        std::unique_ptr<CompileUnitNode> global = NothrowCompileUnitParser::Parse(lexer, &mid, &parsingContext);
    }
    else
    {
        return globalRuleContext;
    }
    return lexer.CursorRuleContext();
}

// #define DEBUG_CC 1

std::vector<std::string> GetRuleNames(const std::vector<int>& rules)
{
    std::vector<std::string> ruleNames;
    std::vector<const char*>* ruleNameVec = GetRuleNameVecPtr();
    for (int r : rules)
    {
        ruleNames.push_back((*ruleNameVec)[r]);
    }
    return ruleNames;
}

CCContext GetCCContext(const std::u32string& cursorLine, const std::string& filePath, int index, const boost::uuids::uuid& moduleId, const std::vector<int>& globalRuleContext)
{
#ifdef DEBUG_CC
    {
        std::vector<std::string> ruleNames = GetRuleNames(globalRuleContext);
        int x = 0;
    }
#endif
    if (Find(globalRuleContext, SourceData::Instance().CompoundStatementRuleId()))
    {
        std::vector<int> lineRuleContext = GetLineRuleContext(cursorLine, filePath, index, moduleId, globalRuleContext);
#ifdef DEBUG_CC
        {
            std::vector<std::string> ruleNames = GetRuleNames(lineRuleContext);
            int x = 0;
        }
#endif
        if (Find(lineRuleContext, SourceData::Instance().DefininingIdRules()))
        {
            return CCContext::emptyContext;
        }
        if (Find(lineRuleContext, SourceData::Instance().StatementRuleIds()))
        {
            return CCContext::statementContext;
        }
        return CCContext::statementContext;
    }
    if (Find(globalRuleContext, SourceData::Instance().ClassRuleId()))
    {
        std::vector<int> lineRuleContext = GetLineRuleContext(cursorLine, filePath, index, moduleId, globalRuleContext);
#ifdef DEBUG_CC
        {
            std::vector<std::string> ruleNames = GetRuleNames(lineRuleContext);
            int x = 0;
        }
#endif
        if (Find(lineRuleContext, SourceData::Instance().DefininingIdRules()))
        {
            return CCContext::emptyContext;
        }
        return CCContext::classContext;
    }
    if (globalRuleContext.empty() || Find(globalRuleContext, SourceData::Instance().GlobalRuleIds()))
    {
        std::vector<int> lineRuleContext = GetLineRuleContext(cursorLine, filePath, index, moduleId, globalRuleContext);
#ifdef DEBUG_CC
        {
            std::vector<std::string> ruleNames = GetRuleNames(lineRuleContext);
            int x = 0;
        }
#endif
        if (Find(lineRuleContext, SourceData::Instance().DefininingIdRules()))
        {
            return CCContext::emptyContext;
        }
        return CCContext::globalContext;
    }
    return CCContext::genericContext;
}

void AddKeywordSymbolEntry(std::vector<CCSymbolEntry>& keywordEntries, int tokenId, const std::u32string& prefix)
{
    Symbol* symbol = GetKeywordSymbol(tokenId);
    if (symbol)
    {
        if (StartsWith(symbol->Name(), prefix))
        {
            keywordEntries.push_back(CCSymbolEntry(symbol, prefix.length(), symbol->Name()));
        }
    }
}

std::vector<CCSymbolEntry> GetKeywordEntries(CCContext ccContext, const std::u32string& prefix)
{
    std::vector<CCSymbolEntry> entries;
    switch (ccContext)
    {
        case CCContext::globalContext:
        {
            AddKeywordSymbolEntry(entries, ENUM, prefix);
            AddKeywordSymbolEntry(entries, CAST, prefix);
            AddKeywordSymbolEntry(entries, INTERFACE, prefix);
            AddKeywordSymbolEntry(entries, NAMESPACE, prefix);
            AddKeywordSymbolEntry(entries, USING, prefix);
            AddKeywordSymbolEntry(entries, STATIC, prefix);
            AddKeywordSymbolEntry(entries, EXTERN, prefix);
            AddKeywordSymbolEntry(entries, DELEGATE, prefix);
            AddKeywordSymbolEntry(entries, INLINE, prefix);
            AddKeywordSymbolEntry(entries, CDECL, prefix);
            AddKeywordSymbolEntry(entries, NOTHROW, prefix);
            AddKeywordSymbolEntry(entries, PUBLIC, prefix);
            AddKeywordSymbolEntry(entries, PROTECTED, prefix);
            AddKeywordSymbolEntry(entries, PRIVATE, prefix);
            AddKeywordSymbolEntry(entries, INTERNAL, prefix);
            AddKeywordSymbolEntry(entries, ABSTRACT, prefix);
            AddKeywordSymbolEntry(entries, WINAPI, prefix);
            AddKeywordSymbolEntry(entries, OPERATOR, prefix);
            AddKeywordSymbolEntry(entries, CLASS, prefix);
            AddKeywordSymbolEntry(entries, TYPEDEF, prefix);
            AddKeywordSymbolEntry(entries, CONST, prefix);
            AddKeywordSymbolEntry(entries, CONSTEXPR, prefix);
            AddKeywordSymbolEntry(entries, THROW, prefix);
            AddKeywordSymbolEntry(entries, CONCEPT, prefix);
            break;
        }
        case CCContext::classContext:
        {
            AddKeywordSymbolEntry(entries, ENUM, prefix);
            AddKeywordSymbolEntry(entries, CAST, prefix);
            AddKeywordSymbolEntry(entries, INTERFACE, prefix);
            AddKeywordSymbolEntry(entries, STATIC, prefix);
            AddKeywordSymbolEntry(entries, EXTERN, prefix);
            AddKeywordSymbolEntry(entries, DELEGATE, prefix);
            AddKeywordSymbolEntry(entries, INLINE, prefix);
            AddKeywordSymbolEntry(entries, CDECL, prefix);
            AddKeywordSymbolEntry(entries, NOTHROW, prefix);
            AddKeywordSymbolEntry(entries, PUBLIC, prefix);
            AddKeywordSymbolEntry(entries, PROTECTED, prefix);
            AddKeywordSymbolEntry(entries, PRIVATE, prefix);
            AddKeywordSymbolEntry(entries, INTERNAL, prefix);
            AddKeywordSymbolEntry(entries, VIRTUAL, prefix);
            AddKeywordSymbolEntry(entries, ABSTRACT, prefix);
            AddKeywordSymbolEntry(entries, OVERRIDE, prefix);
            AddKeywordSymbolEntry(entries, SUPPRESS, prefix);
            AddKeywordSymbolEntry(entries, WINAPI, prefix);
            AddKeywordSymbolEntry(entries, OPERATOR, prefix);
            AddKeywordSymbolEntry(entries, CLASS, prefix);
            AddKeywordSymbolEntry(entries, TYPEDEF, prefix);
            AddKeywordSymbolEntry(entries, CONST, prefix);
            AddKeywordSymbolEntry(entries, CONSTEXPR, prefix);
            AddKeywordSymbolEntry(entries, THROW, prefix);
            break;
        }
        case CCContext::statementContext:
        {
            AddKeywordSymbolEntry(entries, ASSERT, prefix);
            AddKeywordSymbolEntry(entries, ELIF, prefix);
            AddKeywordSymbolEntry(entries, ENDIF, prefix);
            AddKeywordSymbolEntry(entries, TRUE, prefix);
            AddKeywordSymbolEntry(entries, FALSE, prefix);
            AddKeywordSymbolEntry(entries, CAST, prefix);
            AddKeywordSymbolEntry(entries, AS, prefix);
            AddKeywordSymbolEntry(entries, IS, prefix);
            AddKeywordSymbolEntry(entries, RETURN, prefix);
            AddKeywordSymbolEntry(entries, IF, prefix);
            AddKeywordSymbolEntry(entries, ELSE, prefix);
            AddKeywordSymbolEntry(entries, SWITCH, prefix);
            AddKeywordSymbolEntry(entries, CASE, prefix);
            AddKeywordSymbolEntry(entries, DEFAULT, prefix);
            AddKeywordSymbolEntry(entries, WHILE, prefix);
            AddKeywordSymbolEntry(entries, DO, prefix);
            AddKeywordSymbolEntry(entries, FOR, prefix);
            AddKeywordSymbolEntry(entries, BREAK, prefix);
            AddKeywordSymbolEntry(entries, CONTINUE, prefix);
            AddKeywordSymbolEntry(entries, GOTO, prefix);
            AddKeywordSymbolEntry(entries, TYPEDEF, prefix);
            AddKeywordSymbolEntry(entries, TYPENAME, prefix);
            AddKeywordSymbolEntry(entries, TYPEID, prefix);
            AddKeywordSymbolEntry(entries, CONST, prefix);
            AddKeywordSymbolEntry(entries, NULLLIT, prefix);
            AddKeywordSymbolEntry(entries, THIS, prefix);
            AddKeywordSymbolEntry(entries, BASE, prefix);
            AddKeywordSymbolEntry(entries, CONSTRUCT, prefix);
            AddKeywordSymbolEntry(entries, DESTROY, prefix);
            AddKeywordSymbolEntry(entries, NEW, prefix);
            AddKeywordSymbolEntry(entries, DELETE, prefix);
            AddKeywordSymbolEntry(entries, SIZEOF, prefix);
            AddKeywordSymbolEntry(entries, TRY, prefix);
            AddKeywordSymbolEntry(entries, CATCH, prefix);
            AddKeywordSymbolEntry(entries, THROW, prefix);
            break;
        }
    }
    return entries;
}

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
    lexer.SetFlag(LexerFlags::gcc);
    boost::uuids::uuid mid = moduleId;
    NothrowParsingContext parsingContext;
    std::unique_ptr<CompileUnitNode> parsedCompileUnit = NothrowCompileUnitParser::Parse(lexer, &mid, &parsingContext);
    rc.clear();
    SetRuleContext(lexer.CursorRuleContext());
    std::vector<std::exception> parsingErrors = lexer.Errors();
    if (!parsingErrors.empty())
    {
        CmajorNothrowLexer lexer(Start(), End(), FilePath(), index);
        lexer.SetFlag(LexerFlags::gcc);
        lexer.SetFlag(LexerFlags::synchronize);
        NothrowParsingContext parsingContext;
        parsedCompileUnit = NothrowCompileUnitParser::Parse(lexer, &mid, &parsingContext);
        parsingErrors = lexer.Errors();
        synchronized = lexer.GetFlag(LexerFlags::synchronizedAtLeastOnce);
        SetRuleContext(lexer.CursorRuleContext());
    }
    else
    {
        synchronized = lexer.GetFlag(LexerFlags::synchronizedAtLeastOnce);
    }
    for (const std::exception& ex : parsingErrors)
    {
        errors.push_back(ex.what());
    }
    compileUnit = std::move(parsedCompileUnit);
    SetRuleContext();
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

std::string Source::GetCCList(Module* module, const std::u32string& ccText, const std::u32string& cursorLine, int index, const std::vector<int>& ruleContext)
{
    ccSymbols.clear();
    FunctionSymbol* fromFunction = nullptr;
    if (cursorContainer)
    {
        fromFunction = cursorContainer->FunctionNoThrow();
    }
    CCContext ccContext = GetCCContext(cursorLine, FilePath(), index, module->Id(), ruleContext);
    sngxml::dom::Document ccListDoc;
    sngxml::dom::Element* ccListElement = new sngxml::dom::Element(U"ccList");
    ccListDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(ccListElement));
    if (ccContext != CCContext::emptyContext)
    {
        std::vector<CCSymbolEntry> ccSymbolEntries;
        std::vector<CCSymbolEntry> keywordEntries = GetKeywordEntries(ccContext, ccText);
        AddMatches(ccSymbolEntries, keywordEntries);
        std::vector<CCSymbolEntry> scopeEntries = LookupSymbolsBeginningWith(ccText);
        AddMatches(ccSymbolEntries, scopeEntries);
        std::sort(ccSymbolEntries.begin(), ccSymbolEntries.end(), CCSymbolEntryLess());
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
    }
    std::stringstream s;
    CodeFormatter formatter(s);
    ccListDoc.Write(formatter);
    return s.str();
}

std::string Source::GetParamHelpList(int symbolIndex)
{
    if (symbolIndex >= 0 && symbolIndex < ccSymbols.size())
    {
        Symbol* symbol = ccSymbols[symbolIndex];
        sngxml::dom::Document paramHelpListDoc;
        sngxml::dom::Element* paramHelpListElement = new sngxml::dom::Element(U"paramHelpList");
        paramHelpListDoc.AppendChild(std::unique_ptr<sngxml::dom::Node>(paramHelpListElement));
        std::vector<Symbol*> paramHelpSymbols = symbol->GetParamHelpSymbols();
        for (Symbol* paramHelpSymbol : paramHelpSymbols)
        {
            sngxml::dom::Element* paramHelpElement = new sngxml::dom::Element(U"element");
            paramHelpElement->SetAttribute(U"name", paramHelpSymbol->FullName());
            paramHelpListElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(paramHelpElement));
        }
        std::stringstream s;
        CodeFormatter formatter(s);
        paramHelpListDoc.Write(formatter);
        return s.str();
    }
    else
    {
        throw std::runtime_error("invalid symbol index");
    }
}

void Source::SetRuleContext(const std::vector<int>& rc_)
{
    if (rc.size() < rc_.size())
    {
        rc = rc_;
    }
}

void Source::SetRuleContext()
{
    ruleContext = rc;
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
        result.ruleContext = src->RuleContext();
#ifdef DEBUG_CC
        {
            std::vector<std::string> ruleNames = GetRuleNames(result.ruleContext);
            int x = 0;
        }
#endif 
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

std::string Sources::GetCCList(Module* module, const std::string& sourceFilePath, const std::u32string& ccText, const std::u32string& cursorLine, const std::vector<int>& ruleContext)
{
    int index = GetSourceIndex(sourceFilePath);
    if (index == -1)
    {
        throw std::runtime_error("source file path '" + sourceFilePath + "' not found");
    }
    Source* source = GetSource(index);
    return source->GetCCList(module, ccText, cursorLine, index, ruleContext);
}

std::string Sources::GetParamHelpList(Module* module, const std::string& sourceFilePath, int symbolIndex)
{
    int index = GetSourceIndex(sourceFilePath);
    if (index == -1)
    {
        throw std::runtime_error("source file path '" + sourceFilePath + "' not found");
    }
    Source* source = GetSource(index);
    return source->GetParamHelpList(symbolIndex);
}

void InitSources()
{
    SourceData::Init();
}

void DoneSources()
{
    SourceData::Done();
}

} } // namespace cmajor::symbols
