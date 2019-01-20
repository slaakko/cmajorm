// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/Concept.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/binder/BoundConstraint.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/OverloadResolution.hpp>
#include <cmajor/binder/Evaluator.hpp>
#include <cmajor/symbols/TypedefSymbol.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>
#include <cmajor/ast/Visitor.hpp>
#include <cmajor/ast/Identifier.hpp>
#include <cmajor/ast/Expression.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::unicode;

class ConceptIdResolver : public Visitor
{
public:
    ConceptIdResolver(BoundCompileUnit& boundCompileUnit_, ContainerScope* containerScope_);
    ConceptSymbol* GetConcept() const { return conceptSymbol; }
    void Visit(ConceptIdNode& conceptIdNode) override;
    void Visit(IdentifierNode& identifierNode) override;
    void Visit(DotNode& dotNode) override;
private:
    BoundCompileUnit& boundCompileUnit;
    ContainerScope* containerScope;
    ConceptGroupSymbol* conceptGroup;
    ConceptSymbol* conceptSymbol;
    NamespaceSymbol* ns;
};

ConceptIdResolver::ConceptIdResolver(BoundCompileUnit& boundCompileUnit_, ContainerScope* containerScope_) : 
    boundCompileUnit(boundCompileUnit_), containerScope(containerScope_), conceptGroup(nullptr), conceptSymbol(nullptr), ns(nullptr)
{
}

void ConceptIdResolver::Visit(ConceptIdNode& conceptIdNode)
{
    conceptIdNode.Id()->Accept(*this);
    if (conceptGroup)
    {
        int arity = conceptIdNode.Arity();
        conceptSymbol = conceptGroup->GetConcept(arity);
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "concept symbol '" + conceptIdNode.Id()->ToString() + "' not found", conceptIdNode.GetSpan());
    }
}

void ConceptIdResolver::Visit(IdentifierNode& identifierNode)
{
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
        else if (symbol->GetSymbolType() == SymbolType::namespaceSymbol)
        {
            ns = static_cast<NamespaceSymbol*>(symbol);
        }
        else
        {
            throw Exception(&boundCompileUnit.GetModule(), "symbol '" + ToUtf8(name) + "' does not denote a concept", identifierNode.GetSpan());
        }
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "concept symbol '" + ToUtf8(name) + "' not found", identifierNode.GetSpan());
    }
}

void ConceptIdResolver::Visit(DotNode& dotNode)
{
    dotNode.Subject()->Accept(*this);
    if (!ns)
    {
        throw Exception(&boundCompileUnit.GetModule(), "concept symbol '" + dotNode.ToString() + "' not found", dotNode.GetSpan());
    }
    ContainerScope* containerScope = ns->GetContainerScope();
    const std::u32string& name = dotNode.MemberId()->Str();
    Symbol* symbol = containerScope->Lookup(name, ScopeLookup::this_);
    if (symbol)
    {
        if (symbol->GetSymbolType() == SymbolType::conceptGroupSymbol)
        {
            conceptGroup = static_cast<ConceptGroupSymbol*>(symbol);
        }
        else if (symbol->GetSymbolType() == SymbolType::namespaceSymbol)
        {
            ns = static_cast<NamespaceSymbol*>(symbol);
        }
        else
        {
            throw Exception(&boundCompileUnit.GetModule(), "symbol '" + ToUtf8(name) + "' does not denote a concept", dotNode.GetSpan());
        }
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "concept symbol '" + ToUtf8(name) + "' not found", dotNode.GetSpan());
    }
}

ConceptSymbol* ResolveConceptId(ConceptIdNode* conceptIdNode, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope)
{
    ConceptIdResolver conceptIdResolver(boundCompileUnit, containerScope);
    conceptIdNode->Accept(conceptIdResolver);
    ConceptSymbol* conceptSymbol = conceptIdResolver.GetConcept();
    if (conceptSymbol)
    {
        return conceptSymbol;
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "concept symbol '" + conceptIdNode->Id()->ToString() + "' not found", conceptIdNode->GetSpan());
    }
}

std::unique_ptr<BoundConcept> Instantiate(ConceptSymbol* conceptSymbol, const std::vector<TypeSymbol*>& typeArguments, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, 
    BoundFunction* currentFunction, std::unique_ptr<BoundConstraint>& boundConstraint, const Span& span, std::unique_ptr<Exception>& exception);

class ConstraintChecker : public Visitor
{
public:
    ConstraintChecker(TypeSymbol* firstTypeArgument_, TypeSymbol* secondTypeArgument_, BoundCompileUnit& boundCompileUnit_, ContainerScope* containerScope_, BoundFunction* currentFunction_, 
        const Span& span_, std::unique_ptr<Exception>& exception_);
    ~ConstraintChecker();
    bool Result() const { return result; }
    std::unique_ptr<BoundConstraint> GetBoundConstraint() { return std::move(boundConstraint); }
    void Visit(BoolNode& boolNode) override;
    void Visit(SByteNode& sbyteNode) override;
    void Visit(ByteNode& byteNode) override;
    void Visit(ShortNode& shortNode) override;
    void Visit(UShortNode& ushortNode) override;
    void Visit(IntNode& intNode) override;
    void Visit(UIntNode& uintNode) override;
    void Visit(LongNode& longNode) override;
    void Visit(ULongNode& ulongNode) override;
    void Visit(FloatNode& floatNode) override;
    void Visit(DoubleNode& doubleNode) override;
    void Visit(CharNode& charNode) override;
    void Visit(WCharNode& wcharNode) override;
    void Visit(UCharNode& ucharNode) override;
    void Visit(VoidNode& voidNode) override;
    void Visit(ConstNode& constNode) override;
    void Visit(LValueRefNode& lvalueRefNode) override;
    void Visit(RValueRefNode& rvalueRefNode) override;
    void Visit(PointerNode& pointerNode) override;
    void Visit(ArrayNode& arrayNode) override;
    void Visit(IdentifierNode& identifierNode) override;
    void Visit(TemplateIdNode& templateIdNode) override;
    void Visit(DotNode& dotNode) override;

    void Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode) override;
    void Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode) override;
    void Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode) override;
    void Visit(WhereConstraintNode& whereConstraintNode) override;
    void Visit(PredicateConstraintNode& predicateConstraintNode) override;
    void Visit(IsConstraintNode& isConstraintNode) override;
    void Visit(MultiParamConstraintNode& multiParamConstraintNode) override;
    void Visit(TypeNameConstraintNode& typeNameConstraintNode) override;
    void Visit(ConstructorConstraintNode& constructorConstraintNode) override;
    void Visit(DestructorConstraintNode& destructorConstraintNode) override;
    void Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode) override;
    void Visit(FunctionConstraintNode& functionConstraintNode) override;
    void Visit(ConceptIdNode& conceptIdNode) override;
    void Visit(ConceptNode& conceptNode) override;

    void Visit(SameConstraintNode& sameConstraintNode) override;
    void Visit(DerivedConstraintNode& derivedConstraintNode) override;
    void Visit(ConvertibleConstraintNode& convertibleConstraintNode) override;
    void Visit(ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode) override;
    void Visit(CommonConstraintNode& commonConstraintNode) override;
    void Visit(NonreferenceTypeConstraintNode& nonreferenceTypeConstraintNode) override;
