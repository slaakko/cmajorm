// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/SymbolTableXml.hpp>
#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/cmdoclib/Global.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>
#include <cmajor/symbols/EnumSymbol.hpp>
#include <cmajor/symbols/ConstantSymbol.hpp>
#include <cmajor/symbols/TypedefSymbol.hpp>
#include <cmajor/symbols/DelegateSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/ast/Visitor.hpp>
#include <cmajor/ast/Expression.hpp>
#include <cmajor/dom/Parser.hpp>
#include <cmajor/dom/CharacterData.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/Log.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <fstream>

namespace cmajor { namespace cmdoclib {

using namespace cmajor::unicode;

struct ByCodeName
{
    bool operator()(Symbol* left, Symbol* right) const
    {
        if (left->GetSymbolType() == SymbolType::staticConstructorSymbol) return true;
        if (right->GetSymbolType() == SymbolType::staticConstructorSymbol) return false;
        if (left->GetSymbolType() == SymbolType::constructorSymbol && right->GetSymbolType() != SymbolType::constructorSymbol) return true;
        if (right->GetSymbolType() == SymbolType::constructorSymbol && left->GetSymbolType() != SymbolType::constructorSymbol) return false;
        if (left->CodeName() < right->CodeName()) return true;
        if (right->CodeName() < left->CodeName()) return false;
        if (left->GetSymbolType() == SymbolType::conceptSymbol && right->GetSymbolType() == SymbolType::conceptSymbol)
        {
            ConceptSymbol* leftConcecpt = static_cast<ConceptSymbol*>(left);
            ConceptSymbol* rightConcecpt = static_cast<ConceptSymbol*>(right);
            return leftConcecpt->Arity() < rightConcecpt->Arity();
        }
        if (left->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol && right->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* leftSpecialization = static_cast<ClassTemplateSpecializationSymbol*>(left);
            ClassTemplateSpecializationSymbol* rightSpecialization = static_cast<ClassTemplateSpecializationSymbol*>(right);
            return leftSpecialization->TemplateArgumentTypes().size() < rightSpecialization->TemplateArgumentTypes().size();
        }
        if (left->IsFunctionSymbol() && right->IsFunctionSymbol())
        {
            FunctionSymbol* leftFun = static_cast<FunctionSymbol*>(left);
            FunctionSymbol* rightFun = static_cast<FunctionSymbol*>(right);
            int leftArity = leftFun->Arity();
            int rightArity = rightFun->Arity();
            int n = std::min(leftArity, rightArity);
            for (int i = 0; i < n; ++i)
            {
                ParameterSymbol* leftParam = leftFun->Parameters()[i];
                ParameterSymbol* rightParam = rightFun->Parameters()[i];
                if (leftParam->GetType()->CodeName() < rightParam->GetType()->CodeName()) return true;
                if (rightParam->GetType()->CodeName() < leftParam->GetType()->CodeName()) return false;
            }
            if (leftArity < rightArity) return true;
            if (rightArity < leftArity) return false;
            if (leftFun->TemplateParameters().size() < rightFun->TemplateParameters().size()) return true;
            if (rightFun->TemplateParameters().size() < leftFun->TemplateParameters().size()) return false;
            if (!leftFun->Constraint() && rightFun->Constraint()) return true;
            if (!rightFun->Constraint() && leftFun->Constraint()) return false;
            leftFun->SetFlag(FunctionSymbolFlags::includeConstraint);
            rightFun->SetFlag(FunctionSymbolFlags::includeConstraint);
            return leftFun->Constraint()->ToString() < rightFun->Constraint()->ToString();
        }
        return left->Name() < right->Name();
    }
};

std::pair<std::string, bool> ResolveModuleXmlDocFilePath(Input* input, const std::u32string& moduleName)
{
    bool isExternalModuleDocFilePath = false;
    auto it = input->libraryPrefixMap.find(moduleName);
    if (it != input->libraryPrefixMap.cend())
    {
        std::string libraryPrefix = it->second;
        if (!libraryPrefix.empty())
        {
            isExternalModuleDocFilePath = true;
        }
        else
        {
            libraryPrefix = "content";
        }
        std::string moduleDir = Path::Combine(Path::Combine(input->baseDir, libraryPrefix), ToUtf8(moduleName));
        if (boost::filesystem::exists(moduleDir))
        {
            std::string moduleXmlDocFilePath = Path::Combine(moduleDir, ToUtf8(moduleName) + ".xml");
            if (boost::filesystem::exists(moduleXmlDocFilePath))
            {
                return std::make_pair(moduleXmlDocFilePath, isExternalModuleDocFilePath);
            }
            else
            {
                throw std::runtime_error("cmdoc module xml document '" + moduleXmlDocFilePath + "' not found");
            }
        }
    }
    throw std::runtime_error("cmdoc module directory for module '" + ToUtf8(moduleName) + "' not found");
}

struct FileByName
{
    bool operator()(File* left, File* right) const
    {
        return left->name < right->name;
    }
};

class SymbolTableXmlBuilder : public Visitor
{
public:
    SymbolTableXmlBuilder(const std::u32string& moduleName_, SymbolTable& symbolTable_, std::unordered_map<int, File>& fileMap_, const std::string& modulePrefix_,
        const std::string& extModulePrefix_);
    void WriteDocument(const std::string& symbolTableXmlFilePath);
    void AddModuleXmlDocument(dom::Document* moduleXmlDocument);
    std::unique_ptr<dom::Element> CreateSymbolElement(const std::u32string& elementName, Symbol& symbol) const;
    bool AddNamespace(NamespaceSymbol& ns);
    void AddConcept(ConceptSymbol& concept);
    void AddClass(ClassTypeSymbol& cls);
    void AddCtorOrDtor(FunctionSymbol& function);
    void AddFunction(FunctionSymbol& function);
    void AddDelegate(DelegateTypeSymbol& delegate);
    void AddClassDelegate(ClassDelegateTypeSymbol& classDelegate);
    void AddTypedef(TypedefSymbol& typedef_);
    void AddEnumType(EnumTypeSymbol& enumType);
    void AddEnumConstant(EnumConstantSymbol& enumConstant);
    void AddConstant(ConstantSymbol& constant);
    void AddVariable(VariableSymbol& variable);
    void AddChildren(ContainerSymbol& container);
    std::vector<NamespaceSymbol*> GetNamespaces(ContainerSymbol& container);
    std::vector<ConceptSymbol*> GetConcepts(ContainerSymbol& container);
    std::vector<ClassTypeSymbol*> GetClasses(ContainerSymbol& container);
    std::vector<FunctionSymbol*> GetConstructorsAndDestructors(ContainerSymbol& container);
    std::vector<FunctionSymbol*> GetFunctions(ContainerSymbol& container);
    std::vector<DelegateTypeSymbol*> GetDelegates(ContainerSymbol& container); 
    std::vector<ClassDelegateTypeSymbol*> GetClassDelegates(ContainerSymbol& container);
    std::vector<TypedefSymbol*> GetTypedefs(ContainerSymbol& container);
    std::vector<EnumTypeSymbol*> GetEnumTypes(ContainerSymbol& container);
    std::vector<EnumConstantSymbol*> GetEnumConstants(ContainerSymbol& container);
    std::vector<ConstantSymbol*> GetConstants(ContainerSymbol& container);
    std::vector<VariableSymbol*> GetVariables(ContainerSymbol& container, bool sort, bool skipFirst);
    bool IsNonemptyNamespace(dom::Element* nsElement) const;
    std::u32string GetOrInsertType(TypeSymbol* type);
    void GenerateTypes();
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
    void Visit(AxiomStatementNode& axiomStatementNode) override;
    void Visit(AxiomNode& axiomNode) override;
    void Visit(ConceptIdNode& conceptIdNode) override;
    void Visit(IdentifierNode& identifierNode) override;
    void Visit(DotNode& dotNode) override;
    void Visit(ParameterNode& parameterNode) override;
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
private:
    std::u32string moduleName;
    SymbolTable& symbolTable;
    std::unordered_map<int, File>& fileMap;
    std::string modulePrefix;
    std::string extModulePrefix;
    std::vector<dom::Document*> moduleXmlDocuments;
    std::unique_ptr<dom::Document> symbolTableXmlDocument;
    std::unique_ptr<dom::Element> symbolTableElement;
    dom::Element* currentElement;
    std::unordered_set<std::u32string> typeIdSet;
    std::vector<TypeSymbol*> types;
};

SymbolTableXmlBuilder::SymbolTableXmlBuilder(const std::u32string& moduleName_, SymbolTable& symbolTable_, std::unordered_map<int, File>& fileMap_, 
    const std::string& modulePrefix_, const std::string& extModulePrefix_) :
    moduleName(moduleName_), symbolTableXmlDocument(new dom::Document()), 
    symbolTableElement(new dom::Element(U"symbolTable")), currentElement(symbolTableElement.get()), symbolTable(symbolTable_),
    fileMap(fileMap_), modulePrefix(modulePrefix_), extModulePrefix(extModulePrefix_)
{
    symbolTableElement->SetAttribute(U"module", moduleName);
    symbolTableElement->SetAttribute(U"id", moduleName);
    std::vector<File*> files;
    for (auto& p : fileMap)
    {
        files.push_back(&p.second);
    }
    std::sort(files.begin(), files.end(), FileByName());
    std::unique_ptr<dom::Element> filesElement(new dom::Element(U"files"));
    for (File* file : files)
    {
        std::unique_ptr<dom::Element> fileElement(new dom::Element(U"file"));
        fileElement->SetAttribute(U"name", file->name);
        fileElement->SetAttribute(U"path", ToUtf32(Path::Combine("file", Path::GetFileName(file->htmlFilePath))));
        filesElement->AppendChild(std::unique_ptr<dom::Node>(fileElement.release()));
    }
    symbolTableElement->AppendChild(std::unique_ptr<dom::Node>(filesElement.release()));
}

void SymbolTableXmlBuilder::WriteDocument(const std::string& symbolTableXmlFilePath)
{
    GenerateTypes();
    symbolTableXmlDocument->AppendChild(std::unique_ptr<dom::Node>(symbolTableElement.release()));
    std::ofstream symbolTableXmlFile(symbolTableXmlFilePath);
    CodeFormatter formatter(symbolTableXmlFile);
    formatter.SetIndentSize(1);
    symbolTableXmlDocument->Write(formatter);
}

void SymbolTableXmlBuilder::AddModuleXmlDocument(dom::Document* moduleXmlDocument)
{
    moduleXmlDocuments.push_back(moduleXmlDocument);
}

bool SymbolTableXmlBuilder::IsNonemptyNamespace(dom::Element* nsElement) const
{
    dom::Node* child = nsElement->FirstChild();
    while (child)
    {
        if (child->GetNodeType() == dom::NodeType::elementNode)
        {
            dom::Element* element = static_cast<dom::Element*>(child);
            if (element->Name()  == U"namespaces" || element->Name() == U"namespace")
            {
                if (IsNonemptyNamespace(element))
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
        child = child->NextSibling();
    }
    return false;
}

std::u32string SymbolTableXmlBuilder::GetOrInsertType(TypeSymbol* type)
{
    std::u32string typeId = type->Id();
    int n = moduleXmlDocuments.size();
    for (int i = 0; i < n; ++i)
    {
        dom::Document* moduleDoc = moduleXmlDocuments[i];
        std::lock_guard<std::mutex> lock(GetInputMutex());
        dom::Element* element = moduleDoc->GetElementById(typeId);
        if (element)
        {
            return typeId;
        }
    }
    auto it = typeIdSet.find(typeId);
    if (it != typeIdSet.cend())
    {
        return typeId;
    }
    else
    {
        if (type->GetSymbolType() == SymbolType::classTypeSymbol ||
            type->GetSymbolType() == SymbolType::delegateTypeSymbol ||
            type->GetSymbolType() == SymbolType::classDelegateTypeSymbol ||
            type->GetSymbolType() == SymbolType::enumTypeSymbol ||
            type->GetSymbolType() == SymbolType::interfaceTypeSymbol || 
            type->GetSymbolType() == SymbolType::templateParameterSymbol)
        {
            return typeId;
        }
        else
        {
            if (type->IsBasicTypeSymbol())
            {
                typeIdSet.insert(typeId);
                types.push_back(type);
                return typeId;
            }
            else if (type->GetSymbolType() == SymbolType::derivedTypeSymbol)
            {
                DerivedTypeSymbol* derivedTypeSymbol = static_cast<DerivedTypeSymbol*>(type);
                GetOrInsertType(derivedTypeSymbol->BaseType());
                typeIdSet.insert(typeId);
                types.push_back(type);
                return typeId;
            }
            else if (type->GetSymbolType() == SymbolType::arrayTypeSymbol)
            {
                ArrayTypeSymbol* arrayTypeSymbol = static_cast<ArrayTypeSymbol*>(type);
                GetOrInsertType(arrayTypeSymbol->ElementType());
                typeIdSet.insert(typeId);
                types.push_back(type);
                return typeId;
            }
            else if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
            {
                ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
                if (!specialization->IsPrototype())
                {
                    ClassTypeSymbol* classTemplate = specialization->GetClassTemplate();
                    GetOrInsertType(classTemplate->Prototype());
                    for (TypeSymbol* templateArgumentType : specialization->TemplateArgumentTypes())
                    {
                        GetOrInsertType(templateArgumentType);
                    }
                    typeIdSet.insert(typeId);
                    types.push_back(type);
                    return typeId;
                }
                else
                {
                    return typeId;
                }
            }
            else
            {
                throw std::runtime_error("unexpected type symbol");
            }
        }
    }
}

void SymbolTableXmlBuilder::GenerateTypes()
{
    std::unique_ptr<dom::Element> typesElement(new dom::Element(U"types"));
    for (TypeSymbol* type : types)
    {
        if (type->IsBasicTypeSymbol())
        {
            std::unique_ptr<dom::Element> typeElement = CreateSymbolElement(U"basicType", *type);
            typesElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
        }
        else if (type->GetSymbolType() == SymbolType::derivedTypeSymbol)
        {
            DerivedTypeSymbol* derivedTypeSymbol = static_cast<DerivedTypeSymbol*>(type);
            std::unique_ptr<dom::Element> typeElement = CreateSymbolElement(U"derivedType", *derivedTypeSymbol);
            if (HasFrontConstDerivation(derivedTypeSymbol->DerivationRec().derivations))
            {
                std::unique_ptr<dom::Element> constDerivationElement(new dom::Element(U"derivation"));
                constDerivationElement->SetAttribute(U"name", U"const");
                typeElement->AppendChild(std::unique_ptr<dom::Node>(constDerivationElement.release()));
            }
            std::unique_ptr<dom::Element> baseDerivationElement(new dom::Element(U"derivation"));
            baseDerivationElement->SetAttribute(U"baseType", type->BaseType()->Id());
            typeElement->AppendChild(std::unique_ptr<dom::Node>(baseDerivationElement.release()));
            for (Derivation derivation : derivedTypeSymbol->DerivationRec().derivations)
            {
                switch (derivation)
                {
                    case Derivation::lvalueRefDerivation:
                    {
                        std::unique_ptr<dom::Element> lvalueRefDerivationElement(new dom::Element(U"derivation"));
                        lvalueRefDerivationElement->SetAttribute(U"name", U"&");
                        typeElement->AppendChild(std::unique_ptr<dom::Node>(lvalueRefDerivationElement.release()));
                        break;
                    }
                    case Derivation::rvalueRefDerivation:
                    {
                        std::unique_ptr<dom::Element> rvalueRefDerivationElement(new dom::Element(U"derivation"));
                        rvalueRefDerivationElement->SetAttribute(U"name", U"&&");
                        typeElement->AppendChild(std::unique_ptr<dom::Node>(rvalueRefDerivationElement.release()));
                        break;
                    }
                    case Derivation::pointerDerivation:
                    {
                        std::unique_ptr<dom::Element> pointerDerivationElement(new dom::Element(U"derivation"));
                        pointerDerivationElement->SetAttribute(U"name", U"*");
                        typeElement->AppendChild(std::unique_ptr<dom::Node>(pointerDerivationElement.release()));
                        break;
                    }
                }
            }
            typesElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
        }
        else if (type->GetSymbolType() == SymbolType::arrayTypeSymbol)
        {
            ArrayTypeSymbol* arrayTypeSymbol = static_cast<ArrayTypeSymbol*>(type);
            std::unique_ptr<dom::Element> typeElement = CreateSymbolElement(U"arrayType", *arrayTypeSymbol);
            typeElement->SetAttribute(U"elementType", arrayTypeSymbol->ElementType()->Id());
            if (arrayTypeSymbol->Size() != -1)
            {
                typeElement->SetAttribute(U"size", ToUtf32(std::to_string(arrayTypeSymbol->Size())));
            }
            typesElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
        }
        else if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
            if (!specialization->IsPrototype())
            {
                std::unique_ptr<dom::Element> typeElement = CreateSymbolElement(U"specialization", *specialization);
                ClassTypeSymbol* classTemplate = specialization->GetClassTemplate();
                typeElement->SetAttribute(U"primaryType", classTemplate->Id());
                std::unique_ptr<dom::Element> argumentTypesElement(new dom::Element(U"templateArgumentTypes"));
                for (TypeSymbol* templateArgumentType : specialization->TemplateArgumentTypes())
                {
                    std::unique_ptr<dom::Element> argumentTypeElement(new dom::Element(U"templateArgumentType"));
                    argumentTypeElement->SetAttribute(U"ref", templateArgumentType->Id());
                    argumentTypesElement->AppendChild(std::unique_ptr<dom::Node>(argumentTypeElement.release()));
                }
                typeElement->AppendChild(std::unique_ptr<dom::Node>(argumentTypesElement.release()));
                typesElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
            }
        }
        else
        {
            throw std::runtime_error("unexpected type symbol");
        }
    }
    symbolTableElement->AppendChild(std::unique_ptr<dom::Node>(typesElement.release()));
}

void SymbolTableXmlBuilder::Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode)
{
    std::unique_ptr<dom::Element> parensElement(new dom::Element(U"parens"));
    dom::Element* prevElement = currentElement;
    currentElement = parensElement.get();
    parenthesizedConstraintNode.Constraint()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(parensElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode)
{
    std::unique_ptr<dom::Element> orElement(new dom::Element(U"or"));
    dom::Element* prevElement = currentElement;
    currentElement = orElement.get();
    disjunctiveConstraintNode.Left()->Accept(*this);
    disjunctiveConstraintNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(orElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode)
{
    std::unique_ptr<dom::Element> andElement(new dom::Element(U"and"));
    dom::Element* prevElement = currentElement;
    currentElement = andElement.get();
    conjunctiveConstraintNode.Left()->Accept(*this);
    conjunctiveConstraintNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(andElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(WhereConstraintNode& whereConstraintNode)
{
    std::unique_ptr<dom::Element> whereElement(new dom::Element(U"where"));
    dom::Element* prevElement = currentElement;
    currentElement = whereElement.get();
    whereConstraintNode.Constraint()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(whereElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(PredicateConstraintNode& predicateConstraintNode)
{
    std::unique_ptr<dom::Element> predicateElement(new dom::Element(U"predicate"));
    dom::Element* prevElement = currentElement;
    currentElement = predicateElement.get();
    currentElement ->SetAttribute(U"value", ToUtf32(predicateConstraintNode.InvokeExpr()->ToString()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(predicateElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(IsConstraintNode& isConstraintNode)
{
    std::unique_ptr<dom::Element> isElement(new dom::Element(U"is"));
    dom::Element* prevElement = currentElement;
    currentElement = isElement.get();
    isConstraintNode.TypeExpr()->Accept(*this);
    isConstraintNode.ConceptOrTypeName()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(isElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(MultiParamConstraintNode& multiParamConstraintNode)
{
    std::unique_ptr<dom::Element> multiElement(new dom::Element(U"multi"));
    dom::Element* prevElement = currentElement;
    currentElement = multiElement.get();
    multiParamConstraintNode.ConceptId()->Accept(*this);
    std::unique_ptr<dom::Element> typesElement(new dom::Element(U"types"));
    dom::Element* prevTypesElement = currentElement;
    currentElement = typesElement.get();
    int n = multiParamConstraintNode.TypeExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        multiParamConstraintNode.TypeExprs()[i]->Accept(*this);
    }
    prevTypesElement->AppendChild(std::unique_ptr<dom::Node>(typesElement.release()));
    currentElement = prevTypesElement;
    prevElement->AppendChild(std::unique_ptr<dom::Node>(multiElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(TypeNameConstraintNode& typeNameConstraintNode)
{
    std::unique_ptr<dom::Element> typenameElement(new dom::Element(U"typename"));
    dom::Element* prevElement = currentElement;
    currentElement = typenameElement.get();
    typeNameConstraintNode.TypeId()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(typenameElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ConstructorConstraintNode& constructorConstraintNode)
{
    std::unique_ptr<dom::Element> signatureConstraintElement(new dom::Element(U"signature"));
    dom::Element* prevElement = currentElement;
    currentElement = signatureConstraintElement.get();
    signatureConstraintElement->SetAttribute(U"value", ToUtf32(constructorConstraintNode.ToString()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(signatureConstraintElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(DestructorConstraintNode& destructorConstraintNode)
{
    std::unique_ptr<dom::Element> signatureConstraintElement(new dom::Element(U"signature"));
    dom::Element* prevElement = currentElement;
    currentElement = signatureConstraintElement.get();
    signatureConstraintElement->SetAttribute(U"value", ToUtf32(destructorConstraintNode.ToString()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(signatureConstraintElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode)
{
    std::unique_ptr<dom::Element> signatureConstraintElement(new dom::Element(U"signature"));
    dom::Element* prevElement = currentElement;
    currentElement = signatureConstraintElement.get();
    signatureConstraintElement->SetAttribute(U"value", ToUtf32(memberFunctionConstraintNode.ToString()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(signatureConstraintElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(FunctionConstraintNode& functionConstraintNode)
{
    std::unique_ptr<dom::Element> signatureConstraintElement(new dom::Element(U"signature"));
    dom::Element* prevElement = currentElement;
    currentElement = signatureConstraintElement.get();
    signatureConstraintElement->SetAttribute(U"value", ToUtf32(functionConstraintNode.ToString()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(signatureConstraintElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(AxiomStatementNode& axiomStatementNode)
{
    std::unique_ptr<dom::Element> axiomStatementElement(new dom::Element(U"axiomStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = axiomStatementElement.get();
    axiomStatementElement->SetAttribute(U"value", axiomStatementNode.Text());
    prevElement->AppendChild(std::unique_ptr<dom::Node>(axiomStatementElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(AxiomNode& axiomNode)
{
    std::unique_ptr<dom::Element> axiomElement(new dom::Element(U"axiom"));
    dom::Element* prevElement = currentElement;
    currentElement = axiomElement.get();
    if (axiomNode.Id())
    {
        axiomNode.Id()->Accept(*this);
    }
    int np = axiomNode.Parameters().Count();
    if (np > 0)
    {
        std::unique_ptr<dom::Element> parametersElement(new dom::Element(U"parameters"));
        dom::Element* prevElement = currentElement;
        currentElement = parametersElement.get();
        for (int i = 0; i < np; ++i)
        {
            axiomNode.Parameters()[i]->Accept(*this);
        }
        prevElement->AppendChild(std::unique_ptr<dom::Node>(parametersElement.release()));
        currentElement = prevElement;
    }
    int n = axiomNode.Statements().Count();
    if (n > 0)
    {
        std::unique_ptr<dom::Element> statementsElement(new dom::Element(U"axiomStatements"));
        dom::Element* prevElement = currentElement;
        currentElement = statementsElement.get();
        for (int i = 0; i < n; ++i)
        {
            axiomNode.Statements()[i]->Accept(*this);
        }
        prevElement->AppendChild(std::unique_ptr<dom::Node>(statementsElement.release()));
        currentElement = prevElement;
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(axiomElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ConceptIdNode& conceptIdNode)
{
    conceptIdNode.Id()->Accept(*this);
    std::unique_ptr<dom::Element> typeParamsElement(new dom::Element(U"typeParams"));
    dom::Element* prevElement = currentElement;
    currentElement = typeParamsElement.get();
    int n = conceptIdNode.TypeParameters().Count();
    for (int i = 0; i < n; ++i)
    {
        conceptIdNode.TypeParameters()[i]->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(typeParamsElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(IdentifierNode& identifierNode)
{
    std::unique_ptr<dom::Element> symbolElement(new dom::Element(U"symbol"));
    Symbol* symbol = symbolTable.GetMappedSymbol(&identifierNode);
    if (symbol)
    {
        symbolElement->SetAttribute(U"ref", symbol->Id());
    }
    symbolElement->SetAttribute(U"name", identifierNode.Str());
    currentElement->AppendChild(std::unique_ptr<dom::Node>(symbolElement.release()));
}

void SymbolTableXmlBuilder::Visit(DotNode& dotNode)
{
    std::unique_ptr<dom::Element> dotElement(new dom::Element(U"dot"));
    dom::Element* prevElement = currentElement;
    currentElement = dotElement.get();
    dotNode.Subject()->Accept(*this); 
    dotNode.MemberId()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(dotElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ParameterNode& parameterNode)
{
    std::unique_ptr<dom::Element> parameterElement(new dom::Element(U"parameter"));
    dom::Element* prevElement = currentElement;
    currentElement = parameterElement.get();
    parameterNode.TypeExpr()->Accept(*this);
    if (parameterNode.Id() && !parameterNode.ArtificialId())
    {
        parameterNode.Id()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(parameterElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(BoolNode& boolNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"bool");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(SByteNode& sbyteNode) 
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"sbyte");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ByteNode& byteNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"byte");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ShortNode& shortNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"short");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(UShortNode& ushortNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"ushort");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(IntNode& intNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"int");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(UIntNode& uintNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"uint");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(LongNode& longNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"long");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ULongNode& ulongNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"ulong");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(FloatNode& floatNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"float");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(DoubleNode& doubleNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"double");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(CharNode& charNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"char");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(WCharNode& wcharNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"wchar");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(UCharNode& ucharNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"uchar");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(VoidNode& voidNode)
{
    std::unique_ptr<dom::Element> keywordElement(new dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"void");
    dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

std::unique_ptr<dom::Element> SymbolTableXmlBuilder::CreateSymbolElement(const std::u32string& elementName, Symbol& symbol) const
{
    std::unique_ptr<dom::Element> symbolElement(new dom::Element(elementName));
    symbolElement->SetAttribute(U"name", symbol.CodeName());
    symbolElement->SetAttribute(U"id", symbol.Id());
    return symbolElement;
}

bool SymbolTableXmlBuilder::AddNamespace(NamespaceSymbol& ns)
{
    bool added = false;
    std::unique_ptr<dom::Element> nsElement = CreateSymbolElement(U"namespace", ns);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(ns.Id()) + ".html"));
    nsElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(ns.Id()) + ".html"));
    nsElement->SetAttribute(U"extPath", extPath);
    dom::Element* prevElement = currentElement;
    currentElement = nsElement.get();
    AddChildren(ns);
    if (IsNonemptyNamespace(nsElement.get()))
    {
        prevElement->AppendChild(std::unique_ptr<dom::Node>(nsElement.release()));
        added = true;
    }
    currentElement = prevElement;
    return added;
}

void SymbolTableXmlBuilder::AddConcept(ConceptSymbol& concept)
{
    std::unique_ptr<dom::Element> conceptElement = CreateSymbolElement(U"concept", concept);
    if (concept.RefinedConcept())
    {
        conceptElement->SetAttribute(U"refines", concept.RefinedConcept()->Id());
        AddConceptRefinement(concept.RefinedConcept(), &concept);
    }
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(concept.Id()) + ".html"));
    conceptElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(concept.Id()) + ".html"));
    conceptElement->SetAttribute(U"extPath", extPath);
    int n = concept.TemplateParameters().size();
    if (n > 0)
    {
        std::unique_ptr<dom::Element> typeParametersElement(new dom::Element(U"typeParameters"));
        for (int i = 0; i < n; ++i)
        {
            std::unique_ptr<dom::Element> typeParameterElement = CreateSymbolElement(U"typeParameter", *concept.TemplateParameters()[i]);
            std::u32string docPath = conceptElement->GetAttribute(U"docPath") + U"#" + typeParameterElement->GetAttribute(U"id");
            typeParameterElement->SetAttribute(U"docPath", docPath);
            typeParametersElement->AppendChild(std::unique_ptr<dom::Node>(typeParameterElement.release()));
        }
        conceptElement->AppendChild(std::unique_ptr<dom::Node>(typeParametersElement.release()));
    }
    if (concept.HasSource())
    {
        File& file = fileMap[concept.GetSpan().FileIndex()];
        conceptElement->SetAttribute(U"fileName", file.name);
        conceptElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
        conceptElement->SetAttribute(U"line", ToUtf32(std::to_string(concept.GetSpan().LineNumber())));
    }
    Node* node = symbolTable.GetNodeNoThrow(&concept);
    ConceptNode* conceptNode = nullptr;
    if (node && node->GetNodeType() == NodeType::conceptNode)
    {
        conceptNode = static_cast<ConceptNode*>(node);
        conceptElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(conceptNode->GetSpecifiers())));
    }
    dom::Element* prevElement = currentElement;
    currentElement = conceptElement.get();
    if (conceptNode)
    {
        int nc = conceptNode->Constraints().Count();
        if (nc > 0)
        {
            std::unique_ptr<dom::Element> constraintsElement(new dom::Element(U"constraints"));
            dom::Element* prevElement = currentElement;
            currentElement = constraintsElement.get();
            for (int i = 0; i < nc; ++i)
            {
                ConstraintNode* constraint = conceptNode->Constraints()[i];
                constraint->Accept(*this);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(constraintsElement.release()));
            currentElement = prevElement;
        }
        int na = conceptNode->Axioms().Count();
        if (na > 0)
        {
            std::unique_ptr<dom::Element> axiomsElement(new dom::Element(U"axioms"));
            dom::Element* prevElement = currentElement;
            currentElement = axiomsElement.get();
            for (int i = 0; i < na; ++i)
            {
                AxiomNode* axiom = conceptNode->Axioms()[i];
                axiom->Accept(*this);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(axiomsElement.release()));
            currentElement = prevElement;
        }
    }
    AddChildren(concept);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conceptElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddClass(ClassTypeSymbol& cls)
{
    std::unique_ptr<dom::Element> classElement = CreateSymbolElement(U"class", cls);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(cls.Id()) + ".html"));
    classElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(cls.Id()) + ".html"));
    classElement->SetAttribute(U"extPath", extPath);
    File& file = fileMap[cls.GetSpan().FileIndex()];
    classElement->SetAttribute(U"fileName", file.name);
    classElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
    classElement->SetAttribute(U"line", ToUtf32(std::to_string(cls.GetSpan().LineNumber())));
    Node* node = symbolTable.GetNodeNoThrow(&cls);
    if (node && node->GetNodeType() == NodeType::classNode)
    {
        ClassNode* classNode = static_cast<ClassNode*>(node);
        classElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(classNode->GetSpecifiers())));
    }
    else
    {
        throw std::runtime_error("class node expected"); 
    }
    if (cls.GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(&cls);
        if (specialization->IsPrototype())
        {
            std::unique_ptr<dom::Element> templateParametersElement(new dom::Element(U"templateParameters"));
            int n = specialization->TemplateArgumentTypes().size();
            for (int i = 0; i < n; ++i)
            {
                TypeSymbol* templateArgumentType = specialization->TemplateArgumentTypes()[i];
                if (templateArgumentType->GetSymbolType() == SymbolType::templateParameterSymbol)
                {
                    TemplateParameterSymbol* templateParameter = static_cast<TemplateParameterSymbol*>(templateArgumentType);
                    std::unique_ptr<dom::Element> templateParameterElement = CreateSymbolElement(U"templateParameter", *templateParameter);
                    std::u32string docPath = classElement->GetAttribute(U"docPath") + U"#" + templateParameter->Id();
                    templateParameterElement->SetAttribute(U"docPath", docPath);
                    if (templateParameter->HasDefault())
                    {
                        GetOrInsertType(templateParameter->DefaultType());
                        templateParameterElement->SetAttribute(U"default", templateParameter->DefaultType()->Id());
                    }
                    templateParametersElement->AppendChild(std::unique_ptr<dom::Node>(templateParameterElement.release()));
                }
                else
                {
                    throw std::runtime_error("template parameter expected");
                }
            }
            classElement->AppendChild(std::unique_ptr<dom::Node>(templateParametersElement.release()));
        }
        else
        {
            throw std::runtime_error("prototype expected");
        }
    }
    if (cls.BaseClass())
    {
        ClassTypeSymbol* baseClass = cls.BaseClass();
        std::u32string baseClassSpecializationName;
        if (baseClass->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(baseClass);
            baseClass = specialization->GetClassTemplate();
            baseClassSpecializationName = baseClass->Ns()->FullName();
            if (!baseClassSpecializationName.empty())
            {
                baseClassSpecializationName.append(1, '.');
            }
            baseClassSpecializationName.append(specialization->Name());
        }
        std::u32string baseClassRef = GetOrInsertType(baseClass);
        classElement->SetAttribute(U"baseClass", baseClassRef);
        if (!baseClassSpecializationName.empty())
        {
            classElement->SetAttribute(U"baseClassSpecializationName", baseClassSpecializationName);
        }
        AddDerivedClass(baseClass, &cls);
    }
    dom::Element* prevElement = currentElement;
    currentElement = classElement.get();
    AddChildren(cls);
    if (cls.Constraint())
    {
        cls.Constraint()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(classElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddCtorOrDtor(FunctionSymbol& function)
{
    std::unique_ptr<dom::Element> ctorDtorElement = CreateSymbolElement(U"ctorDtor", function);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(function.Parent()->Id()) + ".html#" + ToUtf8(function.Id())));
    ctorDtorElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(function.Parent()->Id()) + ".html#" + ToUtf8(function.Id())));
    ctorDtorElement->SetAttribute(U"extPath", extPath);
    File& file = fileMap[function.GetSpan().FileIndex()];
    ctorDtorElement->SetAttribute(U"fileName", file.name);
    ctorDtorElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
    ctorDtorElement->SetAttribute(U"line", ToUtf32(std::to_string(function.GetSpan().LineNumber())));
    if (function.GetSymbolType() == SymbolType::staticConstructorSymbol)
    {
        ctorDtorElement->SetAttribute(U"kind", U"Static Constructor");
    }
    else if (function.GetSymbolType() == SymbolType::constructorSymbol)
    {
        ctorDtorElement->SetAttribute(U"kind", U"Constructor");
    }
    else if (function.GetSymbolType() == SymbolType::destructorSymbol)
    {
        ctorDtorElement->SetAttribute(U"kind", U"Destructor");
    }
    Node* node = symbolTable.GetNodeNoThrow(&function);
    if (node && node->IsFunctionNode())
    {
        FunctionNode* functionNode = static_cast<FunctionNode*>(node);
        ctorDtorElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(functionNode->GetSpecifiers())));
    }
    else
    {
        throw std::runtime_error("function node expected");
    }
    dom::Element* prevElement = currentElement;
    currentElement = ctorDtorElement.get();
    AddChildren(function);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(ctorDtorElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddFunction(FunctionSymbol& function)
{
    std::unique_ptr<dom::Element> functionElement = CreateSymbolElement(U"function", function);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(function.Parent()->Id()) + ".html#" + ToUtf8(function.Id())));
    if (function.Parent()->Id().empty() || function.Parent()->Id() == U"ns_")
    {
        docPath = ToUtf32(Path::Combine(modulePrefix, "../index.html#" + ToUtf8(function.Id())));
    }
    functionElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(function.Parent()->Id()) + ".html#" + ToUtf8(function.Id())));
    if (function.Parent()->Id().empty() || function.Parent()->Id() == U"ns_")
    {
        extPath = ToUtf32(Path::Combine(extModulePrefix, "../index.html#" + ToUtf8(function.Id())));
    }
    functionElement->SetAttribute(U"extPath", extPath);
    if (function.HasSource())
    {
        File& file = fileMap[function.GetSpan().FileIndex()];
        functionElement->SetAttribute(U"fileName", file.name);
        functionElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
        functionElement->SetAttribute(U"line", ToUtf32(std::to_string(function.GetSpan().LineNumber())));
    }
    if (function.GetSymbolType() == SymbolType::functionSymbol)
    {
        functionElement->SetAttribute(U"kind", U"Function");
    }
    else if (function.GetSymbolType() == SymbolType::memberFunctionSymbol)
    {
        if (function.IsStatic())
        {
            functionElement->SetAttribute(U"kind", U"Static Member Function");
        }
        else
        {
            functionElement->SetAttribute(U"kind", U"Member Function");
        }
    }
    else if (function.GetSymbolType() == SymbolType::conversionFunctionSymbol)
    {
        functionElement->SetAttribute(U"kind", U"Conversion Function");
    }
    if (function.GetFlag(FunctionSymbolFlags::includeConstraint))
    {
        functionElement->SetAttribute(U"includeConstraint", U"true");
    }
    if (function.HasSource())
    {
        Node* node = symbolTable.GetNodeNoThrow(&function);
        if (node && node->IsFunctionNode())
        {
            FunctionNode* functionNode = static_cast<FunctionNode*>(node);
            functionElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(functionNode->GetSpecifiers())));
        }
        else
        {
            throw std::runtime_error("function node expected");
        }
    }
    if (function.IsConst())
    {
        functionElement->SetAttribute(U"const", U"true");
    }
    int n = function.TemplateParameters().size();
    if (n > 0)
    {
        std::unique_ptr<dom::Element> templateParametersElement(new dom::Element(U"templateParameters"));
        for (int i = 0; i < n; ++i)
        {
            std::unique_ptr<dom::Element> templateParameterElement = CreateSymbolElement(U"templateParameter", *function.TemplateParameters()[i]);
            std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(function.Parent()->Id()) + ".html#" + ToUtf8(function.TemplateParameters()[i]->Id())));
            templateParameterElement->SetAttribute(U"docPath", docPath);
            std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(function.Parent()->Id()) + ".html#" + ToUtf8(function.TemplateParameters()[i]->Id())));
            templateParameterElement->SetAttribute(U"extPath", extPath);
            templateParametersElement->AppendChild(std::unique_ptr<dom::Node>(templateParameterElement.release()));
        }
        functionElement->AppendChild(std::unique_ptr<dom::Node>(templateParametersElement.release()));
    }
    TypeSymbol* returnType = function.ReturnType();
    Assert(returnType, "nonnull return type expected");
    std::u32string returnTypeId = GetOrInsertType(returnType);
    functionElement->SetAttribute(U"returnType", returnTypeId);
    dom::Element* prevElement = currentElement;
    currentElement = functionElement.get();
    AddChildren(function);
    if (function.Constraint())
    {
        function.Constraint()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(functionElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddDelegate(DelegateTypeSymbol& delegate)
{
    std::unique_ptr<dom::Element> delegateElement = CreateSymbolElement(U"delegate", delegate);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(delegate.Parent()->Id()) + ".html#" + ToUtf8(delegate.Id())));
    delegateElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(delegate.Parent()->Id()) + ".html#" + ToUtf8(delegate.Id())));
    delegateElement->SetAttribute(U"extPath", extPath);
    File& file = fileMap[delegate.GetSpan().FileIndex()];
    delegateElement->SetAttribute(U"fileName", file.name);
    delegateElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
    delegateElement->SetAttribute(U"line", ToUtf32(std::to_string(delegate.GetSpan().LineNumber())));
    TypeSymbol* returnType = delegate.ReturnType();
    Assert(returnType, "nonnull return type expected");
    std::u32string returnTypeId = GetOrInsertType(returnType);
    delegateElement->SetAttribute(U"returnType", returnTypeId);
    Node* node = symbolTable.GetNodeNoThrow(&delegate);
    if (node && node->GetNodeType() == NodeType::delegateNode)
    {
        DelegateNode* delegateNode = static_cast<DelegateNode*>(node);
        delegateElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(delegateNode->GetSpecifiers())));
    }
    else
    {
        throw std::runtime_error("delegate node expected");
    }
    dom::Element* prevElement = currentElement;
    currentElement = delegateElement.get();
    AddChildren(delegate);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(delegateElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddClassDelegate(ClassDelegateTypeSymbol& classDelegate)
{
    std::unique_ptr<dom::Element> classDelegateElement = CreateSymbolElement(U"classDelegate", classDelegate);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(classDelegate.Parent()->Id()) + ".html#" + ToUtf8(classDelegate.Id())));
    classDelegateElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(classDelegate.Parent()->Id()) + ".html#" + ToUtf8(classDelegate.Id())));
    classDelegateElement->SetAttribute(U"extPath", extPath);
    File& file = fileMap[classDelegate.GetSpan().FileIndex()];
    classDelegateElement->SetAttribute(U"fileName", file.name);
    classDelegateElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
    classDelegateElement->SetAttribute(U"line", ToUtf32(std::to_string(classDelegate.GetSpan().LineNumber())));
    TypeSymbol* returnType = classDelegate.ReturnType();
    Assert(returnType, "nonnull return type expected");
    std::u32string returnTypeId = GetOrInsertType(returnType);
    classDelegateElement->SetAttribute(U"returnType", returnTypeId);
    Node* node = symbolTable.GetNodeNoThrow(&classDelegate);
    if (node && node->GetNodeType() == NodeType::classDelegateNode)
    {
        ClassDelegateNode* classDelegateNode = static_cast<ClassDelegateNode*>(node);
        classDelegateElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(classDelegateNode->GetSpecifiers())));
    }
    else
    {
        throw std::runtime_error("class delegate node expected");
    }
    dom::Element* prevElement = currentElement;
    currentElement = classDelegateElement.get();
    AddChildren(classDelegate);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(classDelegateElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddTypedef(TypedefSymbol& typedef_)
{
    std::unique_ptr<dom::Element> typedefElement = CreateSymbolElement(U"typedef", typedef_);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(typedef_.Parent()->Id()) + ".html#" + ToUtf8(typedef_.Id())));
    typedefElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(typedef_.Parent()->Id()) + ".html#" + ToUtf8(typedef_.Id())));
    typedefElement->SetAttribute(U"extPath", extPath);
    TypeSymbol* type = typedef_.GetType();
    Assert(type, "nonnull type expected");
    std::u32string typeId = GetOrInsertType(type);
    typedefElement->SetAttribute(U"type", typeId);
    Node* node = symbolTable.GetNodeNoThrow(&typedef_);
    if (node && node->GetNodeType() == NodeType::typedefNode)
    {
        TypedefNode* typedefNode = static_cast<TypedefNode*>(node);
        typedefElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(typedefNode->GetSpecifiers())));
    }
    else
    {
        throw std::runtime_error("typedef node expected");
    }
    dom::Element* prevElement = currentElement;
    currentElement = typedefElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(typedefElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddEnumType(EnumTypeSymbol& enumType)
{
    std::unique_ptr<dom::Element> enumTypeElement = CreateSymbolElement(U"enumType", enumType);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(enumType.Id()) + ".html"));
    enumTypeElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(enumType.Id()) + ".html"));
    enumTypeElement->SetAttribute(U"extPath", extPath);
    File& file = fileMap[enumType.GetSpan().FileIndex()];
    enumTypeElement->SetAttribute(U"fileName", file.name);
    enumTypeElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
    enumTypeElement->SetAttribute(U"line", ToUtf32(std::to_string(enumType.GetSpan().LineNumber())));
    TypeSymbol* underlyingType = enumType.UnderlyingType();
    Assert(underlyingType, "nonnull type expected");
    std::u32string underlyingTypeId = GetOrInsertType(underlyingType);
    enumTypeElement->SetAttribute(U"underlyingType", underlyingTypeId);
    Node* node = symbolTable.GetNodeNoThrow(&enumType);
    if (node && node->GetNodeType() == NodeType::enumTypeNode)
    {
        EnumTypeNode* enumTypeNode = static_cast<EnumTypeNode*>(node);
        enumTypeElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(enumTypeNode->GetSpecifiers())));
    }
    else
    {
        throw std::runtime_error("enum type node expected");
    }
    dom::Element* prevElement = currentElement;
    currentElement = enumTypeElement.get();
    AddChildren(enumType);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(enumTypeElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddEnumConstant(EnumConstantSymbol& enumConstant)
{
    std::unique_ptr<dom::Element> enumConstantElement = CreateSymbolElement(U"enumConstant", enumConstant);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(enumConstant.Parent()->Id()) + ".html#" + ToUtf8(enumConstant.Id())));
    enumConstantElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(enumConstant.Parent()->Id()) + ".html#" + ToUtf8(enumConstant.Id())));
    enumConstantElement->SetAttribute(U"extPath", extPath);
    if (!enumConstant.StrValue().empty())
    {
        enumConstantElement->SetAttribute(U"value", enumConstant.StrValue());
    }
    dom::Element* prevElement = currentElement;
    currentElement = enumConstantElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(enumConstantElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddConstant(ConstantSymbol& constant)
{
    std::unique_ptr<dom::Element> constantElement = CreateSymbolElement(U"constant", constant);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(constant.Parent()->Id()) + ".html#" + ToUtf8(constant.Id())));
    constantElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(constant.Parent()->Id()) + ".html#" + ToUtf8(constant.Id())));
    constantElement->SetAttribute(U"extPath", extPath);
    if (!constant.StrValue().empty())
    {
        constantElement->SetAttribute(U"value", constant.StrValue());
    }
    TypeSymbol* type = constant.GetType();
    Assert(type, "nonnull type expected");
    std::u32string typeId = GetOrInsertType(type);
    constantElement->SetAttribute(U"type", typeId);
    Node* node = symbolTable.GetNodeNoThrow(&constant);
    if (node && node->GetNodeType() == NodeType::constantNode)
    {
        ConstantNode* constantNode = static_cast<ConstantNode*>(node);
        constantElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(constantNode->GetSpecifiers())));
    }
    else
    {
        throw std::runtime_error("constant node expected");
    }
    dom::Element* prevElement = currentElement;
    currentElement = constantElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(constantElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddVariable(VariableSymbol& variable)
{
    if (variable.GetSymbolType() == SymbolType::localVariableSymbol) return;
    std::u32string elementName = U"variable";
    if (variable.GetSymbolType() == SymbolType::parameterSymbol)
    {
        elementName = U"parameter";
    }
    std::unique_ptr<dom::Element> variableElement = CreateSymbolElement(elementName, variable);
    if (variable.GetSymbolType() == SymbolType::memberVariableSymbol)
    {
        std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(variable.Parent()->Id()) + ".html#" + ToUtf8(variable.Id())));
        variableElement->SetAttribute(U"docPath", docPath);
        std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(variable.Parent()->Id()) + ".html#" + ToUtf8(variable.Id())));
        variableElement->SetAttribute(U"extPath", extPath);
    }
    TypeSymbol* type = variable.GetType();
    Assert(type, "nonnull type expected");
    std::u32string typeId = GetOrInsertType(type);
    variableElement->SetAttribute(U"type", typeId);
    Node* node = symbolTable.GetNodeNoThrow(&variable);
    if (node)
    {
        if (node->GetNodeType() == NodeType::memberVariableNode)
        {
            MemberVariableNode* memberVariableNode = static_cast<MemberVariableNode*>(node);
            variableElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(memberVariableNode->GetSpecifiers())));
        }
    }
    else
    {
        throw std::runtime_error("variable node expected");
    }
    dom::Element* prevElement = currentElement;
    currentElement = variableElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(variableElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddChildren(ContainerSymbol& container)
{
    if (container.GetSymbolType() != SymbolType::classDelegateTypeSymbol && container.GetSymbolType() != SymbolType::delegateTypeSymbol)
    {
        bool added = false;
        std::vector<NamespaceSymbol*> namespaces = GetNamespaces(container);
        if (!namespaces.empty())
        {
            std::unique_ptr<dom::Element> namespacesElement(new dom::Element(U"namespaces"));
            dom::Element* prevElement = currentElement;
            currentElement = namespacesElement.get();
            for (NamespaceSymbol* namespaceSymbol : namespaces)
            {
                if (AddNamespace(*namespaceSymbol))
                {
                    added = true;
                }
            }
            if (added)
            {
                prevElement->AppendChild(std::unique_ptr<dom::Node>(namespacesElement.release()));
            }
            currentElement = prevElement;
        }
        std::vector<ConceptSymbol*> concepts = GetConcepts(container);
        if (!concepts.empty())
        {
            std::unique_ptr<dom::Element> conceptsElement(new dom::Element(U"concepts"));
            dom::Element* prevElement = currentElement;
            currentElement = conceptsElement.get();
            for (ConceptSymbol* concept : concepts)
            {
                AddConcept(*concept);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(conceptsElement.release()));
            currentElement = prevElement;
        }
        std::vector<ClassTypeSymbol*> classes = GetClasses(container);
        if (!classes.empty())
        {
            std::unique_ptr<dom::Element> classesElement(new dom::Element(U"classes"));
            dom::Element* prevElement = currentElement;
            currentElement = classesElement.get();
            for (ClassTypeSymbol* cls : classes)
            {
                AddClass(*cls);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(classesElement.release()));
            currentElement = prevElement;
        }
        std::vector<FunctionSymbol*> ctorsDtors = GetConstructorsAndDestructors(container);
        if (!ctorsDtors.empty())
        {
            std::unique_ptr<dom::Element> ctorsDtorsElement(new dom::Element(U"ctorsDtors"));
            dom::Element* prevElement = currentElement;
            currentElement = ctorsDtorsElement.get();
            for (FunctionSymbol* function : ctorsDtors)
            {
                AddCtorOrDtor(*function);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(ctorsDtorsElement.release()));
            currentElement = prevElement;
        }
        std::vector<FunctionSymbol*> functions = GetFunctions(container);
        if (!functions.empty())
        {
            std::unique_ptr<dom::Element> functionsElement(new dom::Element(U"functions"));
            dom::Element* prevElement = currentElement;
            currentElement = functionsElement.get();
            for (FunctionSymbol* function : functions)
            {
                AddFunction(*function);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(functionsElement.release()));
            currentElement = prevElement;
        }
        std::vector<DelegateTypeSymbol*> delegates = GetDelegates(container);
        if (!delegates.empty())
        {
            std::unique_ptr<dom::Element> delegatesElement(new dom::Element(U"delegates"));
            dom::Element* prevElement = currentElement;
            currentElement = delegatesElement.get();
            for (DelegateTypeSymbol* delegate : delegates)
            {
                AddDelegate(*delegate);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(delegatesElement.release()));
            currentElement = prevElement;
        }
        std::vector<ClassDelegateTypeSymbol*> classDelegates = GetClassDelegates(container);
        if (!classDelegates.empty())
        {
            std::unique_ptr<dom::Element> classDelegatesElement(new dom::Element(U"classDelegates"));
            dom::Element* prevElement = currentElement;
            currentElement = classDelegatesElement.get();
            for (ClassDelegateTypeSymbol* classDelegate : classDelegates)
            {
                AddClassDelegate(*classDelegate);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(classDelegatesElement.release()));
            currentElement = prevElement;
        }
        std::vector<TypedefSymbol*> typedefs = GetTypedefs(container);
        if (!typedefs.empty())
        {
            std::unique_ptr<dom::Element> typedefsElement(new dom::Element(U"typedefs"));
            dom::Element* prevElement = currentElement;
            currentElement = typedefsElement.get();
            for (TypedefSymbol* typedef_ : typedefs)
            {
                AddTypedef(*typedef_);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(typedefsElement.release()));
            currentElement = prevElement;
        }
        std::vector<EnumTypeSymbol*> enumTypes = GetEnumTypes(container);
        if (!enumTypes.empty())
        {
            std::unique_ptr<dom::Element> enumTypesElement(new dom::Element(U"enumTypes"));
            dom::Element* prevElement = currentElement;
            currentElement = enumTypesElement.get();
            for (EnumTypeSymbol* enumType : enumTypes)
            {
                AddEnumType(*enumType);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(enumTypesElement.release()));
            currentElement = prevElement;
        }
        std::vector<EnumConstantSymbol*> enumConstants = GetEnumConstants(container);
        if (!enumConstants.empty())
        {
            std::unique_ptr<dom::Element> enumConstantsElement(new dom::Element(U"enumConstants"));
            dom::Element* prevElement = currentElement;
            currentElement = enumConstantsElement.get();
            for (EnumConstantSymbol* enumConstant : enumConstants)
            {
                AddEnumConstant(*enumConstant);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(enumConstantsElement.release()));
            currentElement = prevElement;
        }
        std::vector<ConstantSymbol*> constants = GetConstants(container);
        if (!constants.empty())
        {
            std::unique_ptr<dom::Element> constantsElement(new dom::Element(U"constants"));
            dom::Element* prevElement = currentElement;
            currentElement = constantsElement.get();
            for (ConstantSymbol* constant : constants)
            {
                AddConstant(*constant);
            }
            prevElement->AppendChild(std::unique_ptr<dom::Node>(constantsElement.release()));
            currentElement = prevElement;
        }
    }
    bool functionOrDelegateContainer = container.IsFunctionSymbol() || 
        container.GetSymbolType() == SymbolType::delegateTypeSymbol || 
        container.GetSymbolType() == SymbolType::classDelegateTypeSymbol;
    bool skipFirst = false;
    if (container.IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(&container);
        if (functionSymbol->IsConstructorDestructorOrNonstaticMemberFunction())
        {
            skipFirst = true;
        }
    }
    std::vector<VariableSymbol*> variables = GetVariables(container, !functionOrDelegateContainer, skipFirst);
    if (!variables.empty())
    {
        std::u32string varContainerName = U"variables";
        if (functionOrDelegateContainer)
        {
            varContainerName = U"parameters";
        }
        std::unique_ptr<dom::Element> variablesElement(new dom::Element(varContainerName));
        dom::Element* prevElement = currentElement;
        currentElement = variablesElement.get();
        for (VariableSymbol* variable : variables)
        {
            AddVariable(*variable);
        }
        prevElement->AppendChild(std::unique_ptr<dom::Node>(variablesElement.release()));
        currentElement = prevElement;
    }
}

std::vector<NamespaceSymbol*> SymbolTableXmlBuilder::GetNamespaces(ContainerSymbol& container)
{
    std::vector<NamespaceSymbol*> namespaceSymbols;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->GetSymbolType() == SymbolType::namespaceSymbol)
        {
            namespaceSymbols.push_back(static_cast<NamespaceSymbol*>(member.get()));
        }
    }
    std::sort(namespaceSymbols.begin(), namespaceSymbols.end(), ByCodeName());
    return namespaceSymbols;
}

