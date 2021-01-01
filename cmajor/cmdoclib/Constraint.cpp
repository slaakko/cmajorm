// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/Constraint.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/Expression.hpp>

namespace cmajor { namespace cmdoclib {

class ConstraintSymbolBinder : public cmajor::binder::ConstraintSymbolBinder
{
public:
    void BindConstraintSymbols(Node* node, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit) override;
};

ConstraintSymbolBinder constraintSymbolBinder;

void SetConstraintSymbolBinder()
{
    cmajor::binder::SetConstraintSymbolBinder(&constraintSymbolBinder);
}

void ConstraintSymbolBinder::BindConstraintSymbols(Node* node, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit)
{
    cmajor::cmdoclib::BindConstraintSymbols(node, containerScope, boundCompileUnit);
}

class ConstraintBinderVisitor : public Visitor
{
public:
    ConstraintBinderVisitor(ContainerScope* containerScope_, BoundCompileUnit& boundCompileUnit_);
    void Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode) override;
    void Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode) override;
    void Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode) override;
    void Visit(WhereConstraintNode& whereConstraintNode) override;
    void Visit(ConceptNode& conceptNode) override;
    void Visit(ConceptIdNode& conceptIdNode) override;
    void Visit(IsConstraintNode& isConstraintnode) override;
    void Visit(MultiParamConstraintNode& multiParamConstraintNode) override;
    void Visit(IdentifierNode& identifierNode) override;
    void Visit(DotNode& dotNode) override;
    IdentifierNode* PrevId() const { return prevId; }
private:
    ContainerScope* containerScope;
    BoundCompileUnit& boundCompileUnit;
    SymbolTable& symbolTable;
    ConceptGroupSymbol* conceptGroup;
    NamespaceSymbol* ns;
    IdentifierNode* prevId;
};

ConstraintBinderVisitor::ConstraintBinderVisitor(ContainerScope* containerScope_, BoundCompileUnit& boundCompileUnit_) :
    symbolTable(boundCompileUnit_.GetSymbolTable()), containerScope(containerScope_), boundCompileUnit(boundCompileUnit_), conceptGroup(nullptr), ns(nullptr), 
    prevId(boundCompileUnit.GetLatestIdentifier())
{
    boundCompileUnit.SetLatestIdentifier(nullptr);
}

void ConstraintBinderVisitor::Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode)
{
    parenthesizedConstraintNode.Constraint()->Accept(*this);
}

void ConstraintBinderVisitor::Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode)
{
    disjunctiveConstraintNode.Left()->Accept(*this);
    disjunctiveConstraintNode.Right()->Accept(*this);
}

void ConstraintBinderVisitor::Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode)
{
    conjunctiveConstraintNode.Left()->Accept(*this);
    conjunctiveConstraintNode.Right()->Accept(*this);
}

void ConstraintBinderVisitor::Visit(WhereConstraintNode& whereConstraintNode)
{
    whereConstraintNode.Constraint()->Accept(*this);
}

void ConstraintBinderVisitor::Visit(ConceptNode& conceptNode)
{
    if (conceptNode.Refinement())
    {
        conceptNode.Refinement()->Accept(*this);
    }
    int n = conceptNode.Constraints().Count();
    for (int i = 0; i < n; ++i)
    {
        conceptNode.Constraints()[i]->Accept(*this);
    }
}

void ConstraintBinderVisitor::Visit(ConceptIdNode& conceptIdNode)
{
    conceptIdNode.Id()->Accept(*this);
    if (conceptGroup)
    {
        int n = conceptIdNode.Arity();
        ConceptSymbol* conceptSymbol = conceptGroup->GetConcept(n);
        if (conceptSymbol)
        {
            if (boundCompileUnit.GetLatestIdentifier())
            {
                symbolTable.MapSymbol(boundCompileUnit.GetLatestIdentifier(), conceptSymbol);
                MapIdentifierToSymbolDefinition(boundCompileUnit.GetLatestIdentifier(), conceptSymbol);
            }
        }
    }
    int n = conceptIdNode.TypeParameters().Count();
    for (int i = 0; i < n; ++i)
    {
        conceptIdNode.TypeParameters()[i]->Accept(*this);
    }
}