private:
    TypeSymbol* firstTypeArgument;
    TypeSymbol* secondTypeArgument;
    BoundCompileUnit& boundCompileUnit;
    SymbolTable& symbolTable;
    ContainerScope* containerScope;
    BoundFunction* currentFunction;
    Span span;
    TypeSymbol* type;
    TypeDerivationRec derivationRec;
    ConceptGroupSymbol* conceptGroup;
    bool result;
    std::unique_ptr<BoundConstraint> boundConstraint;
    int fileScopesAdded;
    std::vector<std::unique_ptr<NamespaceTypeSymbol>> namespaceTypeSymbols;
    std::vector<std::unique_ptr<BoundTemplateParameterSymbol>> boundTemplateParameters;
    std::unique_ptr<Exception>& exception;
    void Reset();
    TypeSymbol* GetType();
};

ConstraintChecker::ConstraintChecker(TypeSymbol* firstTypeArgument_, TypeSymbol* secondTypeArgument_, BoundCompileUnit& boundCompileUnit_, ContainerScope* containerScope_,
    BoundFunction* currentFunction_, const Span& span_, std::unique_ptr<Exception>& exception_) :
    firstTypeArgument(firstTypeArgument_), secondTypeArgument(secondTypeArgument_), boundCompileUnit(boundCompileUnit_), symbolTable(boundCompileUnit.GetSymbolTable()),
    containerScope(containerScope_), currentFunction(currentFunction_), span(span_), type(nullptr), derivationRec(), conceptGroup(nullptr), result(false), boundConstraint(), fileScopesAdded(0),
    exception(exception_)
{
}

ConstraintChecker::~ConstraintChecker()
{
    for (int i = 0; i < fileScopesAdded; ++i)
    {
        boundCompileUnit.RemoveLastFileScope();
    }
}

void ConstraintChecker::Reset()
{
    type = nullptr;
    derivationRec = TypeDerivationRec();
    conceptGroup = nullptr;
}

TypeSymbol* ConstraintChecker::GetType()
{
    if (type && type->GetSymbolType() == SymbolType::classGroupTypeSymbol)
    {
        ClassGroupTypeSymbol* classGroup = static_cast<ClassGroupTypeSymbol*>(type);
        type = classGroup->GetClass(0);
    }
    if (type && !derivationRec.IsEmpty())
    {
        TypeDerivationRec unifiedDerivationRec = UnifyDerivations(derivationRec, type->DerivationRec());
        if (!unifiedDerivationRec.derivations.empty())
        {
            type = boundCompileUnit.GetSymbolTable().MakeDerivedType(type->BaseType(), unifiedDerivationRec, span);
        }
    }
    return type;
}

void ConstraintChecker::Visit(BoolNode& boolNode)
{
    type = symbolTable.GetTypeByName(U"bool");
}

void ConstraintChecker::Visit(SByteNode& sbyteNode)
{
    type = symbolTable.GetTypeByName(U"sbyte");
}

void ConstraintChecker::Visit(ByteNode& byteNode)
{
    type = symbolTable.GetTypeByName(U"byte");
}

void ConstraintChecker::Visit(ShortNode& shortNode)
{
    type = symbolTable.GetTypeByName(U"short");
}

void ConstraintChecker::Visit(UShortNode& ushortNode)
{
    type = symbolTable.GetTypeByName(U"ushort");
}

void ConstraintChecker::Visit(IntNode& intNode)
{
    type = symbolTable.GetTypeByName(U"int");
}

void ConstraintChecker::Visit(UIntNode& uintNode)
{
    type = symbolTable.GetTypeByName(U"uint");
}

void ConstraintChecker::Visit(LongNode& longNode)
{
    type = symbolTable.GetTypeByName(U"long");
}

void ConstraintChecker::Visit(ULongNode& ulongNode)
{
    type = symbolTable.GetTypeByName(U"ulong");
}

void ConstraintChecker::Visit(FloatNode& floatNode)
{
    type = symbolTable.GetTypeByName(U"float");
}

void ConstraintChecker::Visit(DoubleNode& doubleNode)
{
    type = symbolTable.GetTypeByName(U"double");
}

void ConstraintChecker::Visit(CharNode& charNode)
{
    type = symbolTable.GetTypeByName(U"char");
}

void ConstraintChecker::Visit(WCharNode& wcharNode)
{
    type = symbolTable.GetTypeByName(U"wchar");
}

void ConstraintChecker::Visit(UCharNode& ucharNode)
{
    type = symbolTable.GetTypeByName(U"uchar");
}

void ConstraintChecker::Visit(VoidNode& voidNode)
{
    type = symbolTable.GetTypeByName(U"void");
}

void ConstraintChecker::Visit(ConstNode& constNode)
{
    derivationRec.derivations.push_back(Derivation::constDerivation);
    constNode.Subject()->Accept(*this);
}

void ConstraintChecker::Visit(LValueRefNode& lvalueRefNode)
{
    lvalueRefNode.Subject()->Accept(*this);
    if (HasReferenceDerivation(derivationRec.derivations))
    {
        throw Exception(&boundCompileUnit.GetModule(), "cannot have reference to reference type", lvalueRefNode.GetSpan());
    }
    derivationRec.derivations.push_back(Derivation::lvalueRefDerivation);
}

void ConstraintChecker::Visit(RValueRefNode& rvalueRefNode)
{
    rvalueRefNode.Subject()->Accept(*this);
    if (HasReferenceDerivation(derivationRec.derivations))
    {
        throw Exception(&boundCompileUnit.GetModule(), "cannot have reference to reference type", rvalueRefNode.GetSpan());
    }
    derivationRec.derivations.push_back(Derivation::rvalueRefDerivation);
}

void ConstraintChecker::Visit(PointerNode& pointerNode)
{
    pointerNode.Subject()->Accept(*this);
    if (HasReferenceDerivation(derivationRec.derivations))
    {
        throw Exception(&boundCompileUnit.GetModule(), "cannot have pointer to reference type", pointerNode.GetSpan());
    }
    derivationRec.derivations.push_back(Derivation::pointerDerivation);
}

