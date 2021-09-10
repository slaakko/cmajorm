// =================================
// Copyright (c) 2021 Seppo Laakko
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
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngxml/dom/Parser.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Log.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <fstream>

namespace cmajor { namespace cmdoclib {

using namespace soulng::unicode;

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
    return std::pair<std::string, bool>();
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
    void AddModuleXmlDocument(sngxml::dom::Document* moduleXmlDocument);
    std::unique_ptr<sngxml::dom::Element> CreateSymbolElement(const std::u32string& elementName, Symbol& symbol) const;
    bool AddNamespace(NamespaceSymbol& ns);
    void AddConcept(ConceptSymbol& conceptSymbol);
    void AddClass(ClassTypeSymbol& cls);
    void AddInterface(InterfaceTypeSymbol& interface);
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
    std::vector<InterfaceTypeSymbol*> GetInterfaces(ContainerSymbol& container);
    std::vector<FunctionSymbol*> GetConstructorsAndDestructors(ContainerSymbol& container);
    std::vector<FunctionSymbol*> GetFunctions(ContainerSymbol& container);
    std::vector<DelegateTypeSymbol*> GetDelegates(ContainerSymbol& container); 
    std::vector<ClassDelegateTypeSymbol*> GetClassDelegates(ContainerSymbol& container);
    std::vector<TypedefSymbol*> GetTypedefs(ContainerSymbol& container);
    std::vector<EnumTypeSymbol*> GetEnumTypes(ContainerSymbol& container);
    std::vector<EnumConstantSymbol*> GetEnumConstants(ContainerSymbol& container);
    std::vector<ConstantSymbol*> GetConstants(ContainerSymbol& container);
    std::vector<VariableSymbol*> GetVariables(ContainerSymbol& container, bool sort, bool skipFirst);
    bool IsNonemptyNamespace(sngxml::dom::Element* nsElement) const;
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
    std::vector<sngxml::dom::Document*> moduleXmlDocuments;
    std::unique_ptr<sngxml::dom::Document> symbolTableXmlDocument;
    std::unique_ptr<sngxml::dom::Element> symbolTableElement;
    sngxml::dom::Element* currentElement;
    std::unordered_set<std::u32string> typeIdSet;
    std::vector<TypeSymbol*> types;
};

SymbolTableXmlBuilder::SymbolTableXmlBuilder(const std::u32string& moduleName_, SymbolTable& symbolTable_, std::unordered_map<int, File>& fileMap_, 
    const std::string& modulePrefix_, const std::string& extModulePrefix_) :
    moduleName(moduleName_), symbolTableXmlDocument(new sngxml::dom::Document()),
    symbolTableElement(new sngxml::dom::Element(U"symbolTable")), currentElement(symbolTableElement.get()), symbolTable(symbolTable_),
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
    std::unique_ptr<sngxml::dom::Element> filesElement(new sngxml::dom::Element(U"files"));
    for (File* file : files)
    {
        std::unique_ptr<sngxml::dom::Element> fileElement(new sngxml::dom::Element(U"file"));
        fileElement->SetAttribute(U"name", file->name);
        fileElement->SetAttribute(U"path", ToUtf32(Path::Combine("file", Path::GetFileName(file->htmlFilePath))));
        filesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(fileElement.release()));
    }
    symbolTableElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(filesElement.release()));
}

void SymbolTableXmlBuilder::WriteDocument(const std::string& symbolTableXmlFilePath)
{
    GenerateTypes();
    symbolTableXmlDocument->AppendChild(std::unique_ptr<sngxml::dom::Node>(symbolTableElement.release()));
    std::ofstream symbolTableXmlFile(symbolTableXmlFilePath);
    CodeFormatter formatter(symbolTableXmlFile);
    formatter.SetIndentSize(1);
    symbolTableXmlDocument->Write(formatter);
}

void SymbolTableXmlBuilder::AddModuleXmlDocument(sngxml::dom::Document* moduleXmlDocument)
{
    moduleXmlDocuments.push_back(moduleXmlDocument);
}