std::vector<ConceptSymbol*> SymbolTableXmlBuilder::GetConcepts(ContainerSymbol& container)
{
    std::vector<ConceptSymbol*> conceptSymbols;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->GetSymbolType() == SymbolType::conceptSymbol && member->IsProject())
        {
            conceptSymbols.push_back(static_cast<ConceptSymbol*>(member.get()));
        }
    }
    std::sort(conceptSymbols.begin(), conceptSymbols.end(), ByCodeName());
    return conceptSymbols;
}

std::vector<ClassTypeSymbol*> SymbolTableXmlBuilder::GetClasses(ContainerSymbol& container)
{
    std::vector<ClassTypeSymbol*> classSymbols;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->GetSymbolType() == SymbolType::classTypeSymbol && member->IsProject())
        {
            ClassTypeSymbol* cls = static_cast<ClassTypeSymbol*>(member.get());
            if (cls->IsClassTemplate())
            {
                ClassTemplateSpecializationSymbol* prototype = cls->Prototype();
                if (!prototype)
                {
                    throw std::runtime_error("prototype not set");
                }
                cls = prototype;
            }
            classSymbols.push_back(cls);
        }
    }
    std::sort(classSymbols.begin(), classSymbols.end(), ByCodeName());
    return classSymbols;
}