void ConstraintChecker::Visit(ArrayNode& arrayNode)
{
    arrayNode.Subject()->Accept(*this);
    if (HasReferenceDerivation(derivationRec.derivations))
    {
        throw Exception(&boundCompileUnit.GetModule(), "cannot have array of reference type", arrayNode.GetSpan());
    }
    // todo: evaluate size
}

void ConstraintChecker::Visit(IdentifierNode& identifierNode)
{
    Reset();
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
        if (symbol->IsTypeSymbol())
        {
            type = static_cast<TypeSymbol*>(symbol);
        }
        else 
        {
            switch (symbol->GetSymbolType())
            {
                case SymbolType::typedefSymbol:
                {
                    TypedefSymbol* typedefSymbol = static_cast<TypedefSymbol*>(symbol);
                    type = typedefSymbol->GetType();
                    break;
                }
                case SymbolType::boundTemplateParameterSymbol:
                {
                    BoundTemplateParameterSymbol* boundTemplateParameterSymbol = static_cast<BoundTemplateParameterSymbol*>(symbol);
                    type = boundTemplateParameterSymbol->GetType();
                    break;
                }
                case SymbolType::conceptGroupSymbol:
                {
                    conceptGroup = static_cast<ConceptGroupSymbol*>(symbol);
                    break;
                }
                case SymbolType::namespaceSymbol:
                {
                    NamespaceSymbol* ns = static_cast<NamespaceSymbol*>(symbol);
                    NamespaceTypeSymbol* namespaceTypeSymbol = new NamespaceTypeSymbol(ns);
                    boundCompileUnit.GetSymbolTable().SetTypeIdFor(namespaceTypeSymbol);
                    namespaceTypeSymbols.push_back(std::unique_ptr<NamespaceTypeSymbol>(namespaceTypeSymbol));
                    type = namespaceTypeSymbol;
                    FileScope* fileScope = new FileScope(&boundCompileUnit.GetModule());
                    NamespaceImportNode importNode(span, new IdentifierNode(span, ns->FullName()));
                    fileScope->InstallNamespaceImport(containerScope, &importNode);
                    boundCompileUnit.AddFileScope(fileScope);
                    ++fileScopesAdded;
                    break;
                }
                default:
                {
                    throw Exception(&boundCompileUnit.GetModule(), "symbol '" + ToUtf8(symbol->FullName()) + "' does not denote a type or a concept", symbol->GetSpan());
                }
            }
        }
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "type or concept symbol '" + ToUtf8(name) + "' not found", identifierNode.GetSpan());
    }
}

void ConstraintChecker::Visit(TemplateIdNode& templateIdNode)
{
    type = ResolveType(&templateIdNode, boundCompileUnit, containerScope);
}

void ConstraintChecker::Visit(DotNode& dotNode)
{
    Reset();
    dotNode.Subject()->Accept(*this);
    TypeSymbol* subjectType = GetType();
    if (!subjectType)
    {
        throw Exception(&boundCompileUnit.GetModule(), "symbol '" + dotNode.Subject()->ToString() + "' does not denote a type", dotNode.Subject()->GetSpan());
    }
    Scope* typeContainerScope = nullptr;
    if (subjectType->IsPointerType())
    {
        typeContainerScope = subjectType->GetContainerScope();
    }
    else
    {
        typeContainerScope = subjectType->BaseType()->GetContainerScope();
    }
    if (subjectType->GetSymbolType() == SymbolType::namespaceTypeSymbol)
    {
        NamespaceTypeSymbol* nsTypeSymbol = static_cast<NamespaceTypeSymbol*>(subjectType);
        typeContainerScope = nsTypeSymbol->Ns()->GetContainerScope();
    }
    Reset();
    const std::u32string& name = dotNode.MemberId()->Str();
    Symbol* symbol = typeContainerScope->Lookup(name, ScopeLookup::this_and_base);
    if (symbol)
    {
        switch (symbol->GetSymbolType())
        {
            case SymbolType::typedefSymbol:
            {
                TypedefSymbol* typedefSymbol = static_cast<TypedefSymbol*>(symbol);
                type = typedefSymbol->GetType();
                break;
            }
            case SymbolType::boundTemplateParameterSymbol:
            {
                BoundTemplateParameterSymbol* boundTemplateParameterSymbol = static_cast<BoundTemplateParameterSymbol*>(symbol);
                type = boundTemplateParameterSymbol->GetType();
                break;
            }
            case SymbolType::conceptGroupSymbol:
            {
                conceptGroup = static_cast<ConceptGroupSymbol*>(symbol);
                break;
            }
            case SymbolType::namespaceSymbol:
            {
                NamespaceSymbol* ns = static_cast<NamespaceSymbol*>(symbol);
                NamespaceTypeSymbol* namespaceTypeSymbol = new NamespaceTypeSymbol(ns);
                boundCompileUnit.GetSymbolTable().SetTypeIdFor(namespaceTypeSymbol);
                namespaceTypeSymbols.push_back(std::unique_ptr<NamespaceTypeSymbol>(namespaceTypeSymbol));
                type = namespaceTypeSymbol;
                FileScope* fileScope = new FileScope(&boundCompileUnit.GetModule());
                NamespaceImportNode importNode(span, new IdentifierNode(span, ns->FullName()));
                fileScope->InstallNamespaceImport(containerScope, &importNode);
                boundCompileUnit.AddFileScope(fileScope);
                ++fileScopesAdded;
                break;
            }
            default:
            {
                throw Exception(&boundCompileUnit.GetModule(), "symbol '" + ToUtf8(symbol->FullName()) + "' does not denote a type or a concept", symbol->GetSpan());
            }
        }
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "type or concept symbol '" + ToUtf8(name) + "' not found", dotNode.GetSpan());
    }
}

void ConstraintChecker::Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode)
{
    Reset();
    parenthesizedConstraintNode.Constraint()->Accept(*this);
}

void ConstraintChecker::Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode)
{
    bool exceptionWasSet = exception != nullptr;
    bool left = false;
    bool right = false;
    std::unique_ptr<BoundConstraint> leftBoundConstraint;
    std::unique_ptr<BoundConstraint> rightBoundConstraint;
    Reset();
    try
    {
        disjunctiveConstraintNode.Left()->Accept(*this);
        left = result;
        leftBoundConstraint = std::move(boundConstraint);
    }
    catch (const Exception& ex)
    {
        left = false;
        leftBoundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
        if (!exception)
        {
            exception.reset(new Exception(ex));
        }
    }
    catch (...)
    {
        left = false;
        leftBoundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
    }
    Reset();
    try
    {
        disjunctiveConstraintNode.Right()->Accept(*this);
        right = result;
        rightBoundConstraint = std::move(boundConstraint);
    }
    catch (const Exception& ex)
    {
        right = false;
        rightBoundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
        if (!exception)
        {
            exception.reset(new Exception(ex));
        }
    }
    catch (...)
    {
        right = false;
        rightBoundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
    }
    result = left || right;
    boundConstraint.reset(new BoundDisjunctiveConstraint(&boundCompileUnit.GetModule(), span, leftBoundConstraint.release(), rightBoundConstraint.release()));
    if (result && !exceptionWasSet)
    {
        exception.reset();
    }
}