bool SymbolTableXmlBuilder::IsNonemptyNamespace(sngxml::dom::Element* nsElement) const
{
    sngxml::dom::Node* child = nsElement->FirstChild();
    while (child)
    {
        if (child->GetNodeType() == sngxml::dom::NodeType::elementNode)
        {
            sngxml::dom::Element* element = static_cast<sngxml::dom::Element*>(child);
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
        sngxml::dom::Document* moduleDoc = moduleXmlDocuments[i];
        std::lock_guard<std::mutex> lock(GetInputMutex());
        sngxml::dom::Element* element = moduleDoc->GetElementById(typeId);
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
    std::unique_ptr<sngxml::dom::Element> typesElement(new sngxml::dom::Element(U"types"));
    for (TypeSymbol* type : types)
    {
        if (type->IsBasicTypeSymbol())
        {
            std::unique_ptr<sngxml::dom::Element> typeElement = CreateSymbolElement(U"basicType", *type);
            typesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
        }
        else if (type->GetSymbolType() == SymbolType::derivedTypeSymbol)
        {
            DerivedTypeSymbol* derivedTypeSymbol = static_cast<DerivedTypeSymbol*>(type);
            std::unique_ptr<sngxml::dom::Element> typeElement = CreateSymbolElement(U"derivedType", *derivedTypeSymbol);
            if (HasFrontConstDerivation(derivedTypeSymbol->DerivationRec().derivations))
            {
                std::unique_ptr<sngxml::dom::Element> constDerivationElement(new sngxml::dom::Element(U"derivation"));
                constDerivationElement->SetAttribute(U"name", U"const");
                typeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constDerivationElement.release()));
            }
            std::unique_ptr<sngxml::dom::Element> baseDerivationElement(new sngxml::dom::Element(U"derivation"));
            baseDerivationElement->SetAttribute(U"baseType", type->BaseType()->Id());
            typeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(baseDerivationElement.release()));
            for (Derivation derivation : derivedTypeSymbol->DerivationRec().derivations)
            {
                switch (derivation)
                {
                    case Derivation::lvalueRefDerivation:
                    {
                        std::unique_ptr<sngxml::dom::Element> lvalueRefDerivationElement(new sngxml::dom::Element(U"derivation"));
                        lvalueRefDerivationElement->SetAttribute(U"name", U"&");
                        typeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(lvalueRefDerivationElement.release()));
                        break;
                    }
                    case Derivation::rvalueRefDerivation:
                    {
                        std::unique_ptr<sngxml::dom::Element> rvalueRefDerivationElement(new sngxml::dom::Element(U"derivation"));
                        rvalueRefDerivationElement->SetAttribute(U"name", U"&&");
                        typeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rvalueRefDerivationElement.release()));
                        break;
                    }
                    case Derivation::pointerDerivation:
                    {
                        std::unique_ptr<sngxml::dom::Element> pointerDerivationElement(new sngxml::dom::Element(U"derivation"));
                        pointerDerivationElement->SetAttribute(U"name", U"*");
                        typeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(pointerDerivationElement.release()));
                        break;
                    }
                }
            }
            typesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
        }
        else if (type->GetSymbolType() == SymbolType::arrayTypeSymbol)
        {
            ArrayTypeSymbol* arrayTypeSymbol = static_cast<ArrayTypeSymbol*>(type);
            std::unique_ptr<sngxml::dom::Element> typeElement = CreateSymbolElement(U"arrayType", *arrayTypeSymbol);
            typeElement->SetAttribute(U"elementType", arrayTypeSymbol->ElementType()->Id());
            if (arrayTypeSymbol->Size() != -1)
            {
                typeElement->SetAttribute(U"size", ToUtf32(std::to_string(arrayTypeSymbol->Size())));
            }
            typesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
        }
        else if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
            if (!specialization->IsPrototype())
            {
                std::unique_ptr<sngxml::dom::Element> typeElement = CreateSymbolElement(U"specialization", *specialization);
                ClassTypeSymbol* classTemplate = specialization->GetClassTemplate();
                typeElement->SetAttribute(U"primaryType", classTemplate->Id());
                std::unique_ptr<sngxml::dom::Element> argumentTypesElement(new sngxml::dom::Element(U"templateArgumentTypes"));
                for (TypeSymbol* templateArgumentType : specialization->TemplateArgumentTypes())
                {
                    std::unique_ptr<sngxml::dom::Element> argumentTypeElement(new sngxml::dom::Element(U"templateArgumentType"));
                    argumentTypeElement->SetAttribute(U"ref", templateArgumentType->Id());
                    argumentTypesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(argumentTypeElement.release()));
                }
                typeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(argumentTypesElement.release()));
                typesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
            }
        }
        else
        {
            throw std::runtime_error("unexpected type symbol");
        }
    }
    symbolTableElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typesElement.release()));
}

