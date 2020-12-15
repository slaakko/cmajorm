// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/FunctionTemplateRepository.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/symbols/SymbolCreatorVisitor.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <soulng/util/Util.hpp>

namespace cmajor { namespace binder {

using namespace soulng::util;
using namespace cmajor::symbols;

bool operator==(const FunctionTemplateKey& left, const FunctionTemplateKey& right)
{
    if (left.functionTemplate != right.functionTemplate) return false;
    if (left.templateArgumentTypes.size() != right.templateArgumentTypes.size()) return false;
    int n = left.templateArgumentTypes.size();
    for (int i = 0; i < n; ++i)
    {
        if (!TypesEqual(left.templateArgumentTypes[i], right.templateArgumentTypes[i])) return false;
    }
    return true;
}

bool operator!=(const FunctionTemplateKey& left, const FunctionTemplateKey& right)
{
    return !(left == right);
}

FunctionTemplateRepository::FunctionTemplateRepository(BoundCompileUnit& boundCompileUnit_) : boundCompileUnit(boundCompileUnit_)
{
}

FunctionSymbol* FunctionTemplateRepository::Instantiate(FunctionSymbol* functionTemplate, const std::unordered_map<TemplateParameterSymbol*, TypeSymbol*>& templateParameterMapping,  
    const Span& span, const boost::uuids::uuid& moduleId)
{
    std::vector<TypeSymbol*> templateArgumentTypes;
    for (TemplateParameterSymbol* templateParameter : functionTemplate->TemplateParameters())
    {
        auto it = templateParameterMapping.find(templateParameter);
        if (it != templateParameterMapping.cend())
        {
            TypeSymbol* templateArgumentType = it->second;
            templateArgumentTypes.push_back(templateArgumentType);
        }
        else
        {
            throw Exception("template parameter type not found", span, moduleId);
        }
    }
    FunctionTemplateKey key(functionTemplate, templateArgumentTypes);
    auto it = functionTemplateMap.find(key);
    if (it != functionTemplateMap.cend())
    {
        return it->second;
    }
    SymbolTable& symbolTable = boundCompileUnit.GetSymbolTable();
    Node* node = symbolTable.GetNodeNoThrow(functionTemplate);
    if (!node)
    {
        node = functionTemplate->GetFunctionNode();
        symbolTable.MapNode(node, functionTemplate);
        Assert(node, "function node not read");
    }
    Assert(node->GetNodeType() == NodeType::functionNode, "function node expected");
    FunctionNode* functionNode = static_cast<FunctionNode*>(node);
    std::unique_ptr<NamespaceNode> globalNs(new NamespaceNode(functionNode->GetSpan(), functionNode->ModuleId(), new IdentifierNode(functionNode->GetSpan(), functionNode->ModuleId(), U"")));
    NamespaceNode* currentNs = globalNs.get();
    CloneContext cloneContext;
    cloneContext.SetInstantiateFunctionNode();
    int n = functionTemplate->UsingNodes().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* usingNode = functionTemplate->UsingNodes()[i];
        globalNs->AddMember(usingNode->Clone(cloneContext));
    }
    bool fileScopeAdded = false;
    if (!functionTemplate->Ns()->IsGlobalNamespace())
    {
        FileScope* primaryFileScope = new FileScope();
        primaryFileScope->AddContainerScope(functionTemplate->Ns()->GetContainerScope());
        boundCompileUnit.AddFileScope(primaryFileScope);
        fileScopeAdded = true;
        std::u32string fullNsName = functionTemplate->Ns()->FullName();
        std::vector<std::u32string> nsComponents = Split(fullNsName, '.');
        for (const std::u32string& nsComponent : nsComponents)
        {
            NamespaceNode* nsNode = new NamespaceNode(functionNode->GetSpan(), functionNode->ModuleId(), new IdentifierNode(functionNode->GetSpan(), functionNode->ModuleId(), nsComponent));
            currentNs->AddMember(nsNode);
            currentNs = nsNode;
        }
    }
    FunctionNode* functionInstanceNode = static_cast<FunctionNode*>(functionNode->Clone(cloneContext));
    currentNs->AddMember(functionInstanceNode);
    symbolTable.SetCurrentCompileUnit(boundCompileUnit.GetCompileUnitNode());
    SymbolCreatorVisitor symbolCreatorVisitor(symbolTable);
    symbolCreatorVisitor.InsertTracer(functionInstanceNode->Body());
    globalNs->Accept(symbolCreatorVisitor);
    Symbol* symbol = symbolTable.GetSymbol(functionInstanceNode);
    Assert(symbol->GetSymbolType() == SymbolType::functionSymbol, "function symbol expected");
    FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(symbol);
    functionSymbol->SetLinkOnceOdrLinkage();
    functionSymbol->SetTemplateSpecialization();
    functionSymbol->SetFunctionTemplate(functionTemplate);
    functionSymbol->SetTemplateArgumentTypes(templateArgumentTypes);
    functionTemplateMap[key] = functionSymbol;
    for (TemplateParameterSymbol* templateParameter : functionTemplate->TemplateParameters())
    {
        auto it = templateParameterMapping.find(templateParameter);
        if (it != templateParameterMapping.cend())
        {
            TypeSymbol* boundType = it->second;
            BoundTemplateParameterSymbol* boundTemplateParameter = new BoundTemplateParameterSymbol(span, moduleId, templateParameter->Name());
            boundTemplateParameter->SetType(boundType);
            functionSymbol->AddMember(boundTemplateParameter);
        }
        else
        {
            throw Exception("template parameter type not found", span, moduleId);
        }
    }
    TypeBinder typeBinder(boundCompileUnit);
    globalNs->Accept(typeBinder);
    StatementBinder statementBinder(boundCompileUnit);
    globalNs->Accept(statementBinder);
    if (fileScopeAdded)
    {
        boundCompileUnit.RemoveLastFileScope();
    }
    boundCompileUnit.AddGlobalNs(std::move(globalNs));
    functionSymbol->SetFlag(FunctionSymbolFlags::dontReuse);
    if (functionTemplate->IsSystemDefault())
    {
        functionSymbol->SetSystemDefault();
    }
    boundCompileUnit.SetCanReuse(functionSymbol);
    return functionSymbol;
}

} } // namespace cmajor::binder