void ConstraintChecker::Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode)
{
    bool left = false;
    bool right = false;
    std::unique_ptr<BoundConstraint> leftBoundConstraint;
    std::unique_ptr<BoundConstraint> rightBoundConstraint;
    Reset();
    try
    {
        conjunctiveConstraintNode.Left()->Accept(*this);
        left = result;
        leftBoundConstraint = std::move(boundConstraint);
    }
    catch (const Exception& ex)
    {
        left = false;
        leftBoundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
        if (!exception)
        {
            exception.reset(new Exception(ex));
        }
    }
    catch (...)
    {
        left = false;
        leftBoundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
    }
    Reset();
    try
    {
        conjunctiveConstraintNode.Right()->Accept(*this);
        right = result;
        rightBoundConstraint = std::move(boundConstraint);
    }
    catch (const Exception& ex)
    {
        right = false;
        rightBoundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
        if (!exception)
        {
            exception.reset(new Exception(ex));
        }
    }
    catch (...)
    {
        right = false;
        rightBoundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
    }
    result = left && right;
    boundConstraint.reset(new BoundConjunctiveConstraint(&boundCompileUnit.GetModule(), span, leftBoundConstraint.release(), rightBoundConstraint.release()));
}

void ConstraintChecker::Visit(WhereConstraintNode& whereConstraintNode)
{
    Reset();
    whereConstraintNode.Constraint()->Accept(*this);
}

void ConstraintChecker::Visit(PredicateConstraintNode& predicateConstraintNode)
{
    Reset();
    Node* invokeExprNode = predicateConstraintNode.InvokeExpr();
    std::unique_ptr<Value> evaluationResult = Evaluate(invokeExprNode, symbolTable.GetTypeByName(U"bool"), containerScope, boundCompileUnit, false, currentFunction, predicateConstraintNode.GetSpan());
    BoolValue* boolResult = static_cast<BoolValue*>(evaluationResult.get());
    result = boolResult->GetValue();
    boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), predicateConstraintNode.GetSpan(), result));
}

void ConstraintChecker::Visit(IsConstraintNode& isConstraintNode)
{
    Reset();
    isConstraintNode.TypeExpr()->Accept(*this);
    TypeSymbol* leftType = GetType();
    if (!leftType)
    {
        throw Exception(&boundCompileUnit.GetModule(), "left operand of 'is' must be a type", isConstraintNode.TypeExpr()->GetSpan());
    }
    Reset();
    isConstraintNode.ConceptOrTypeName()->Accept(*this);
    TypeSymbol* rightType = GetType();
    if (rightType)
    {
        TypeSymbol* leftPlainType = leftType->PlainType(span);
        TypeSymbol* rightPlainType = rightType->PlainType(span);
        if (TypesEqual(leftPlainType, rightPlainType))
        {
            result = true;
            boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
        }
        else
        {
            result = false;
            boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
        }
    }
    else if (conceptGroup)
    {
        ConceptSymbol* conceptSymbol = conceptGroup->GetConcept(1);
        std::vector<TypeSymbol*> typeArguments;
        typeArguments.push_back(leftType);
        BoundConceptKey key(conceptSymbol, typeArguments);
        BoundConcept* boundConcept = boundCompileUnit.GetConceptRepository().GetBoundConcept(key);
        if (boundConcept)
        {
            result = true;
            boundConstraint.reset(boundConcept->GetBoundConstraint()->Clone());
            if (boundConcept->CommonType())
            {
                BoundTemplateParameterSymbol* boundCommonTypeSymbol = new BoundTemplateParameterSymbol(span, U"CommonType");
                boundCommonTypeSymbol->SetType(boundConcept->CommonType());
                containerScope->Install(boundCommonTypeSymbol);
                boundConcept->AddBoundTemplateParameter(std::unique_ptr<BoundTemplateParameterSymbol>(boundCommonTypeSymbol));
            }
        }
        else
        {
            std::unique_ptr<BoundConstraint> constraint;
            std::unique_ptr<BoundConcept> boundConcept = Instantiate(conceptSymbol, typeArguments, boundCompileUnit, containerScope, currentFunction, constraint, span, exception);
            if (boundConcept)
            {
                result = true;
                boundConstraint.reset(constraint.release());
                boundCompileUnit.GetConceptRepository().AddBoundConcept(key, std::move(boundConcept));
            }
            else
            {
                result = false;
                boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
            }
        }
    }
}

void ConstraintChecker::Visit(MultiParamConstraintNode& multiParamConstraintNode)
{
    Reset();
    multiParamConstraintNode.ConceptId()->Accept(*this);
    if (conceptGroup)
    {
        int n = multiParamConstraintNode.TypeExprs().Count();
        ConceptSymbol* conceptSymbol = conceptGroup->GetConcept(n);
        std::vector<TypeSymbol*> typeArguments;
        for (int i = 0; i < n; ++i)
        {
            Node* typeExprNode = multiParamConstraintNode.TypeExprs()[i];
            Reset();
            typeExprNode->Accept(*this);
            TypeSymbol* resolvedType = GetType();
            if (resolvedType)
            {
                typeArguments.push_back(resolvedType);
            }
            else
            {
                throw Exception(&boundCompileUnit.GetModule(), "type parameter '" + typeExprNode->ToString() + "' is not bound to a type", typeExprNode->GetSpan());
            }
        }
        BoundConceptKey key(conceptSymbol, typeArguments);
        BoundConcept* boundConcept = boundCompileUnit.GetConceptRepository().GetBoundConcept(key);
        if (boundConcept)
        {
            result = true;
            boundConstraint.reset(boundConcept->GetBoundConstraint()->Clone());
            if (boundConcept->CommonType())
            {
                BoundTemplateParameterSymbol* boundCommonTypeSymbol = new BoundTemplateParameterSymbol(span, U"CommonType");
                boundCommonTypeSymbol->SetType(boundConcept->CommonType());
                containerScope->Install(boundCommonTypeSymbol);
                boundConcept->AddBoundTemplateParameter(std::unique_ptr<BoundTemplateParameterSymbol>(boundCommonTypeSymbol));
            }
        }
        else
        {
            std::unique_ptr<BoundConstraint> constraint;
            std::unique_ptr<BoundConcept> boundConcept = Instantiate(conceptSymbol, typeArguments, boundCompileUnit, containerScope, currentFunction, constraint, span, exception);
            if (boundConcept)
            {
                result = true;
                boundConstraint.reset(constraint.release());
                boundCompileUnit.GetConceptRepository().AddBoundConcept(key, std::move(boundConcept));
            }
            else
            {
                result = false;
                boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
            }
        }
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "symbol '" + multiParamConstraintNode.ConceptId()->ToString() + "' does not denote a concept", multiParamConstraintNode.ConceptId()->GetSpan());
    }
}

