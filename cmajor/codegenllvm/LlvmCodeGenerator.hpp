// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGENLLVM_LLVMCODEGENERATOR_INCLUDED
#define CMAJOR_CODEGENLLVM_LLVMCODEGENERATOR_INCLUDED
#include <cmajor/codegenllvm/CodeGenLlvmApi.hpp>
#include <cmajor/codegenbase/CodeGenerator.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/binder/BoundStatement.hpp>

namespace cmajor { namespace codegenllvm {

using namespace cmajor::binder;

struct Cleanup
{
    Cleanup(void* cleanupBlock_, void* handlerBlock_, Pad* currentPad_) : cleanupBlock(cleanupBlock_), handlerBlock(handlerBlock_), currentPad(currentPad_) {}
    void* cleanupBlock;
    void* handlerBlock;
    Pad* currentPad;
    std::vector<std::unique_ptr<BoundFunctionCall>> destructors;
};

class CODEGENLLVM_API LlvmCodeGenerator : public cmajor::codegenbase::CodeGenerator, public BoundNodeVisitor, public cmajor::ir::EmittingDelegate
{
public:
    LlvmCodeGenerator(cmajor::ir::EmittingContext& emittingContext_);
    LlvmCodeGenerator(const LlvmCodeGenerator&) = delete;
    LlvmCodeGenerator& operator=(const LlvmCodeGenerator&) = delete;
    void GenerateCode(void* boundCompileUnit) override;
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
    void Visit(BoundInitializationStatement& boundInitializationStatement) override;
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
    void Visit(BoundBitCast& boundBitCast) override;
    void Visit(BoundFunctionPtr& boundFunctionPtr) override;
    void Visit(BoundDisjunction& boundDisjunction) override;
    void Visit(BoundConjunction& boundConjunction) override;
    void Visit(BoundGlobalVariable& boundGlobalVariable) override;
    void* GetGlobalStringPtr(int stringId) override;
    void* GetGlobalWStringConstant(int stringId) override;
    void* GetGlobalUStringConstant(int stringId) override;
    void* GetGlobalUuidConstant(int uuidId) override;
    void SetLineNumber(int32_t lineNumber) override;
    void* HandlerBlock() override;
    void* CleanupBlock() override;
    bool NewCleanupNeeded() override;
    std::string GetSourceFilePath(const Span& span, const boost::uuids::uuid& moduleId) override;
    cmajor::ir::Pad* CurrentPad() override;
    void* CreateClassDIType(void* classPtr) override;
    void CreateExitFunctionCall();
    void GenerateEnterFunctionCode(BoundFunction& boundFunction);
    void GenerateExitFunctionCode(BoundFunction& boundFunction);
    void GenerateInitUnwindInfoFunction(BoundCompileUnit& boundCompileUnit);
    void GenerateInitCompileUnitFunction(BoundCompileUnit& boundCompileUnit);
    void GenerateGlobalInitFuncion(BoundCompileUnit& boundCompileUnit);
    virtual void* GetPersonalityFunction() const = 0;
    virtual void GenerateCodeForCleanups() = 0;
    void SetTarget(BoundStatement* labeledStatement);
    void ExitBlocks(BoundCompoundStatement* targetBlock);
    void GenJumpingBoolCode();
    void ClearFlags();
    int Install(const std::string& str) override;
    int Install(const std::u16string& str) override;
    int Install(const std::u32string& str) override;
    void MoveAllocaIntoEntryBlock(void* allocaInst);
    void Optimize(int optimizationLevel, const std::string& bcFilePath, const std::string& optBCFilePath);
    void* EntryBasicBlock() const { return entryBasicBlock; }
protected:
    cmajor::symbols::SymbolTable* symbolTable;
    cmajor::symbols::Module* symbolsModule;
    cmajor::ir::Emitter* emitter;
    cmajor::ir::EmittingContext* emittingContext;
    void* module;
    bool debugInfo;
    BoundCompileUnit* compileUnit;
    BoundClass* currentClass;
    BoundFunction* currentFunction;
    std::stack<BoundClass*> classStack;
    std::unordered_map<int, void*> utf8stringMap;
    std::unordered_map<int, void*> utf16stringMap;
    std::unordered_map<int, void*> utf32stringMap;
    std::unordered_map<int, void*> uuidMap;
    void* trueBlock;
    void* falseBlock;
    void* breakTarget;
    void* continueTarget;
    void* handlerBlock;
    void* cleanupBlock;
    void* entryBasicBlock;
    bool newCleanupNeeded;
    Pad* currentPad;
    int prevLineNumber;
    bool destructorCallGenerated;
    bool lastInstructionWasRet;
    bool basicBlockOpen;
    void* lastAlloca;
    int compoundLevel;
    std::string compileUnitId;
    BoundStatement* sequenceSecond;
    BoundCompoundStatement* currentBlock;
    std::vector<std::unique_ptr<Cleanup>> cleanups;
    std::vector<std::unique_ptr<Pad>> pads;
    std::unordered_map<BoundStatement*, void*> labeledStatementMap;
    void* function;
    std::unordered_map<BoundCompoundStatement*, std::vector<std::unique_ptr<BoundFunctionCall>>> blockDestructionMap;
    std::vector<BoundCompoundStatement*> blocks;
    bool genJumpingBoolCode;
    BoundCompoundStatement* breakTargetBlock;
    BoundCompoundStatement* continueTargetBlock;
    void* defaultDest;
    std::unordered_map<IntegralValue, void*, IntegralValueHash>* currentCaseMap;
    std::set<FunctionSymbol*> compileUnitFunctions;
};

} } // namespace cmajor::codegen

#endif // CMAJOR_CODEGENLLVM_LLVMCODEGENERATOR_INCLUDED