void ConstraintBinderVisitor::Visit(IsConstraintNode& isConstraintNode)
{
    isConstraintNode.TypeExpr()->Accept(*this);
    isConstraintNode.ConceptOrTypeName()->Accept(*this);
    if (conceptGroup)
    {
        ConceptSymbol* conceptSymbol = conceptGroup->GetConcept(1);
        if (conceptSymbol)
        {
            if (boundCompileUnit.GetLatestIdentifier())
            {
                symbolTable.MapSymbol(boundCompileUnit.GetLatestIdentifier(), conceptSymbol);
                MapIdentifierToSymbolDefinition(boundCompileUnit.GetLatestIdentifier(), conceptSymbol);
            }
        }
    }
}

void ConstraintBinderVisitor::Visit(MultiParamConstraintNode& multiParamConstraintNode)
{
    multiParamConstraintNode.ConceptId()->Accept(*this);
    if (conceptGroup)
    {
        int n = multiParamConstraintNode.TypeExprs().Count();
        ConceptSymbol* conceptSymbol = conceptGroup->GetConcept(n);
        if (conceptSymbol)
        {
            if (boundCompileUnit.GetLatestIdentifier())
            {
                symbolTable.MapSymbol(boundCompileUnit.GetLatestIdentifier(), conceptSymbol);
                MapIdentifierToSymbolDefinition(boundCompileUnit.GetLatestIdentifier(), conceptSymbol);
            }
        }
    }
    int n = multiParamConstraintNode.TypeExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        multiParamConstraintNode.TypeExprs()[i]->Accept(*this);
    }
}

void ConstraintBinderVisitor::Visit(IdentifierNode& identifierNode)
{
    ns = nullptr;
    conceptGroup = nullptr;
    boundCompileUnit.SetLatestIdentifier(&identifierNode);
    const std::u32string& name = identifierNode.Str();
    Symbol* symbol = containerScope->Lookup(name, ScopeLookup::this_and_base_and_parent);
    if (!symbol)
    {
        for (const std::unique_ptr<FileScope>& fileScope : boundCompileUnit.FileScopes())
        {
            symbol = fileScope->Lookup(name);
            if (symbol)
            {
                break;
            }
        }
    }
    if (symbol)
    {
        if (symbol->GetSymbolType() == SymbolType::conceptGroupSymbol)
        {
            conceptGroup = static_cast<ConceptGroupSymbol*>(symbol);
        }
        else 
        {
            if (symbol->GetSymbolType() == SymbolType::boundTemplateParameterSymbol)
            {
                BoundTemplateParameterSymbol* boundTemplateParam = static_cast<BoundTemplateParameterSymbol*>(symbol);
                symbol = boundTemplateParam->GetType();
            }
            symbolTable.MapSymbol(&identifierNode, symbol);
            MapIdentifierToSymbolDefinition(&identifierNode, symbol);
        }
    }
}

void ConstraintBinderVisitor::Visit(DotNode& dotNode)
{
    conceptGroup = nullptr;
    ns = nullptr;
    dotNode.Subject()->Accept(*this);
    boundCompileUnit.SetLatestIdentifier(dotNode.MemberId());
    if (ns)
    {
        Symbol* symbol = ns->GetContainerScope()->Lookup(dotNode.MemberId()->Str());
        if (symbol)
        {
            if (symbol->GetSymbolType() == SymbolType::conceptGroupSymbol)
            {
                conceptGroup = static_cast<ConceptGroupSymbol*>(symbol);
            }
            else if (symbol->GetSymbolType() != SymbolType::namespaceSymbol)
            {
                symbolTable.MapSymbol(dotNode.MemberId(), symbol);
            }
        }
    }
}

void BindConstraintSymbols(Node* node, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit)
{
    ConstraintBinderVisitor visitor(containerScope, boundCompileUnit);
    node->Accept(visitor);
    boundCompileUnit.SetLatestIdentifier(visitor.PrevId());
}

} } // namespace cmajor::cmdoclib