void ConstraintChecker::Visit(TypeNameConstraintNode& typeNameConstraintNode)
{
    Reset();
    typeNameConstraintNode.TypeId()->Accept(*this);
    TypeSymbol* resolvedType = GetType();
    result = resolvedType != nullptr;
    boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, result));
}

void ConstraintChecker::Visit(ConstructorConstraintNode& constructorConstraintNode)
{
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    arguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(&boundCompileUnit.GetModule(), span, firstTypeArgument->AddPointer(span))));
    std::vector<TypeSymbol*> parameterTypes;
    int n = constructorConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = constructorConstraintNode.Parameters()[i];
        TypeSymbol* parameterType = ResolveType(parameterNode->TypeExpr(), boundCompileUnit, containerScope);
        parameterTypes.push_back(parameterType);
        arguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(&boundCompileUnit.GetModule(), span, parameterType)));
    }
    std::vector<FunctionScopeLookup> lookups;
    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, firstTypeArgument->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
    std::vector<TypeSymbol*> templateArgumentTypes;
    std::unique_ptr<Exception> exception;
    std::unique_ptr<BoundFunctionCall> constructorCall = ResolveOverload(U"@constructor", containerScope, lookups, arguments, boundCompileUnit, currentFunction, span,
        OverloadResolutionFlags::dontInstantiate | OverloadResolutionFlags::dontThrow, templateArgumentTypes, exception);
    if (!constructorCall)
    {
        std::string signature;
        signature.append(ToUtf8(firstTypeArgument->FullName()) + "." + ToUtf8(firstTypeArgument->Name()).append(1, '('));
        bool first = true;
        for (TypeSymbol* parameterType : parameterTypes)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                signature.append(", ");
            }
            signature.append(ToUtf8(parameterType->FullName()));
        }
        signature.append(1, ')');
        std::string message = "constructor signature '" + signature + "' not found";
        std::vector<Span> references;
        if (exception)
        {
            message.append(": ").append(exception->Message());
            references.push_back(exception->Defined());
            references.insert(references.end(), exception->References().begin(), exception->References().end());
        }
        throw Exception(&boundCompileUnit.GetModule(), message, span, references);
    }
    else
    {
        result = true;
        boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
    }
}

void ConstraintChecker::Visit(DestructorConstraintNode& destructorConstraintNode)
{
    result = true;
    boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
}

void ConstraintChecker::Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode)
{
    Reset();
    memberFunctionConstraintNode.TypeParamId()->Accept(*this);
    TypeSymbol* firstType = GetType();
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    arguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(&boundCompileUnit.GetModule(), span, firstType->AddPointer(span))));
    std::vector<TypeSymbol*> parameterTypes;
    int n = memberFunctionConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = memberFunctionConstraintNode.Parameters()[i];
        TypeSymbol* parameterType = ResolveType(parameterNode->TypeExpr(), boundCompileUnit, containerScope);
        parameterTypes.push_back(parameterType);
        arguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(&boundCompileUnit.GetModule(), span, parameterType)));
    }
    std::vector<FunctionScopeLookup> lookups;
    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, firstType->BaseType()->ClassInterfaceOrNsScope()));
    std::vector<TypeSymbol*> templateArgumentTypes;
    std::unique_ptr<Exception> exception;
    std::unique_ptr<BoundFunctionCall> memberFunctionCall = ResolveOverload(memberFunctionConstraintNode.GroupId(), containerScope, lookups, arguments, boundCompileUnit, currentFunction, span,
        OverloadResolutionFlags::dontInstantiate | OverloadResolutionFlags::dontThrow | OverloadResolutionFlags::noTemplates, templateArgumentTypes, exception);
    if (!memberFunctionCall)
    {
        std::string signature;
        signature.append(ToUtf8(firstType->FullName()) + "." + ToUtf8(memberFunctionConstraintNode.GroupId()).append(1, '('));
        bool first = true;
        for (TypeSymbol* parameterType : parameterTypes)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                signature.append(", ");
            }
            signature.append(ToUtf8(parameterType->FullName()));
        }
        signature.append(1, ')');
        std::string message = "member function signature '" + signature + "' not found";
        std::vector<Span> references;
        if (exception)
        {
            message.append(": ").append(exception->Message());
            references.push_back(exception->Defined());
            references.insert(references.end(), exception->References().begin(), exception->References().end());
        }
        throw Exception(&boundCompileUnit.GetModule(), message, span, references);
    }
    else
    {
        result = true;
        boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
    }
}

