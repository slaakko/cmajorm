// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_OPERATION_REPOSITORY_INCLUDED
#define CMAJOR_BINDER_OPERATION_REPOSITORY_INCLUDED
#include <cmajor/binder/BoundExpression.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <sngcm/ast/Class.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;
using namespace sngcm::ast;

class BoundCompileUnit;
class BoundClass;
class BoundFunction;
class BoundCompoundStatement;
class StatementBinder;

enum class CollectFlags : int
{
    none = 0, noRvalueRef = 1 << 0
};

inline CollectFlags operator|(CollectFlags left, CollectFlags right)
{
    return static_cast<CollectFlags>(static_cast<int>(left) | static_cast<int>(right));
}

inline CollectFlags operator&(CollectFlags left, CollectFlags right)
{
    return static_cast<CollectFlags>(static_cast<int>(left) & static_cast<int>(right));
}

class Operation
{
public:
    Operation(const std::u32string& groupName_, int arity_, BoundCompileUnit& boundCompileUnit_);
    virtual ~Operation();
    virtual void CollectViableFunctions(ContainerScope* containerScope_, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span, CollectFlags flags) = 0;
    const std::u32string& GroupName() const { return groupName; }
    int Arity() const { return arity; }
    SymbolTable* GetSymbolTable();
    BoundCompileUnit& GetBoundCompileUnit();
    Module* GetModule() { return module; }
private:
    std::u32string groupName;
    int arity;
    BoundCompileUnit& boundCompileUnit;
    Module* module;
};

class ArityOperation
{
public:
    void Add(Operation* operation);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span, CollectFlags flags);
private:
    std::vector<Operation*> operations;
};

class OperationGroup
{
public:
    void Add(Operation* operation);
    void CollectViableFunctions(ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundFunction* currentFunction, 
        ViableFunctionSet& viableFunctions,  std::unique_ptr<Exception>& exception, const Span& span, CollectFlags flags);
private:
    std::vector<std::unique_ptr<ArityOperation>> arityOperations;
};

class OperationRepository
{
public:
    OperationRepository(BoundCompileUnit& boundCompileUnit_);
    void Add(Operation* operation);
    void CollectViableFunctions(const std::u32string& groupName, ContainerScope* containerScope, const std::vector<std::unique_ptr<BoundExpression>>& arguments, 
        BoundFunction* currentFunction, ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span, CollectFlags flags);
    void GenerateCopyConstructorFor(ClassTypeSymbol* classTypeSymbol, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span);
    void GenerateCopyConstructorFor(InterfaceTypeSymbol* interfaceTypeSymbol, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span);
private:
    BoundCompileUnit& boundCompileUnit;
    std::unordered_map<std::u32string, OperationGroup*> operationGroupMap;
    std::vector<std::unique_ptr<OperationGroup>> operationGroups;
    std::vector<std::unique_ptr<Operation>> operations;
    Operation* copyConstructorOperation;
};

void GenerateDestructorImplementation(BoundClass* boundClass, DestructorSymbol* destructorSymbol, BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, BoundFunction* currentFunction, 
    const Span& span);
void GenerateStaticClassInitialization(StaticConstructorSymbol* staticConstructorSymbol, StaticConstructorNode* staticConstructorNode, BoundCompileUnit& boundCompileUnit,
    BoundCompoundStatement* boundCompoundStatement, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder, const Span& span);
void GenerateClassInitialization(ConstructorSymbol* constructorSymbol, ConstructorNode* constructorNode, BoundCompoundStatement* boundCompoundStatement, BoundFunction* boundFunction, 
    BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, StatementBinder* statementBinder, bool generateDefault, const Span& span);
void GenerateClassAssignment(MemberFunctionSymbol* assignmentFunctionSymbol, MemberFunctionNode* assignmentNode, BoundCompoundStatement* boundCompoundStatement, BoundFunction* boundFunction,
    BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, StatementBinder* statementBinder, bool generateDefault, const Span& span);
void GenerateClassTermination(DestructorSymbol* destructorSymbol, DestructorNode* destructorNode, BoundCompoundStatement* boundCompoundStatement, BoundFunction* boundFunction,
    BoundCompileUnit& boundCompileUnit, ContainerScope* containerScope, StatementBinder* statementBinder, const Span& span);

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_OPERATION_REPOSITORY_INCLUDED
