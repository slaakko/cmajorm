// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_SYMBOL_TABLE_INCLUDED
#define CMAJOR_SYMBOLS_SYMBOL_TABLE_INCLUDED
#include <cmajor/symbols/NamespaceSymbol.hpp>
#include <cmajor/symbols/DerivedTypeSymbol.hpp>
#include <cmajor/symbols/ClassTemplateSpecializationSymbol.hpp>
#include <cmajor/symbols/ArrayTypeSymbol.hpp>
#include <cmajor/symbols/ConversionTable.hpp>
#include <sngxml/dom/Document.hpp>
#include <sngcm/ast/Namespace.hpp>
#include <sngcm/ast/Function.hpp>
#include <sngcm/ast/Class.hpp>
#include <sngcm/ast/Interface.hpp>
#include <sngcm/ast/Delegate.hpp>
#include <sngcm/ast/Typedef.hpp>
#include <sngcm/ast/Constant.hpp>
#include <sngcm/ast/GlobalVariable.hpp>
#include <sngcm/ast/Enumeration.hpp>
#include <boost/functional/hash.hpp>
#include <stack>

namespace cmajor { namespace symbols {

using namespace sngcm::ast;

class Module;
class FunctionSymbol;
class TypeSymbol;
class ConceptSymbol;
class TypedefSymbol;
class ConstantSymbol;

struct TypeOrConceptRequest
{
    TypeOrConceptRequest(Symbol* symbol_, const boost::uuids::uuid& typeId_, int index_) : symbol(symbol_), typeId(typeId_), index(index_) {}
    Symbol* symbol;
    boost::uuids::uuid typeId;
    int index;
};

struct FunctionRequest
{
    FunctionRequest(Symbol* symbol_, const boost::uuids::uuid& functionId_, int index_) : symbol(symbol_), functionId(functionId_), index(index_) {}
    Symbol* symbol;
    boost::uuids::uuid functionId;
    int index;
};

struct ClassTemplateSpecializationKey
{
    ClassTemplateSpecializationKey(ClassTypeSymbol* classTemplate_, const std::vector<TypeSymbol*>& templateArgumentTypes_) : 
        classTemplate(classTemplate_), templateArgumentTypes(templateArgumentTypes_)
    {
    }
    ClassTypeSymbol* classTemplate;
    std::vector<TypeSymbol*> templateArgumentTypes;
};

struct ClassTemplateSpecializationKeyHash
{
    size_t operator()(const ClassTemplateSpecializationKey& key) const
    {
        size_t x = boost::hash<boost::uuids::uuid>()(key.classTemplate->TypeId());
        int n = key.templateArgumentTypes.size();
        for (int i = 0; i < n; ++i)
        {
            x = x ^ boost::hash<boost::uuids::uuid>()(key.templateArgumentTypes[i]->TypeId());
        }
        return x;
    }
};

bool operator==(const ClassTemplateSpecializationKey& left, const ClassTemplateSpecializationKey& right);
bool operator!=(const ClassTemplateSpecializationKey& left, const ClassTemplateSpecializationKey& right);

struct ArrayKey
{
    ArrayKey(TypeSymbol* elementType_, int64_t size_) : elementType(elementType_), size(size_) {}
    TypeSymbol* elementType;
    int64_t size;
};

bool operator==(const ArrayKey& left, const ArrayKey& right);
bool operator!=(const ArrayKey& left, const ArrayKey& right);

struct ArrayKeyHash
{
    size_t operator()(const ArrayKey& key) const
    {
        size_t x = boost::hash<boost::uuids::uuid>()(key.elementType->TypeId());
        x = x ^ std::hash<int64_t>()(key.size);
        return x;
    }
};

SYMBOLS_API void MapIdentifierToSymbolDefinition(IdentifierNode* identifierNode, Symbol* symbol);

class SYMBOLS_API SymbolTable
{
public:
    SymbolTable(Module* module_);
    ~SymbolTable();
    void Write(SymbolWriter& writer);
    void Read(SymbolReader& reader);
    void Import(const SymbolTable& symbolTable);
    void FinishRead(const std::vector<ArrayTypeSymbol*>& arrayTypes, const std::vector<DerivedTypeSymbol*>& derivedTypes,
        const std::vector<ClassTemplateSpecializationSymbol*>& classTemplateSpecializations,
        const std::vector<TypeOrConceptRequest>& typeAndConceptRequests, const std::vector<FunctionRequest>& functionRequests, std::vector<FunctionSymbol*>& conversions);
    void Clear();
    const NamespaceSymbol& GlobalNs() const { return globalNs; }
    NamespaceSymbol& GlobalNs() { return globalNs; }
    const ContainerSymbol* Container() const { return container; }
    ContainerSymbol* Container() { return container; }
    void SetCurrentCompileUnit(CompileUnitNode* currentCompileUnit_) { currentCompileUnit = currentCompileUnit_; }
    void BeginContainer(ContainerSymbol* container_);
    void EndContainer();
    void MapNs(NamespaceSymbol* fromNs, NamespaceSymbol* toNs);
    NamespaceSymbol* GetMappedNs(NamespaceSymbol* fromNs) const;
    NamespaceSymbol* BeginNamespace(NamespaceNode& namespaceNode);
    NamespaceSymbol* BeginNamespace(const std::u32string& namespaceName, const Span& span, const boost::uuids::uuid& sourceModuleId);
    void EndNamespace();
    void BeginFunction(FunctionNode& functionNode, int32_t functionIndex);
    void EndFunction(bool addMember);
    void AddParameter(ParameterNode& parameterNode);
    void BeginClass(ClassNode& classNode);
    void EndClass();
    void BeginClassTemplateSpecialization(ClassNode& classInstanceNode, ClassTemplateSpecializationSymbol* classTemplateSpecialization);
    void EndClassTemplateSpecialization();
    void AddTemplateParameter(TemplateParameterNode& templateParameterNode);
    void AddTemplateParameter(IdentifierNode& identifierNode);
    void BeginInterface(InterfaceNode& interfaceNode);
    void EndInterface();
    void BeginStaticConstructor(StaticConstructorNode& staticConstructorNode, int32_t functionIndex);
    void EndStaticConstructor(bool addMember);
    void BeginConstructor(ConstructorNode& constructorNode, int32_t functionIndex);
    void EndConstructor(bool addMember);
    void BeginDestructor(DestructorNode& destructorNode, int32_t functionIndex);
    void EndDestructor(bool addMember);
    void BeginMemberFunction(MemberFunctionNode& memberFunctionNode, int32_t functionIndex);
    void EndMemberFunction(bool addMember);
    void BeginConversionFunction(ConversionFunctionNode& conversionFunctionNode, int32_t functionIndex);
    void EndConversionFunction(bool addMember);
    void AddMemberVariable(MemberVariableNode& memberVariableNode);
    void BeginDelegate(DelegateNode& delegateNode);
    void EndDelegate();
    void BeginClassDelegate(ClassDelegateNode& classDelegateNode);
    void EndClassDelegate();
    void BeginConcept(ConceptNode& conceptNode, bool hasSource);
    void EndConcept();
    void BeginAxiom(AxiomNode& axiomNode);
    void EndAxiom();
    void BeginDeclarationBlock(Node& node);
    void EndDeclarationBlock();
    void AddLocalVariable(ConstructionStatementNode& constructionStatementNode);
    void AddLocalVariable(IdentifierNode& identifierNode);
    TypedefSymbol* AddTypedef(TypedefNode& typedefNode);
    ConstantSymbol* AddConstant(ConstantNode& constantNode);
    GlobalVariableSymbol* AddGlobalVariable(GlobalVariableNode& globalVariableNode);
    void BeginEnumType(EnumTypeNode& enumTypeNode);
    void EndEnumType();
    void AddEnumConstant(EnumConstantNode& enumConstantNode);
    void AddTypeSymbolToGlobalScope(TypeSymbol* typeSymbol);
    void AddFunctionSymbolToGlobalScope(FunctionSymbol* functionSymbol);
    void MapNode(Node* node, Symbol* symbol);
    Symbol* GetSymbolNoThrow(Node* node) const;
    Symbol* GetSymbol(Node* node) const;
    Node* GetNodeNoThrow(Symbol* symbol) const;
    Node* GetNode(Symbol* symbol) const;
    void SetTypeIdFor(TypeSymbol* typeSymbol);
    void SetTypeIdFor(ConceptSymbol* conceptSymbol);
    void SetFunctionIdFor(FunctionSymbol* functionSymbol);
    FunctionSymbol* GetFunctionById(const boost::uuids::uuid& functionId) const;
    void AddTypeOrConceptSymbolToTypeIdMap(Symbol* typeOrConceptSymbol);
    void AddFunctionSymbolToFunctionIdMap(FunctionSymbol* functionSymbol);
    void EmplaceTypeRequest(SymbolReader& reader, Symbol* forSymbol, const boost::uuids::uuid& typeId, int index);
    void EmplaceConceptRequest(SymbolReader& reader, Symbol* forSymbol, const boost::uuids::uuid& typeId);
    void EmplaceFunctionRequest(SymbolReader& reader, Symbol* forSymbol, const boost::uuids::uuid& functionId, int index);
    void ProcessTypeConceptAndFunctionRequests(const std::vector<TypeOrConceptRequest>& typeAndConceptRequests, const std::vector<FunctionRequest>& functionRequests);
    TypeSymbol* GetTypeByNameNoThrow(const std::u32string& typeName) const;
    TypeSymbol* GetTypeByName(const std::u32string& typeName) const;
    TypeSymbol* MakeDerivedType(TypeSymbol* baseType, const TypeDerivationRec& derivationRec, const Span& span, const boost::uuids::uuid& moduleId);
    ClassTemplateSpecializationSymbol* MakeClassTemplateSpecialization(ClassTypeSymbol* classTemplate, const std::vector<TypeSymbol*>& templateArgumentTypes, 
        const Span& span, const boost::uuids::uuid& moduleId);
    ClassTemplateSpecializationSymbol* CopyClassTemplateSpecialization(ClassTemplateSpecializationSymbol* source);
    ClassTemplateSpecializationSymbol* GetCurrentClassTemplateSpecialization(ClassTemplateSpecializationSymbol* source);
    void AddClassTemplateSpecializationsToClassTemplateSpecializationMap(const std::vector<ClassTemplateSpecializationSymbol*>& classTemplateSpecializations);
    ArrayTypeSymbol* MakeArrayType(TypeSymbol* elementType, int64_t size, const Span& span, const boost::uuids::uuid& moduleId);
    const FunctionSymbol* MainFunctionSymbol() const { return mainFunctionSymbol; }
    FunctionSymbol* MainFunctionSymbol() { return mainFunctionSymbol; }
    void AddConversion(FunctionSymbol* conversion);
    void AddConversion(FunctionSymbol* conversion, Module* module);
    FunctionSymbol* GetConversion(TypeSymbol* sourceType, TypeSymbol* targetType, const Span& span, const boost::uuids::uuid& moduleId) const;
    ConversionTable& GetConversionTable() { return conversionTable; }
    const ConversionTable& GetConversionTable() const { return conversionTable; }
    void AddPolymorphicClass(ClassTypeSymbol* polymorphicClass);
    const std::unordered_set<ClassTypeSymbol*>& PolymorphicClasses() const { return polymorphicClasses; }
    void AddClassHavingStaticConstructor(ClassTypeSymbol* classHavingStaticConstructor);
    const std::unordered_set<ClassTypeSymbol*>& ClassesHavingStaticConstructor() const { return classesHavingStaticConstructor; }
    void AddJsonClass(const std::u32string& jsonClass);
    const std::unordered_set<std::u32string>& JsonClasses() const { return jsonClasses; }
    std::vector<TypeSymbol*> Types() const;
    void Copy(const SymbolTable& that);
    ClassTypeSymbol* CurrentClass() { return currentClass; }
    InterfaceTypeSymbol* CurrentInterface() { return currentInterface; }
    void SetCurrentClass(ClassTypeSymbol* currentClass_) { currentClass = currentClass_; }
    void SetCurrentFunctionSymbol(FunctionSymbol* currentFunctionSymbol_) { currentFunctionSymbol = currentFunctionSymbol_; }
    void MapProfiledFunction(const boost::uuids::uuid& functionId, const std::u32string& profiledFunctionName);
    std::u32string GetProfiledFunctionName(const boost::uuids::uuid& functionId) const;
    Module* GetModule() { return module; }
    std::unique_ptr<sngxml::dom::Document> ToDomDocument();
    void MapInvoke(IdentifierNode* invokeId, FunctionSymbol* functionSymbol);
    FunctionSymbol* GetInvoke(IdentifierNode* invokeId) const;
    void MapSymbol(Node* node, Symbol* symbol);
    Symbol* GetMappedSymbol(Node* node) const;
    void MapIdentifierToSymbolDefinition(IdentifierNode* identifierNode, Module* module, Symbol* symbol);
    SymbolLocation* GetDefinitionLocation(const SymbolLocation& identifierLocation);
    void InitUuids();
    const boost::uuids::uuid& GetDerivationId(Derivation derivation) const;
    const boost::uuids::uuid& GetPositionId(int index) const;
    int NumSpecializations() const { return classTemplateSpecializationMap.size(); }
    int NumSpecializationsNew() const { return numSpecializationsNew; }
    int NumSpecializationsCopied() const { return numSpecializationsCopied; }
    void Check();
    FunctionSymbol* GetCreatedFunctionSymbol() { return createdFunctionSymbol; }
    void AddFunctionSymbol(std::unique_ptr<FunctionSymbol>&& functionSymbol);
    void ResetCursorContainer() { cursorContainer = nullptr; }
    ContainerSymbol* CursorContainer() const { return cursorContainer; }
    void SetCursorContainer(const sngcm::ast::Node& node);
    int NextAxiomNumber() { return axiomNumber++; }
    void ResetAxiomNumber() { axiomNumber = 0; }
    void ResetAliasNodesAndNamespaceImports();
    void ResetMainFunctionSymbol() { mainFunctionSymbol = nullptr; }
    const std::vector<AliasNode*>& AliasNodes() const { return aliasNodes; }
    void AddAliasNode(AliasNode* aliasNode) { aliasNodes.push_back(aliasNode); }
    const std::vector<NamespaceImportNode*>& NamespaceImports() const { return namespaceImports; }
    void AddNamespaceImport(NamespaceImportNode* namespaceImport) { namespaceImports.push_back(namespaceImport); }
private:
    Module* module;
    std::vector<boost::uuids::uuid> derivationIds;
    std::vector<boost::uuids::uuid> positionIds;
    NamespaceSymbol globalNs;
    std::vector<AliasNode*> aliasNodes;
    std::vector<NamespaceImportNode*> namespaceImports;
    std::unordered_map<IdentifierNode*, Symbol*> identifierSymbolDefinitionMap;
    std::map<SymbolLocation, SymbolLocation> symbolDefinitionMap;
    CompileUnitNode* currentCompileUnit;
    ContainerSymbol* container;
    ContainerSymbol* cursorContainer;
    ClassTypeSymbol* currentClass;
    std::stack<ClassTypeSymbol*> currentClassStack;
    InterfaceTypeSymbol* currentInterface;
    std::stack<InterfaceTypeSymbol*> currentInterfaceStack;
    std::stack<ContainerSymbol*> containerStack;
    FunctionSymbol* mainFunctionSymbol;
    FunctionSymbol* currentFunctionSymbol;
    FunctionSymbol* createdFunctionSymbol;
    int parameterIndex;
    int declarationBlockIndex;
    int axiomNumber;
    std::unordered_map<NamespaceSymbol*, NamespaceSymbol*> nsMap;
    std::unordered_map<Node*, Symbol*> nodeSymbolMap;
    std::unordered_map<Symbol*, Node*> symbolNodeMap;
    std::unordered_map<boost::uuids::uuid, Symbol*, boost::hash<boost::uuids::uuid>> typeIdMap;
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> functionIdMap;
    std::unordered_map<std::u32string, TypeSymbol*> typeNameMap;
    std::unordered_map<boost::uuids::uuid, std::u32string, boost::hash<boost::uuids::uuid>> profiledFunctionNameMap;
    std::unordered_map<boost::uuids::uuid, std::vector<DerivedTypeSymbol*>, boost::hash<boost::uuids::uuid>> derivedTypeMap;
    std::vector<std::unique_ptr<DerivedTypeSymbol>> derivedTypes;
    std::unordered_map<ClassTemplateSpecializationKey, ClassTemplateSpecializationSymbol*, ClassTemplateSpecializationKeyHash> classTemplateSpecializationMap;
    std::vector<std::unique_ptr<ClassTemplateSpecializationSymbol>> classTemplateSpecializations;
    std::unordered_map<ClassTemplateSpecializationSymbol*, ClassTemplateSpecializationSymbol*> specializationCopyMap;
    std::unordered_map<ArrayKey, ArrayTypeSymbol*, ArrayKeyHash> arrayTypeMap;
    std::vector<std::unique_ptr<ArrayTypeSymbol>> arrayTypes;
    std::unordered_map<IdentifierNode*, FunctionSymbol*> invokeMap;
    std::unordered_map<Node*, Symbol*> mappedNodeSymbolMap;
    std::vector<std::unique_ptr<FunctionSymbol>> functionSymbols;
    ConversionTable conversionTable;
    std::unordered_set<ClassTypeSymbol*> polymorphicClasses;
    std::unordered_set<ClassTypeSymbol*> classesHavingStaticConstructor;
    std::unordered_set<std::u32string> jsonClasses;
    int numSpecializationsCopied;
    int numSpecializationsNew;
    int GetNextDeclarationBlockIndex() { return declarationBlockIndex++; }
    void ResetDeclarationBlockIndex() { declarationBlockIndex = 0; }
    void EmplaceTypeOrConceptRequest(SymbolReader& reader, Symbol* forSymbol, const boost::uuids::uuid& typeId, int index);
    void WriteSymbolDefinitionMap(SymbolWriter& writer);
    void ReadSymbolDefinitionMap(SymbolReader& reader);
};

void InitCoreSymbolTable(SymbolTable& symbolTable);

void InitSymbolTable();
void DoneSymbolTable();

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_SYMBOL_TABLE_INCLUDED