void ConstraintChecker::Visit(FunctionConstraintNode& functionConstraintNode)
{
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    arguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(&boundCompileUnit.GetModule(), span, firstTypeArgument->AddPointer(span))));
    std::vector<TypeSymbol*> parameterTypes;
    std::vector<FunctionScopeLookup> lookups;
    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    lookups.push_back(FunctionScopeLookup(ScopeLookup::this_, firstTypeArgument->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
    int n = functionConstraintNode.Parameters().Count();
    if (firstTypeArgument->IsPointerType() &&
        ((n == 0 &&
            (functionConstraintNode.GroupId() == U"operator*" ||
            functionConstraintNode.GroupId() == U"operator++" ||
            functionConstraintNode.GroupId() == U"operator--")) ||
        (n == 1 &&
            functionConstraintNode.GroupId() == U"operator[]")))
    {
        result = true;
        boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
        return;
    }
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameterNode = functionConstraintNode.Parameters()[i];
        TypeSymbol* parameterType = ResolveType(parameterNode->TypeExpr(), boundCompileUnit, containerScope);
        lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
        parameterTypes.push_back(parameterType);
        arguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(&boundCompileUnit.GetModule(), span, parameterType)));
    }
    std::vector<TypeSymbol*> templateArgumentTypes;
    std::unique_ptr<Exception> exception;
    std::unique_ptr<BoundFunctionCall> functionCall = ResolveOverload(functionConstraintNode.GroupId(), containerScope, lookups, arguments, boundCompileUnit, currentFunction, span,
        OverloadResolutionFlags::dontInstantiate | OverloadResolutionFlags::dontThrow, templateArgumentTypes, exception);
    if (!functionCall)
    {
        arguments.clear();
        parameterTypes.clear();
        arguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(&boundCompileUnit.GetModule(), span, firstTypeArgument->AddPointer(span))));
        std::vector<TypeSymbol*> parameterTypes;
        int n = functionConstraintNode.Parameters().Count();
        for (int i = 1; i < n; ++i)
        {
            ParameterNode* parameterNode = functionConstraintNode.Parameters()[i];
            TypeSymbol* parameterType = ResolveType(parameterNode->TypeExpr(), boundCompileUnit, containerScope);
            parameterTypes.push_back(parameterType);
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(&boundCompileUnit.GetModule(), span, parameterType)));
        }
        std::vector<TypeSymbol*> templateArgumentTypes;
        std::unique_ptr<Exception> exception;
        std::unique_ptr<BoundFunctionCall> functionCall = ResolveOverload(functionConstraintNode.GroupId(), containerScope, lookups, arguments, boundCompileUnit, currentFunction, span,
            OverloadResolutionFlags::dontInstantiate | OverloadResolutionFlags::dontThrow, templateArgumentTypes, exception);
        if (!functionCall)
        {
            arguments.clear();
            lookups.clear();
            lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
            parameterTypes.clear();
            int n = functionConstraintNode.Parameters().Count();
            for (int i = 0; i < n; ++i)
            {
                ParameterNode* parameterNode = functionConstraintNode.Parameters()[i];
                TypeSymbol* parameterType = ResolveType(parameterNode->TypeExpr(), boundCompileUnit, containerScope);
                lookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, parameterType->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
                parameterTypes.push_back(parameterType);
                arguments.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(&boundCompileUnit.GetModule(), span, parameterType)));
            }
            std::vector<TypeSymbol*> templateArgumentTypes;
            std::unique_ptr<Exception> exception;
            std::unique_ptr<BoundFunctionCall> functionCall = ResolveOverload(functionConstraintNode.GroupId(), containerScope, lookups, arguments, boundCompileUnit, currentFunction, span,
                OverloadResolutionFlags::dontInstantiate | OverloadResolutionFlags::dontThrow, templateArgumentTypes, exception);
            if (!functionCall)
            {
                std::string signature;
                signature.append(ToUtf8(functionConstraintNode.GroupId())).append(1, '(');
                bool first = true;
                for (TypeSymbol* parameterType : parameterTypes)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        signature.append(", ");
                    }
                    signature.append(ToUtf8(parameterType->FullName()));
                }
                signature.append(1, ')');
                std::string message = "function signature '" + signature + "' not found";
                std::vector<Span> references;
                if (exception)
                {
                    message.append(": ").append(exception->Message());
                    references.push_back(exception->Defined());
                    references.insert(references.end(), exception->References().begin(), exception->References().end());
                }
                throw Exception(&boundCompileUnit.GetModule(), message, span, references);
            }
            else
            {
                result = true;
                boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
            }
        }
        else
        {
            result = true;
            boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
        }
    }
    else
    {
        result = true;
        boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
    }
}

void ConstraintChecker::Visit(ConceptIdNode& conceptIdNode)
{
    Reset();
    conceptIdNode.Id()->Accept(*this);
    if (conceptGroup)
    {
        int n = conceptIdNode.Arity();
        ConceptSymbol* conceptSymbol = conceptGroup->GetConcept(n);
        std::vector<TypeSymbol*> typeArguments;
        for (int i = 0; i < n; ++i)
        {
            Node* typeParameterNode = conceptIdNode.TypeParameters()[i];
            Reset();
            typeParameterNode->Accept(*this);
            TypeSymbol* resolvedType = GetType();
            if (resolvedType)
            {
                typeArguments.push_back(resolvedType);
            }
            else
            {
                throw Exception(&boundCompileUnit.GetModule(), "type parameter " + std::to_string(i) + " does not denote a type", span, conceptIdNode.GetSpan());
            }
        }
        BoundConceptKey key(conceptSymbol, typeArguments);
        BoundConcept* boundConcept = boundCompileUnit.GetConceptRepository().GetBoundConcept(key);
        if (boundConcept)
        {
            result = true;
            boundConstraint = std::unique_ptr<BoundConstraint>(boundConcept->GetBoundConstraint()->Clone());
            if (boundConcept->CommonType())
            {
                BoundTemplateParameterSymbol* boundCommonTypeSymbol = new BoundTemplateParameterSymbol(span, U"CommonType");
                boundCommonTypeSymbol->SetType(boundConcept->CommonType());
                containerScope->Install(boundCommonTypeSymbol);
                boundConcept->AddBoundTemplateParameter(std::unique_ptr<BoundTemplateParameterSymbol>(boundCommonTypeSymbol));
            }
        }
        else
        {
            std::unique_ptr<BoundConstraint> constraint;
            std::unique_ptr<BoundConcept> boundConcept = Instantiate(conceptSymbol, typeArguments, boundCompileUnit, containerScope, currentFunction, constraint, span, exception);
            if (boundConcept)
            {
                result = true;
                boundConstraint.reset(constraint.release());
                boundCompileUnit.GetConceptRepository().AddBoundConcept(key, std::move(boundConcept));
            }
            else
            {
                result = false;
                boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, false));
            }
        }
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), conceptIdNode.Id()->ToString() + " does not denote a concept", conceptIdNode.Id()->GetSpan());
    }
}

void ConstraintChecker::Visit(ConceptNode& conceptNode)
{
    Reset();
    conceptNode.Id()->Accept(*this);
    if (!conceptGroup)
    {
        throw Exception(&boundCompileUnit.GetModule(), "symbol '" + conceptNode.Id()->ToString() + "' does not denote a concept", conceptNode.Id()->GetSpan());
    }
    int arity = conceptNode.Arity();
    ConceptSymbol* conceptSymbol = conceptGroup->GetConcept(arity);
    if (conceptNode.Refinement())
    {
        Reset();
        conceptNode.Refinement()->Accept(*this);
        if (!result) return;
    }
    int n = conceptNode.Constraints().Count();
    for (int i = 0; i < n; ++i)
    {
        ConstraintNode* constraintNode = conceptNode.Constraints()[i];
        Reset();
        constraintNode->Accept(*this);
        if (!result) return;
    }
    result = true;
    BoundAtomicConstraint* atomicConstraint = new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true);
    atomicConstraint->SetConcept(conceptSymbol);
    boundConstraint.reset(atomicConstraint);
}

void ConstraintChecker::Visit(SameConstraintNode& sameConstraintNode)
{
    if (firstTypeArgument && secondTypeArgument)
    {
        bool same = TypesEqual(firstTypeArgument, secondTypeArgument);
        if (!same)
        {
            throw Exception(&boundCompileUnit.GetModule(), "type '" + ToUtf8(firstTypeArgument->FullName()) + "' is not same type as '" + ToUtf8(secondTypeArgument->FullName()) + "'", span);
        }
        else
        {
            result = true;
            boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
        }
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "the same type constraint needs two type arguments", span);
    }
}

