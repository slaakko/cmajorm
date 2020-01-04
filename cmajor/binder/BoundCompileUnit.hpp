// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_BOUND_COMPILE_UNIT_INCLUDED
#define CMAJOR_BINDER_BOUND_COMPILE_UNIT_INCLUDED
#include <cmajor/binder/BoundNode.hpp>
#include <cmajor/binder/OperationRepository.hpp>
#include <cmajor/binder/FunctionTemplateRepository.hpp>
#include <cmajor/binder/ClassTemplateRepository.hpp>
#include <cmajor/binder/InlineFunctionRepository.hpp>
#include <cmajor/binder/ConstExprFunctionRepository.hpp>
#include <cmajor/binder/StringRepository.hpp>
#include <cmajor/binder/UuidRepository.hpp>
#include <cmajor/binder/ConstantArrayRepository.hpp>
#include <cmajor/binder/ConstantStructureRepository.hpp>
#include <cmajor/binder/ConceptRepository.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/ConversionTable.hpp>
#include <sngcm/ast/CompileUnit.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class BoundExpression;
struct ArgumentMatch;
class AttributeBinder;
class BoundNamespace;

class BINDER_API BoundCompileUnit : public BoundNode
{
public:
    BoundCompileUnit(Module& module_, CompileUnitNode* compileUnitNode_, AttributeBinder* attributeBinder_);
    BoundCompileUnit(const BoundCompileUnit&) = delete;
    BoundCompileUnit& operator=(const BoundCompileUnit&) = delete;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    Module& GetModule() { return module; }
    SymbolTable& GetSymbolTable() { return symbolTable; }
    CompileUnitNode* GetCompileUnitNode() const { return compileUnitNode; }
    void AddFileScope(FileScope* fileScope);
    void RemoveLastFileScope();
    FileScope* ReleaseLastFileScope();
    FileScope* FirstFileScope() const { Assert(!fileScopes.empty(), "file scopes empty");  return fileScopes.front().get(); }
    const std::vector<std::unique_ptr<FileScope>>& FileScopes() const { return fileScopes; }
    void AddBoundNode(std::unique_ptr<BoundNode>&& boundNode);
    const std::vector<std::unique_ptr<BoundNode>>& BoundNodes() const { return boundNodes; }
    FunctionSymbol* GetConversion(TypeSymbol* sourceType, TypeSymbol* targetType, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span, ArgumentMatch& argumentMatch);
    void CollectViableFunctions(const std::u32string& groupName, ContainerScope* containerScope, std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction,
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span, CollectFlags flags);
    FunctionSymbol* InstantiateFunctionTemplate(FunctionSymbol* functionTemplate, const std::unordered_map<TemplateParameterSymbol*, TypeSymbol*>& templateParameterMapping, const Span& span);
    bool InstantiateClassTemplateMemberFunction(FunctionSymbol* memberFunction, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span);
    FunctionSymbol* InstantiateInlineFunction(FunctionSymbol* inlineFunction, ContainerScope* containerScope, const Span& span);
    FunctionNode* GetFunctionNodeFor(FunctionSymbol* constExprFunctionSymbol);
    void GenerateCopyConstructorFor(ClassTypeSymbol* classTypeSymbol, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span);
    void GenerateCopyConstructorFor(InterfaceTypeSymbol* interfaceTypeSymbol, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span);
    int Install(const std::string& str);
    int Install(const std::u16string& str);
    int Install(const std::u32string& str);
    int Install(const boost::uuids::uuid& uuid);
    const std::string& GetUtf8String(int stringId) const;
    const std::u16string& GetUtf16String(int stringId) const;
    const std::u32string& GetUtf32String(int stringId) const;
    const unsigned char* GetUtf8CharPtr(int stringId) const;
    const char16_t* GetUtf16CharPtr(int stringId) const;
    const char32_t* GetUtf32CharPtr(int stringId) const;
    const boost::uuids::uuid& GetUuid(int uuidId) const;
    void AddConstantArray(ConstantSymbol* constantArraySymbol);
    ConstantArrayRepository& GetConstantArrayRepository() { return constantArrayRepository; }
    void AddConstantStructure(ConstantSymbol* constantStructureSymbol);
    ConstantStructureRepository& GetConstantStructureRepository() { return constantStructureRepository; }
    const std::string& SourceFilePath() const { return compileUnitNode->FilePath(); }
    const std::string& LLFilePath() const { return llFilePath; }
    const std::string& OptLLFilePath() const { return optLLFilePath; }
    const std::string& ObjectFilePath() const { return objectFilePath; }
    const std::string& Id() const { return compileUnitNode->Id(); }
    void SetHasGotos() { hasGotos = true; }
    bool HasGotos() const { return hasGotos; }
    ClassTemplateRepository& GetClassTemplateRepository() { return classTemplateRepository; }
    ConceptRepository& GetConceptRepository() { return conceptRepository; }
    void PushBindingTypes();
    void PopBindingTypes();
    bool BindingTypes() const { return bindingTypes; }
    void FinalizeBinding(ClassTemplateSpecializationSymbol* classTemplateSpecialization);
    AttributeBinder* GetAttributeBinder() const { return attributeBinder; }
    void PushNamespace(BoundNamespace* ns);
    void PopNamespace();
    bool HasCopyConstructorFor(const boost::uuids::uuid& typeId) const;
    FunctionSymbol* GetCopyConstructorFor(const boost::uuids::uuid& typeId) const;
    void AddCopyConstructorFor(const boost::uuids::uuid& typeId, std::unique_ptr<FunctionSymbol>&& copyConstructor);
    void AddCopyConstructorToMap(const boost::uuids::uuid& typeId, FunctionSymbol* copyConstructor);
    bool Immutable() const { return immutable; }
    void SetImmutable() { immutable = true; }
    void AddGlobalNs(std::unique_ptr<NamespaceNode>&& globalNs);
    void AddFunctionSymbol(std::unique_ptr<FunctionSymbol>&& functionSymbol);
    int GetNextExitEntryIndex() { return nextExitEntryIndex++; }
private:
    Module& module;
    SymbolTable& symbolTable;
    CompileUnitNode* compileUnitNode;
    AttributeBinder* attributeBinder;
    std::stack<BoundNamespace*> namespaceStack;
    BoundNamespace* currentNamespace;
    std::string llFilePath;
    std::string optLLFilePath;
    std::string objectFilePath;
    std::vector<std::unique_ptr<FileScope>> fileScopes;
    std::vector<std::unique_ptr<BoundNode>> boundNodes;
    std::vector<std::unique_ptr<FunctionSymbol>> functionSymbols;
    std::vector<std::unique_ptr<NamespaceNode>> globalNamespaceNodes;
    bool hasGotos;
    OperationRepository operationRepository;
    FunctionTemplateRepository functionTemplateRepository;
    ClassTemplateRepository classTemplateRepository;
    InlineFunctionRepository inlineFunctionRepository;
    ConstExprFunctionRepository constExprFunctionRepository;
    StringRepository<std::string, const unsigned char*> utf8StringRepository;
    StringRepository<std::u16string, const char16_t*> utf16StringRepository;
    StringRepository<std::u32string, const char32_t*> utf32StringRepository;
    UuidRepository uuidRepository;
    ConstantArrayRepository constantArrayRepository;
    ConstantStructureRepository constantStructureRepository;
    ConceptRepository conceptRepository;
    ConversionTable conversionTable;
    bool bindingTypes;
    std::stack<bool> bindingTypesStack;
    int32_t compileUnitIndex;
    bool immutable;
    std::unordered_map<boost::uuids::uuid, FunctionSymbol*, boost::hash<boost::uuids::uuid>> copyConstructorMap;
    std::vector<std::unique_ptr<FunctionSymbol>> copyConstructors;
    int nextExitEntryIndex;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_BOUND_COMPILE_UNIT_INCLUDED