std::vector<FunctionSymbol*> SymbolTableXmlBuilder::GetConstructorsAndDestructors(ContainerSymbol& container)
{
    std::vector<FunctionSymbol*> ctorsAndDtors;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if ((member->GetSymbolType() == SymbolType::staticConstructorSymbol || 
            member->GetSymbolType() == SymbolType::constructorSymbol ||
            member->GetSymbolType() == SymbolType::destructorSymbol) && member->IsProject())
        {
            FunctionSymbol* ctorOrDtor = static_cast<FunctionSymbol*>(member.get());
            if (ctorOrDtor->HasSource())
            {
                ctorsAndDtors.push_back(ctorOrDtor);
            }
        }
    }
    std::sort(ctorsAndDtors.begin(), ctorsAndDtors.end(), ByCodeName());
    return ctorsAndDtors;
}

std::vector<FunctionSymbol*> SymbolTableXmlBuilder::GetFunctions(ContainerSymbol& container)
{
    std::vector<FunctionSymbol*> functions;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->GetSymbolType() == SymbolType::staticConstructorSymbol ||
            member->GetSymbolType() == SymbolType::constructorSymbol ||
            member->GetSymbolType() == SymbolType::destructorSymbol)
        {
            continue;
        }
        if (member->IsFunctionSymbol() && member->IsProject())
        {
            FunctionSymbol* fun = static_cast<FunctionSymbol*>(member.get());
            if (fun->HasSource())
            {
                if (fun->GetSymbolType() == SymbolType::functionSymbol && !fun->IsFunctionTemplate() && fun->IsTemplateSpecialization())
                {
                    continue;
                }
                functions.push_back(fun);
            }
            else if (moduleName == U"System.Core" && container.FullName() == U"System.Meta")
            {
                functions.push_back(fun);
            }
        }
    }
    std::sort(functions.begin(), functions.end(), ByCodeName());
    return functions;
}