void ConstraintChecker::Visit(DerivedConstraintNode& derivedConstraintNode)
{
    if (firstTypeArgument && secondTypeArgument)
    {
        bool derived = false;
        if (firstTypeArgument->IsClassTypeSymbol() && secondTypeArgument->IsClassTypeSymbol())
        {
            ClassTypeSymbol* firstClassType = static_cast<ClassTypeSymbol*>(firstTypeArgument);
            ClassTypeSymbol* secondClassType = static_cast<ClassTypeSymbol*>(secondTypeArgument);
            derived = firstClassType->HasBaseClass(secondClassType);
        }
        if (!derived)
        {
            throw Exception(&boundCompileUnit.GetModule(), "type '" + ToUtf8(firstTypeArgument->FullName()) + "' is not derived from '" + ToUtf8(secondTypeArgument->FullName()) + "'", span);
        }
        else
        {
            result = true;
            boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
        }
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "the derivded type constraint needs two type arguments", span);
    }
}

void ConstraintChecker::Visit(ConvertibleConstraintNode& convertibleConstraintNode)
{
    if (firstTypeArgument && secondTypeArgument)
    {
        ArgumentMatch argumentMatch;
        FunctionSymbol* conversion = boundCompileUnit.GetConversion(firstTypeArgument, secondTypeArgument, containerScope, currentFunction, span, argumentMatch);
        if (!conversion || conversion->GetConversionType() == ConversionType::explicit_)
        {
            throw Exception(&boundCompileUnit.GetModule(), "type '" + ToUtf8(firstTypeArgument->FullName()) + "' is not implicitly convertible to '" + ToUtf8(secondTypeArgument->FullName()) + "'", span);
        }
        else
        {
            result = true;
            boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
        }
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "the convertible constraint needs two type arguments", span);
    }
}

void ConstraintChecker::Visit(ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode)
{
    if (firstTypeArgument && secondTypeArgument)
    {
        ArgumentMatch argumentMatch;
        FunctionSymbol* conversion = boundCompileUnit.GetConversion(firstTypeArgument, secondTypeArgument, containerScope, currentFunction, span, argumentMatch);
        if (!conversion || conversion->GetConversionType() != ConversionType::explicit_)
        {
            throw Exception(&boundCompileUnit.GetModule(), "type '" + ToUtf8(firstTypeArgument->FullName()) + "' is not explicitly convertible to '" + ToUtf8(secondTypeArgument->FullName()) + "'", span);
        }
        else
        {
            result = true;
            boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
        }
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "the explicitly convertible constraint needs two type arguments", span);
    }
}

void ConstraintChecker::Visit(CommonConstraintNode& commonConstraintNode)
{
    if (firstTypeArgument && secondTypeArgument)
    {
        BoundTemplateParameterSymbol* commonType = new BoundTemplateParameterSymbol(span, U"CommonType");
        bool same = TypesEqual(firstTypeArgument, secondTypeArgument);
        if (same)
        {
            commonType->SetType(firstTypeArgument);
        }
        else
        {
            ArgumentMatch argumentMatch;
            FunctionSymbol* conversion = boundCompileUnit.GetConversion(firstTypeArgument, secondTypeArgument, containerScope, currentFunction, span, argumentMatch);
            if (conversion && conversion->GetConversionType() == ConversionType::implicit_)
            {
                commonType->SetType(secondTypeArgument);
            }
            else
            {
                ArgumentMatch argumentMatch;
                FunctionSymbol* conversion = boundCompileUnit.GetConversion(secondTypeArgument, firstTypeArgument, containerScope, currentFunction, span, argumentMatch);
                if (conversion && conversion->GetConversionType() == ConversionType::implicit_)
                {
                    commonType->SetType(firstTypeArgument);
                }
                else
                {
                    throw Exception(&boundCompileUnit.GetModule(), "type '" + ToUtf8(firstTypeArgument->FullName()) + "' is not same or convertible to type '" + ToUtf8(secondTypeArgument->FullName()) + "' or vice versa", span);
                }
            }
        }
        containerScope->Install(commonType);
        boundTemplateParameters.push_back(std::unique_ptr<BoundTemplateParameterSymbol>(commonType));
        result = true;
        boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "the common constraint needs two type arguments", span);
    }
}

void ConstraintChecker::Visit(NonreferenceTypeConstraintNode& nonreferenceTypeConstraintNode)
{
    if (firstTypeArgument)
    {
        bool referenceType = firstTypeArgument->IsReferenceType();
        if (referenceType)
        {
            throw Exception(&boundCompileUnit.GetModule(), "type '" + ToUtf8(firstTypeArgument->FullName()) + "' is a reference type", span);
        }
        else
        {
            result = true;
            boundConstraint.reset(new BoundAtomicConstraint(&boundCompileUnit.GetModule(), span, true));
        }
    }
    else
    {
        throw Exception(&boundCompileUnit.GetModule(), "the nonreference type constraint needs one type argument", span);
    }
}

