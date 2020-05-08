/*
#include <cmajor/binder/AttributeBinder.hpp>
#include <sngcm/ast/InitDone.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Expression.hpp>
#include <cmajor/symbols/InitDone.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <cmajor/xpath/InitDone.hpp>
#include <cmajor/xpath/XPathEvaluate.hpp>
#include <soulng/util/InitDone.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Sha1.hpp>
#include <cmajor/parsing/InitDone.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

using namespace sngxml::dom;
using namespace soulng::unicode;
using namespace cmajor::symbols;
using namespace cmajor::binder;
using namespace soulng::util;

bool verbose = false;
bool force = false;

void GenerateClassXml(Element* parentElement, ClassTypeSymbol* cls);
void GenerateXmlForParameter(ParameterSymbol* parameter, Element* parametersElement);
void GenerateXmlForFunctions(Element* parentElement, const std::vector<FunctionSymbol*>& functions, int startParam);
void GenerateXmlForTypedefs(Element* parentElement, const std::vector<TypedefSymbol*>& typedefs);
void GenerateXmlForConcepts(Element* parentElement, const std::vector<ConceptSymbol*>& concepts);
void GenerateXmlForDelegates(Element* parentElement, const std::vector<DelegateTypeSymbol*>& delegates);
void GenerateXmlForClassDelegates(Element* parentElement, const std::vector<ClassDelegateTypeSymbol*>& classDelegates);
void GenerateXmlForConstants(Element* parentElement, const std::vector<ConstantSymbol*>& constants);
void GenerateXmlForEnumeratedTypes(Element* parentElement, const std::vector<EnumTypeSymbol*>& enumTypes);
void GenerateXmlForMemberVariables(Element* parentElement, const std::vector<MemberVariableSymbol*>& memberVariables);
void GenerateXmlForTypes(Element* parentElement, std::vector<TypeSymbol*>& types, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs);
std::u32string GetName(sngxml::dom::Element* element, bool optional);
std::u32string GetName(sngxml::dom::Element* element)
{
    return GetName(element, false);
}

std::u32string GetTypeId(TypeSymbol* type)
{
    if (type->IsBasicTypeSymbol())
    {
        return U"type." + type->Id();
    }
    else if (type->GetSymbolType() == SymbolType::derivedTypeSymbol)
    {
        return U"type." + type->Id();
    }
    else if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        return U"type." + type->Id();
    }
    else
    {
        return type->Id();
    }
}

std::u32string FullClassName(Element* element)
{
    std::u32string fullClassName;
    if (element->Name() == U"namespace")
    {
        fullClassName = GetName(element);
    }
    else
    {
        ParentNode* parent = element->Parent();
        if (parent && parent->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            fullClassName = FullClassName(static_cast<Element*>(parent));
            std::u32string name = GetName(element, true);
            if (!name.empty())
            {
                if (!fullClassName.empty())
                {
                    fullClassName.append(1, '.');
                }
                fullClassName.append(name);
            }
        }
    }
    return fullClassName;
}

class ConstraintHyperTextGenerator : public sngcm::ast::Visitor
{
public:
    ConstraintHyperTextGenerator(Element* constraintElement_, const std::vector<ContainerScope*>& containerScopes_, SymbolTable* symbolTable_);
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
    void Visit(DotNode& dotNode) override;
    void Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode) override;
    void Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode) override;
    void Visit(WhereConstraintNode& whereConstraintNode) override;
    void Visit(IsConstraintNode& isConstraintNode) override;
    void Visit(MultiParamConstraintNode& multiParamConstraintNode) override;
    void Visit(PredicateConstraintNode& predicateConstratintNode) override;
    void Visit(TypeNameConstraintNode& typeNameConstraintNode) override;
    void Visit(ConstructorConstraintNode& constructorConstraintNode) override;
    void Visit(DestructorConstraintNode& destructorConstraintNode) override;
    void Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode) override;
    void Visit(FunctionConstraintNode& functionConstraintNode) override;
private: 
    Element* parentElement;
    const std::vector<ContainerScope*>& containerScopes;
    SymbolTable* symbolTable;
    ConceptGroupSymbol* conceptGroup;
    bool typeAdded;
    NamespaceSymbol* ns;
    bool parens;
    std::stack<bool> parensStack;
    void ResolveSymbol(Symbol* symbol);
    void PushParens(bool parens_)
    {
        parensStack.push(parens);
        parens = parens_;
    }
    void PopParens()
    {
        parens = parensStack.top();
        parensStack.pop();
    }
};

ConstraintHyperTextGenerator::ConstraintHyperTextGenerator(Element* parentElement_, const std::vector<ContainerScope*>& containerScopes_, SymbolTable* symbolTable_) :
    parentElement(parentElement_), containerScopes(containerScopes_), symbolTable(symbolTable_), conceptGroup(nullptr), typeAdded(false), parens(false)
{
}

void ConstraintHyperTextGenerator::Visit(BoolNode& boolNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"bool");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(SByteNode& sbyteNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"sbyte");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(ByteNode& byteNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"byte");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(ShortNode& shortNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"short");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(UShortNode& ushortNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"ushort");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(IntNode& intNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"int");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(UIntNode& uintNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"uint");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(LongNode& longNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"long");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(ULongNode& ulongNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"ulong");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(FloatNode& floatNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"float");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(DoubleNode& doubleNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"double");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(CharNode& charNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"char");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(WCharNode& wcharNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"wchar");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(UCharNode& ucharNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"uchar");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(VoidNode& voidNode)
{
    TypeSymbol* type = symbolTable->GetTypeByName(U"void");
    type->SetProject();
    std::unique_ptr<Element> typeElement(new Element(U"type"));
    typeElement->SetAttribute(U"ref", GetTypeId(type));
    parentElement->AppendChild(std::move(typeElement));
    typeAdded = true;
}

void ConstraintHyperTextGenerator::Visit(ConstNode& constNode)
{
    std::unique_ptr<Element> qualifierElement(new Element(U"qualifier"));
    qualifierElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"const")));
    parentElement->AppendChild(std::move(qualifierElement));
    constNode.Subject()->Accept(*this);
}

void ConstraintHyperTextGenerator::Visit(LValueRefNode& lvalueRefNode)
{
    lvalueRefNode.Subject()->Accept(*this);
    std::unique_ptr<Element> qualifierElement(new Element(U"qualifier"));
    qualifierElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"&")));
    parentElement->AppendChild(std::move(qualifierElement));
}

void ConstraintHyperTextGenerator::Visit(RValueRefNode& rvalueRefNode) 
{
    rvalueRefNode.Subject()->Accept(*this);
    std::unique_ptr<Element> qualifierElement(new Element(U"qualifier"));
    qualifierElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"&&")));
    parentElement->AppendChild(std::move(qualifierElement));
}

void ConstraintHyperTextGenerator::Visit(PointerNode& pointerNode)
{
    pointerNode.Subject()->Accept(*this);
    std::unique_ptr<Element> qualifierElement(new Element(U"qualifier"));
    qualifierElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"*")));
    parentElement->AppendChild(std::move(qualifierElement));
}

void ConstraintHyperTextGenerator::Visit(ArrayNode& arrayNode)
{
    arrayNode.Subject()->Accept(*this);
    std::unique_ptr<Element> qualifierElement(new Element(U"qualifier"));
    qualifierElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"[]")));
    parentElement->AppendChild(std::move(qualifierElement));
}

void ConstraintHyperTextGenerator::Visit(IdentifierNode& identifierNode)
{
    const std::u32string& name = identifierNode.Str();
    bool resolved = false;
    for (ContainerScope* containerScope : containerScopes)
    {
        Symbol* symbol = containerScope->Lookup(name, ScopeLookup::this_and_base_and_parent);
        if (symbol)
        {
            ResolveSymbol(symbol);
            resolved = true;
            break;
        }
    }
    if (!resolved)
    {
        parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(name)));
    }
}

void ConstraintHyperTextGenerator::ResolveSymbol(Symbol* symbol)
{
    if (symbol)
    {
        if (symbol->GetSymbolType() == SymbolType::conceptGroupSymbol)
        {
            conceptGroup = static_cast<ConceptGroupSymbol*>(symbol);
        }
        else if (symbol->GetSymbolType() == SymbolType::boundTemplateParameterSymbol)
        {
            BoundTemplateParameterSymbol* boundTemplateParameter = static_cast<BoundTemplateParameterSymbol*>(symbol);
            TypeSymbol* type = boundTemplateParameter->GetType();
            type->SetProject();
            std::unique_ptr<Element> typeElement(new Element(U"type"));
            typeElement->SetAttribute(U"ref", GetTypeId(type));
            parentElement->AppendChild(std::move(typeElement));
            conceptGroup = nullptr;
            typeAdded = true;
        }
        else if (symbol->GetSymbolType() == SymbolType::typedefSymbol)
        {
            TypedefSymbol* typedefSymbol = static_cast<TypedefSymbol*>(symbol);
            TypeSymbol* type = typedefSymbol->GetType();
            type->SetProject();
            std::unique_ptr<Element> typeElement(new Element(U"type"));
            typeElement->SetAttribute(U"ref", GetTypeId(type));
            parentElement->AppendChild(std::move(typeElement));
            conceptGroup = nullptr;
            typeAdded = true;
        }
        else if (symbol->IsTypeSymbol())
        {
            std::unique_ptr<Element> typeElement(new Element(U"type"));
            symbol->SetProject();
            typeElement->SetAttribute(U"ref", GetTypeId(static_cast<TypeSymbol*>(symbol)));
            parentElement->AppendChild(std::move(typeElement));
            conceptGroup = nullptr;
            typeAdded = true;
        }
        else if (symbol->GetSymbolType() == SymbolType::namespaceSymbol)
        {
            ns = static_cast<NamespaceSymbol*>(symbol);
        }
        else
        {
            throw std::runtime_error("type, namespace or concept group expected");
        }
    }
    else
    {
        throw std::runtime_error("symbol not found");
    }
}

void ConstraintHyperTextGenerator::Visit(DotNode& dotNode)
{
    dotNode.Subject()->Accept(*this);
    const std::u32string& name = dotNode.MemberId()->Str();
    if (ns)
    {
        Symbol* symbol = ns->GetContainerScope()->Lookup(name);
        ResolveSymbol(symbol);
    }
    else
    {
        std::unique_ptr<Element> dot(new Element(U"dot"));
        dot->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(name)));
        parentElement->AppendChild(std::move(dot));
    }
}

void ConstraintHyperTextGenerator::Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode)
{
    if (parens)
    {
        std::unique_ptr<Element> openParen(new Element(U"openParen"));
        parentElement->AppendChild(std::move(openParen));
    }
    disjunctiveConstraintNode.Left()->Accept(*this);
    std::unique_ptr<Element> connector(new Element(U"connector"));
    connector->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"or")));
    parentElement->AppendChild(std::move(connector));
    disjunctiveConstraintNode.Right()->Accept(*this);
    if (parens)
    {
        std::unique_ptr<Element> closeParen(new Element(U"closeParen"));
        parentElement->AppendChild(std::move(closeParen));
    }
}

void ConstraintHyperTextGenerator::Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode)
{
    if (conjunctiveConstraintNode.Left()->GetNodeType() == sngcm::ast::NodeType::disjunctiveConstraintNode)
    {
        PushParens(true);
        conjunctiveConstraintNode.Left()->Accept(*this);
        PopParens();
    }
    else
    {
        conjunctiveConstraintNode.Left()->Accept(*this);
    }
    std::unique_ptr<Element> connector(new Element(U"connector"));
    connector->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"and")));
    parentElement->AppendChild(std::move(connector));
    if (conjunctiveConstraintNode.Right()->GetNodeType() == sngcm::ast::NodeType::disjunctiveConstraintNode)
    {
        PushParens(true);
        conjunctiveConstraintNode.Right()->Accept(*this);
        PopParens();
    }
    else
    {
        conjunctiveConstraintNode.Right()->Accept(*this);
    }
}

void ConstraintHyperTextGenerator::Visit(WhereConstraintNode& whereConstraintNode)
{
    whereConstraintNode.Constraint()->Accept(*this);
}

void ConstraintHyperTextGenerator::Visit(IsConstraintNode& isConstraintNode)
{
    isConstraintNode.TypeExpr()->Accept(*this);
    std::unique_ptr<Element> connector(new Element(U"connector"));
    connector->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"is")));
    parentElement->AppendChild(std::move(connector));
    typeAdded = false;
    isConstraintNode.ConceptOrTypeName()->Accept(*this);
    if (conceptGroup)
    {
        ConceptSymbol* concept = conceptGroup->GetConcept(1);
        std::unique_ptr<Element> conceptElement(new Element(U"concept"));
        conceptElement->SetAttribute(U"ref", concept->Id());
        parentElement->AppendChild(std::move(conceptElement));
    }
    else if (!typeAdded)
    {
        throw std::runtime_error("concept group or type expected");
    }
}

void ConstraintHyperTextGenerator::Visit(MultiParamConstraintNode& multiParamConstraintNode)
{
    conceptGroup = nullptr;
    multiParamConstraintNode.ConceptId()->Accept(*this);
    ConceptGroupSymbol* group = conceptGroup;
    conceptGroup = nullptr;
    Element* prevParent = parentElement;
    std::unique_ptr<Element> paramsElement(new Element(U"params"));
    parentElement = paramsElement.get();
    int n = multiParamConstraintNode.TypeExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* node = multiParamConstraintNode.TypeExprs()[i];
        node->Accept(*this);
    }
    parentElement = prevParent;
    if (group)
    {
        ConceptSymbol* concept = group->GetConcept(n);
        std::unique_ptr<Element> conceptElement(new Element(U"concept"));
        conceptElement->SetAttribute(U"ref", concept->Id());
        conceptElement->AppendChild(std::move(paramsElement));
        parentElement->AppendChild(std::move(conceptElement));
    }
    else
    {
        std::unique_ptr<Element> conceptElement(new Element(U"concept"));
        conceptElement->SetAttribute(U"name", multiParamConstraintNode.ConceptId()->Str());
        conceptElement->AppendChild(std::move(paramsElement));
        parentElement->AppendChild(std::move(conceptElement));
    }
}

void ConstraintHyperTextGenerator::Visit(TypeNameConstraintNode& typeNameConstraintNode)
{
    std::unique_ptr<Element> span(new Element(U"span"));
    span->SetAttribute(U"class", U"kw");
    span->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"typename")));
    parentElement->AppendChild(std::move(span));
    parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U" " + ToUtf32(typeNameConstraintNode.TypeId()->ToString()))));
}

void ConstraintHyperTextGenerator::Visit(PredicateConstraintNode& predicateConstratintNode)
{
    parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(predicateConstratintNode.ToString()))));
}

void ConstraintHyperTextGenerator::Visit(ConstructorConstraintNode& constructorConstraintNode)
{
    parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(constructorConstraintNode.ToString()))));
}

void ConstraintHyperTextGenerator::Visit(DestructorConstraintNode& destructorConstraintNode)
{
    parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(destructorConstraintNode.ToString()))));
}

void ConstraintHyperTextGenerator::Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode)
{
    parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(memberFunctionConstraintNode.ToString()))));
}

void ConstraintHyperTextGenerator::Visit(FunctionConstraintNode& functionConstraintNode)
{
    parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(functionConstraintNode.ToString()))));
}

std::unique_ptr<Element> ConstraintToXml(ConstraintNode* constraintNode, Symbol* parent)
{
    std::unique_ptr<Element> constraintElement(new Element(U"constraint"));
    ContainerScope* containerScope = parent->GetContainerScope();
    std::vector<ContainerScope*> containerScopes;
    containerScopes.push_back(containerScope);
    const sngcm::ast::NodeList<sngcm::ast::Node>* usingNodes = nullptr;
    if (parent->IsFunctionSymbol())
    {
        FunctionSymbol* fun = static_cast<FunctionSymbol*>(parent);
        containerScopes.push_back(fun->Ns()->GetContainerScope());
        if (fun->GetSymbolType() == SymbolType::functionSymbol)
        {
            if (!fun->GetFunctionNode())
            {
                fun->ReadAstNodes();
            }
            usingNodes = &fun->UsingNodes();
        }
        else if (fun->GetSymbolType() == SymbolType::memberFunctionSymbol)
        {
            parent = fun->Parent();
        }
    }
    if (parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(parent);
        ClassTypeSymbol* classTemplate = specialization->GetClassTemplate();
        containerScopes.push_back(classTemplate->Ns()->GetContainerScope());
        if (!classTemplate->GetClassNode())
        {
            classTemplate->ReadAstNodes();
        }
        usingNodes = &classTemplate->UsingNodes();
    }
    if (parent->GetSymbolType() == SymbolType::conceptSymbol)
    {
        containerScopes.push_back(parent->Ns()->GetContainerScope());
    }
    if (usingNodes)
    {
        int n = usingNodes->Count();
        for (int i = 0; i < n; ++i)
        {
            sngcm::ast::Node* usingNode = (*usingNodes)[i];
            if (usingNode->GetNodeType() == sngcm::ast::NodeType::namespaceImportNode)
            {
                sngcm::ast::NamespaceImportNode* nsImport = static_cast<sngcm::ast::NamespaceImportNode*>(usingNode);
                std::u32string importedNamespaceName = nsImport->Ns()->Str();
                Symbol* symbol = containerScope->Lookup(importedNamespaceName, ScopeLookup::this_and_parent);
                containerScopes.push_back(symbol->GetContainerScope());
            }
        }
    }
    ConstraintHyperTextGenerator generator(constraintElement.get(), containerScopes, parent->GetSymbolTable());
    constraintNode->Accept(generator);
    return constraintElement;
}

void GenerateXmlForClasses(Element* parentElement, const std::vector<ClassTypeSymbol*>& classes)
{
    if (!classes.empty())
    {
        std::unique_ptr<Element> classesElement(new Element(U"classes"));
        for (ClassTypeSymbol* cls : classes)
        {
            GenerateClassXml(classesElement.get(), cls);
        }
        parentElement->AppendChild(std::move(classesElement));
    }
}

void GenerateClassXml(Element* parentElement, ClassTypeSymbol* cls)
{
    std::unique_ptr<Element> classElement(new Element(U"class"));
    classElement->SetAttribute(U"id", cls->Id());
    std::unique_ptr<Element> classNameElement(new Element(U"name"));
    classNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(cls->DocName())));
    classElement->AppendChild(std::move(classNameElement));
    std::unique_ptr<Element> classGroupNameElement(new Element(U"groupName"));
    classGroupNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(cls->GroupName())));
    classElement->AppendChild(std::move(classGroupNameElement));
    std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
    syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(cls->Syntax()))));
    classElement->AppendChild(std::move(syntaxElement));
    if (cls->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(cls);
        if (!specialization->TemplateArgumentTypes().empty())
        {
            std::unique_ptr<Element> templateParametersElement(new Element(U"templateParameters"));
            for (TypeSymbol* templateParameter : specialization->TemplateArgumentTypes())
            {
                std::unique_ptr<Element> templateParameterElement(new Element(U"templateParameter"));
                templateParameterElement->SetAttribute(U"id", templateParameter->Id());
                std::unique_ptr<Element> templateParameterNameElement(new Element(U"name"));
                templateParameterNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(templateParameter->DocName())));
                templateParameterElement->AppendChild(std::move(templateParameterNameElement));
                if (templateParameter->GetSymbolType() == SymbolType::templateParameterSymbol)
                {
                    TemplateParameterSymbol* symbol = static_cast<TemplateParameterSymbol*>(templateParameter);
                    if (symbol->HasDefault())
                    {
                        std::unique_ptr<Element> defaultElement(new Element(U"default"));
                        defaultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(symbol->DefaultStr()))));
                        templateParameterElement->AppendChild(std::move(defaultElement));
                    }
                }
                templateParametersElement->AppendChild(std::move(templateParameterElement));
            }
            classElement->AppendChild(std::move(templateParametersElement));
        }
    }
    if (cls->BaseClass())
    {
        std::unique_ptr<Element> baseClassElement(new Element(U"baseClass"));
        baseClassElement->SetAttribute(U"ref", GetTypeId(cls->BaseClass()));
        classElement->AppendChild(std::move(baseClassElement));
    }
    if (!cls->ImplementedInterfaces().empty())
    {
        std::vector<InterfaceTypeSymbol*> implementedInterfaces = cls->ImplementedInterfaces();
        std::sort(implementedInterfaces.begin(), implementedInterfaces.end(), ByDocName());
        std::unique_ptr<Element> interfacesElement(new Element(U"implementedInterfaces"));
        for (InterfaceTypeSymbol* implementedInterface : implementedInterfaces)
        {
            std::unique_ptr<Element> interfaceElement(new Element(U"interface"));
            interfaceElement->SetAttribute(U"ref", GetTypeId(implementedInterface));
            interfacesElement->AppendChild(std::move(interfaceElement));
        }
        classElement->AppendChild(std::move(interfacesElement));
    }
    if (cls->Constraint())
    {
        std::unique_ptr<Element> constraintElement = ConstraintToXml(cls->Constraint(), cls);
        classElement->AppendChild(std::move(constraintElement));
    }
    SymbolCollector collector;
    cls->CollectMembers(&collector);
    collector.SortByDocName();
    GenerateXmlForFunctions(classElement.get(), collector.Functions(), 1);
    GenerateXmlForTypedefs(classElement.get(), collector.Typedefs());
    GenerateXmlForConcepts(classElement.get(), collector.Concepts());
    GenerateXmlForDelegates(classElement.get(), collector.Delegates());
    GenerateXmlForClassDelegates(classElement.get(), collector.ClassDelegates());
    GenerateXmlForConstants(classElement.get(), collector.Constants());
    GenerateXmlForEnumeratedTypes(classElement.get(), collector.EnumeratedTypes());
    GenerateXmlForMemberVariables(classElement.get(), collector.MemberVariables());
    parentElement->AppendChild(std::move(classElement));
}

void GenerateXmlForTypedefs(Element* parentElement, const std::vector<TypedefSymbol*>& typedefs)
{
    if (typedefs.empty()) return;
    std::unique_ptr<Element> typedefsElement(new Element(U"typedefs"));
    for (TypedefSymbol* typedef_ : typedefs)
    {
        std::unique_ptr<Element> typedefElement(new Element(U"typedef"));
        typedefElement->SetAttribute(U"id", typedef_->Id());
        std::unique_ptr<Element> typedefNameElement(new Element(U"name"));
        typedefNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(typedef_->DocName())));
        typedefElement->AppendChild(std::move(typedefNameElement));
        std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
        syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(typedef_->Syntax()))));
        typedefElement->AppendChild(std::move(syntaxElement));
        std::unique_ptr<Element> typeElement(new Element(U"type"));
        typeElement->SetAttribute(U"ref", GetTypeId(typedef_->GetType()));
        typedef_->GetType()->SetProject();
        typedefElement->AppendChild(std::move(typeElement));
        typedefsElement->AppendChild(std::move(typedefElement));
    }
    parentElement->AppendChild(std::move(typedefsElement));
}

void GenerateXmlForConcepts(Element* parentElement, const std::vector<ConceptSymbol*>& concepts)
{
    if (concepts.empty()) return;
    std::vector<ConceptSymbol*> allConcepts;
    for (ConceptSymbol* concept : concepts)
    {
        allConcepts.push_back(concept);
        if (concept->RefinedConcept())
        {
            allConcepts.push_back(concept->RefinedConcept());
        }
    }
    std::sort(allConcepts.begin(), allConcepts.end(), ByDocName());
    auto it = std::unique(allConcepts.begin(), allConcepts.end());
    allConcepts.erase(it, allConcepts.end());
    std::unique_ptr<Element> conceptsElement(new Element(U"concepts"));
    for (ConceptSymbol* concept : allConcepts)
    {
        std::unique_ptr<Element> conceptElement(new Element(U"concept"));
        conceptElement->SetAttribute(U"id", concept->Id());
        std::unique_ptr<Element> conceptNameElement(new Element(U"name"));
        conceptNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(concept->DocName())));
        conceptElement->AppendChild(std::move(conceptNameElement));
        std::unique_ptr<Element> conceptGroupNameElement(new Element(U"groupName"));
        conceptGroupNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(concept->GroupName())));
        conceptElement->AppendChild(std::move(conceptGroupNameElement));
        std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
        syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(concept->Syntax()))));
        conceptElement->AppendChild(std::move(syntaxElement));
        std::unique_ptr<Element> templateParametersElement(new Element(U"templateParameters"));
        for (TemplateParameterSymbol* templateParameter : concept->TemplateParameters())
        {
            std::unique_ptr<Element> templateParameterElement(new Element(U"templateParameter"));
            templateParameterElement->SetAttribute(U"id", templateParameter->Id());
            std::unique_ptr<Element> templateParameterNameElement(new Element(U"name"));
            templateParameterNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(templateParameter->DocName())));
            templateParameterElement->AppendChild(std::move(templateParameterNameElement));
            if (templateParameter->HasDefault())
            {
                std::unique_ptr<Element> defaultElement(new Element(U"default"));
                defaultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(templateParameter->DefaultStr()))));
                templateParameterElement->AppendChild(std::move(defaultElement));
            }
            templateParametersElement->AppendChild(std::move(templateParameterElement));
        }
        conceptElement->AppendChild(std::move(templateParametersElement));
        if (concept->RefinedConcept())
        {
            std::unique_ptr<Element> refinesElement(new Element(U"refines"));
            refinesElement->SetAttribute(U"ref", concept->RefinedConcept()->Id());
            conceptElement->AppendChild(std::move(refinesElement));
        }
        std::unique_ptr<Element> constraintsElement(new Element(U"constraints"));
        ConceptNode* conceptNode = concept->GetConceptNode();
        int n = conceptNode->Constraints().Count();
        for (int i = 0; i < n; ++i)
        {
            ConstraintNode* constraintNode = conceptNode->Constraints()[i];
            std::unique_ptr<Element> constraintElement = ConstraintToXml(constraintNode, concept);
            constraintsElement->AppendChild(std::move(constraintElement));
        }
        conceptElement->AppendChild(std::move(constraintsElement));
        std::unique_ptr<Element> axiomsElement(new Element(U"axioms"));
        int m = conceptNode->Axioms().Count();
        for (int i = 0; i < m; ++i)
        {
            AxiomNode* axiom = conceptNode->Axioms()[i];
            std::unique_ptr<Element> axiomElement(new Element(U"axiom"));
            std::unique_ptr<Element> axiomNameElement(new Element(U"name"));
            std::u32string axiomName = ToUtf32(axiom->Id()->ToString());
            int p = axiom->Parameters().Count();
            if (p > 0)
            {
                axiomName.append(U"(");
                for (int i = 0; i < p; ++i)
                {
                    if (i > 0)
                    {
                        axiomName.append(U", ");
                    }
                    axiomName.append(axiom->Parameters()[i]->Id()->Str());
                }
                axiomName.append(U")");
            }
            axiomNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(axiomName)));
            axiomElement->AppendChild(std::move(axiomNameElement));
            std::unique_ptr<Element> axiomStatementsElement(new Element(U"axiomStatements"));
            int s = axiom->Statements().Count();
            for (int i = 0; i < s; ++i)
            {
                std::unique_ptr<Element> axiomStatementElement(new Element(U"axiomStatement"));
                axiomStatementElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(axiom->Statements()[i]->ToString()))));
                axiomStatementsElement->AppendChild(std::move(axiomStatementElement));
            }
            axiomElement->AppendChild(std::move(axiomStatementsElement));
            axiomsElement->AppendChild(std::move(axiomElement));
        }
        conceptElement->AppendChild(std::move(axiomsElement));
        conceptsElement->AppendChild(std::move(conceptElement));
    }
    parentElement->AppendChild(std::move(conceptsElement));
}

void GenerateXmlForDelegates(Element* parentElement, const std::vector<DelegateTypeSymbol*>& delegates)
{
    if (delegates.empty()) return;
    std::unique_ptr<Element> delegatesElement(new Element(U"delegates"));
    for (DelegateTypeSymbol* delegate : delegates)
    {
        std::unique_ptr<Element> delegateElement(new Element(U"delegate"));
        delegateElement->SetAttribute(U"id", delegate->Id());
        std::unique_ptr<Element> delegateNameElement(new Element(U"name"));
        delegateNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(delegate->DocName())));
        delegateElement->AppendChild(std::move(delegateNameElement));
        std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
        syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(delegate->Syntax()))));
        delegateElement->AppendChild(std::move(syntaxElement));
        int n = delegate->Parameters().size();
        if (n > 0)
        {
            std::unique_ptr<Element> delegateParametersElement(new Element(U"parameters"));
            for (int i = 0; i < n; ++i)
            {
                ParameterSymbol* parameter = delegate->Parameters()[i];
                GenerateXmlForParameter(parameter, delegateParametersElement.get());
            }
            delegateElement->AppendChild(std::move(delegateParametersElement));
        }
        if (!delegate->ReturnType()->IsVoidType())
        {
            std::unique_ptr<Element> returnTypeElement(new Element(U"returnType"));
            returnTypeElement->SetAttribute(U"ref", GetTypeId(delegate->ReturnType()));
            delegate->ReturnType()->SetProject();
            delegateElement->AppendChild(std::move(returnTypeElement));
        }
        delegatesElement->AppendChild(std::move(delegateElement));
    }
    parentElement->AppendChild(std::move(delegatesElement));
}

void GenerateXmlForClassDelegates(Element* parentElement, const std::vector<ClassDelegateTypeSymbol*>& classDelegates)
{
    if (classDelegates.empty()) return;
    std::unique_ptr<Element> classDelegatesElement(new Element(U"classDelegates"));
    for (ClassDelegateTypeSymbol* classDelegate : classDelegates)
    {
        std::unique_ptr<Element> classDelegateElement(new Element(U"classDelegate"));
        classDelegateElement->SetAttribute(U"id", classDelegate->Id());
        std::unique_ptr<Element> classDelegateNameElement(new Element(U"name"));
        classDelegateNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(classDelegate->DocName())));
        classDelegateElement->AppendChild(std::move(classDelegateNameElement));
        std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
        syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(classDelegate->Syntax()))));
        classDelegateElement->AppendChild(std::move(syntaxElement));
        int n = classDelegate->Parameters().size();
        if (n > 0)
        {
            std::unique_ptr<Element> classDelegateParametersElement(new Element(U"parameters"));
            for (int i = 0; i < n; ++i)
            {
                ParameterSymbol* parameter = classDelegate->Parameters()[i];
                GenerateXmlForParameter(parameter, classDelegateParametersElement.get());
            }
            classDelegateElement->AppendChild(std::move(classDelegateParametersElement));
        }
        if (!classDelegate->ReturnType()->IsVoidType())
        {
            std::unique_ptr<Element> returnTypeElement(new Element(U"returnType"));
            returnTypeElement->SetAttribute(U"ref", GetTypeId(classDelegate->ReturnType()));
            classDelegate->ReturnType()->SetProject();
            classDelegateElement->AppendChild(std::move(returnTypeElement));
        }
        classDelegatesElement->AppendChild(std::move(classDelegateElement));
    }
    parentElement->AppendChild(std::move(classDelegatesElement));
}

void GenerateXmlForConstants(Element* parentElement, const std::vector<ConstantSymbol*>& constants)
{
    if (constants.empty()) return;
    std::unique_ptr<Element> constantsElement(new Element(U"constants"));
    for (ConstantSymbol* constant : constants)
    {
        std::unique_ptr<Element> constantElement(new Element(U"constant"));
        constantElement->SetAttribute(U"id", constant->Id());
        std::unique_ptr<Element> constantNameElement(new Element(U"name"));
        constantNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(constant->DocName())));
        constantElement->AppendChild(std::move(constantNameElement));
        std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
        syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(constant->Syntax()))));
        constantElement->AppendChild(std::move(syntaxElement));
        std::unique_ptr<Element> constantTypeElement(new Element(U"type"));
        constantTypeElement->SetAttribute(U"ref", GetTypeId(constant->GetType()));
        constant->GetType()->SetProject();
        constantElement->AppendChild(std::move(constantTypeElement));
        std::unique_ptr<Element> constantValueElement(new Element(U"value"));
        std::string constantValue = constant->GetValue()->ToString();
        if (constant->GetType()->IsUnsignedType())
        {
            constantValue.append(1, 'u');
        }
        constantValueElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(constantValue))));
        constantElement->AppendChild(std::move(constantValueElement));
        constantsElement->AppendChild(std::move(constantElement));
    }
    parentElement->AppendChild(std::move(constantsElement));
}

void GenerateXmlForEnumeratedTypes(Element* parentElement, const std::vector<EnumTypeSymbol*>& enumTypes)
{
    if (enumTypes.empty()) return;
    std::unique_ptr<Element> enumerationsElement(new Element(U"enumerations"));
    for (EnumTypeSymbol* enumType : enumTypes)
    {
        std::unique_ptr<Element> enumerationElement(new Element(U"enumeration"));
        enumerationElement->SetAttribute(U"id", enumType->Id());
        std::unique_ptr<Element> enumerationNameElement(new Element(U"name"));
        enumerationNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(enumType->DocName())));
        enumerationElement->AppendChild(std::move(enumerationNameElement));
        std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
        syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(enumType->Syntax()))));
        enumerationElement->AppendChild(std::move(syntaxElement));
        std::unique_ptr<Element> underlyingTypeElement(new Element(U"underlyingType"));
        TypeSymbol* underlyingType = enumType->UnderlyingType();
        underlyingType->SetProject();
        underlyingTypeElement->SetAttribute(U"ref", GetTypeId(underlyingType));
        enumerationElement->AppendChild(std::move(underlyingTypeElement));
        SymbolCollector collector;
        enumType->CollectMembers(&collector);
        collector.SortByDocName();
        for (EnumConstantSymbol* enumConstant : collector.EnumerationConstants())
        {
            std::unique_ptr<Element> enumConstantElement(new Element(U"enumConstant"));
            std::unique_ptr<Element> enumConstantNameElement(new Element(U"name"));
            enumConstantNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(enumConstant->DocName())));
            enumConstantElement->AppendChild(std::move(enumConstantNameElement));
            std::unique_ptr<Element> enumConstantValueElement(new Element(U"value"));
            std::string enumConstantValue = enumConstant->GetValue()->ToString();
            if (underlyingType->IsUnsignedType())
            {
                enumConstantValue.append(1, 'u');
            }
            enumConstantValueElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(enumConstantValue))));
            enumConstantElement->AppendChild(std::move(enumConstantValueElement));
            enumerationElement->AppendChild(std::move(enumConstantElement));
        }
        enumerationsElement->AppendChild(std::move(enumerationElement));
    }
    parentElement->AppendChild(std::move(enumerationsElement));
}

void GenerateXmlForMemberVariables(Element* parentElement, const std::vector<MemberVariableSymbol*>& memberVariables)
{
    if (memberVariables.empty()) return;
    std::unique_ptr<Element> memberVariablesElement(new Element(U"memberVariables"));
    for (MemberVariableSymbol* memberVariable : memberVariables)
    {
        std::unique_ptr<Element> memberVariableElement(new Element(U"memberVariable"));
        memberVariableElement->SetAttribute(U"id", memberVariable->Id());
        std::unique_ptr<Element> memberVariableNameElement(new Element(U"name"));
        memberVariableNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(memberVariable->DocName())));
        memberVariableElement->AppendChild(std::move(memberVariableNameElement));
        std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
        syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(memberVariable->Syntax()))));
        memberVariableElement->AppendChild(std::move(syntaxElement));
        std::unique_ptr<Element> typeElement(new Element(U"type"));
        typeElement->SetAttribute(U"ref", GetTypeId(memberVariable->GetType()));
        memberVariableElement->AppendChild(std::move(typeElement));
        memberVariablesElement->AppendChild(std::move(memberVariableElement));
    }
    parentElement->AppendChild(std::move(memberVariablesElement));
}

struct IdEqual
{
    bool operator()(TypeSymbol* left, TypeSymbol* right) const
    {
        return left->Id() == right->Id();
    }
};

void GenerateXmlForTypes(Element* parentElement, std::vector<TypeSymbol*>& types, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs)
{
    std::unique_ptr<Element> typesElement(new Element(U"types"));
    std::vector<TypeSymbol*> liveTypes;
    bool changed = true; 
    while (changed)
    {
        changed = false;
        for (TypeSymbol* type : types)
        {
            if (!type->IsProject()) continue;
            std::u32string typeId = GetTypeId(type);
            bool found = false;
            for (const std::unique_ptr<Document>& referenceDoc : referenceXmlDocs)
            {
                if (referenceDoc->GetElementById(typeId) != nullptr)
                {
                    found = true;
                    break;
                }
            }
            if (found)
            {
                continue;
            }
            if (type->GetSymbolType() == SymbolType::derivedTypeSymbol)
            {
                DerivedTypeSymbol* derivedType = static_cast<DerivedTypeSymbol*>(type);
                if (!derivedType->BaseType()->IsProject())
                {
                    derivedType->BaseType()->SetProject();
                    changed = true;
                }
            }
            else if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
            {
                ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
                if (!specialization->GetClassTemplate()->IsProject())
                {
                    specialization->GetClassTemplate()->SetProject();
                    changed = true;
                }
                for (TypeSymbol* typeArgument : specialization->TemplateArgumentTypes())
                {
                    if (!typeArgument->IsProject())
                    {
                        typeArgument->SetProject();
                        changed = true;
                    }
                }
            }
        }
    }
    for (TypeSymbol* type : types)
    {
        if (!type->IsProject()) continue;
        liveTypes.push_back(type);
    }
    std::sort(liveTypes.begin(), liveTypes.end(), ByDocName());
    auto it = std::unique(liveTypes.begin(), liveTypes.end(), IdEqual());
    liveTypes.erase(it, liveTypes.end());
    for (TypeSymbol* type : liveTypes)
    {
        std::unique_ptr<Element> typeElement(new Element(U"type"));
        if (type->IsBasicTypeSymbol())
        {
            typeElement->SetAttribute(U"id", U"type." + type->Id());
            typeElement->SetAttribute(U"basic", U"true");
        }
        else if (type->GetSymbolType() == SymbolType::derivedTypeSymbol)
        {
            typeElement->SetAttribute(U"id", U"type." + type->Id());
            typeElement->SetAttribute(U"derived", U"true");
        }
        else if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            typeElement->SetAttribute(U"id", U"type." + type->Id());
            typeElement->SetAttribute(U"specialization", U"true");
        }
        else 
        {
            bool found = false;
            int n = referenceXmlDocs.size();
            for (int i = 0; i < n; ++i)
            {
                Document* doc = referenceXmlDocs[i].get();
                Element* element = doc->GetElementById(type->Id());
                if (!element)
                {
                    element = doc->GetElementById(U"type." + type->Id());
                }
                if (element)
                {
                    found = true;
                    break;
                }
            }
            if (found)
            {
                continue;
            }
            typeElement->SetAttribute(U"id", type->Id());
        }
        std::unique_ptr<Element> typeNameElement(new Element(U"name"));
        typeNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(type->DocName())));
        typeElement->AppendChild(std::move(typeNameElement));
        if (type->GetSymbolType() == SymbolType::derivedTypeSymbol)
        {
            std::unique_ptr<Element> contentElement(new Element(U"content"));
            DerivedTypeSymbol* derivedType = static_cast<DerivedTypeSymbol*>(type);
            if (derivedType->IsConstType())
            {
                std::unique_ptr<Element> qualifierElement(new Element(U"qualifier"));
                qualifierElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"const")));
                contentElement->AppendChild(std::move(qualifierElement));
            }
            std::unique_ptr<Element> baseTypeElement(new Element(U"baseType"));
            baseTypeElement->SetAttribute(U"ref", GetTypeId(derivedType->BaseType()));
            contentElement->AppendChild(std::move(baseTypeElement));
            const TypeDerivationRec& derivationRec = derivedType->DerivationRec();
            for (Derivation derivation : derivationRec.derivations)
            {
                std::u32string qualifier;
                switch (derivation)
                {
                    case Derivation::pointerDerivation: qualifier = U"*"; break;
                    case Derivation::lvalueRefDerivation: qualifier = U"&"; break;
                    case Derivation::rvalueRefDerivation: qualifier = U"&&"; break;
                    default: continue;
                }
                std::unique_ptr<Element> qualifierElement(new Element(U"qualifier"));
                qualifierElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(qualifier)));
                contentElement->AppendChild(std::move(qualifierElement));
            }
            typeElement->AppendChild(std::move(contentElement));
        }
        else if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            std::unique_ptr<Element> contentElement(new Element(U"content"));
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
            std::unique_ptr<Element> primaryTypeElement(new Element(U"primaryType"));
            primaryTypeElement->SetAttribute(U"ref", specialization->GetClassTemplate()->Prototype()->Id());
            contentElement->AppendChild(std::move(primaryTypeElement));
            for (TypeSymbol* templateArgumentType : specialization->TemplateArgumentTypes())
            {
                std::unique_ptr<Element> templateArgumentTypeElement(new Element(U"argumentType"));
                templateArgumentTypeElement->SetAttribute(U"ref", GetTypeId(templateArgumentType));
                contentElement->AppendChild(std::move(templateArgumentTypeElement));
            }
            typeElement->AppendChild(std::move(contentElement));
        }
        typesElement->AppendChild(std::move(typeElement));
    }
    parentElement->AppendChild(std::move(typesElement));
}

void GenerateXmlForParameter(ParameterSymbol* parameter, Element* parametersElement)
{
    std::unique_ptr<Element> parameterElement(new Element(U"parameter"));
    std::unique_ptr<Element> parameterNameElement(new Element(U"name"));
    std::u32string docName = parameter->DocName();
    parameterNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(docName)));
    parameterElement->AppendChild(std::move(parameterNameElement));
    std::unique_ptr<Element> parameterTypeElement(new Element(U"type"));
    if (parameter->GetType()->GetSymbolType() == SymbolType::derivedTypeSymbol)
    {
        parameter->GetType()->SetProject();
    }
    parameterTypeElement->SetAttribute(U"ref", GetTypeId(parameter->GetType()));
    parameterElement->AppendChild(std::move(parameterTypeElement));
    parametersElement->AppendChild(std::move(parameterElement));
}

void GenerateXmlForFunctions(Element* parentElement, const std::vector<FunctionSymbol*>& functions, int startParam)
{
    if (functions.empty()) return;
    std::vector<FunctionSymbol*> constructors;
    std::vector<FunctionSymbol*> staticMemberFunctions;
    for (FunctionSymbol* function : functions)
    {
        if (function->GetSymbolType() == SymbolType::constructorSymbol)
        {
            if (!function->IsSuppressed())
            {
                constructors.push_back(function);
            }
        }
        else if (function->IsStatic())
        {
            if (!function->IsSuppressed())
            {
                staticMemberFunctions.push_back(function);
            }
        }
    }
    if (!constructors.empty())
    {
        std::unique_ptr<Element> constructorsElement(new Element(U"constructors"));
        std::sort(constructors.begin(), constructors.end(), ByDocName());
        for (FunctionSymbol* constructor : constructors)
        {
            std::unique_ptr<Element> constructorElement(new Element(U"constructor"));
            if (constructor->Id().empty())
            {
                constructor->ComputeMangledName();
            }
            constructorElement->SetAttribute(U"id", constructor->Id());
            std::unique_ptr<Element> constructorNameElement(new Element(U"name"));
            std::u32string docName = constructor->DocName();
            constructorNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(docName)));
            constructorElement->AppendChild(std::move(constructorNameElement));
            std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
            syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(constructor->Syntax()))));
            constructorElement->AppendChild(std::move(syntaxElement));
            int n = constructor->Parameters().size();
            if (n > 1)
            {
                std::unique_ptr<Element> constructorParametersElement(new Element(U"parameters"));
                for (int i = 1; i < n; ++i)
                {
                    ParameterSymbol* parameter = constructor->Parameters()[i];
                    GenerateXmlForParameter(parameter, constructorParametersElement.get());
                }
                constructorElement->AppendChild(std::move(constructorParametersElement));
            }
            constructorsElement->AppendChild(std::move(constructorElement));
        }
        parentElement->AppendChild(std::move(constructorsElement));
    }
    std::map<std::u32string, std::vector<FunctionSymbol*>> functionsByGroupName;
    std::map<std::u32string, std::vector<FunctionSymbol*>> staticFunctionsByGroupName;
    bool hasFunctions = false;
    bool hasStaticFunctions = false;
    for (FunctionSymbol* function : functions)
    {
        if (function->GetSymbolType() != SymbolType::constructorSymbol)
        {
            if (!function->IsStatic())
            {
                if (!function->IsSuppressed())
                {
                    functionsByGroupName[function->GroupName()].push_back(function);
                    hasFunctions = true;
                }
            }
            else
            {
                if (!function->IsSuppressed())
                {
                    staticFunctionsByGroupName[function->GroupName()].push_back(function);
                    hasStaticFunctions = true;
                }
            }
        }
    }
    if (hasFunctions)
    {
        std::unique_ptr<Element> functionsElement(new Element(U"functions"));
        for (auto& p : functionsByGroupName)
        {
            const std::u32string& groupName = p.first;
            std::vector<FunctionSymbol*>& overloads = p.second;
            std::unique_ptr<Element> functionElement(new Element(U"function"));
            std::unique_ptr<Element> functionNameElement(new Element(U"name"));
            if (overloads.size() > 1)
            {
                functionNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(groupName)));
                functionElement->AppendChild(std::move(functionNameElement));
                if (overloads.front()->FunctionGroup()->Id().empty())
                {
                    overloads.front()->FunctionGroup()->ComputeMangledName();
                    functionElement->SetAttribute(U"id", overloads.front()->FunctionGroup()->Id());
                }
                std::sort(overloads.begin(), overloads.end(), ByDocName());
                std::unique_ptr<Element> overloadsElement(new Element(U"overloads"));
                std::unordered_set<std::u32string> docNames;
                bool sameDocName = false;
                for (FunctionSymbol* overload : overloads)
                {
                    if (docNames.find(overload->DocName()) != docNames.end())
                    {
                        sameDocName = true;
                        break;
                    }
                    docNames.insert(overload->DocName());
                }
                for (FunctionSymbol* overload : overloads)
                {
                    std::unique_ptr<Element> overloadElement(new Element(U"overload"));
                    if (overload->Id().empty())
                    {
                        overload->ComputeMangledName();
                    }
                    overloadElement->SetAttribute(U"id", overload->Id());
                    std::unique_ptr<Element> overloadNameElement(new Element(U"name"));
                    std::u32string docName = overload->DocName();
                    if (sameDocName && overload->Constraint())
                    {
                        docName.append(1, ' ').append(ToUtf32(overload->Constraint()->ToString()));
                    }
                    overloadNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(docName)));
                    overloadElement->AppendChild(std::move(overloadNameElement));
                    std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
                    syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(overload->Syntax()))));
                    overloadElement->AppendChild(std::move(syntaxElement));
                    if (!overload->TemplateParameters().empty())
                    {
                        std::unique_ptr<Element> templateParametersElement(new Element(U"templateParameters"));
                        for (TemplateParameterSymbol* templateParameter : overload->TemplateParameters())
                        {
                            std::unique_ptr<Element> templateParameterElement(new Element(U"templateParameter"));
                            templateParameterElement->SetAttribute(U"id", templateParameter->Id());
                            std::unique_ptr<Element> templateParameterNameElement(new Element(U"name"));
                            templateParameterNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(templateParameter->DocName())));
                            templateParameterElement->AppendChild(std::move(templateParameterNameElement));
                            if (templateParameter->HasDefault())
                            {
                                std::unique_ptr<Element> defaultElement(new Element(U"default"));
                                defaultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(templateParameter->DefaultStr()))));
                                templateParameterElement->AppendChild(std::move(defaultElement));
                            }
                            templateParametersElement->AppendChild(std::move(templateParameterElement));
                        }
                        overloadElement->AppendChild(std::move(templateParametersElement));
                    }
                    int n = overload->Parameters().size();
                    if (n > startParam)
                    {
                        std::unique_ptr<Element> overloadParametersElement(new Element(U"parameters"));
                        for (int i = startParam; i < n; ++i)
                        {
                            ParameterSymbol* parameter = overload->Parameters()[i];
                            GenerateXmlForParameter(parameter, overloadParametersElement.get());
                        }
                        overloadElement->AppendChild(std::move(overloadParametersElement));
                    }
                    if (overload->ReturnType() && !overload->ReturnType()->IsVoidType())
                    {
                        std::unique_ptr<Element> returnTypeElement(new Element(U"returnType"));
                        returnTypeElement->SetAttribute(U"ref", GetTypeId(overload->ReturnType()));
                        overload->ReturnType()->SetProject();
                        overloadElement->AppendChild(std::move(returnTypeElement));
                    }
                    if (overload->Constraint())
                    {
                        std::unique_ptr<Element> constraintElement = ConstraintToXml(overload->Constraint(), overload);
                        overloadElement->AppendChild(std::move(constraintElement));
                    }
                    overloadsElement->AppendChild(std::move(overloadElement));
                }
                functionElement->AppendChild(std::move(overloadsElement));
            }
            else if (!overloads.empty())
            {
                FunctionSymbol* function = overloads.front();
                if (function->Id().empty())
                {
                    function->ComputeMangledName();
                }
                functionElement->SetAttribute(U"id", function->Id());
                functionNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(function->DocName())));
                functionElement->AppendChild(std::move(functionNameElement));
                std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
                syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(function->Syntax()))));
                functionElement->AppendChild(std::move(syntaxElement));
                if (!function->TemplateParameters().empty())
                {
                    std::unique_ptr<Element> templateParametersElement(new Element(U"templateParameters"));
                    for (TemplateParameterSymbol* templateParameter : function->TemplateParameters())
                    {
                        std::unique_ptr<Element> templateParameterElement(new Element(U"templateParameter"));
                        templateParameterElement->SetAttribute(U"id", templateParameter->Id());
                        std::unique_ptr<Element> templateParameterNameElement(new Element(U"name"));
                        templateParameterNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(templateParameter->DocName())));
                        templateParameterElement->AppendChild(std::move(templateParameterNameElement));
                        if (templateParameter->HasDefault())
                        {
                            std::unique_ptr<Element> defaultElement(new Element(U"default"));
                            defaultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(templateParameter->DefaultStr()))));
                            templateParameterElement->AppendChild(std::move(defaultElement));
                        }
                        templateParametersElement->AppendChild(std::move(templateParameterElement));
                    }
                    functionElement->AppendChild(std::move(templateParametersElement));
                }
                int n = function->Parameters().size();
                if (n > startParam)
                {
                    std::unique_ptr<Element> functionParametersElement(new Element(U"parameters"));
                    for (int i = startParam; i < n; ++i)
                    {
                        ParameterSymbol* parameter = function->Parameters()[i];
                        GenerateXmlForParameter(parameter, functionParametersElement.get());
                    }
                    functionElement->AppendChild(std::move(functionParametersElement));
                }
                if (function->ReturnType() && !function->ReturnType()->IsVoidType())
                {
                    std::unique_ptr<Element> returnTypeElement(new Element(U"returnType"));
                    returnTypeElement->SetAttribute(U"ref", GetTypeId(function->ReturnType()));
                    function->ReturnType()->SetProject();
                    functionElement->AppendChild(std::move(returnTypeElement));
                }
                if (function->Constraint())
                {
                    std::unique_ptr<Element> constraintElement = ConstraintToXml(function->Constraint(), function);
                    functionElement->AppendChild(std::move(constraintElement));
                }
            }
            functionsElement->AppendChild(std::move(functionElement));
        }
        parentElement->AppendChild(std::move(functionsElement));
    }
    if (hasStaticFunctions)
    {
        std::unique_ptr<Element> functionsElement(new Element(U"staticMemberFunctions"));
        for (auto& p : staticFunctionsByGroupName)
        {
            const std::u32string& groupName = p.first;
            std::vector<FunctionSymbol*>& overloads = p.second;
            std::unique_ptr<Element> functionElement(new Element(U"function"));
            std::unique_ptr<Element> functionNameElement(new Element(U"name"));
            if (overloads.size() > 1)
            {
                functionNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(groupName)));
                functionElement->AppendChild(std::move(functionNameElement));
                if (overloads.front()->FunctionGroup()->Id().empty())
                {
                    overloads.front()->FunctionGroup()->ComputeMangledName();
                    functionElement->SetAttribute(U"id", overloads.front()->FunctionGroup()->Id());
                }
                std::sort(overloads.begin(), overloads.end(), ByDocName());
                std::unique_ptr<Element> overloadsElement(new Element(U"overloads"));
                std::unordered_set<std::u32string> docNames;
                bool sameDocName = false;
                for (FunctionSymbol* overload : overloads)
                {
                    if (docNames.find(overload->DocName()) != docNames.end())
                    {
                        sameDocName = true;
                        break;
                    }
                    docNames.insert(overload->DocName());
                }
                for (FunctionSymbol* overload : overloads)
                {
                    std::unique_ptr<Element> overloadElement(new Element(U"overload"));
                    if (overload->Id().empty())
                    {
                        overload->ComputeMangledName();
                    }
                    overloadElement->SetAttribute(U"id", overload->Id());
                    std::unique_ptr<Element> overloadNameElement(new Element(U"name"));
                    std::u32string docName = overload->DocName();
                    if (sameDocName && overload->Constraint())
                    {
                        docName.append(1, ' ').append(ToUtf32(overload->Constraint()->ToString()));
                    }
                    overloadNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(docName)));
                    overloadElement->AppendChild(std::move(overloadNameElement));
                    std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
                    syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(overload->Syntax()))));
                    overloadElement->AppendChild(std::move(syntaxElement));
                    if (!overload->TemplateParameters().empty())
                    {
                        std::unique_ptr<Element> templateParametersElement(new Element(U"templateParameters"));
                        for (TemplateParameterSymbol* templateParameter : overload->TemplateParameters())
                        {
                            std::unique_ptr<Element> templateParameterElement(new Element(U"templateParameter"));
                            templateParameterElement->SetAttribute(U"id", templateParameter->Id());
                            std::unique_ptr<Element> templateParameterNameElement(new Element(U"name"));
                            templateParameterNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(templateParameter->DocName())));
                            templateParameterElement->AppendChild(std::move(templateParameterNameElement));
                            if (templateParameter->HasDefault())
                            {
                                std::unique_ptr<Element> defaultElement(new Element(U"default"));
                                defaultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(templateParameter->DefaultStr()))));
                                templateParameterElement->AppendChild(std::move(defaultElement));
                            }
                            templateParametersElement->AppendChild(std::move(templateParameterElement));
                        }
                        overloadElement->AppendChild(std::move(templateParametersElement));
                    }
                    int n = overload->Parameters().size();
                    if (n > 0)
                    {
                        std::unique_ptr<Element> overloadParametersElement(new Element(U"parameters"));
                        for (int i = 0; i < n; ++i)
                        {
                            ParameterSymbol* parameter = overload->Parameters()[i];
                            GenerateXmlForParameter(parameter, overloadParametersElement.get());
                        }
                        overloadElement->AppendChild(std::move(overloadParametersElement));
                    }
                    if (overload->ReturnType() && !overload->ReturnType()->IsVoidType())
                    {
                        std::unique_ptr<Element> returnTypeElement(new Element(U"returnType"));
                        returnTypeElement->SetAttribute(U"ref", GetTypeId(overload->ReturnType()));
                        overload->ReturnType()->SetProject();
                        overloadElement->AppendChild(std::move(returnTypeElement));
                    }
                    if (overload->Constraint())
                    {
                        std::unique_ptr<Element> constraintElement = ConstraintToXml(overload->Constraint(), overload);
                        overloadElement->AppendChild(std::move(constraintElement));
                    }
                    overloadsElement->AppendChild(std::move(overloadElement));
                }
                functionElement->AppendChild(std::move(overloadsElement));
            }
            else if (!overloads.empty())
            {
                FunctionSymbol* function = overloads.front();
                if (function->Id().empty())
                {
                    function->ComputeMangledName();
                }
                functionElement->SetAttribute(U"id", function->Id());
                functionNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(function->DocName())));
                functionElement->AppendChild(std::move(functionNameElement));
                std::unique_ptr<Element> syntaxElement(new Element(U"syntax"));
                syntaxElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(function->Syntax()))));
                functionElement->AppendChild(std::move(syntaxElement));
                if (!function->TemplateParameters().empty())
                {
                    std::unique_ptr<Element> templateParametersElement(new Element(U"templateParameters"));
                    for (TemplateParameterSymbol* templateParameter : function->TemplateParameters())
                    {
                        std::unique_ptr<Element> templateParameterElement(new Element(U"templateParameter"));
                        templateParameterElement->SetAttribute(U"id", templateParameter->Id());
                        std::unique_ptr<Element> templateParameterNameElement(new Element(U"name"));
                        templateParameterNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(templateParameter->DocName())));
                        templateParameterElement->AppendChild(std::move(templateParameterNameElement));
                        templateParametersElement->AppendChild(std::move(templateParameterElement));
                        if (templateParameter->HasDefault())
                        {
                            std::unique_ptr<Element> defaultElement(new Element(U"default"));
                            defaultElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(ToUtf32(templateParameter->DefaultStr()))));
                            templateParameterElement->AppendChild(std::move(defaultElement));
                        }
                    }
                    functionElement->AppendChild(std::move(templateParametersElement));
                }
                int n = function->Parameters().size();
                if (n > 0)
                {
                    std::unique_ptr<Element> functionParametersElement(new Element(U"parameters"));
                    for (int i = 0; i < n; ++i)
                    {
                        ParameterSymbol* parameter = function->Parameters()[i];
                        GenerateXmlForParameter(parameter, functionParametersElement.get());
                    }
                    functionElement->AppendChild(std::move(functionParametersElement));
                }
                if (function->ReturnType() && !function->ReturnType()->IsVoidType())
                {
                    std::unique_ptr<Element> returnTypeElement(new Element(U"returnType"));
                    returnTypeElement->SetAttribute(U"ref", GetTypeId(function->ReturnType()));
                    function->ReturnType()->SetProject();
                    functionElement->AppendChild(std::move(returnTypeElement));
                }
                if (function->Constraint())
                {
                    std::unique_ptr<Element> constraintElement = ConstraintToXml(function->Constraint(), function);
                    functionElement->AppendChild(std::move(constraintElement));
                }
            }
            functionsElement->AppendChild(std::move(functionElement));
        }
        parentElement->AppendChild(std::move(functionsElement));
    }
}

void GenerateLibraryXml(SymbolTable& symbolTable, std::map<std::u32string, SymbolCollector>& byNs, std::map<std::u32string, std::u32string>& nsMap, Document* libraryXmlDoc,
    const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs)
{
    std::vector<TypeSymbol*> types = symbolTable.Types();
    std::unique_ptr<Element> namespacesElement(new Element(U"namespaces"));
    for (auto& p : byNs)
    {
        const std::u32string& nsName = p.first;
        if (nsName.empty())
        {
            continue;
        }
        SymbolCollector& collector = p.second;
        if (collector.IsEmpty())
        {
            continue; 
        }
        collector.SortByDocName();
        std::u32string namespaceId;
        auto it = nsMap.find(nsName);
        if (it != nsMap.cend())
        {
            namespaceId = it->second;
        }
        std::unique_ptr<Element> namespaceElement(new Element(U"namespace"));
        if (!namespaceId.empty())
        {
            namespaceElement->SetAttribute(U"id", namespaceId);
        }
        std::unique_ptr<Element> namespaceNameElement(new Element(U"name"));
        namespaceNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(nsName)));
        namespaceElement->AppendChild(std::move(namespaceNameElement));
        GenerateXmlForClasses(namespaceElement.get(), collector.Classes());
        GenerateXmlForFunctions(namespaceElement.get(), collector.Functions(), 0);
        GenerateXmlForTypedefs(namespaceElement.get(), collector.Typedefs());
        GenerateXmlForConcepts(namespaceElement.get(), collector.Concepts());
        GenerateXmlForDelegates(namespaceElement.get(), collector.Delegates());
        GenerateXmlForClassDelegates(namespaceElement.get(), collector.ClassDelegates());
        GenerateXmlForConstants(namespaceElement.get(), collector.Constants());
        GenerateXmlForEnumeratedTypes(namespaceElement.get(), collector.EnumeratedTypes());
        namespacesElement->AppendChild(std::move(namespaceElement));
    }
    libraryXmlDoc->DocumentElement()->AppendChild(std::move(namespacesElement));
    GenerateXmlForTypes(libraryXmlDoc->DocumentElement(), types, referenceXmlFilePaths, referenceXmlDocs);
}

void DistributeToNamespaces(std::map<std::u32string, SymbolCollector>& byNs, std::map<std::u32string, std::u32string>& nsMap,  SymbolCollector& collector)
{
    for (ClassTypeSymbol* cls : collector.Classes())
    {
        SymbolCollector& nsCollector = byNs[cls->Ns()->FullName()];
        nsMap[cls->Ns()->FullName()] = cls->Ns()->Id();
        nsCollector.AddClass(cls);
    }
    for (FunctionSymbol* fun : collector.Functions())
    {
        SymbolCollector& nsCollector = byNs[fun->Ns()->FullName()];
        nsMap[fun->Ns()->FullName()] = fun->Ns()->Id();
        nsCollector.AddFunction(fun);
    }
    for (InterfaceTypeSymbol* intf : collector.Interfaces())
    {
        SymbolCollector& nsCollector = byNs[intf->Ns()->FullName()];
        nsMap[intf->Ns()->FullName()] = intf->Ns()->Id();
        nsCollector.AddInterface(intf);
    }
    for (TypedefSymbol* typedef_ : collector.Typedefs())
    {
        SymbolCollector& nsCollector = byNs[typedef_->Ns()->FullName()];
        nsMap[typedef_->Ns()->FullName()] = typedef_->Ns()->Id();
        nsCollector.AddTypedef(typedef_);
    }
    for (ConceptSymbol* concept : collector.Concepts())
    {
        SymbolCollector& nsCollector = byNs[concept->Ns()->FullName()];
        nsMap[concept->Ns()->FullName()] = concept->Ns()->Id();
        nsCollector.AddConcept(concept);
    }
    for (DelegateTypeSymbol* delegate : collector.Delegates())
    {
        SymbolCollector& nsCollector = byNs[delegate->Ns()->FullName()];
        nsMap[delegate->Ns()->FullName()] = delegate->Ns()->Id();
        nsCollector.AddDelegate(delegate);
    }
    for (ClassDelegateTypeSymbol* classDelegate : collector.ClassDelegates())
    {
        SymbolCollector& nsCollector = byNs[classDelegate->Ns()->FullName()];
        nsMap[classDelegate->Ns()->FullName()] = classDelegate->Ns()->Id();
        nsCollector.AddClassDelegate(classDelegate);
    }
    for (ConstantSymbol* constant : collector.Constants())
    {
        SymbolCollector& nsCollector = byNs[constant->Ns()->FullName()];
        nsMap[constant->Ns()->FullName()] = constant->Ns()->Id();
        nsCollector.AddConstant(constant);
    }
    for (EnumTypeSymbol* enumType : collector.EnumeratedTypes())
    {
        SymbolCollector& nsCollector = byNs[enumType->Ns()->FullName()];
        nsMap[enumType->Ns()->FullName()] = enumType->Ns()->Id();
        nsCollector.AddEnumeratedType(enumType);
    }
}

struct CheckRefsVisitor : sngxml::dom::Visitor
{
    CheckRefsVisitor(sngxml::dom::Document* document_, const std::vector<std::unique_ptr<sngxml::dom::Document>>& referenceXmlDocs_) : document(document_), referenceXmlDocs(referenceXmlDocs_)
    {
    }
    void BeginVisit(Element* element) override
    {
        std::u32string ref = element->GetAttribute(U"ref");
        if (!ref.empty())
        {
            Element* refencedElement = document->GetElementById(ref);
            if (!refencedElement)
            {
                int n = referenceXmlDocs.size();
                for (int i = 0; i < n; ++i) 
                {
                    sngxml::dom::Document* referenceDocuments = referenceXmlDocs[i].get();
                    refencedElement = referenceDocuments->GetElementById(ref);
                    if (refencedElement)
                    {
                        break;
                    }
                }
                if (!refencedElement)
                {
                    std::cout << "ref " << ToUtf8(ref) << " not found" << std::endl;
                }
            }
        }
    }
    sngxml::dom::Document* document;
    const std::vector<std::unique_ptr<sngxml::dom::Document>>& referenceXmlDocs;
};

std::unique_ptr<Document> GenerateLibraryXmlFile(const std::string& moduleFilePath, const std::string& libraryXmlFilePath, 
    const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, 
    const std::u32string& stylePath)
{
    if (verbose)
    {
        std::cout << "generating xml for module " << moduleFilePath << "..." << std::endl;
    }
    boost::filesystem::path cmmPath(moduleFilePath);
    if (cmmPath.extension() != ".cmm")
    {
        throw std::runtime_error("File path '" + cmmPath.generic_string() + "' has invalid extension. Not Cmajor module file (.cmm).");
    }
    if (!boost::filesystem::exists(cmmPath))
    {
        throw std::runtime_error("Cmajor module file '" + moduleFilePath + "' not found.");
    }
    std::vector<ClassTypeSymbol*> classTypes;
    std::vector<ClassTemplateSpecializationSymbol*> classTemplateSpecializations;
    Module module(moduleFilePath, classTypes, classTemplateSpecializations);
    if (module.Name() == U"System.Base")
    {
        cmajor::symbols::MetaInit(module.GetSymbolTable());
    }
    std::unique_ptr<ModuleBinder> moduleBinder;
    CompileUnitNode compileUnit(Span(), "foo");
    AttributeBinder attributeBinder(&module);
    moduleBinder.reset(new ModuleBinder(module, &compileUnit, &attributeBinder));
    moduleBinder->SetBindingTypes();
    module.GetSymbolTable().AddClassTemplateSpecializationsToClassTemplateSpecializationMap(classTemplateSpecializations);
    for (ClassTemplateSpecializationSymbol* classTemplateSpecialization : classTemplateSpecializations)
    {
        moduleBinder->BindClassTemplateSpecialization(classTemplateSpecialization);
    }
    for (ClassTypeSymbol* classType : classTypes)
    {
        classType->SetSpecialMemberFunctions();
        classType->CreateLayouts();
    }
    SymbolCollector collector;
    module.GetSymbolTable().GlobalNs().Accept(&collector);
    std::map<std::u32string, SymbolCollector> collectorMap;
    std::map<std::u32string, std::u32string> nsMap;
    DistributeToNamespaces(collectorMap, nsMap, collector);
    std::unique_ptr<Document> libraryXmlDoc(new Document());
    std::unique_ptr<Element> libraryElement(new Element(U"library"));
    std::unique_ptr<Element> libraryNameElement(new Element(U"name"));
    libraryNameElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(module.Name())));
    libraryElement->AppendChild(std::move(libraryNameElement));
    if (!stylePath.empty())
    {
        std::unique_ptr<Element> styleElement(new Element(U"style"));
        styleElement->AppendChild((std::unique_ptr<sngxml::dom::Node>(new Text(stylePath))));
        libraryElement->AppendChild(std::move(styleElement));
    }
    libraryXmlDoc->AppendChild(std::move(libraryElement));
    GenerateLibraryXml(module.GetSymbolTable(), collectorMap, nsMap, libraryXmlDoc.get(), referenceXmlFilePaths, referenceXmlDocs);
    CheckRefsVisitor checkRefsVisitor(libraryXmlDoc.get(), referenceXmlDocs);
    libraryXmlDoc->DocumentElement()->Accept(checkRefsVisitor);
    std::ofstream libraryXmlFile(libraryXmlFilePath);
    CodeFormatter formatter(libraryXmlFile);
    if (verbose)
    {
        std::cout << "=> " << libraryXmlFilePath << std::endl;
    }
    libraryXmlDoc->Write(formatter);
    return libraryXmlDoc;
}

std::u32string GetLibraryName(Document* libraryXmlDoc)
{
    std::unique_ptr<cmajor::xpath::XPathObject> libraryNameObject = cmajor::xpath::Evaluate(U"/library/name/text()", libraryXmlDoc);
    if (libraryNameObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("nodeset expected");
    }
    cmajor::xpath::XPathNodeSet* libraryNameNodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(libraryNameObject.get());
    if (libraryNameNodeSet->Length() != 1)
    {
        throw std::runtime_error("one node expected");
    }
    sngxml::dom::Node* libraryNameNode = (*libraryNameNodeSet)[0];
    if (libraryNameNode->GetNodeType() != sngxml::dom::NodeType::textNode)
    {
        throw std::runtime_error("text node expected");
    }
    sngxml::dom::Text* libraryName = static_cast<sngxml::dom::Text*>(libraryNameNode);
    return libraryName->Data();
}

std::vector<sngxml::dom::Element*> GetNamespaceElements(Document* libraryXmlDoc)
{
    std::vector<sngxml::dom::Element*> namespaceElements;
    std::unique_ptr<cmajor::xpath::XPathObject> namespaceObject = cmajor::xpath::Evaluate(U"/library/namespaces/namespace", libraryXmlDoc);
    if (namespaceObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("nodeset expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(namespaceObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            namespaceElements.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return namespaceElements;
}

std::vector<sngxml::dom::Element*> GetOverloadElements(Element* parentElement)
{
    std::vector<sngxml::dom::Element*> overloadElements;
    std::unique_ptr<cmajor::xpath::XPathObject> overloadsObject = cmajor::xpath::Evaluate(U"overloads/overload", parentElement);
    if (overloadsObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("nodeset expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(overloadsObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            overloadElements.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return overloadElements;
}

std::u32string GetStylePath(Document* libraryXmlDoc)
{
    std::unique_ptr<cmajor::xpath::XPathObject> stylePathObject = cmajor::xpath::Evaluate(U"/library/style/text()", libraryXmlDoc);
    if (stylePathObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("nodeset expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(stylePathObject.get());
    if (nodeSet->Length() == 0)
    {
        return std::u32string();
    }
    if (nodeSet->Length() != 1)
    {
        throw std::runtime_error("one node expected");
    }
    sngxml::dom::Node* node = (*nodeSet)[0];
    if (node->GetNodeType() == sngxml::dom::NodeType::textNode)
    {
        sngxml::dom::Text* text = static_cast<sngxml::dom::Text*>(node);
        return text->Data();
    }
    else
    {
        throw std::runtime_error("text node expected");
    }
}

std::u32string GetName(sngxml::dom::Element* element, bool optional)
{
    std::unique_ptr<cmajor::xpath::XPathObject> nameObject = cmajor::xpath::Evaluate(U"name/text()", element);
    if (nameObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("nodeset expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(nameObject.get());
    if (nodeSet->Length() != 1)
    {
        if (optional)
        {
            return std::u32string();
        }
        throw std::runtime_error("one node expected");
    }
    sngxml::dom::Node* nameNode = (*nodeSet)[0];
    if (nameNode->GetNodeType() != sngxml::dom::NodeType::textNode)
    {
        throw std::runtime_error("text node expected");
    }
    sngxml::dom::Text* name = static_cast<sngxml::dom::Text*>(nameNode);
    return name->Data();
}

std::u32string GetDefault(sngxml::dom::Element* element)
{
    std::unique_ptr<cmajor::xpath::XPathObject> nameObject = cmajor::xpath::Evaluate(U"default/text()", element);
    if (nameObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("nodeset expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(nameObject.get());
    if (nodeSet->Length() != 1)
    {
        return std::u32string();
    }
    sngxml::dom::Node* defaultNode = (*nodeSet)[0];
    if (defaultNode->GetNodeType() != sngxml::dom::NodeType::textNode)
    {
        throw std::runtime_error("text node expected");
    }
    sngxml::dom::Text* default_ = static_cast<sngxml::dom::Text*>(defaultNode);
    return default_->Data();
}

std::u32string GetValue(sngxml::dom::Element* element)
{
    std::unique_ptr<cmajor::xpath::XPathObject> valueObject = cmajor::xpath::Evaluate(U"value/text()", element);
    if (valueObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("nodeset expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(valueObject.get());
    if (nodeSet->Length() != 1)
    {
        throw std::runtime_error("one node expected");
    }
    sngxml::dom::Node* valueNode = (*nodeSet)[0];
    if (valueNode->GetNodeType() != sngxml::dom::NodeType::textNode)
    {
        throw std::runtime_error("text node expected");
    }
    sngxml::dom::Text* value = static_cast<sngxml::dom::Text*>(valueNode);
    return value->Data();
}

std::u32string GetGroupName(sngxml::dom::Element* element)
{
    std::unique_ptr<cmajor::xpath::XPathObject> groupNameObject = cmajor::xpath::Evaluate(U"groupName/text()", element);
    if (groupNameObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("nodeset expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(groupNameObject.get());
    if (nodeSet->Length() != 1)
    {
        throw std::runtime_error("one node expected");
    }
    sngxml::dom::Node* groupNameNode = (*nodeSet)[0];
    if (groupNameNode->GetNodeType() != sngxml::dom::NodeType::textNode)
    {
        throw std::runtime_error("text node expected");
    }
    sngxml::dom::Text* groupName = static_cast<sngxml::dom::Text*>(groupNameNode);
    return groupName->Data();
}

std::vector<sngxml::dom::Element*> GetClassElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> classes;
    std::unique_ptr<cmajor::xpath::XPathObject> classObject = cmajor::xpath::Evaluate(U"classes/class", parentElement);
    if (classObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(classObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            classes.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return classes;
}

std::vector<sngxml::dom::Element*> GetConstructorElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> constructors;
    std::unique_ptr<cmajor::xpath::XPathObject> constructorObject = cmajor::xpath::Evaluate(U"constructors/constructor", parentElement);
    if (constructorObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(constructorObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            constructors.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return constructors;
}

std::vector<sngxml::dom::Element*> GetFunctionElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> functions;
    std::unique_ptr<cmajor::xpath::XPathObject> functionObject = cmajor::xpath::Evaluate(U"functions/function", parentElement);
    if (functionObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(functionObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            functions.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return functions;
}

std::vector<sngxml::dom::Element*> GetStaticMemberFunctionElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> functions;
    std::unique_ptr<cmajor::xpath::XPathObject> functionObject = cmajor::xpath::Evaluate(U"staticMemberFunctions/function", parentElement);
    if (functionObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(functionObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            functions.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return functions;
}

std::vector<sngxml::dom::Element*> GetTypedefElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> typedefs;
    std::unique_ptr<cmajor::xpath::XPathObject> typedefObject = cmajor::xpath::Evaluate(U"typedefs/typedef", parentElement);
    if (typedefObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(typedefObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            typedefs.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return typedefs;
}

std::vector<sngxml::dom::Element*> GetConceptElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> concepts;
    std::unique_ptr<cmajor::xpath::XPathObject> conceptObject = cmajor::xpath::Evaluate(U"concepts/concept", parentElement);
    if (conceptObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(conceptObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            concepts.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return concepts;
}

std::vector<sngxml::dom::Element*> GetDelegateElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> delegates;
    std::unique_ptr<cmajor::xpath::XPathObject> delegateObject = cmajor::xpath::Evaluate(U"delegates/delegate", parentElement);
    if (delegateObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(delegateObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            delegates.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return delegates;
}

std::vector<sngxml::dom::Element*> GetClassDelegateElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> classDelegates;
    std::unique_ptr<cmajor::xpath::XPathObject> classDelegateObject = cmajor::xpath::Evaluate(U"classDelegates/classDelegate", parentElement);
    if (classDelegateObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(classDelegateObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            classDelegates.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return classDelegates;
}

std::vector<sngxml::dom::Element*> GetConstantElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> constants;
    std::unique_ptr<cmajor::xpath::XPathObject> constantObject = cmajor::xpath::Evaluate(U"constants/constant", parentElement);
    if (constantObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(constantObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            constants.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return constants;
}

std::vector<sngxml::dom::Element*> GetEnumerationElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> enumerations;
    std::unique_ptr<cmajor::xpath::XPathObject> enumObject = cmajor::xpath::Evaluate(U"enumerations/enumeration", parentElement);
    if (enumObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(enumObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            enumerations.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return enumerations;
}

std::vector<sngxml::dom::Element*> GetMemberVariableElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> memberVariables;
    std::unique_ptr<cmajor::xpath::XPathObject> meberVariableObject = cmajor::xpath::Evaluate(U"memberVariables/memberVariable", parentElement);
    if (meberVariableObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(meberVariableObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            memberVariables.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return memberVariables;
}

std::vector<sngxml::dom::Element*> GetTemplateParameterElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> templateParameters;
    std::unique_ptr<cmajor::xpath::XPathObject> templateParameterObject = cmajor::xpath::Evaluate(U"templateParameters/templateParameter", parentElement);
    if (templateParameterObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(templateParameterObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            templateParameters.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return templateParameters;
}

std::vector<sngxml::dom::Element*> GetParameterElements(sngxml::dom::Element* parentElement)
{
    std::vector<sngxml::dom::Element*> parameters;
    std::unique_ptr<cmajor::xpath::XPathObject> parameterObject = cmajor::xpath::Evaluate(U"parameters/parameter", parentElement);
    if (parameterObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(parameterObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            parameters.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return parameters;
}

void AppendRule(Element* parent)
{
    std::unique_ptr<Element> br(new Element(U"br"));
    parent->AppendChild(std::move(br));
    std::unique_ptr<Element> hr(new Element(U"hr"));
    parent->AppendChild(std::move(hr));
}

sngxml::dom::Element* GetTypeById(const std::u32string& typeId, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs,
    int& index)
{
    sngxml::dom::Element* element = libraryXmlDoc->GetElementById(typeId);
    if (element)
    {
        index = -1;
        return element;
    }
    else
    {
        int docIndex = 0;
        for (const std::unique_ptr<Document>& doc : referenceXmlDocs)
        {
            element = doc->GetElementById(typeId);
            if (element)
            {
                index = docIndex;
                return element;
            }
            ++docIndex;
        }
        return nullptr;
    }
}

Element* GetContentElement(Element* typeElement)
{
    std::unique_ptr<cmajor::xpath::XPathObject> contentObject = cmajor::xpath::Evaluate(U"content", typeElement);
    if (contentObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(contentObject.get());
        if (nodeSet->Length() != 1)
        {
            throw std::runtime_error("one node expected");
        }
        sngxml::dom::Node* node = (*nodeSet)[0];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
            return element;
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
}

Element* GetPrimaryTypeElement(Element* contentElement)
{
    std::unique_ptr<cmajor::xpath::XPathObject> primaryTypeObject = cmajor::xpath::Evaluate(U"primaryType", contentElement);
    if (primaryTypeObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(primaryTypeObject.get());
        if (nodeSet->Length() != 1)
        {
            throw std::runtime_error("one node expected");
        }
        sngxml::dom::Node* node = (*nodeSet)[0];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(node);
            return element;
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
}

std::u32string GetClassGroupName(const std::u32string& classId, Document* libraryXmlDoc, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, int& index)
{
    Element* classElement = libraryXmlDoc->GetElementById(classId);
    if (!classElement)
    {
        int n = referenceXmlDocs.size();
        for (int i = 0; i < n; ++i)
        {
            Document* doc = referenceXmlDocs[i].get();
            classElement = doc->GetElementById(classId);
            if (classElement)
            {
                index = i;
                break;
            }
        }
    }
    if (classElement)
    {
        std::unique_ptr<cmajor::xpath::XPathObject> groupNameObject = cmajor::xpath::Evaluate(U"groupName/text()", classElement);
        if (groupNameObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
        {
            cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(groupNameObject.get());
            if (nodeSet->Length() != 1)
            {
                throw std::runtime_error("one node expected");
            }
            else
            {
                sngxml::dom::Node* node = (*nodeSet)[0];
                if (node->GetNodeType() == sngxml::dom::NodeType::textNode)
                {
                    Text* text = static_cast<Text*>(node);
                    return text->Data();
                }
                else
                {
                    throw std::runtime_error("text node expected");
                }
            }
        }
        else
        {
            throw std::runtime_error("node set expected");
        }
    }
    else
    {
        throw std::runtime_error("class '" + ToUtf8(classId) + "' not found");
    }
}

std::vector<std::u32string> GetArgumentTypeIds(Element* contentElement)
{
    std::vector<std::u32string> argumentTypeIds;
    std::unique_ptr<cmajor::xpath::XPathObject> argumentTypes = cmajor::xpath::Evaluate(U"argumentType", contentElement);
    if (argumentTypes->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(argumentTypes.get());
        int n = nodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* node = (*nodeSet)[i];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                Element* element = static_cast<Element*>(node);
                std::u32string ref = element->GetAttribute(U"ref");
                argumentTypeIds.push_back(ref);
            }
            else
            {
                throw std::runtime_error("element node expected");
            }
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
    return argumentTypeIds;
}

std::vector<Element*> GetEnumConstantElements(Element* enumerationElement)
{
    std::vector<Element*> enumConstantElements;
    std::unique_ptr<cmajor::xpath::XPathObject> enumerationConstantsObject = cmajor::xpath::Evaluate(U"enumConstant", enumerationElement);
    if (enumerationConstantsObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(enumerationConstantsObject.get());
        int n = nodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* node = (*nodeSet)[i];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                Element* element = static_cast<Element*>(node);
                enumConstantElements.push_back(element);
            }
            else
            {
                throw std::runtime_error("element node expected");
            }
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
    return enumConstantElements;
}

void AppendTypeHtml(const std::u32string& typeId, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, 
    Element* parentElement)
{
    int index = -1;
    sngxml::dom::Element* type = GetTypeById(typeId, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, index);
    if (!type)
    {
        parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"typeId " + typeId + U" not found")));
        return;
    }
    if (type->GetAttribute(U"basic") == U"true")
    {
        std::unique_ptr<sngxml::dom::Element> spanElement(new Element(U"span"));
        spanElement->SetAttribute(U"class", U"kw");
        spanElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(GetName(type))));
        parentElement->AppendChild(std::move(spanElement));
    }
    else if (type->GetAttribute(U"derived") == U"true")
    {
        Element* contentElement = GetContentElement(type);
        sngxml::dom::NodeList children = contentElement->ChildNodes();
        int n = children.Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* node = children[i];
            if (node->Name() == U"baseType")
            {
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    Element* baseType = static_cast<Element*>(node);
                    std::u32string ref = baseType->GetAttribute(U"ref");
                    AppendTypeHtml(ref, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, parentElement);
                }
                else
                {
                    throw std::runtime_error("element node expected");
                }
            }
            else if (node->Name() == U"qualifier")
            {
                if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    Element* qualifier = static_cast<Element*>(node);
                    sngxml::dom::NodeList children = qualifier->ChildNodes();
                    if (children.Length() != 1)
                    {
                        throw std::runtime_error("one node expected");
                    }
                    sngxml::dom::Node* child = children[0];
                    if (child->GetNodeType() == sngxml::dom::NodeType::textNode)
                    {
                        sngxml::dom::Text* text = static_cast<Text*>(child);
                        std::u32string qualifierText = text->Data();
                        if (qualifierText == U"const")
                        {
                            std::unique_ptr<Element> span(new Element(U"span"));
                            span->SetAttribute(U"class", U"kw");
                            span->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"const ")));
                            parentElement->AppendChild(std::move(span));
                        }
                        else
                        {
                            parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(qualifierText)));
                        }
                    }
                    else
                    {
                        throw std::runtime_error("text node expected");
                    }
                }
                else
                {
                    throw std::runtime_error("element node expected");
                }
            }
            else
            {
                throw std::runtime_error("baseType or qualifier node expected");
            }
        }
    }
    else if (type->GetAttribute(U"specialization") == U"true")
    {
        Element* contentElement = GetContentElement(type);
        Element* primaryTypeElement = GetPrimaryTypeElement(contentElement);
        std::u32string ref = primaryTypeElement->GetAttribute(U"ref");
        std::u32string groupName = GetClassGroupName(ref, libraryXmlDoc, referenceXmlDocs, index);
        std::unique_ptr<Element> link(new Element(U"a"));
        std::u32string filePath;
        if (index >= 0)
        {
            filePath = ToUtf32(Path::ChangeExtension(ToUtf8(referenceXmlFilePaths[index]), ".html"));
        }
        link->SetAttribute(U"href", filePath + U"#" + ref);
        link->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(groupName)));
        parentElement->AppendChild(std::move(link));
        parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"<")));
        std::vector<std::u32string> argumentTypeIds = GetArgumentTypeIds(contentElement);
        int n = argumentTypeIds.size();
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U", ")));
            }
            AppendTypeHtml(argumentTypeIds[i], libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, parentElement);
        }
        parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U">")));
    }
    else
    {
        std::unique_ptr<Element> link(new Element(U"a"));
        std::u32string filePath;
        if (index >= 0)
        {
            filePath = ToUtf32(Path::ChangeExtension(ToUtf8(referenceXmlFilePaths[index]), ".html"));
        }
        link->SetAttribute(U"href", filePath + U"#" + type->GetAttribute(U"id"));
        std::u32string typeName = GetName(type);
        link->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(typeName)));
        parentElement->AppendChild(std::move(link));
    }
}

void AppendConceptHtml(const std::u32string& conceptId, Document* libraryXmlDoc, Element* parentElement, bool groupName)
{
    Element* conceptElement = libraryXmlDoc->GetElementById(conceptId);
    if (!conceptElement) return;
    std::unique_ptr<Element> link(new Element(U"a"));
    link->SetAttribute(U"href", U"#" + conceptId);
    std::u32string conceptName;
    if (groupName)
    {
        conceptName = GetGroupName(conceptElement);
    }
    else
    {
        conceptName = GetName(conceptElement);
    }
    link->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(conceptName)));
    parentElement->AppendChild(std::move(link));
}

void AppendConstraintHtml(Element* constraintElement, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, 
    Element* parentElement)
{
    sngxml::dom::NodeList children = constraintElement->ChildNodes();
    int n = children.Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = children[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::textNode)
        {
            Text* text = static_cast<Text*>(node);
            parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(text->Data())));
        }
        else if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            Element* element = static_cast<Element*>(node);
            if (element->Name() == U"connector")
            {
                sngxml::dom::Node* firstChild = element->FirstChild();
                if (firstChild && firstChild->GetNodeType() == sngxml::dom::NodeType::textNode)
                {
                    Text* text = static_cast<Text*>(firstChild);
                    std::u32string connector = text->Data();
                    std::unique_ptr<Element> span(new Element(U"span"));
                    span->SetAttribute(U"class", U"kw");
                    span->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U" " + connector + U" ")));
                    parentElement->AppendChild(std::move(span));
                }
                else
                {
                    throw std::runtime_error("text node expected");
                }
            }
            else if (element->Name() == U"openParen")
            {
                parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"( ")));
            }
            else if (element->Name() == U"closeParen")
            {
                parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U")")));
            }
            else if (element->Name() == U"qualifier")
            {
                Element* qualifier = element;
                sngxml::dom::NodeList children = qualifier->ChildNodes();
                if (children.Length() != 1)
                {
                    throw std::runtime_error("one node expected");
                }
                sngxml::dom::Node* child = children[0];
                if (child->GetNodeType() == sngxml::dom::NodeType::textNode)
                {
                    sngxml::dom::Text* text = static_cast<Text*>(child);
                    std::u32string qualifierText = text->Data();
                    if (qualifierText == U"const")
                    {
                        std::unique_ptr<Element> span(new Element(U"span"));
                        span->SetAttribute(U"class", U"kw");
                        span->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"const ")));
                        parentElement->AppendChild(std::move(span));
                    }
                    else
                    {
                        parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(qualifierText)));
                    }
                }
                else
                {
                    throw std::runtime_error("text node expected");
                }
            }
            else if (element->Name() == U"span")
            {
                parentElement->AppendChild(std::move(element->CloneNode(true)));
            }
            else if (element->Name() == U"type")
            {
                std::u32string ref = element->GetAttribute(U"ref");
                AppendTypeHtml(ref, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, parentElement);
            }
            else if (element->Name() == U"concept")
            {
                std::u32string ref = element->GetAttribute(U"ref");
                if (!ref.empty())
                {
                    AppendConceptHtml(ref, libraryXmlDoc, parentElement, true);
                }
                sngxml::dom::Node* firstChild = element->FirstChild();
                if (firstChild && firstChild->GetNodeType() == sngxml::dom::NodeType::elementNode)
                {
                    Element* childElement = static_cast<Element*>(firstChild);
                    if (childElement->Name() == U"params")
                    {
                        sngxml::dom::NodeList children = childElement->ChildNodes();
                        parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"<")));
                        int n = children.Length();
                        for (int i = 0; i < n; ++i)
                        {
                            sngxml::dom::Node* childNode = children[i];
                            if (childNode->GetNodeType() == sngxml::dom::NodeType::elementNode)
                            {
                                Element* childElement = static_cast<Element*>(childNode);
                                if (childElement->Name() == U"type")
                                {
                                    if (i > 0)
                                    {
                                        parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U", ")));
                                    }
                                    std::u32string ref = childElement->GetAttribute(U"ref");
                                    AppendTypeHtml(ref, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, parentElement);
                                }
                                else if (childElement->Name() == U"dot")
                                {
                                    sngxml::dom::Node* firstChild = childElement->FirstChild();
                                    std::u32string content;
                                    if (firstChild && firstChild->GetNodeType() == sngxml::dom::NodeType::textNode)
                                    {
                                        Text* text = static_cast<Text*>(firstChild);
                                        content = text->Data();
                                    }
                                    else
                                    {
                                        throw std::runtime_error("text node expected");
                                    }
                                    parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"." + content)));
                                }
                                else
                                {
                                    throw std::runtime_error("type or dot expected");
                                }
                            }
                            else
                            {
                                throw std::runtime_error("element expected");
                            }
                        }
                        parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U">")));
                    }
                    else
                    {
                        throw std::runtime_error("params element expected");
                    }
                }
            }
            else if (element->Name() == U"dot")
            {
                sngxml::dom::Node* firstChild = element->FirstChild();
                std::u32string content;
                if (firstChild && firstChild->GetNodeType() == sngxml::dom::NodeType::textNode)
                {
                    Text* text = static_cast<Text*>(firstChild);
                    content = text->Data();
                }
                else
                {
                    throw std::runtime_error("text node expected");
                }
                parentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"." + content)));
            }
            else
            {
                throw std::runtime_error("connector, type or concept expected");
            }
        }
        else
        {
            throw std::runtime_error("element or text node expected");
        }
    }
}

Element* GetBaseClassElement(Element* classElement)
{
    std::unique_ptr<cmajor::xpath::XPathObject> baseClassObject = cmajor::xpath::Evaluate(U"baseClass", classElement);
    if (baseClassObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(baseClassObject.get());
        if (nodeSet->Length() == 1)
        {
            sngxml::dom::Node* node = (*nodeSet)[0];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                return static_cast<Element*>(node);
            }
            else
            {
                throw std::runtime_error("element node expected");
            }
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
}

Element* GetRefinesElement(Element* conceptElement)
{
    std::unique_ptr<cmajor::xpath::XPathObject> refinedObject = cmajor::xpath::Evaluate(U"refines", conceptElement);
    if (refinedObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(refinedObject.get());
        if (nodeSet->Length() == 1)
        {
            sngxml::dom::Node* node = (*nodeSet)[0];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                return static_cast<Element*>(node);
            }
            else
            {
                throw std::runtime_error("element node expected");
            }
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
}

Element* GetReturnTypeElement(Element* parentElement)
{
    std::unique_ptr<cmajor::xpath::XPathObject> returnTypeObject = cmajor::xpath::Evaluate(U"returnType", parentElement);
    if (returnTypeObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(returnTypeObject.get());
        if (nodeSet->Length() == 1)
        {
            sngxml::dom::Node* node = (*nodeSet)[0];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                return static_cast<Element*>(node);
            }
            else
            {
                throw std::runtime_error("element node expected");
            }
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
}

Element* GetTypeElement(Element* parentElement)
{
    std::unique_ptr<cmajor::xpath::XPathObject> typeObject = cmajor::xpath::Evaluate(U"type", parentElement);
    if (typeObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(typeObject.get());
        if (nodeSet->Length() == 1)
        {
            sngxml::dom::Node* node = (*nodeSet)[0];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                return static_cast<Element*>(node);
            }
            else
            {
                throw std::runtime_error("element node expected");
            }
        }
        else
        {
            throw std::runtime_error("one node expected");
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
}

Element* GetUnderlyingTypeElement(Element* parentElement)
{
    std::unique_ptr<cmajor::xpath::XPathObject> typeObject = cmajor::xpath::Evaluate(U"underlyingType", parentElement);
    if (typeObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(typeObject.get());
        if (nodeSet->Length() == 1)
        {
            sngxml::dom::Node* node = (*nodeSet)[0];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                return static_cast<Element*>(node);
            }
            else
            {
                throw std::runtime_error("element node expected");
            }
        }
        else
        {
            throw std::runtime_error("one node expected");
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
}

Element* GetConstraintElement(Element* parentElement)
{
    std::unique_ptr<cmajor::xpath::XPathObject> constraintObject = cmajor::xpath::Evaluate(U"constraint", parentElement);
    if (constraintObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(constraintObject.get());
        if (nodeSet->Length() == 1)
        {
            sngxml::dom::Node* node = (*nodeSet)[0];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                return static_cast<Element*>(node);
            }
            else
            {
                throw std::runtime_error("element node expected");
            }
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
}

std::vector<Element*> GetConstraintElements(Element* parentElement)
{
    std::vector<Element*> constraintElements;
    std::unique_ptr<cmajor::xpath::XPathObject> constraintObject = cmajor::xpath::Evaluate(U"constraints/constraint", parentElement);
    if (constraintObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(constraintObject.get());
        int n = nodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* node = (*nodeSet)[i];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                constraintElements.push_back(static_cast<Element*>(node));
            }
            else
            {
                throw std::runtime_error("element node expected");
            }
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
    return constraintElements;
}

std::vector<Element*> GetAxiomElements(Element* parentElement)
{
    std::vector<Element*> axiomElements;
    std::unique_ptr<cmajor::xpath::XPathObject> constraintObject = cmajor::xpath::Evaluate(U"axioms/axiom", parentElement);
    if (constraintObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(constraintObject.get());
        int n = nodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* node = (*nodeSet)[i];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                axiomElements.push_back(static_cast<Element*>(node));
            }
            else
            {
                throw std::runtime_error("element node expected");
            }
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
    return axiomElements;
}

std::vector<Element*> GetAxiomStatementElements(Element* parentElement)
{
    std::vector<Element*> axiomStatementElements;
    std::unique_ptr<cmajor::xpath::XPathObject> constraintObject = cmajor::xpath::Evaluate(U"axiomStatements/axiomStatement", parentElement);
    if (constraintObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(constraintObject.get());
        int n = nodeSet->Length();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Node* node = (*nodeSet)[i];
            if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
            {
                axiomStatementElements.push_back(static_cast<Element*>(node));
            }
            else
            {
                throw std::runtime_error("element node expected");
            }
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
    return axiomStatementElements;
}

std::u32string GetSyntaxText(Element* parentElement)
{
    std::unique_ptr<cmajor::xpath::XPathObject> syntaxObject = cmajor::xpath::Evaluate(U"syntax/text()", parentElement);
    if (syntaxObject->Type() == cmajor::xpath::XPathObjectType::nodeSet)
    {
        cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(syntaxObject.get());
        if (nodeSet->Length() == 1)
        {
            sngxml::dom::Node* node = (*nodeSet)[0];
            if (node->GetNodeType() == sngxml::dom::NodeType::textNode)
            {
                Text* syntaxText = static_cast<Text*>(node);
                return syntaxText->Data();
            }
            else
            {
                throw std::runtime_error("text node expected");
            }
        }
        else
        {
            return std::u32string();
        }
    }
    else
    {
        throw std::runtime_error("node set expected");
    }
}

void GenerateMembersHtml(Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, 
    sngxml::dom::Element* bodyElement, sngxml::dom::Element* parentElement, int level, const std::u32string& functionsText, const std::u32string& functionText, Document* docXml);

void GenerateTemplateParameterHtml(sngxml::dom::Element* parentElement, sngxml::dom::Element* bodyElement, int level, Document* docXml, const std::u32string& parentId, const std::u32string& title)
{
    std::vector<sngxml::dom::Element*> templateParameterElements = GetTemplateParameterElements(parentElement);
    if (!templateParameterElements.empty())
    {
        std::unique_ptr<Element> templateParametersHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        templateParametersHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(title)));
        bodyElement->AppendChild(std::move(templateParametersHeadingElement));
        std::unique_ptr<Element> templateParameterTableElement(new Element(U"table"));
        templateParameterTableElement->SetAttribute(U"class", U"params");
        int m = templateParameterElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* templateParameterElement = templateParameterElements[i];
            std::u32string templateParameterName = GetName(templateParameterElement);
            std::u32string templateParameterDefault = GetDefault(templateParameterElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            tableRowElement->SetAttribute(U"class", U"params");
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            tableColElement->SetAttribute(U"class", U"params");
            std::unique_ptr<Element> span(new Element(U"span"));
            span->SetAttribute(U"id", templateParameterElement->GetAttribute(U"id"));
            span->SetAttribute(U"class", U"param");
            std::u32string defaultStr;
            if (!templateParameterDefault.empty())
            {
                defaultStr = U" = " + templateParameterDefault;
            }
            span->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(templateParameterName + defaultStr)));
            tableColElement->AppendChild(std::move(span));
            tableRowElement->AppendChild(std::move(tableColElement));
            if (docXml)
            {
                std::u32string templateParamId = parentId + U"." + templateParameterName;
                Element* descriptionElement = docXml->GetElementById(templateParamId);
                if (descriptionElement)
                {
                    std::unique_ptr<Element> tableColElement(new Element(U"td"));
                    tableColElement->SetAttribute(U"class", U"params");
                    descriptionElement->CloneChildrenTo(tableColElement.get());
                    tableRowElement->AppendChild(std::move(tableColElement));
                }
            }
            templateParameterTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(templateParameterTableElement));
    }
}

void GenerateParameterHtml(Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs,
    sngxml::dom::Element* parentElement, sngxml::dom::Element* bodyElement, int level, Document* docXml, const std::u32string& parentId)
{
    std::vector<sngxml::dom::Element*> parameterElements = GetParameterElements(parentElement);
    if (!parameterElements.empty())
    {
        std::unique_ptr<Element> parametersHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        parametersHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Parameters")));
        bodyElement->AppendChild(std::move(parametersHeadingElement));
        std::unique_ptr<Element> parameterTableElement(new Element(U"table"));
        parameterTableElement->SetAttribute(U"class", U"params");
        int n = parameterElements.size();
        for (int i = 0; i < n; ++i)
        {
            sngxml::dom::Element* parameterElement = parameterElements[i];
            std::u32string parameterName = GetName(parameterElement);
            sngxml::dom::Element* parameterTypeElement = GetTypeElement(parameterElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            tableRowElement->SetAttribute(U"class", U"params");
            std::unique_ptr<Element> tableDataElement(new Element(U"td"));
            tableDataElement->SetAttribute(U"class", U"params");
            std::unique_ptr<Element> span(new Element(U"span"));
            span->SetAttribute(U"id", parameterElement->GetAttribute(U"id"));
            span->SetAttribute(U"class", U"param");
            span->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(parameterName)));
            tableDataElement->AppendChild(std::move(span));
            tableRowElement->AppendChild(std::move(tableDataElement));
            std::unique_ptr<Element> tableDataTypeElement(new Element(U"td"));
            tableDataTypeElement->SetAttribute(U"class", U"params");
            AppendTypeHtml(parameterTypeElement->GetAttribute(U"ref"), libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, tableDataTypeElement.get());
            tableRowElement->AppendChild(std::move(tableDataTypeElement));
            if (docXml)
            {
                std::u32string paramId = parentId + U"." + parameterName;
                Element* descriptionElement = docXml->GetElementById(paramId);
                if (descriptionElement)
                {
                    std::unique_ptr<Element> tableDescElement(new Element(U"td"));
                    tableDescElement->SetAttribute(U"class", U"params");
                    descriptionElement->CloneChildrenTo(tableDescElement.get());
                    tableRowElement->AppendChild(std::move(tableDescElement));
                }
            }
            parameterTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(parameterTableElement));
    }
}

void AppendSyntax(sngxml::dom::Element* parentElement, sngxml::dom::Element* bodyElement, int level)
{
    std::u32string syntax = GetSyntaxText(parentElement);
    if (!syntax.empty())
    {
        std::unique_ptr<Element> syntaxHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        syntaxHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Syntax")));
        bodyElement->AppendChild(std::move(syntaxHeadingElement));
        std::unique_ptr<Element> tableElement(new Element(U"table"));
        std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
        std::unique_ptr<Element> tableDataElement(new Element(U"td"));
        std::unique_ptr<Element> span(new Element(U"span"));
        span->SetAttribute(U"class", U"mono");
        span->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(syntax)));
        tableDataElement->AppendChild(std::move(span));
        tableRowElement->AppendChild(std::move(tableDataElement));
        tableElement->AppendChild(std::move(tableRowElement));
        bodyElement->AppendChild(std::move(tableElement));
    }
}

void GenerateClassHtml(sngxml::dom::Element* classElement, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs,
    sngxml::dom::Element* bodyElement, int level, Document* docXml)
{
    AppendRule(bodyElement);
    std::u32string className = GetName(classElement);
    std::unique_ptr<Element> classHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
    classHeadingElement->SetAttribute(U"id", classElement->GetAttribute(U"id"));
    classHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(className + U" Class")));
    bodyElement->AppendChild(std::move(classHeadingElement));
    if (docXml)
    {
        Element* classDescriptionElement = docXml->GetElementById(classElement->GetAttribute(U"id"));
        if (classDescriptionElement)
        {
            std::unique_ptr<Element> descriptionParagraphElement(new Element(U"p"));
            classDescriptionElement->CloneChildrenTo(descriptionParagraphElement.get());
            bodyElement->AppendChild(std::move(descriptionParagraphElement));
        }
    }
    AppendSyntax(classElement, bodyElement, level + 1);
    GenerateTemplateParameterHtml(classElement, bodyElement, level + 1, docXml, classElement->GetAttribute(U"id"), U"Template Parameters");
    sngxml::dom::Element* baseClassElement = GetBaseClassElement(classElement);
    if (baseClassElement)
    {
        std::unique_ptr<Element> baseClassHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
        baseClassHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Base Class")));
        bodyElement->AppendChild(std::move(baseClassHeadingElement));
        AppendTypeHtml(baseClassElement->GetAttribute(U"ref"), libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement);
    }
    sngxml::dom::Element* constraintElement = GetConstraintElement(classElement);
    if (constraintElement)
    {
        std::unique_ptr<Element> constraintHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
        constraintHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Constraint")));
        bodyElement->AppendChild(std::move(constraintHeadingElement));
        std::unique_ptr<Element> tableElement(new Element(U"table"));
        std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
        std::unique_ptr<Element> tableDataElement(new Element(U"td"));
        AppendConstraintHtml(constraintElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, tableDataElement.get());
        tableRowElement->AppendChild(std::move(tableDataElement));
        tableElement->AppendChild(std::move(tableRowElement));
        bodyElement->AppendChild(std::move(tableElement));
    }
    GenerateMembersHtml(libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, classElement, level + 1, U"Member Functions", U"Member Function", docXml);
}

void GenerateTypedefHtml(sngxml::dom::Element* typedefElement, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, 
    sngxml::dom::Element* bodyElement, int level, Document* docXml)
{
    AppendRule(bodyElement);
    std::u32string typedefName = GetName(typedefElement);
    std::unique_ptr<Element> typedefHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
    std::u32string typedefId = typedefElement->GetAttribute(U"id");
    typedefHeadingElement->SetAttribute(U"id", typedefId);
    typedefHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(typedefName + U" Typedef")));
    bodyElement->AppendChild(std::move(std::move(typedefHeadingElement)));
    if (docXml)
    {
        Element* descriptionElement = docXml->GetElementById(typedefId);
        if (descriptionElement)
        {
            std::unique_ptr<Element> descriptionParagraphElement(new Element(U"p"));
            descriptionElement->CloneChildrenTo(descriptionParagraphElement.get());
            bodyElement->AppendChild(std::move(descriptionParagraphElement));
        }
    }
    AppendSyntax(typedefElement, bodyElement, level + 1);
    Element* typeElement = GetTypeElement(typedefElement);
    std::unique_ptr<Element> typeHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
    typeHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Type")));
    bodyElement->AppendChild(std::move(typeHeadingElement));
    std::u32string ref = typeElement->GetAttribute(U"ref");
    std::unique_ptr<Element> tableElement(new Element(U"table"));
    std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
    std::unique_ptr<Element> tableDataElement(new Element(U"td"));
    AppendTypeHtml(ref, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, tableDataElement.get());
    tableRowElement->AppendChild(std::move(tableDataElement));
    tableElement->AppendChild(std::move(tableRowElement));
    bodyElement->AppendChild(std::move(tableElement));
}

void GenerateConceptHtml(sngxml::dom::Element* conceptElement, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs,
    sngxml::dom::Element* bodyElement, int level, Document* docXml)
{
    AppendRule(bodyElement);
    std::u32string conceptName = GetName(conceptElement);
    std::unique_ptr<Element> conceptHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
    std::u32string conceptId = conceptElement->GetAttribute(U"id");
    conceptHeadingElement->SetAttribute(U"id", conceptId);
    conceptHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(conceptName + U" Concept")));
    bodyElement->AppendChild(std::move(conceptHeadingElement));
    if (docXml)
    {
        Element* descriptionElement = docXml->GetElementById(conceptId);
        if (descriptionElement)
        {
            std::unique_ptr<Element> descriptionParagraphElement(new Element(U"p"));
            descriptionElement->CloneChildrenTo(descriptionParagraphElement.get());
            bodyElement->AppendChild(std::move(descriptionParagraphElement));
        }
    }
    AppendSyntax(conceptElement, bodyElement, level + 1);
    GenerateTemplateParameterHtml(conceptElement, bodyElement, level + 1, docXml, conceptId, U"Type Parameters");
    sngxml::dom::Element* refinesElement = GetRefinesElement(conceptElement);
    if (refinesElement)
    {
        std::unique_ptr<Element> refinesHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
        refinesHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Refines")));
        bodyElement->AppendChild(std::move(refinesHeadingElement));
        std::unique_ptr<Element> tableElement(new Element(U"table"));
        std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
        std::unique_ptr<Element> tableDataElement(new Element(U"td"));
        AppendConceptHtml(refinesElement->GetAttribute(U"ref"), libraryXmlDoc, tableDataElement.get(), false);
        tableRowElement->AppendChild(std::move(tableDataElement));
        tableElement->AppendChild(std::move(tableRowElement));
        bodyElement->AppendChild(std::move(tableElement));
    }
    std::vector<Element*> constraintElements = GetConstraintElements(conceptElement);
    if (!constraintElements.empty())
    {
        std::unique_ptr<Element> constraintsHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
        constraintsHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Constraints")));
        bodyElement->AppendChild(std::move(constraintsHeadingElement));
        std::unique_ptr<Element> tableElement(new Element(U"table"));
        int n = constraintElements.size();
        for (int i = 0; i < n; ++i)
        {
            Element* constraintElement = constraintElements[i];
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableDataElement(new Element(U"td"));
            AppendConstraintHtml(constraintElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, tableDataElement.get());
            tableRowElement->AppendChild(std::move(tableDataElement));
            tableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(tableElement));
    }
    std::vector<Element*> axiomElements = GetAxiomElements(conceptElement);
    if (!axiomElements.empty())
    {
        std::unique_ptr<Element> axiomsHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
        axiomsHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Axioms")));
        bodyElement->AppendChild(std::move(axiomsHeadingElement));
        int n = axiomElements.size();
        for (int i = 0; i < n; ++i)
        {
            Element* axiomElement = axiomElements[i];
            std::u32string axiomName = GetName(axiomElement);
            std::unique_ptr<Element> axiomHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 2))));
            axiomHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(axiomName + U" Axiom")));
            bodyElement->AppendChild(std::move(axiomHeadingElement));
            std::unique_ptr<Element> tableElement(new Element(U"table"));
            std::vector<Element*> axiomStatementElements = GetAxiomStatementElements(axiomElement);
            for (int i = 0; i < axiomStatementElements.size(); ++i)
            {
                Element* axiomStatementElement = axiomStatementElements[i];
                std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
                std::unique_ptr<Element> tableDataElement(new Element(U"td"));
                axiomStatementElement->CloneChildrenTo(tableDataElement.get());
                tableRowElement->AppendChild(std::move(tableDataElement));
                tableElement->AppendChild(std::move(tableRowElement));
            }
            bodyElement->AppendChild(std::move(tableElement));
        }
    }
}

void GenerateDelegateHtml(sngxml::dom::Element* delegateElement, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, 
    sngxml::dom::Element* bodyElement, int level, Document* docXml)
{
    AppendRule(bodyElement);
    std::u32string delegateName = GetName(delegateElement);
    std::unique_ptr<Element> delegateHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
    std::u32string delegateId = delegateElement->GetAttribute(U"id");
    delegateHeadingElement->SetAttribute(U"id", delegateId);
    delegateHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(delegateName + U" Delegate")));
    bodyElement->AppendChild(std::move(delegateHeadingElement));
    if (docXml)
    {
        Element* descriptionElement = docXml->GetElementById(delegateId);
        if (descriptionElement)
        {
            std::unique_ptr<Element> descriptionParagraphElement(new Element(U"p"));
            descriptionElement->CloneChildrenTo(descriptionParagraphElement.get());
            bodyElement->AppendChild(std::move(descriptionParagraphElement));
        }
    }
    AppendSyntax(delegateElement, bodyElement, level + 1);
    GenerateParameterHtml(libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, delegateElement, bodyElement, level + 1, docXml, delegateId);
    sngxml::dom::Element* returnTypeElement = GetReturnTypeElement(delegateElement);
    if (returnTypeElement)
    {
        std::unique_ptr<Element> returnTypeHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
        returnTypeHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Return Type")));
        bodyElement->AppendChild(std::move(returnTypeHeadingElement));
        std::unique_ptr<Element> tableElement(new Element(U"table"));
        std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
        std::unique_ptr<Element> tableDataElement(new Element(U"td"));
        AppendTypeHtml(returnTypeElement->GetAttribute(U"ref"), libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, tableDataElement.get());
        tableRowElement->AppendChild(std::move(tableDataElement));
        tableElement->AppendChild(std::move(tableRowElement));
        bodyElement->AppendChild(std::move(tableElement));
        if (docXml)
        {
            std::u32string returnsId = delegateId + U".returns";
            Element* returnDescriptionElement = docXml->GetElementById(returnsId);
            if (returnDescriptionElement)
            {
                std::unique_ptr<Element> returnValueHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
                returnValueHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Return Value")));
                bodyElement->AppendChild(std::move(returnValueHeadingElement));
                std::unique_ptr<Element> tableElement(new Element(U"table"));
                std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
                std::unique_ptr<Element> tableDataElement(new Element(U"td"));
                returnDescriptionElement->CloneChildrenTo(tableDataElement.get());
                tableRowElement->AppendChild(std::move(tableDataElement));
                tableElement->AppendChild(std::move(tableRowElement));
                bodyElement->AppendChild(std::move(tableElement));
            }
        }
    }
}

void GenerateClassDelegateHtml(sngxml::dom::Element* classDelegateElement, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, 
    const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, sngxml::dom::Element* bodyElement, int level, Document* docXml)
{
    AppendRule(bodyElement);
    std::u32string classDelegateName = GetName(classDelegateElement);
    std::unique_ptr<Element> classDelegateHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
    std::u32string classDelegateId = classDelegateElement->GetAttribute(U"id");
    classDelegateHeadingElement->SetAttribute(U"id", classDelegateId);
    classDelegateHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(classDelegateName + U" Class Delegate")));
    bodyElement->AppendChild(std::move(classDelegateHeadingElement));
    if (docXml)
    {
        Element* descriptionElement = docXml->GetElementById(classDelegateId);
        if (descriptionElement)
        {
            std::unique_ptr<Element> descriptionParagraphElement(new Element(U"p"));
            descriptionElement->CloneChildrenTo(descriptionParagraphElement.get());
            bodyElement->AppendChild(std::move(descriptionParagraphElement));
        }
    }
    AppendSyntax(classDelegateElement, bodyElement, level + 1);
    GenerateParameterHtml(libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, classDelegateElement, bodyElement, level + 1, docXml, classDelegateId);
    sngxml::dom::Element* returnTypeElement = GetReturnTypeElement(classDelegateElement);
    if (returnTypeElement)
    {
        std::unique_ptr<Element> returnTypeHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
        returnTypeHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Return Type")));
        bodyElement->AppendChild(std::move(returnTypeHeadingElement));
        std::unique_ptr<Element> tableElement(new Element(U"table"));
        std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
        std::unique_ptr<Element> tableDataElement(new Element(U"td"));
        AppendTypeHtml(returnTypeElement->GetAttribute(U"ref"), libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, tableDataElement.get());
        tableRowElement->AppendChild(std::move(tableDataElement));
        tableElement->AppendChild(std::move(tableRowElement));
        bodyElement->AppendChild(std::move(tableElement));
        if (docXml)
        {
            std::u32string returnsId = classDelegateId + U".returns";
            Element* returnDescriptionElement = docXml->GetElementById(returnsId);
            if (returnDescriptionElement)
            {
                std::unique_ptr<Element> returnValueHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
                returnValueHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Return Value")));
                bodyElement->AppendChild(std::move(returnValueHeadingElement));
                std::unique_ptr<Element> tableElement(new Element(U"table"));
                std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
                std::unique_ptr<Element> tableDataElement(new Element(U"td"));
                returnDescriptionElement->CloneChildrenTo(tableDataElement.get());
                tableRowElement->AppendChild(std::move(tableDataElement));
                tableElement->AppendChild(std::move(tableRowElement));
                bodyElement->AppendChild(std::move(tableElement));
            }
        }
    }
}

void GenerateConstantHtml(sngxml::dom::Element* constantElement, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths,
    const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, sngxml::dom::Element* bodyElement, int level, Document* docXml)
{
    AppendRule(bodyElement);
    std::u32string constantName = GetName(constantElement);
    std::unique_ptr<Element> constantHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
    std::u32string constantId = constantElement->GetAttribute(U"id");
    constantHeadingElement->SetAttribute(U"id", constantId);
    constantHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(constantName + U" Constant")));
    bodyElement->AppendChild(std::move(constantHeadingElement));
    if (docXml)
    {
        Element* descriptionElement = docXml->GetElementById(constantId);
        if (descriptionElement)
        {
            std::unique_ptr<Element> descriptionParagraphElement(new Element(U"p"));
            descriptionElement->CloneChildrenTo(descriptionParagraphElement.get());
            bodyElement->AppendChild(std::move(descriptionParagraphElement));
        }
    }
    AppendSyntax(constantElement, bodyElement, level + 1);
    sngxml::dom::Element* typeElement = GetTypeElement(constantElement);
    std::unique_ptr<Element> typeHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
    typeHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Type")));
    bodyElement->AppendChild(std::move(typeHeadingElement));
    AppendTypeHtml(typeElement->GetAttribute(U"ref"), libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement);
    std::u32string constantValue = GetValue(constantElement);
    std::unique_ptr<Element> valueHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
    valueHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Value")));
    bodyElement->AppendChild(std::move(valueHeadingElement));
    std::unique_ptr<Element> tableElement(new Element(U"table"));
    std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
    std::unique_ptr<Element> tableDataElement(new Element(U"td"));
    std::unique_ptr<Element> spanValueElement(new Element(U"span"));
    spanValueElement->SetAttribute(U"class", U"var");
    spanValueElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(constantValue)));
    tableDataElement->AppendChild(std::move(spanValueElement));
    tableRowElement->AppendChild(std::move(tableDataElement));
    tableElement->AppendChild(std::move(tableRowElement));
    bodyElement->AppendChild(std::move(tableElement));
}

void GenerateEnumerationHtml(sngxml::dom::Element* enumerationElement, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths,
    const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, sngxml::dom::Element* bodyElement, int level, Document* docXml)
{
    AppendRule(bodyElement);
    std::u32string enumerationName = GetName(enumerationElement);
    std::unique_ptr<Element> enumerationHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
    std::u32string enumId = enumerationElement->GetAttribute(U"id");
    enumerationHeadingElement->SetAttribute(U"id", enumId);
    enumerationHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(enumerationName + U" Enumeration")));
    bodyElement->AppendChild(std::move(enumerationHeadingElement));
    if (docXml)
    {
        Element* descriptionElement = docXml->GetElementById(enumId);
        if (descriptionElement)
        {
            std::unique_ptr<Element> descriptionParagraphElement(new Element(U"p"));
            descriptionElement->CloneChildrenTo(descriptionParagraphElement.get());
            bodyElement->AppendChild(std::move(descriptionParagraphElement));
        }
    }
    AppendSyntax(enumerationElement, bodyElement, level + 1);
    sngxml::dom::Element* underlyingTypeElement = GetUnderlyingTypeElement(enumerationElement);
    std::unique_ptr<Element> typeHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
    typeHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Underlying Type")));
    bodyElement->AppendChild(std::move(typeHeadingElement));
    AppendTypeHtml(underlyingTypeElement->GetAttribute(U"ref"), libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement);
    std::unique_ptr<Element> enumConstantsHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
    enumConstantsHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Enumeration Constants")));
    bodyElement->AppendChild(std::move(enumConstantsHeadingElement));
    std::unique_ptr<Element> enumConstantTableElement(new Element(U"table"));
    std::vector<Element*> enumConstantElements = GetEnumConstantElements(enumerationElement);
    int n = enumConstantElements.size();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Element* enumConstantElement = enumConstantElements[i];
        std::u32string enumConstantName = GetName(enumConstantElement);
        std::u32string enumConstantValue = GetValue(enumConstantElement);
        std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
        std::unique_ptr<Element> tableColElement(new Element(U"td"));
        std::unique_ptr<Element> spanVarElement(new Element(U"span"));
        spanVarElement->SetAttribute(U"class", U"var");
        spanVarElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(enumConstantName)));
        tableColElement->AppendChild(std::move(spanVarElement));
        tableRowElement->AppendChild(std::move(tableColElement));
        std::unique_ptr<Element> tableCol2Element(new Element(U"td"));
        std::unique_ptr<Element> spanValueElement(new Element(U"span"));
        spanValueElement->SetAttribute(U"class", U"var");
        spanValueElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(enumConstantValue)));
        tableCol2Element->AppendChild(std::move(spanValueElement));
        tableRowElement->AppendChild(std::move(tableCol2Element));
        enumConstantTableElement->AppendChild(std::move(tableRowElement));
    }
    bodyElement->AppendChild(std::move(enumConstantTableElement));
}

void GenerateMemberVariableHtml(sngxml::dom::Element* memberVariableElement, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths,
    const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, sngxml::dom::Element* bodyElement, int level, Document* docXml)
{
    AppendRule(bodyElement);
    std::u32string memberVariableName = GetName(memberVariableElement);
    std::unique_ptr<Element> memberVariableHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
    std::u32string memberVarId = memberVariableElement->GetAttribute(U"id");
    memberVariableHeadingElement->SetAttribute(U"id", memberVarId);
    memberVariableHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(memberVariableName + U" Member Variable")));
    bodyElement->AppendChild(std::move(memberVariableHeadingElement));
    if (docXml)
    {
        Element* descriptionElement = docXml->GetElementById(memberVarId);
        if (descriptionElement)
        {
            std::unique_ptr<Element> descriptionParagraphElement(new Element(U"p"));
            descriptionElement->CloneChildrenTo(descriptionParagraphElement.get());
            bodyElement->AppendChild(std::move(descriptionParagraphElement));
        }
    }
    AppendSyntax(memberVariableElement, bodyElement, level + 1);
    sngxml::dom::Element* typeElement = GetTypeElement(memberVariableElement);
    std::unique_ptr<Element> typeHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
    typeHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Type")));
    bodyElement->AppendChild(std::move(typeHeadingElement));
    std::unique_ptr<Element> tableElement(new Element(U"table"));
    std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
    std::unique_ptr<Element> tableDataElement(new Element(U"td"));
    AppendTypeHtml(typeElement->GetAttribute(U"ref"), libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, tableDataElement.get());
    tableRowElement->AppendChild(std::move(tableDataElement));
    tableElement->AppendChild(std::move(tableRowElement));
    bodyElement->AppendChild(std::move(tableElement));
}

void GenerateFunctionHtml(sngxml::dom::Element* functionElement, Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, 
    sngxml::dom::Element* bodyElement, int level, const std::u32string& functionKind, Document* docXml)
{
    AppendRule(bodyElement);
    std::u32string functionName = GetName(functionElement);
    std::unique_ptr<Element> functionHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
    std::u32string functionId = functionElement->GetAttribute(U"id");
    functionHeadingElement->SetAttribute(U"id", functionId);
    functionHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(functionName + U" " + functionKind)));
    bodyElement->AppendChild(std::move(std::move(functionHeadingElement)));
    if (docXml)
    {
        Element* descriptionElement = docXml->GetElementById(functionId);
        if (descriptionElement)
        {
            std::unique_ptr<Element> descriptionParagraphElement(new Element(U"p"));
            descriptionElement->CloneChildrenTo(descriptionParagraphElement.get());
            bodyElement->AppendChild(std::move(descriptionParagraphElement));
        }
    }
    AppendSyntax(functionElement, bodyElement, level + 1);
    GenerateTemplateParameterHtml(functionElement, bodyElement, level + 1, docXml, functionId, U"Template Parameters");
    sngxml::dom::Element* constraintElement = GetConstraintElement(functionElement);
    if (constraintElement)
    {
        std::unique_ptr<Element> constraintHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
        constraintHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Constraint")));
        bodyElement->AppendChild(std::move(constraintHeadingElement));
        std::unique_ptr<Element> tableElement(new Element(U"table"));
        std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
        std::unique_ptr<Element> tableDataElement(new Element(U"td"));
        AppendConstraintHtml(constraintElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement);
        tableRowElement->AppendChild(std::move(tableDataElement));
        tableElement->AppendChild(std::move(tableRowElement));
        bodyElement->AppendChild(std::move(tableElement));
    }
    GenerateParameterHtml(libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, functionElement, bodyElement, level + 1, docXml, functionId);
    sngxml::dom::Element* returnTypeElement = GetReturnTypeElement(functionElement);
    if (returnTypeElement)
    {
        std::unique_ptr<Element> returnTypeHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
        returnTypeHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Return Type")));
        bodyElement->AppendChild(std::move(returnTypeHeadingElement));
        std::unique_ptr<Element> tableElement(new Element(U"table"));
        std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
        std::unique_ptr<Element> tableDataElement(new Element(U"td"));
        AppendTypeHtml(returnTypeElement->GetAttribute(U"ref"), libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, tableDataElement.get());
        tableRowElement->AppendChild(std::move(tableDataElement));
        tableElement->AppendChild(std::move(tableRowElement));
        bodyElement->AppendChild(std::move(tableElement));
        if (docXml)
        {
            std::u32string returnsId = functionId + U".returns";
            Element* returnDescriptionElement = docXml->GetElementById(returnsId);
            if (returnDescriptionElement)
            {
                std::unique_ptr<Element> returnValueHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
                returnValueHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Return Value")));
                bodyElement->AppendChild(std::move(returnValueHeadingElement));
                std::unique_ptr<Element> tableElement(new Element(U"table"));
                std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
                std::unique_ptr<Element> tableDataElement(new Element(U"td"));
                returnDescriptionElement->CloneChildrenTo(tableDataElement.get());
                tableRowElement->AppendChild(std::move(tableDataElement));
                tableElement->AppendChild(std::move(tableRowElement));
                bodyElement->AppendChild(std::move(tableElement));
            }
        }
    }
    std::vector<Element*> overloadElements = GetOverloadElements(functionElement);
    if (!overloadElements.empty())
    {
        std::unique_ptr<Element> overloadsHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level + 1))));
        overloadsHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Overloads")));
        bodyElement->AppendChild(std::move(overloadsHeadingElement));
        std::unique_ptr<Element> overloadTableElement(new Element(U"table"));
        int m = overloadElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* overloadElement = overloadElements[i];
            std::u32string overloadName = GetName(overloadElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + overloadElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(overloadName)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            overloadTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(overloadTableElement));
    }
    for (Element* overloadElement : overloadElements)
    {
        GenerateFunctionHtml(overloadElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level + 1, U"Overload", docXml);
    }
}

void GenerateMembersHtml(Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, 
    sngxml::dom::Element* bodyElement, sngxml::dom::Element* parentElement, int level, const std::u32string& functionsText, const std::u32string& functionText, Document* docXml)
{
    std::vector<sngxml::dom::Element*> classElements = GetClassElements(parentElement);
    if (!classElements.empty())
    {
        std::unique_ptr<Element> classesHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        classesHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Classes")));
        bodyElement->AppendChild(std::move(classesHeadingElement));
        std::unique_ptr<Element> classTableElement(new Element(U"table"));
        int m = classElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* classElement = classElements[i];
            std::u32string className = GetName(classElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + classElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(className)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            classTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(classTableElement));
    }
    std::vector<sngxml::dom::Element*> constructorElements = GetConstructorElements(parentElement);
    if (!constructorElements.empty())
    {
        std::unique_ptr<Element> constructorsHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        constructorsHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Constructors")));
        bodyElement->AppendChild(std::move(constructorsHeadingElement));
        std::unique_ptr<Element> constructorTableElement(new Element(U"table"));
        int m = constructorElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* constructorElement = constructorElements[i];
            std::u32string constructorName = GetName(constructorElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + constructorElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(constructorName)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            constructorTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(constructorTableElement));
    }
    std::vector<sngxml::dom::Element*> functionElements = GetFunctionElements(parentElement);
    if (!functionElements.empty())
    {
        std::unique_ptr<Element> functionsHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        functionsHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(functionsText)));
        bodyElement->AppendChild(std::move(functionsHeadingElement));
        std::unique_ptr<Element> functionTableElement(new Element(U"table"));
        int m = functionElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* functionElement = functionElements[i];
            std::u32string functionName = GetName(functionElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + functionElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(functionName)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            functionTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(functionTableElement));
    }
    std::vector<sngxml::dom::Element*> staticMemberFunctionElements = GetStaticMemberFunctionElements(parentElement);
    if (!staticMemberFunctionElements.empty())
    {
        std::unique_ptr<Element> functionsHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        functionsHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Static Member Functions")));
        bodyElement->AppendChild(std::move(functionsHeadingElement));
        std::unique_ptr<Element> functionTableElement(new Element(U"table"));
        int m = staticMemberFunctionElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* functionElement = staticMemberFunctionElements[i];
            std::u32string functionName = GetName(functionElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + functionElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(functionName)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            functionTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(functionTableElement));
    }
    std::vector<sngxml::dom::Element*> typedefElements = GetTypedefElements(parentElement);
    if (!typedefElements.empty())
    {
        std::unique_ptr<Element> typedefsHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        typedefsHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Typedefs")));
        bodyElement->AppendChild(std::move(typedefsHeadingElement));
        std::unique_ptr<Element> typedefTableElement(new Element(U"table"));
        int m = typedefElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* typedefElement = typedefElements[i];
            std::u32string typedefName = GetName(typedefElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + typedefElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(typedefName)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            typedefTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(typedefTableElement));
    }
    std::vector<sngxml::dom::Element*> conceptElements = GetConceptElements(parentElement);
    if (!conceptElements.empty())
    {
        std::unique_ptr<Element> conceptsHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        conceptsHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Concepts")));
        bodyElement->AppendChild(std::move(conceptsHeadingElement));
        std::unique_ptr<Element> conceptTableElement(new Element(U"table"));
        int m = conceptElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* conceptElement = conceptElements[i];
            std::u32string conceptName = GetName(conceptElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + conceptElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(conceptName)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            conceptTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(conceptTableElement));
    }
    std::vector<sngxml::dom::Element*> delegateElements = GetDelegateElements(parentElement);
    if (!delegateElements.empty())
    {
        std::unique_ptr<Element> delegatesHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        delegatesHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Delegates")));
        bodyElement->AppendChild(std::move(delegatesHeadingElement));
        std::unique_ptr<Element> delegateTableElement(new Element(U"table"));
        int m = delegateElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* delegateElement = delegateElements[i];
            std::u32string delegateName = GetName(delegateElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + delegateElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(delegateName)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            delegateTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(delegateTableElement));
    }
    std::vector<sngxml::dom::Element*> classDelegateElements = GetClassDelegateElements(parentElement);
    if (!classDelegateElements.empty())
    {
        std::unique_ptr<Element> classDelegatesHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        classDelegatesHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Class Delegates")));
        bodyElement->AppendChild(std::move(classDelegatesHeadingElement));
        std::unique_ptr<Element> classDelegateTableElement(new Element(U"table"));
        int m = classDelegateElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* classDelegateElement = classDelegateElements[i];
            std::u32string classDelegateName = GetName(classDelegateElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + classDelegateElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(classDelegateName)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            classDelegateTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(classDelegateTableElement));
    }
    std::vector<sngxml::dom::Element*> constantElements = GetConstantElements(parentElement);
    if (!constantElements.empty())
    {
        std::unique_ptr<Element> constantHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        constantHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Constants")));
        bodyElement->AppendChild(std::move(constantHeadingElement));
        std::unique_ptr<Element> constantTableElement(new Element(U"table"));
        int m = constantElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* constantElement = constantElements[i];
            std::u32string constantName = GetName(constantElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + constantElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(constantName)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            constantTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(constantTableElement));
    }
    std::vector<sngxml::dom::Element*> enumerationElements = GetEnumerationElements(parentElement);
    if (!enumerationElements.empty())
    {
        std::unique_ptr<Element> enumerationHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        enumerationHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Enumerations")));
        bodyElement->AppendChild(std::move(enumerationHeadingElement));
        std::unique_ptr<Element> enumTableElement(new Element(U"table"));
        int m = enumerationElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* enumElement = enumerationElements[i];
            std::u32string enumName = GetName(enumElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + enumElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(enumName)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            enumTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(enumTableElement));
    }
    std::vector<sngxml::dom::Element*> memberVariableElements = GetMemberVariableElements(parentElement);
    if (!memberVariableElements.empty())
    {
        std::unique_ptr<Element> memberVariablesHeadingElement(new Element(U"h" + ToUtf32(std::to_string(level))));
        memberVariablesHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Member Variables")));
        bodyElement->AppendChild(std::move(memberVariablesHeadingElement));
        std::unique_ptr<Element> memberVariableTableElement(new Element(U"table"));
        int m = memberVariableElements.size();
        for (int i = 0; i < m; ++i)
        {
            sngxml::dom::Element* memberVariableElement = memberVariableElements[i];
            std::u32string memberVariableName = GetName(memberVariableElement);
            std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
            std::unique_ptr<Element> tableColElement(new Element(U"td"));
            std::unique_ptr<Element> linkElement(new Element(U"a"));
            linkElement->SetAttribute(U"href", U"#" + memberVariableElement->GetAttribute(U"id"));
            linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(memberVariableName)));
            tableColElement->AppendChild(std::move(linkElement));
            tableRowElement->AppendChild(std::move(tableColElement));
            memberVariableTableElement->AppendChild(std::move(tableRowElement));
        }
        bodyElement->AppendChild(std::move(memberVariableTableElement));
    }
    for (sngxml::dom::Element* classElement : classElements)
    {
        GenerateClassHtml(classElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level, docXml);
    }
    for (sngxml::dom::Element* constructorElement : constructorElements)
    {
        GenerateFunctionHtml(constructorElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level, U"Constructor", docXml);
    }
    for (sngxml::dom::Element* functionElement : functionElements)
    {
        GenerateFunctionHtml(functionElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level, functionText, docXml);
    }
    for (sngxml::dom::Element* staticMemberFunctionElement : staticMemberFunctionElements)
    {
        GenerateFunctionHtml(staticMemberFunctionElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level, U"Static Member Function", docXml);
    }
    for (sngxml::dom::Element* typedefElement : typedefElements)
    {
        GenerateTypedefHtml(typedefElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level, docXml);
    }
    for (sngxml::dom::Element* conceptElement : conceptElements)
    {
        GenerateConceptHtml(conceptElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level, docXml);
    }
    for (sngxml::dom::Element* delegateElement : delegateElements)
    {
        GenerateDelegateHtml(delegateElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level, docXml);
    }
    for (sngxml::dom::Element* classDelegateElement : classDelegateElements)
    {
        GenerateClassDelegateHtml(classDelegateElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level, docXml);
    }
    for (sngxml::dom::Element* constantElement : constantElements)
    {
        GenerateConstantHtml(constantElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level, docXml);
    }
    for (sngxml::dom::Element* enumerationElement : enumerationElements)
    {
        GenerateEnumerationHtml(enumerationElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level, docXml);
    }
    for (sngxml::dom::Element* memberVariableElement : memberVariableElements)
    {
        GenerateMemberVariableHtml(memberVariableElement, libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement, level, docXml);
    }
}

void GenerateHtml(Document* libraryXmlDoc, const std::vector<std::u32string>& referenceXmlFilePaths, const std::vector<std::unique_ptr<Document>>& referenceXmlDocs, const std::string& htmlFilePath, Document* docXml)
{
    Document htmlDoc;
    std::unique_ptr<Element> htmlElement(new Element(U"html"));
    htmlElement->SetAttribute(U"lang", U"en");
    htmlElement->SetAttribute(U"xmlns", U"http://www.w3.org/1999/xhtml");
    std::unique_ptr<Element> headElement(new Element(U"head"));
    std::unique_ptr<Element> metaElement(new Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::move(metaElement));
    std::unique_ptr<Element> titleElement(new Element(U"title"));
    std::u32string libraryName = GetLibraryName(libraryXmlDoc);
    titleElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(libraryName + U" Library Reference")));
    headElement->AppendChild(std::move(titleElement));
    std::u32string stylePath = GetStylePath(libraryXmlDoc);
    if (!stylePath.empty())
    {
        std::unique_ptr<Element> styleSheetElement(new Element(U"link"));
        styleSheetElement->SetAttribute(U"rel", U"stylesheet");
        styleSheetElement->SetAttribute(U"type", U"text/css");
        styleSheetElement->SetAttribute(U"href", stylePath);
        headElement->AppendChild(std::move(styleSheetElement));
    }
    htmlElement->AppendChild(std::move(headElement));
    std::unique_ptr<Element> bodyElement(new Element(U"body"));
    std::unique_ptr<Element> libraryHeadingElement(new Element(U"h1"));
    libraryHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(libraryName + U" Library Reference")));
    bodyElement->AppendChild(std::move(libraryHeadingElement));
    std::unique_ptr<Element> namespacesHeadingElement(new Element(U"h2"));
    namespacesHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(U"Namespaces")));
    bodyElement->AppendChild(std::move(namespacesHeadingElement));
    std::vector<sngxml::dom::Element*> namespaceElements = GetNamespaceElements(libraryXmlDoc);
    std::unique_ptr<Element> namespaceTableElement(new Element(U"table"));
    int n = namespaceElements.size();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Element* namespaceElement = namespaceElements[i];
        std::u32string namespaceName = GetName(namespaceElement);
        std::unique_ptr<Element> tableRowElement(new Element(U"tr"));
        std::unique_ptr<Element> tableColElement(new Element(U"td"));
        std::unique_ptr<Element> linkElement(new Element(U"a"));
        linkElement->SetAttribute(U"href", U"#" + namespaceElement->GetAttribute(U"id"));
        linkElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(namespaceName)));
        tableColElement->AppendChild(std::move(linkElement));
        tableRowElement->AppendChild(std::move(tableColElement));
        namespaceTableElement->AppendChild(std::move(tableRowElement));
    }
    bodyElement->AppendChild(std::move(namespaceTableElement));
    for (int i = 0; i < n; ++i)
    {
        AppendRule(bodyElement.get());
        sngxml::dom::Element* namespaceElement = namespaceElements[i];
        std::u32string namespaceName = GetName(namespaceElement);
        std::unique_ptr<Element> namespaceHeadingElement(new Element(U"h2"));
        namespaceHeadingElement->SetAttribute(U"id", namespaceElement->GetAttribute(U"id"));
        namespaceHeadingElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new Text(namespaceName + U" Namespace")));
        bodyElement->AppendChild(std::move(std::move(namespaceHeadingElement)));
        GenerateMembersHtml(libraryXmlDoc, referenceXmlFilePaths, referenceXmlDocs, bodyElement.get(), namespaceElement, 3, U"Functions", U"Function", docXml);
    }
    htmlElement->AppendChild(std::move(bodyElement));
    htmlDoc.AppendChild(std::move(htmlElement));
    std::ofstream htmlFile(htmlFilePath);
    CodeFormatter formatter(htmlFile);
    if (verbose)
    {
        std::cout << "=> " << htmlFilePath << std::endl;
    }
    htmlDoc.Write(formatter);
}

std::vector<sngxml::dom::Element*> GetLibraryElements(Document* document)
{
    std::vector<sngxml::dom::Element*> libraryElements;
    std::unique_ptr<cmajor::xpath::XPathObject> libraryNodeSetObject = cmajor::xpath::Evaluate(U"/cmdoc/libraries/library", document);
    if (libraryNodeSetObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(libraryNodeSetObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            libraryElements.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return libraryElements;
}

std::vector<sngxml::dom::Element*> GetReferenceElements(Element* libraryElement)
{
    std::vector<sngxml::dom::Element*> referenceElements;
    std::unique_ptr<cmajor::xpath::XPathObject> referenceNodeSetObject = cmajor::xpath::Evaluate(U"references/reference", libraryElement);
    if (referenceNodeSetObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(referenceNodeSetObject.get());
    int n = nodeSet->Length();
    for (int i = 0; i < n; ++i)
    {
        sngxml::dom::Node* node = (*nodeSet)[i];
        if (node->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            referenceElements.push_back(static_cast<sngxml::dom::Element*>(node));
        }
        else
        {
            throw std::runtime_error("element node expected");
        }
    }
    return referenceElements;
}

std::u32string GetTextNode(const std::u32string& textNodeName, Element* element, bool optional)
{
    std::unique_ptr<cmajor::xpath::XPathObject> nodeSetObject = cmajor::xpath::Evaluate(textNodeName + U"/text()", element);
    if (nodeSetObject->Type() != cmajor::xpath::XPathObjectType::nodeSet)
    {
        throw std::runtime_error("node set expected");
    }
    cmajor::xpath::XPathNodeSet* nodeSet = static_cast<cmajor::xpath::XPathNodeSet*>(nodeSetObject.get());
    if (optional && nodeSet->Length() == 0)
    {
        return std::u32string();
    }
    if (nodeSet->Length() != 1)
    {
        throw std::runtime_error("node set length 1 expected");
    }
    sngxml::dom::Node* node = (*nodeSet)[0];
    if (node->GetNodeType() != sngxml::dom::NodeType::textNode)
    {
        throw std::runtime_error("text node expected");
    }
    sngxml::dom::Text* text = static_cast<sngxml::dom::Text*>(node);
    return text->Data();
}

void Generate(const std::string& projectXmlFile)
{
    if (verbose)
    {
        std::cout << "reading " << projectXmlFile << "..." << std::endl;
    }
    std::string basePath = Path::GetDirectoryName(GetFullPath(projectXmlFile));
    std::unique_ptr<Document> projectXmlDoc = ReadDocument(projectXmlFile);
    std::vector<sngxml::dom::Element*> libraryElements = GetLibraryElements(projectXmlDoc.get());
    int n = libraryElements.size();
    for (int i = 0; i < n; ++i)
    {
        Element* libraryElement = libraryElements[i];
        std::u32string moduleFilePath = GetTextNode(U"moduleFilePath", libraryElement, false);
        boost::filesystem::path mfp = Path::Combine(basePath, ToUtf8(moduleFilePath));
        if (!boost::filesystem::exists(mfp))
        {
            throw std::runtime_error("error: module file path '" + ToUtf8(moduleFilePath) + "' does not exist");
        }
        std::u32string libraryXmlFilePath = GetTextNode(U"libraryXmlFilePath", libraryElement, false);
        std::u32string docXmlFilePath = GetTextNode(U"docXmlFilePath", libraryElement, false);
        boost::filesystem::path lfp = Path::Combine(basePath, ToUtf8(libraryXmlFilePath));
        boost::filesystem::path dfp = Path::Combine(basePath, ToUtf8(docXmlFilePath));
        boost::filesystem::create_directories(lfp.parent_path());
        std::vector<sngxml::dom::Element*> referenceElements = GetReferenceElements(libraryElement);
        std::vector<std::u32string> referenceXmlFilePaths;
        for (sngxml::dom::Element* referenceElement : referenceElements)
        {
            if (referenceElement->FirstChild() != nullptr)
            {
                if (referenceElement->FirstChild()->GetNodeType() == sngxml::dom::NodeType::textNode)
                {
                    Text* text = static_cast<Text*>(referenceElement->FirstChild());
                    referenceXmlFilePaths.push_back(text->Data());
                }
                else
                {
                    throw std::runtime_error("one text node expected");
                }
            }
            else
            {
                throw std::runtime_error("one text node expected");
            }
        }
        std::u32string stylePath = GetTextNode(U"style", libraryElement, true);
        std::vector<std::unique_ptr<Document>> referenceXmlDocs;
        int n = referenceXmlFilePaths.size();
        for (int i = 0; i < n; ++i)
        {
            boost::filesystem::path rfp = Path::Combine(Path::GetDirectoryName(lfp.generic_string()), ToUtf8(referenceXmlFilePaths[i]));
            if (!boost::filesystem::exists(rfp))
            {
                throw std::runtime_error("reference file path '" + rfp.generic_string() + "' does not exist");
            }
            referenceXmlDocs.push_back(ReadDocument(GetFullPath(rfp.generic_string())));
        }
        std::unique_ptr<Document> libraryXmlDoc;
        if (force || !boost::filesystem::exists(lfp) || boost::filesystem::last_write_time(lfp) < boost::filesystem::last_write_time(mfp))
        {
            libraryXmlDoc = GenerateLibraryXmlFile(GetFullPath(mfp.generic_string()), GetFullPath(lfp.generic_string()), referenceXmlFilePaths, referenceXmlDocs, stylePath);
        }
        else
        {
            libraryXmlDoc = ReadDocument(GetFullPath(lfp.generic_string()));
        }
        std::unique_ptr<Document> docXml;
        if (boost::filesystem::exists(dfp))
        {
            docXml = ReadDocument(GetFullPath(dfp.generic_string()));
        }
        std::string htmlFilePath = GetFullPath(lfp.replace_extension(".html").generic_string());
        GenerateHtml(libraryXmlDoc.get(), referenceXmlFilePaths, referenceXmlDocs, htmlFilePath, docXml.get());
    }
}

const char* version = "3.5.0";

void PrintHelp()
{
    std::cout << "Cmajor Document Generator version " << version << "\n" <<
        "Usage: cmdoc [options] { <project.cmdoc.xml> }\n" <<
        "Options:\n" <<
        "--help (-h)\n" <<
        "    Print help.\n" <<
        "--verbose (-v)\n" <<
        "    Verbose output.\n" <<
        "--force (-f)\n" <<
        "    Force generation.\n" <<
        std::endl;
}

struct InitDone
{
    InitDone()
    {
        soulng::util::Init();
        sngcm::ast::Init();
        cmajor::symbols::Init();
        cmajor::parsing::Init();
        cmajor::xpath::Init();
    }
    ~InitDone()
    {
        cmajor::xpath::Done();
        cmajor::symbols::Done();
        sngcm::ast::Done();
        cmajor::parsing::Done();
        soulng::util::Done();
    }
};

int main(int argc, const char** argv)
{
    try
    {
        if (argc < 2)
        {
            PrintHelp();
            return 0;
        }
        SetGlobalFlag(GlobalFlags::info);
        InitDone initDone;
        std::vector<std::string> projectFiles;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg[0] == '-')
            {
                if (arg == "--help" || arg == "-h")
                {
                    PrintHelp();
                    return 0;
                }
                else if (arg == "--verbose" || arg == "-v")
                {
                    verbose = true;
                }
                else if (arg == "--force" || arg == "-f")
                {
                    force = true;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else
            {
                projectFiles.push_back(GetFullPath(arg));
            }
        }
        for (const std::string& projectFile : projectFiles)
        {
            Generate(projectFile);
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
*/