std::vector<DelegateTypeSymbol*> SymbolTableXmlBuilder::GetDelegates(ContainerSymbol& container)
{
    std::vector<DelegateTypeSymbol*> delegateSymbols;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->GetSymbolType() == SymbolType::delegateTypeSymbol && member->IsProject())
        {
            delegateSymbols.push_back(static_cast<DelegateTypeSymbol*>(member.get()));
        }
    }
    std::sort(delegateSymbols.begin(), delegateSymbols.end(), ByCodeName());
    return delegateSymbols;
}

std::vector<ClassDelegateTypeSymbol*> SymbolTableXmlBuilder::GetClassDelegates(ContainerSymbol& container)
{
    std::vector<ClassDelegateTypeSymbol*> classDelegateSymbols;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->GetSymbolType() == SymbolType::classDelegateTypeSymbol && member->IsProject())
        {
            classDelegateSymbols.push_back(static_cast<ClassDelegateTypeSymbol*>(member.get()));
        }
    }
    std::sort(classDelegateSymbols.begin(), classDelegateSymbols.end(), ByCodeName());
    return classDelegateSymbols;
}

std::vector<TypedefSymbol*> SymbolTableXmlBuilder::GetTypedefs(ContainerSymbol& container)
{
    std::vector<TypedefSymbol*> typedefs;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->GetSymbolType() == SymbolType::typedefSymbol && member->IsProject())
        {
            Node* node = symbolTable.GetNodeNoThrow(member.get());
            if (node)
            {
                typedefs.push_back(static_cast<TypedefSymbol*>(member.get()));
            }
        }
    }
    std::sort(typedefs.begin(), typedefs.end(), ByCodeName());
    return typedefs;
}