std::unique_ptr<BoundConcept> Instantiate(ConceptSymbol* conceptSymbol, const std::vector<TypeSymbol*>& typeArguments, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, 
    BoundFunction* currentFunction, std::unique_ptr<BoundConstraint>& boundConstraint, const Span& span, std::unique_ptr<Exception>& exception)
{
    ConceptNode* conceptNode = conceptSymbol->GetConceptNode();
    if (!conceptNode)
    {
        Node* node = boundCompileUnit.GetSymbolTable().GetNode(conceptSymbol);
        Assert(node->IsConceptNode(), "concept node expected");
        conceptNode = static_cast<ConceptNode*>(node);
    }
    int n = conceptSymbol->Arity();
    if (n != typeArguments.size())
    {
        throw Exception(&boundCompileUnit.GetModule(), "number of type arguments does not match number of template parameters of concept symbol", span, conceptSymbol->GetSpan());
    }
    ContainerScope instantiationScope;
    instantiationScope.SetParentScope(containerScope);
    std::vector<std::unique_ptr<BoundTemplateParameterSymbol>> boundTemplateParameters;
    TypeSymbol* firstTypeArgument = nullptr;
    TypeSymbol* secondTypeArgument = nullptr;
    for (int i = 0; i < n; ++i)
    {
        TemplateParameterSymbol* templateParameterSymbol = conceptSymbol->TemplateParameters()[i];
        TypeSymbol* typeArgument = typeArguments[i];
        if (typeArgument->RemoveConst(span)->IsBasicTypeSymbol())
        {
            typeArgument = typeArgument->RemoveConst(span);
        }
        if (i == 0)
        {
            firstTypeArgument = typeArgument;
        }
        else if (i == 1)
        {
            secondTypeArgument = typeArgument;
        }
        BoundTemplateParameterSymbol* boundTemplateParameter = new BoundTemplateParameterSymbol(span, templateParameterSymbol->Name());
        boundTemplateParameter->SetType(typeArgument);
        boundTemplateParameters.push_back(std::unique_ptr<BoundTemplateParameterSymbol>(boundTemplateParameter));
        instantiationScope.Install(boundTemplateParameter);
    }
    ConstraintChecker checker(firstTypeArgument, secondTypeArgument, boundCompileUnit, &instantiationScope, currentFunction, span, exception);
    try
    {
        conceptNode->Accept(checker);
        bool result = checker.Result();
        boundConstraint = std::move(checker.GetBoundConstraint());
        if (result)
        {
            BoundConcept* boundConcept = new BoundConcept(&boundCompileUnit.GetModule(), conceptSymbol, typeArguments, span);
            boundConcept->SetBoundConstraint(std::unique_ptr<BoundConstraint>(boundConstraint->Clone()));
            Symbol* commonTypeSymbol = instantiationScope.Lookup(U"CommonType");
            if (commonTypeSymbol)
            {
                if (commonTypeSymbol->GetSymbolType() != SymbolType::boundTemplateParameterSymbol)
                {
                    throw Exception(&boundCompileUnit.GetModule(), "'CommonType' symbol found from concept instantiation scope is not bound template parameter", span, commonTypeSymbol->GetSpan());
                }
                BoundTemplateParameterSymbol* commonType = static_cast<BoundTemplateParameterSymbol*>(commonTypeSymbol);
                BoundTemplateParameterSymbol* commonTypeClone = new BoundTemplateParameterSymbol(span, U"CommonType");
                commonTypeClone->SetType(commonType->GetType());
                boundConcept->AddBoundTemplateParameter(std::unique_ptr<BoundTemplateParameterSymbol>(commonTypeClone));
                containerScope->Install(commonTypeClone);
                boundConcept->SetCommonType(commonType->GetType());
            }
            return std::unique_ptr<BoundConcept>(boundConcept);
        }
        else
        {
            return std::unique_ptr<BoundConcept>(nullptr);
        }
    }
    catch (const Exception& ex)
    {
        std::string message;
        if (typeArguments.size() == 1)
        {
            message.append("type '" + ToUtf8(firstTypeArgument->FullName()) + "' does not fulfill the requirements of concept '");
        }
        else
        {
            message.append("types (");
            bool first = true;
            for (TypeSymbol* typeArgument : typeArguments)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    message.append(", ");
                }
                message.append("'" + ToUtf8(typeArgument->FullName()) + "'");
            }
            message.append(") do not fulfill the requirements of concept '");
        }
        message.append(ToUtf8(conceptSymbol->FullName())).append("' because:\n");
        message.append(ex.Message());
        std::vector<Span> references;
        references.push_back(conceptSymbol->GetSpan());
        references.push_back(ex.Defined());
        references.insert(references.end(), ex.References().begin(), ex.References().end());
        throw Exception(&boundCompileUnit.GetModule(), message, span, references);
    }
}

bool CheckConstraint(ConstraintNode* constraint, const NodeList<Node>& usingNodes, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, BoundFunction* currentFunction,
    const std::vector<TemplateParameterSymbol*>& templateParameters, const std::unordered_map<TemplateParameterSymbol*, TypeSymbol*>& templateParameterMap, 
    std::unique_ptr<BoundConstraint>& boundConstraint, const Span& span, FunctionSymbol* viableFunction, std::unique_ptr<Exception>& conceptCheckException)
{
    bool fileScopeAdded = false;
    try
    {
        std::unique_ptr<FileScope> fileScope(new FileScope(&boundCompileUnit.GetModule()));
        int nu = usingNodes.Count();
        for (int i = 0; i < nu; ++i)
        {
            Node* usingNode = usingNodes[i];
            switch (usingNode->GetNodeType())
            {
                case NodeType::aliasNode:
                {
                    AliasNode* aliasNode = static_cast<AliasNode*>(usingNode);
                    fileScope->InstallAlias(containerScope, aliasNode);
                    break;
                }
                case NodeType::namespaceImportNode:
                {
                    NamespaceImportNode* importNode = static_cast<NamespaceImportNode*>(usingNode);
                    fileScope->InstallNamespaceImport(containerScope, importNode);
                    break;
                }
                default:
                {
                    throw Exception(&boundCompileUnit.GetModule(), "unknown using node type", usingNode->GetSpan());
                }
            }
        }
        ContainerScope constraintCheckScope;
        constraintCheckScope.SetParentScope(containerScope);
        std::vector<std::unique_ptr<BoundTemplateParameterSymbol>> boundTemplateParameters;
        TypeSymbol* firstTypeArgument = nullptr;
        TypeSymbol* secondTypeArgument = nullptr;
        int n = templateParameters.size();
        for (int i = 0; i < n; ++i)
        {
            TemplateParameterSymbol* templateParameterSymbol = templateParameters[i];
            auto it = templateParameterMap.find(templateParameterSymbol);
            if (it != templateParameterMap.cend())
            {
                TypeSymbol* templateArgumentType = it->second;
                if (i == 0)
                {
                    firstTypeArgument = templateArgumentType;
                }
                else if (i == 1)
                {
                    secondTypeArgument = templateArgumentType;
                }
                BoundTemplateParameterSymbol* boundTemplateParameterSymbol = new BoundTemplateParameterSymbol(span, templateParameterSymbol->Name());
                boundTemplateParameterSymbol->SetType(templateArgumentType);
                boundTemplateParameters.push_back(std::unique_ptr<BoundTemplateParameterSymbol>(boundTemplateParameterSymbol));
                constraintCheckScope.Install(boundTemplateParameterSymbol);
            }
            else
            {
                throw Exception(&boundCompileUnit.GetModule(), "template parameter symbol '" + ToUtf8(templateParameterSymbol->Name()) + "' not found from template parameter map", span, viableFunction->GetSpan());
            }
        }
        boundCompileUnit.AddFileScope(fileScope.release());
        fileScopeAdded = true;
        ConstraintChecker constraintChecker(firstTypeArgument, secondTypeArgument, boundCompileUnit, &constraintCheckScope, currentFunction, span, conceptCheckException);
        constraint->Accept(constraintChecker);
        boundCompileUnit.RemoveLastFileScope();
        bool result = constraintChecker.Result();
        boundConstraint = std::move(constraintChecker.GetBoundConstraint());
        return result;
    }
    catch (const Exception& ex)
    {
        if (fileScopeAdded)
        {
            boundCompileUnit.RemoveLastFileScope();
        }
        conceptCheckException.reset(new Exception(ex));
        return false;
    }
}

} } // namespace cmajor::binder
