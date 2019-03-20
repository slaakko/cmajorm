// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_EMITTER_BASIC_EMITTER_INCLUDED
#define CMAJOR_EMITTER_BASIC_EMITTER_INCLUDED
#include <cmajor/emitter/EmittingContext.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/BoundExpression.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <cmajor/symbols/Module.hpp>
#include <llvm/IR/Module.h>
#include <llvm/BinaryFormat/Dwarf.h>

namespace cmajor { namespace emitter {

using namespace cmajor::binder;
using namespace cmajor::symbols;

const unsigned cmajorLanguageTag = llvm::dwarf::DW_LANG_C_plus_plus_11; // closest approximation

struct Cleanup
{
    Cleanup(llvm::BasicBlock* cleanupBlock_, llvm::BasicBlock* handlerBlock_, Pad* currentPad_) : cleanupBlock(cleanupBlock_), handlerBlock(handlerBlock_), currentPad(currentPad_) {}
    llvm::BasicBlock* cleanupBlock;
    llvm::BasicBlock* handlerBlock;
    Pad* currentPad;
    std::vector<std::unique_ptr<BoundFunctionCall>> destructors;
};

class BasicEmitter : public cmajor::ir::Emitter, public cmajor::binder::BoundNodeVisitor
{
public:
    BasicEmitter(EmittingContext& emittingContext_, const std::string& compileUnitModuleName_, cmajor::symbols::Module& symbolsModule_);
    void Visit(BoundCompileUnit& boundCompileUnit) override;
    void Visit(BoundNamespace& boundNamespace) override;
    void Visit(BoundClass& boundClass) override;
    void Visit(BoundEnumTypeDefinition& boundEnumTypeDefinition) override;
    void Visit(BoundFunction& boundFunction) override;
    void Visit(BoundSequenceStatement& boundSequenceStatement) override;
    void Visit(BoundCompoundStatement& boundCompoundStatement) override;
    void Visit(BoundIfStatement& boundIfStatement) override;
    void Visit(BoundWhileStatement& boundWhileStatement) override;
    void Visit(BoundDoStatement& boundDoStatement) override;
    void Visit(BoundForStatement& boundForStatement) override;
    void Visit(BoundSwitchStatement& boundSwitchStatement) override;
    void Visit(BoundCaseStatement& boundCaseStatement) override;
    void Visit(BoundDefaultStatement& boundDefaultStatement) override;
    void Visit(BoundConstructionStatement& boundConstructionStatement) override;
    void Visit(BoundAssignmentStatement& boundAssignmentStatement) override;
    void Visit(BoundExpressionStatement& boundExpressionStatement) override;
    void Visit(BoundEmptyStatement& boundEmptyStatement) override;
    void Visit(BoundSetVmtPtrStatement& boundSetVmtPtrStatement) override;
    void Visit(BoundThrowStatement& boundThrowStatement) override;
    void Visit(BoundParameter& boundParameter) override;
    void Visit(BoundLocalVariable& boundLocalVariable) override;
    void Visit(BoundMemberVariable& boundMemberVariable) override;
    void Visit(BoundConstant& boundConstant) override;
    void Visit(BoundEnumConstant& boundEnumConstant) override;
    void Visit(BoundLiteral& boundLiteral) override;
    void Visit(BoundTemporary& boundTemporary) override;
    void Visit(BoundSizeOfExpression& boundSizeOfExpression) override;
    void Visit(BoundAddressOfExpression& boundAddressOfExpression) override;
    void Visit(BoundDereferenceExpression& boundDereferenceExpression) override;
    void Visit(BoundReferenceToPointerExpression& boundReferenceToPointerExpression) override;
    void Visit(BoundFunctionCall& boundFunctionCall) override;
    void Visit(BoundDelegateCall& boundDelegateCall) override;
    void Visit(BoundClassDelegateCall& boundClassDelegateCall) override;
    void Visit(BoundConversion& boundConversion) override;
    void Visit(BoundConstructExpression& boundConstructExpression) override;
    void Visit(BoundConstructAndReturnTemporaryExpression& boundConstructAndReturnTemporaryExpression) override;
    void Visit(BoundClassOrClassDelegateConversionResult& boundClassOrClassDelegateConversionResult) override;
    void Visit(BoundIsExpression& boundIsExpression) override;
    void Visit(BoundAsExpression& boundAsExpression) override;
    void Visit(BoundTypeNameExpression& boundTypeNameExpression) override;
    void Visit(BoundTypeIdExpression& boundTypeIdExpression) override;
    void Visit(BoundBitCast& boundBitCast) override;
    void Visit(BoundFunctionPtr& boundFunctionPtr) override;
    void Visit(BoundDisjunction& boundDisjunction) override;
    void Visit(BoundConjunction& boundConjunction) override;
    std::string GetSourceFilePath(int32_t fileIndex) override;
    llvm::Value* GetGlobalStringPtr(int stringId) override;
    llvm::Value* GetGlobalWStringConstant(int stringId) override;
    llvm::Value* GetGlobalUStringConstant(int stringId) override;
    llvm::Value* GetGlobalUuidConstant(int uuidId) override;
    llvm::BasicBlock* HandlerBlock() override { return handlerBlock; }
    llvm::BasicBlock* CleanupBlock() override { return cleanupBlock; }
    bool NewCleanupNeeded() override { return newCleanupNeeded; }
    Pad* CurrentPad() override { return currentPad; }
    llvm::DIType* CreateClassDIType(void* classPtr) override;
    virtual llvm::Function* GetPersonalityFunction() const = 0;
    virtual void GenerateCodeForCleanups() = 0;
protected:
    EmittingContext & emittingContext;
    SymbolTable* symbolTable;
    std::unique_ptr<llvm::Module> compileUnitModule;
    cmajor::symbols::Module& symbolsModule;
    llvm::IRBuilder<>& builder;
    std::unique_ptr<llvm::DIBuilder> diBuilder;
    bool debugInfo;
    cmajor::ir::ValueStack& stack;
    llvm::LLVMContext& context;
    llvm::Function* function;
    llvm::BasicBlock* trueBlock;
    llvm::BasicBlock* falseBlock;
    llvm::BasicBlock* breakTarget;
    llvm::BasicBlock* continueTarget;
    llvm::BasicBlock* handlerBlock;
    llvm::BasicBlock* cleanupBlock;
    llvm::BasicBlock* entryBasicBlock;
    bool newCleanupNeeded;
    std::vector<std::unique_ptr<Cleanup>> cleanups;
    std::vector<std::unique_ptr<Pad>> pads;
    std::unordered_map<BoundStatement*, llvm::BasicBlock*> labeledStatementMap;
    Pad* currentPad;
    bool genJumpingBoolCode;
    BoundCompileUnit* compileUnit;
    BoundClass* currentClass;
    BoundFunction* currentFunction;
    BoundCompoundStatement* currentBlock;
    BoundCompoundStatement* breakTargetBlock;
    BoundCompoundStatement* continueTargetBlock;
    BoundStatement* sequenceSecond;
    std::unordered_map<IntegralValue, llvm::BasicBlock*, IntegralValueHash>* currentCaseMap;
    llvm::BasicBlock* defaultDest;
    std::stack<BoundClass*> classStack;
    std::vector<BoundCompoundStatement*> blocks;
    std::unordered_map<BoundCompoundStatement*, std::vector<std::unique_ptr<BoundFunctionCall>>> blockDestructionMap;
    std::unordered_map<int, llvm::Value*> utf8stringMap;
    std::unordered_map<int, llvm::Value*> utf16stringMap;
    std::unordered_map<int, llvm::Value*> utf32stringMap;
    std::unordered_map<int, llvm::Value*> uuidMap;
    int prevLineNumber;
    llvm::AllocaInst* lastAlloca;
    int compoundLevel;
    bool destructorCallGenerated;
    bool lastInstructionWasRet;
    bool basicBlockOpen;
    void GenJumpingBoolCode();
    void ExitBlocks(BoundCompoundStatement* targetBlock);
    void CreateExitFunctionCall();
    void SetLineNumber(int32_t lineNumber) override;
    void SetTarget(BoundStatement* labeledStatement);
    void ClearFlags();
    void InsertAllocaIntoEntryBlock(llvm::AllocaInst* allocaInst);
};

} } // namespace cmajor::emitter

#endif // CMAJOR_EMITTER_BASIC_EMITTER_INCLUDED