std::vector<EnumTypeSymbol*> SymbolTableXmlBuilder::GetEnumTypes(ContainerSymbol& container)
{
    std::vector<EnumTypeSymbol*> enumTypes;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->GetSymbolType() == SymbolType::enumTypeSymbol && member->IsProject())
        {
            enumTypes.push_back(static_cast<EnumTypeSymbol*>(member.get()));
        }
    }
    std::sort(enumTypes.begin(), enumTypes.end(), ByCodeName());
    return enumTypes;
}

std::vector<EnumConstantSymbol*> SymbolTableXmlBuilder::GetEnumConstants(ContainerSymbol& container)
{
    std::vector<EnumConstantSymbol*> enumConstants;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->GetSymbolType() == SymbolType::enumConstantSymbol && member->IsProject())
        {
            enumConstants.push_back(static_cast<EnumConstantSymbol*>(member.get()));
        }
    }
    return enumConstants;
}

std::vector<ConstantSymbol*> SymbolTableXmlBuilder::GetConstants(ContainerSymbol& container)
{
    std::vector<ConstantSymbol*> constants;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->GetSymbolType() == SymbolType::constantSymbol && member->IsProject())
        {
            constants.push_back(static_cast<ConstantSymbol*>(member.get()));
        }
    }
    std::sort(constants.begin(), constants.end(), ByCodeName());
    return constants;
}