void SymbolTableXmlBuilder::Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> parensElement(new sngxml::dom::Element(U"parens"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = parensElement.get();
    parenthesizedConstraintNode.Constraint()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(parensElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> orElement(new sngxml::dom::Element(U"or"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = orElement.get();
    disjunctiveConstraintNode.Left()->Accept(*this);
    disjunctiveConstraintNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(orElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> andElement(new sngxml::dom::Element(U"and"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = andElement.get();
    conjunctiveConstraintNode.Left()->Accept(*this);
    conjunctiveConstraintNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(andElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(WhereConstraintNode& whereConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> whereElement(new sngxml::dom::Element(U"where"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = whereElement.get();
    whereConstraintNode.Constraint()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(whereElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(PredicateConstraintNode& predicateConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> predicateElement(new sngxml::dom::Element(U"predicate"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = predicateElement.get();
    currentElement ->SetAttribute(U"value", ToUtf32(predicateConstraintNode.InvokeExpr()->ToString()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(predicateElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(IsConstraintNode& isConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> isElement(new sngxml::dom::Element(U"is"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = isElement.get();
    isConstraintNode.TypeExpr()->Accept(*this);
    isConstraintNode.ConceptOrTypeName()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(isElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(MultiParamConstraintNode& multiParamConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> multiElement(new sngxml::dom::Element(U"multi"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = multiElement.get();
    multiParamConstraintNode.ConceptId()->Accept(*this);
    std::unique_ptr<sngxml::dom::Element> typesElement(new sngxml::dom::Element(U"types"));
    sngxml::dom::Element* prevTypesElement = currentElement;
    currentElement = typesElement.get();
    int n = multiParamConstraintNode.TypeExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        multiParamConstraintNode.TypeExprs()[i]->Accept(*this);
    }
    prevTypesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typesElement.release()));
    currentElement = prevTypesElement;
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(multiElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(TypeNameConstraintNode& typeNameConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> typenameElement(new sngxml::dom::Element(U"typename"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = typenameElement.get();
    typeNameConstraintNode.TypeId()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typenameElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ConstructorConstraintNode& constructorConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> signatureConstraintElement(new sngxml::dom::Element(U"signature"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = signatureConstraintElement.get();
    signatureConstraintElement->SetAttribute(U"value", ToUtf32(constructorConstraintNode.ToString()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(signatureConstraintElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(DestructorConstraintNode& destructorConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> signatureConstraintElement(new sngxml::dom::Element(U"signature"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = signatureConstraintElement.get();
    signatureConstraintElement->SetAttribute(U"value", ToUtf32(destructorConstraintNode.ToString()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(signatureConstraintElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> signatureConstraintElement(new sngxml::dom::Element(U"signature"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = signatureConstraintElement.get();
    signatureConstraintElement->SetAttribute(U"value", ToUtf32(memberFunctionConstraintNode.ToString()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(signatureConstraintElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(FunctionConstraintNode& functionConstraintNode)
{
    std::unique_ptr<sngxml::dom::Element> signatureConstraintElement(new sngxml::dom::Element(U"signature"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = signatureConstraintElement.get();
    signatureConstraintElement->SetAttribute(U"value", ToUtf32(functionConstraintNode.ToString()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(signatureConstraintElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(AxiomStatementNode& axiomStatementNode)
{
    std::unique_ptr<sngxml::dom::Element> axiomStatementElement(new sngxml::dom::Element(U"axiomStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = axiomStatementElement.get();
    axiomStatementElement->SetAttribute(U"value", axiomStatementNode.Text());
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(axiomStatementElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(AxiomNode& axiomNode)
{
    std::unique_ptr<sngxml::dom::Element> axiomElement(new sngxml::dom::Element(U"axiom"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = axiomElement.get();
    if (axiomNode.Id())
    {
        axiomNode.Id()->Accept(*this);
    }
    int np = axiomNode.Parameters().Count();
    if (np > 0)
    {
        std::unique_ptr<sngxml::dom::Element> parametersElement(new sngxml::dom::Element(U"parameters"));
        sngxml::dom::Element* prevElement = currentElement;
        currentElement = parametersElement.get();
        for (int i = 0; i < np; ++i)
        {
            axiomNode.Parameters()[i]->Accept(*this);
        }
        prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(parametersElement.release()));
        currentElement = prevElement;
    }
    int n = axiomNode.Statements().Count();
    if (n > 0)
    {
        std::unique_ptr<sngxml::dom::Element> statementsElement(new sngxml::dom::Element(U"axiomStatements"));
        sngxml::dom::Element* prevElement = currentElement;
        currentElement = statementsElement.get();
        for (int i = 0; i < n; ++i)
        {
            axiomNode.Statements()[i]->Accept(*this);
        }
        prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(statementsElement.release()));
        currentElement = prevElement;
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(axiomElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ConceptIdNode& conceptIdNode)
{
    conceptIdNode.Id()->Accept(*this);
    std::unique_ptr<sngxml::dom::Element> typeParamsElement(new sngxml::dom::Element(U"typeParams"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = typeParamsElement.get();
    int n = conceptIdNode.TypeParameters().Count();
    for (int i = 0; i < n; ++i)
    {
        conceptIdNode.TypeParameters()[i]->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeParamsElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(IdentifierNode& identifierNode)
{
    std::unique_ptr<sngxml::dom::Element> symbolElement(new sngxml::dom::Element(U"symbol"));
    Symbol* symbol = symbolTable.GetMappedSymbol(&identifierNode);
    if (symbol)
    {
        symbolElement->SetAttribute(U"ref", symbol->Id());
    }
    symbolElement->SetAttribute(U"name", identifierNode.Str());
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(symbolElement.release()));
}

void SymbolTableXmlBuilder::Visit(DotNode& dotNode)
{
    std::unique_ptr<sngxml::dom::Element> dotElement(new sngxml::dom::Element(U"dot"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = dotElement.get();
    dotNode.Subject()->Accept(*this); 
    dotNode.MemberId()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(dotElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ParameterNode& parameterNode)
{
    std::unique_ptr<sngxml::dom::Element> parameterElement(new sngxml::dom::Element(U"parameter"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = parameterElement.get();
    parameterNode.TypeExpr()->Accept(*this);
    if (parameterNode.Id() && !parameterNode.ArtificialId())
    {
        parameterNode.Id()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(parameterElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(BoolNode& boolNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"bool");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(SByteNode& sbyteNode) 
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"sbyte");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ByteNode& byteNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"byte");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ShortNode& shortNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"short");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(UShortNode& ushortNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"ushort");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(IntNode& intNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"int");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(UIntNode& uintNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"uint");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(LongNode& longNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"long");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(ULongNode& ulongNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"ulong");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(FloatNode& floatNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"float");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(DoubleNode& doubleNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"double");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(CharNode& charNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"char");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(WCharNode& wcharNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"wchar");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(UCharNode& ucharNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"uchar");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::Visit(VoidNode& voidNode)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"keyword"));
    keywordElement->SetAttribute(U"value", U"void");
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = keywordElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentElement = prevElement;
}

std::unique_ptr<sngxml::dom::Element> SymbolTableXmlBuilder::CreateSymbolElement(const std::u32string& elementName, Symbol& symbol) const
{
    std::unique_ptr<sngxml::dom::Element> symbolElement(new sngxml::dom::Element(elementName));
    symbolElement->SetAttribute(U"name", symbol.CodeName());
    symbolElement->SetAttribute(U"id", symbol.Id());
    return symbolElement;
}

bool SymbolTableXmlBuilder::AddNamespace(NamespaceSymbol& ns)
{
    bool added = false;
    std::unique_ptr<sngxml::dom::Element> nsElement = CreateSymbolElement(U"namespace", ns);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(ns.Id()) + ".html"));
    nsElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(ns.Id()) + ".html"));
    nsElement->SetAttribute(U"extPath", extPath);
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = nsElement.get();
    AddChildren(ns);
    if (IsNonemptyNamespace(nsElement.get()))
    {
        prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(nsElement.release()));
        added = true;
    }
    currentElement = prevElement;
    return added;
}

void SymbolTableXmlBuilder::AddConcept(ConceptSymbol& conceptSymbol)
{
    std::unique_ptr<sngxml::dom::Element> conceptElement = CreateSymbolElement(U"concept", conceptSymbol);
    if (conceptSymbol.RefinedConcept())
    {
        conceptElement->SetAttribute(U"refines", conceptSymbol.RefinedConcept()->Id());
        AddConceptRefinement(conceptSymbol.RefinedConcept(), &conceptSymbol);
    }
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(conceptSymbol.Id()) + ".html"));
    conceptElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(conceptSymbol.Id()) + ".html"));
    conceptElement->SetAttribute(U"extPath", extPath);
    int n = conceptSymbol.TemplateParameters().size();
    if (n > 0)
    {
        std::unique_ptr<sngxml::dom::Element> typeParametersElement(new sngxml::dom::Element(U"typeParameters"));
        for (int i = 0; i < n; ++i)
        {
            std::unique_ptr<sngxml::dom::Element> typeParameterElement = CreateSymbolElement(U"typeParameter", *conceptSymbol.TemplateParameters()[i]);
            std::u32string docPath = conceptElement->GetAttribute(U"docPath") + U"#" + typeParameterElement->GetAttribute(U"id");
            typeParameterElement->SetAttribute(U"docPath", docPath);
            typeParametersElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeParameterElement.release()));
        }
        conceptElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeParametersElement.release()));
    }
    if (conceptSymbol.HasSource())
    {
        File& file = fileMap[conceptSymbol.GetSpan().fileIndex];
        conceptElement->SetAttribute(U"fileName", file.name);
        conceptElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
        conceptElement->SetAttribute(U"line", ToUtf32(std::to_string(conceptSymbol.GetSpan().line)));
    }
    Node* node = symbolTable.GetNodeNoThrow(&conceptSymbol);
    ConceptNode* conceptNode = nullptr;
    if (node && node->GetNodeType() == NodeType::conceptNode)
    {
        conceptNode = static_cast<ConceptNode*>(node);
        conceptElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(conceptNode->GetSpecifiers())));
    }
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conceptElement.get();
    if (conceptNode)
    {
        int nc = conceptNode->Constraints().Count();
        if (nc > 0)
        {
            std::unique_ptr<sngxml::dom::Element> constraintsElement(new sngxml::dom::Element(U"constraints"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = constraintsElement.get();
            for (int i = 0; i < nc; ++i)
            {
                ConstraintNode* constraint = conceptNode->Constraints()[i];
                constraint->Accept(*this);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constraintsElement.release()));
            currentElement = prevElement;
        }
        int na = conceptNode->Axioms().Count();
        if (na > 0)
        {
            std::unique_ptr<sngxml::dom::Element> axiomsElement(new sngxml::dom::Element(U"axioms"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = axiomsElement.get();
            for (int i = 0; i < na; ++i)
            {
                AxiomNode* axiom = conceptNode->Axioms()[i];
                axiom->Accept(*this);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(axiomsElement.release()));
            currentElement = prevElement;
        }
    }
    AddChildren(conceptSymbol);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conceptElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddClass(ClassTypeSymbol& cls)
{
    std::unique_ptr<sngxml::dom::Element> classElement = CreateSymbolElement(U"class", cls);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(cls.Id()) + ".html"));
    classElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(cls.Id()) + ".html"));
    classElement->SetAttribute(U"extPath", extPath);
    File& file = fileMap[cls.GetSpan().fileIndex];
    classElement->SetAttribute(U"fileName", file.name);
    classElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
    classElement->SetAttribute(U"line", ToUtf32(std::to_string(cls.GetSpan().line)));
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
            std::unique_ptr<sngxml::dom::Element> templateParametersElement(new sngxml::dom::Element(U"templateParameters"));
            int n = specialization->TemplateArgumentTypes().size();
            for (int i = 0; i < n; ++i)
            {
                TypeSymbol* templateArgumentType = specialization->TemplateArgumentTypes()[i];
                if (templateArgumentType->GetSymbolType() == SymbolType::templateParameterSymbol)
                {
                    TemplateParameterSymbol* templateParameter = static_cast<TemplateParameterSymbol*>(templateArgumentType);
                    std::unique_ptr<sngxml::dom::Element> templateParameterElement = CreateSymbolElement(U"templateParameter", *templateParameter);
                    std::u32string docPath = classElement->GetAttribute(U"docPath") + U"#" + templateParameter->Id();
                    templateParameterElement->SetAttribute(U"docPath", docPath);
                    if (templateParameter->HasDefault())
                    {
                        GetOrInsertType(templateParameter->DefaultType());
                        templateParameterElement->SetAttribute(U"default", templateParameter->DefaultType()->Id());
                    }
                    templateParametersElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(templateParameterElement.release()));
                }
                else
                {
                    throw std::runtime_error("template parameter expected");
                }
            }
            classElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(templateParametersElement.release()));
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
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = classElement.get();
    AddChildren(cls);
    if (cls.Constraint())
    {
        cls.Constraint()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(classElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddInterface(InterfaceTypeSymbol& interface)
{
    std::unique_ptr<sngxml::dom::Element> interfaceElement = CreateSymbolElement(U"interface", interface);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(interface.Id()) + ".html"));
    interfaceElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(interface.Id()) + ".html"));
    interfaceElement->SetAttribute(U"extPath", extPath);
    File& file = fileMap[interface.GetSpan().fileIndex];
    interfaceElement->SetAttribute(U"fileName", file.name);
    interfaceElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
    interfaceElement->SetAttribute(U"line", ToUtf32(std::to_string(interface.GetSpan().line)));
    Node* node = symbolTable.GetNodeNoThrow(&interface);
    if (node && node->GetNodeType() == NodeType::interfaceNode)
    {
        InterfaceNode* interfaceNode = static_cast<InterfaceNode*>(node);
        interfaceElement->SetAttribute(U"specifiers", ToUtf32(SpecifierStr(interfaceNode->GetSpecifiers())));
    }
    else
    {
        throw std::runtime_error("interface node expected");
    }
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = interfaceElement.get();
    AddChildren(interface);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(interfaceElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddCtorOrDtor(FunctionSymbol& function)
{
    std::unique_ptr<sngxml::dom::Element> ctorDtorElement = CreateSymbolElement(U"ctorDtor", function);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(function.Parent()->Id()) + ".html#" + ToUtf8(function.Id())));
    ctorDtorElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(function.Parent()->Id()) + ".html#" + ToUtf8(function.Id())));
    ctorDtorElement->SetAttribute(U"extPath", extPath);
    File& file = fileMap[function.GetSpan().fileIndex];
    ctorDtorElement->SetAttribute(U"fileName", file.name);
    ctorDtorElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
    ctorDtorElement->SetAttribute(U"line", ToUtf32(std::to_string(function.GetSpan().line)));
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
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = ctorDtorElement.get();
    AddChildren(function);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(ctorDtorElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddFunction(FunctionSymbol& function)
{
    std::unique_ptr<sngxml::dom::Element> functionElement = CreateSymbolElement(U"function", function);
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
        File& file = fileMap[function.GetSpan().fileIndex];
        functionElement->SetAttribute(U"fileName", file.name);
        functionElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
        functionElement->SetAttribute(U"line", ToUtf32(std::to_string(function.GetSpan().line)));
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
        std::unique_ptr<sngxml::dom::Element> templateParametersElement(new sngxml::dom::Element(U"templateParameters"));
        for (int i = 0; i < n; ++i)
        {
            std::unique_ptr<sngxml::dom::Element> templateParameterElement = CreateSymbolElement(U"templateParameter", *function.TemplateParameters()[i]);
            std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(function.Parent()->Id()) + ".html#" + ToUtf8(function.TemplateParameters()[i]->Id())));
            templateParameterElement->SetAttribute(U"docPath", docPath);
            std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(function.Parent()->Id()) + ".html#" + ToUtf8(function.TemplateParameters()[i]->Id())));
            templateParameterElement->SetAttribute(U"extPath", extPath);
            templateParametersElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(templateParameterElement.release()));
        }
        functionElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(templateParametersElement.release()));
    }
    TypeSymbol* returnType = function.ReturnType();
    Assert(returnType, "nonnull return type expected");
    std::u32string returnTypeId = GetOrInsertType(returnType);
    functionElement->SetAttribute(U"returnType", returnTypeId);
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = functionElement.get();
    AddChildren(function);
    if (function.Constraint())
    {
        function.Constraint()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddDelegate(DelegateTypeSymbol& delegate)
{
    std::unique_ptr<sngxml::dom::Element> delegateElement = CreateSymbolElement(U"delegate", delegate);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(delegate.Parent()->Id()) + ".html#" + ToUtf8(delegate.Id())));
    delegateElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(delegate.Parent()->Id()) + ".html#" + ToUtf8(delegate.Id())));
    delegateElement->SetAttribute(U"extPath", extPath);
    File& file = fileMap[delegate.GetSpan().fileIndex];
    delegateElement->SetAttribute(U"fileName", file.name);
    delegateElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
    delegateElement->SetAttribute(U"line", ToUtf32(std::to_string(delegate.GetSpan().line)));
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
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = delegateElement.get();
    AddChildren(delegate);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(delegateElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddClassDelegate(ClassDelegateTypeSymbol& classDelegate)
{
    std::unique_ptr<sngxml::dom::Element> classDelegateElement = CreateSymbolElement(U"classDelegate", classDelegate);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(classDelegate.Parent()->Id()) + ".html#" + ToUtf8(classDelegate.Id())));
    classDelegateElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(classDelegate.Parent()->Id()) + ".html#" + ToUtf8(classDelegate.Id())));
    classDelegateElement->SetAttribute(U"extPath", extPath);
    File& file = fileMap[classDelegate.GetSpan().fileIndex];
    classDelegateElement->SetAttribute(U"fileName", file.name);
    classDelegateElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
    classDelegateElement->SetAttribute(U"line", ToUtf32(std::to_string(classDelegate.GetSpan().line)));
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
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = classDelegateElement.get();
    AddChildren(classDelegate);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(classDelegateElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddTypedef(TypedefSymbol& typedef_)
{
    std::unique_ptr<sngxml::dom::Element> typedefElement = CreateSymbolElement(U"typedef", typedef_);
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
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = typedefElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typedefElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddEnumType(EnumTypeSymbol& enumType)
{
    std::unique_ptr<sngxml::dom::Element> enumTypeElement = CreateSymbolElement(U"enumType", enumType);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(enumType.Id()) + ".html"));
    enumTypeElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(enumType.Id()) + ".html"));
    enumTypeElement->SetAttribute(U"extPath", extPath);
    File& file = fileMap[enumType.GetSpan().fileIndex];
    enumTypeElement->SetAttribute(U"fileName", file.name);
    enumTypeElement->SetAttribute(U"filePath", ToUtf32(file.htmlFilePath));
    enumTypeElement->SetAttribute(U"line", ToUtf32(std::to_string(enumType.GetSpan().line)));
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
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = enumTypeElement.get();
    AddChildren(enumType);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(enumTypeElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddEnumConstant(EnumConstantSymbol& enumConstant)
{
    std::unique_ptr<sngxml::dom::Element> enumConstantElement = CreateSymbolElement(U"enumConstant", enumConstant);
    std::u32string docPath = ToUtf32(Path::Combine(modulePrefix, ToUtf8(enumConstant.Parent()->Id()) + ".html#" + ToUtf8(enumConstant.Id())));
    enumConstantElement->SetAttribute(U"docPath", docPath);
    std::u32string extPath = ToUtf32(Path::Combine(extModulePrefix, ToUtf8(enumConstant.Parent()->Id()) + ".html#" + ToUtf8(enumConstant.Id())));
    enumConstantElement->SetAttribute(U"extPath", extPath);
    if (!enumConstant.StrValue().empty())
    {
        enumConstantElement->SetAttribute(U"value", enumConstant.StrValue());
    }
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = enumConstantElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(enumConstantElement.release()));
    currentElement = prevElement;
}

void SymbolTableXmlBuilder::AddConstant(ConstantSymbol& constant)
{
    std::unique_ptr<sngxml::dom::Element> constantElement = CreateSymbolElement(U"constant", constant);
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
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = constantElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constantElement.release()));
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
    std::unique_ptr<sngxml::dom::Element> variableElement = CreateSymbolElement(elementName, variable);
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
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = variableElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(variableElement.release()));
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
            std::unique_ptr<sngxml::dom::Element> namespacesElement(new sngxml::dom::Element(U"namespaces"));
            sngxml::dom::Element* prevElement = currentElement;
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
                prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(namespacesElement.release()));
            }
            currentElement = prevElement;
        }
        std::vector<ConceptSymbol*> concepts = GetConcepts(container);
        if (!concepts.empty())
        {
            std::unique_ptr<sngxml::dom::Element> conceptsElement(new sngxml::dom::Element(U"concepts"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = conceptsElement.get();
            for (ConceptSymbol* conceptSymbol : concepts)
            {
                AddConcept(*conceptSymbol);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conceptsElement.release()));
            currentElement = prevElement;
        }
        std::vector<ClassTypeSymbol*> classes = GetClasses(container);
        if (!classes.empty())
        {
            std::unique_ptr<sngxml::dom::Element> classesElement(new sngxml::dom::Element(U"classes"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = classesElement.get();
            for (ClassTypeSymbol* cls : classes)
            {
                AddClass(*cls);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(classesElement.release()));
            currentElement = prevElement;
        }
        std::vector<InterfaceTypeSymbol*> interfaces = GetInterfaces(container);
        if (!interfaces.empty())
        {
            std::unique_ptr<sngxml::dom::Element> interfacesElement(new sngxml::dom::Element(U"interfaces"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = interfacesElement.get();
            for (InterfaceTypeSymbol* intf : interfaces)
            {
                AddInterface(*intf);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(interfacesElement.release()));
            currentElement = prevElement;
        }
        std::vector<FunctionSymbol*> ctorsDtors = GetConstructorsAndDestructors(container);
        if (!ctorsDtors.empty())
        {
            std::unique_ptr<sngxml::dom::Element> ctorsDtorsElement(new sngxml::dom::Element(U"ctorsDtors"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = ctorsDtorsElement.get();
            for (FunctionSymbol* function : ctorsDtors)
            {
                AddCtorOrDtor(*function);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(ctorsDtorsElement.release()));
            currentElement = prevElement;
        }
        std::vector<FunctionSymbol*> functions = GetFunctions(container);
        if (!functions.empty())
        {
            std::unique_ptr<sngxml::dom::Element> functionsElement(new sngxml::dom::Element(U"functions"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = functionsElement.get();
            for (FunctionSymbol* function : functions)
            {
                AddFunction(*function);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionsElement.release()));
            currentElement = prevElement;
        }
        std::vector<DelegateTypeSymbol*> delegates = GetDelegates(container);
        if (!delegates.empty())
        {
            std::unique_ptr<sngxml::dom::Element> delegatesElement(new sngxml::dom::Element(U"delegates"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = delegatesElement.get();
            for (DelegateTypeSymbol* delegate : delegates)
            {
                AddDelegate(*delegate);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(delegatesElement.release()));
            currentElement = prevElement;
        }
        std::vector<ClassDelegateTypeSymbol*> classDelegates = GetClassDelegates(container);
        if (!classDelegates.empty())
        {
            std::unique_ptr<sngxml::dom::Element> classDelegatesElement(new sngxml::dom::Element(U"classDelegates"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = classDelegatesElement.get();
            for (ClassDelegateTypeSymbol* classDelegate : classDelegates)
            {
                AddClassDelegate(*classDelegate);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(classDelegatesElement.release()));
            currentElement = prevElement;
        }
        std::vector<TypedefSymbol*> typedefs = GetTypedefs(container);
        if (!typedefs.empty())
        {
            std::unique_ptr<sngxml::dom::Element> typedefsElement(new sngxml::dom::Element(U"typedefs"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = typedefsElement.get();
            for (TypedefSymbol* typedef_ : typedefs)
            {
                AddTypedef(*typedef_);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typedefsElement.release()));
            currentElement = prevElement;
        }
        std::vector<EnumTypeSymbol*> enumTypes = GetEnumTypes(container);
        if (!enumTypes.empty())
        {
            std::unique_ptr<sngxml::dom::Element> enumTypesElement(new sngxml::dom::Element(U"enumTypes"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = enumTypesElement.get();
            for (EnumTypeSymbol* enumType : enumTypes)
            {
                AddEnumType(*enumType);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(enumTypesElement.release()));
            currentElement = prevElement;
        }
        std::vector<EnumConstantSymbol*> enumConstants = GetEnumConstants(container);
        if (!enumConstants.empty())
        {
            std::unique_ptr<sngxml::dom::Element> enumConstantsElement(new sngxml::dom::Element(U"enumConstants"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = enumConstantsElement.get();
            for (EnumConstantSymbol* enumConstant : enumConstants)
            {
                AddEnumConstant(*enumConstant);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(enumConstantsElement.release()));
            currentElement = prevElement;
        }
        std::vector<ConstantSymbol*> constants = GetConstants(container);
        if (!constants.empty())
        {
            std::unique_ptr<sngxml::dom::Element> constantsElement(new sngxml::dom::Element(U"constants"));
            sngxml::dom::Element* prevElement = currentElement;
            currentElement = constantsElement.get();
            for (ConstantSymbol* constant : constants)
            {
                AddConstant(*constant);
            }
            prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constantsElement.release()));
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
        std::unique_ptr<sngxml::dom::Element> variablesElement(new sngxml::dom::Element(varContainerName));
        sngxml::dom::Element* prevElement = currentElement;
        currentElement = variablesElement.get();
        for (VariableSymbol* variable : variables)
        {
            AddVariable(*variable);
        }
        prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(variablesElement.release()));
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

std::vector<InterfaceTypeSymbol*> SymbolTableXmlBuilder::GetInterfaces(ContainerSymbol& container)
{
    std::vector<InterfaceTypeSymbol*> interfaceSymbols;
    for (const std::unique_ptr<Symbol>& member : container.Members())
    {
        if (member->GetSymbolType() == SymbolType::interfaceTypeSymbol && member->IsProject())
        {
            InterfaceTypeSymbol* intf = static_cast<InterfaceTypeSymbol*>(member.get());
            interfaceSymbols.push_back(intf);
        }
    }
    std::sort(interfaceSymbols.begin(), interfaceSymbols.end(), ByCodeName());
    return interfaceSymbols;
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

sngxml::dom::Document* GetModuleDocument(Input* input, const std::u32string& moduleName)
{
    for (const auto& p : input->externalModuleDocs)
    {
        sngxml::dom::Document* doc = p.get();
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
    std::vector<std::unique_ptr<sngxml::dom::Document>> refModuleXmlDocs;
    int n = rootModule->AllReferencedModules().size();
    for (int i = 0; i < n; ++i)
    {
        Module* referencedModule = rootModule->AllReferencedModules()[i];
        std::pair<std::string, bool> refModuleXmlDocFilePathExternal = ResolveModuleXmlDocFilePath(input, referencedModule->Name());
        sngxml::dom::Document* moduleDoc = nullptr;
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
            if (!refModuleXmlDocFilePathExternal.first.empty())
            {
                std::unique_ptr<sngxml::dom::Document> refModuleXmlDoc = sngxml::dom::ReadDocument(refModuleXmlDocFilePathExternal.first);
                sngxml::dom::Document* moduleXmlDoc = refModuleXmlDoc.get();
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
            else
            {
                continue;
            }
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
            sngxml::dom::Document* moduleDoc = GetModuleDocument(input, moduleName);
            if (!moduleDoc)
            {
                std::pair<std::string, bool> refModuleXmlDocFilePathExternal = ResolveModuleXmlDocFilePath(input, moduleName);
                std::unique_ptr<sngxml::dom::Document> refModuleXmlDoc = sngxml::dom::ReadDocument(refModuleXmlDocFilePathExternal.first);
                input->externalModuleDocs.push_back(std::move(refModuleXmlDoc));
            }
        }
    }
}

} } // namespace cmajor::cmdoclib