std::vector<VariableSymbol*> SymbolTableXmlBuilder::GetVariables(ContainerSymbol& container, bool sort, bool skipFirst)
{
    std::vector<VariableSymbol*> variables;
    bool first = true;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->IsVariableSymbol() && member->IsProject() && member->GetSymbolType() != SymbolType::localVariableSymbol)
        {
            if (first)
            {
                first = false;
                if (skipFirst)
                {
                    continue;
                }
            }
            variables.push_back(static_cast<VariableSymbol*>(member.get()));
        }
    }
    if (sort)
    {
        std::sort(variables.begin(), variables.end(), ByCodeName());
    }
    return variables;
}

bool SymbolTableXmlFilesUpToDate(Project* project)
{
    Input* input = GetInputPtr();
    std::string targetDir = input->targetDirPath;
    std::string contentDir = GetFullPath(Path::Combine(targetDir, "content"));
    std::string projectName = ToUtf8(project->Name());
    std::string moduleDir = GetFullPath(Path::Combine(contentDir, projectName));
    std::string symbolTableXmlFilePath = Path::Combine(moduleDir, projectName + ".xml");
    if (!boost::filesystem::exists(symbolTableXmlFilePath)) return false;
    for (const std::string& sourceFilePath : project->SourceFilePaths())
    {
        if (boost::filesystem::last_write_time(symbolTableXmlFilePath) < boost::filesystem::last_write_time(sourceFilePath)) return false;
    }
    return true;
}

dom::Document* GetModuleDocument(Input* input, const std::u32string& moduleName)
{
    for (const auto& p : input->externalModuleDocs)
    {
        dom::Document* doc = p.get();
        if (doc->DocumentElement()->GetAttribute(U"module") == moduleName)
        {
            return doc;
        }
    }
    return nullptr;
}

void GenerateSymbolTableXml(Module* rootModule, std::unordered_map<int, File>& fileMap)
{
    SymbolTable& symbolTable = rootModule->GetSymbolTable();
    std::string modulePrefix = Path::Combine(Path::Combine("../..", ToUtf8(rootModule->Name())), "doc");
    std::string extModulePrefix = Path::Combine(ToUtf8(rootModule->Name()), "doc");
    SymbolTableXmlBuilder symbolTableXmlBuilder(rootModule->Name(), symbolTable, fileMap, modulePrefix, extModulePrefix);
    Input* input = GetInputPtr();
    std::string targetDir = input->targetDirPath;
    boost::filesystem::create_directories(targetDir);
    std::string contentDir = GetFullPath(Path::Combine(targetDir, "content"));
    boost::filesystem::create_directories(contentDir);
    std::string moduleDir = GetFullPath(Path::Combine(contentDir, ToUtf8(rootModule->Name())));
    boost::filesystem::create_directories(moduleDir);
    std::vector<std::unique_ptr<dom::Document>> refModuleXmlDocs;
    int n = rootModule->AllReferencedModules().size();
    for (int i = 0; i < n; ++i)
    {
        Module* referencedModule = rootModule->AllReferencedModules()[i];
        std::pair<std::string, bool> refModuleXmlDocFilePathExternal = ResolveModuleXmlDocFilePath(input, referencedModule->Name());
        dom::Document* moduleDoc = nullptr;
        std::lock_guard<std::mutex> lock(GetInputMutex());
        if (refModuleXmlDocFilePathExternal.second)
        {
            moduleDoc = GetModuleDocument(input, referencedModule->Name());
            if (moduleDoc)
            {
                symbolTableXmlBuilder.AddModuleXmlDocument(moduleDoc);
            }
        }
        if (!moduleDoc)
        {
            std::unique_ptr<dom::Document> refModuleXmlDoc = dom::ReadDocument(refModuleXmlDocFilePathExternal.first);
            dom::Document* moduleXmlDoc = refModuleXmlDoc.get();
            if (refModuleXmlDocFilePathExternal.second)
            {
                input->externalModuleDocs.push_back(std::move(refModuleXmlDoc));
            }
            else
            {
                refModuleXmlDocs.push_back(std::move(refModuleXmlDoc));
            }
            symbolTableXmlBuilder.AddModuleXmlDocument(moduleXmlDoc);
        }
    }
    symbolTableXmlBuilder.AddNamespace(symbolTable.GlobalNs());
    std::string symbolTableXmlFilePath = Path::Combine(moduleDir, ToUtf8(rootModule->Name()) + ".xml");
    symbolTableXmlBuilder.WriteDocument(symbolTableXmlFilePath);
    if (GetGlobalFlag(GlobalFlags::verbose))
    {
        LogMessage(rootModule->LogStreamId(), "==> " + symbolTableXmlFilePath);
    }
}

void ReadExternalModuleDocuments(Input* input)
{
    for (const auto& p : input->libraryPrefixMap)
    {
        if (!p.second.empty())
        {
            const std::u32string& moduleName = p.first;
            std::lock_guard<std::mutex> lock(GetInputMutex());
            dom::Document* moduleDoc = GetModuleDocument(input, moduleName);
            if (!moduleDoc)
            {
                std::pair<std::string, bool> refModuleXmlDocFilePathExternal = ResolveModuleXmlDocFilePath(input, moduleName);
                std::unique_ptr<dom::Document> refModuleXmlDoc = dom::ReadDocument(refModuleXmlDocFilePathExternal.first);
                input->externalModuleDocs.push_back(std::move(refModuleXmlDoc));
            }
        }
    }
}

} } // namespace cmajor::cmdoclib
