// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEGENCPP_CMCPP_CODEGENERATOR_INCLUDED
#define CMAJOR_CODEGENCPP_CMCPP_CODEGENERATOR_INCLUDED
#include <cmajor/codegencpp/CodeGenCppApi.hpp>
#include <cmajor/codegenbase/CodeGenerator.hpp>
#include <cmajor/cmcppi/CompileUnit.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/ir/Emitter.hpp>

namespace cmajor { namespace codegencpp {

using namespace cmajor::binder;
using namespace cmajor::symbols;

const int64_t fileInfoNodeType = 0;
const int64_t funcInfoNodeType = 1;
const int64_t lineInfoNodeType = 2;
const int64_t beginTryNodeType = 3;
const int64_t endTryNodeType = 4;
const int64_t catchNodeType = 5;

struct Cleanup
{
    Cleanup(void* cleanupBlock_) : cleanupBlock(cleanupBlock_) {}
    void* cleanupBlock;
    std::vector<std::unique_ptr<BoundFunctionCall>> destructors;
};

class CODEGENCPP_API CmCppCodeGenerator : public cmajor::codegenbase::CodeGenerator, public BoundNodeVisitor, public cmajor::ir::EmittingDelegate
{
public:
    CmCppCodeGenerator(cmajor::ir::EmittingContext& emittingContext_);
    CmCppCodeGenerator(const CmCppCodeGenerator&) = delete;
    CmCppCodeGenerator& operator=(const CmCppCodeGenerator&) = delete;
    void GenerateCode(void* boundCompileUnit) override;
    void Visit(BoundCompileUnit& boundCompileUnit) override;
    void Visit(BoundNamespace& boundNamespace) override;
    void Visit(BoundClass& boundClass) override;
    void Visit(BoundFunction& boundFunction) override;
    void Visit(BoundCompoundStatement& boundCompoundStatement) override;
    void Visit(BoundReturnStatement& boundReturnStatement) override;
    void Visit(BoundGotoCaseStatement& boundGotoCaseStatement) override;
    void Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement) override;
    void Visit(BoundBreakStatement& boundBreakStatement) override;
    void Visit(BoundContinueStatement& boundContinueStatement) override;
    void Visit(BoundGotoStatement& boundGotoStatement) override;
    void Visit(BoundSequenceStatement& boundSequenceStatement) override;
    void Visit(BoundIfStatement& boundIfStatement) override;
    void Visit(BoundWhileStatement& boundWhileStatement) override;
    void Visit(BoundDoStatement& boundDoStatement) override;
    void Visit(BoundForStatement& boundForStatement) override;
    void Visit(BoundSwitchStatement& boundSwitchStatement) override;
    void Visit(BoundCaseStatement& boundCaseStatement) override;
    void Visit(BoundDefaultStatement& boundDefaultStatement) override;
    void Visit(BoundExpressionStatement& boundExpressionStatement) override;
    void Visit(BoundInitializationStatement& boundInitializationStatement) override;
    void Visit(BoundConstructionStatement& boundConstructionStatement) override;
    void Visit(BoundAssignmentStatement& boundAssignmentStatement) override;
    void Visit(BoundEmptyStatement& boundEmptyStatement) override;
    void Visit(BoundSetVmtPtrStatement& boundSetVmtPtrStatement) override;
    void Visit(BoundThrowStatement& boundThrowStatement) override;
    void Visit(BoundTryStatement& boundTryStatement) override;
    void Visit(BoundRethrowStatement& boundRethrowStatement) override;
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
    void GenJumpingBoolCode();
    void SetTarget(BoundStatement* labeledStatement);
    void ExitBlocks(BoundCompoundStatement* targetBlock);
    void* GetGlobalStringPtr(int stringId) override;
    void* GetGlobalWStringConstant(int stringId) override;
    void* GetGlobalUStringConstant(int stringId) override;
    void* GetGlobalUuidConstant(int uuidId) override;
    void* HandlerBlock() override;
    void* CleanupBlock() override;
    void GenerateCleanup(int tryIndex, BoundFunctionCall* destructorCall);
    void GenerateCodeForCleanups();
    bool InTryBlock() const override;
    int CurrentTryBlockId() const override;
    int Install(const std::string& str) override;
    int Install(const std::u16string& str) override;
    int Install(const std::u32string& str) override;
    void Compile(const std::string& intermediateCodeFile);
    void SetLineNumber(int32_t lineNumber) override;
    std::string GetSourceFilePath(const Span& span, const boost::uuids::uuid& moduleId) override;
    void GenerateEnterFunctionCode(BoundFunction& boundFunction);
    void GenerateExitFunctionCode(BoundFunction& boundFunction);
    void GenerateInitUnwindInfoFunction(BoundCompileUnit& boundCompileUnit);
    void GenerateInitCompileUnitFunction(BoundCompileUnit& boundCompileUnit);
    void GenerateGlobalInitFuncion(BoundCompileUnit& boundCompileUnit);
private:
    cmajor::ir::Emitter* emitter;
    cmajor::ir::EmittingContext* emittingContext;
    SymbolTable* symbolTable;
    Module* module;
    BoundCompileUnit* compileUnit;
    cmcppi::CompileUnit* nativeCompileUnit;
    void* function;
    void* entryBasicBlock;
    bool lastInstructionWasRet;
    bool prevWasTerminator;
    bool destructorCallGenerated;
    bool genJumpingBoolCode;
    void* trueBlock;
    void* falseBlock;
    void* breakTarget;
    void* continueTarget;
    int32_t continueTargetNodeId;
    int32_t loopNodeId;
    BoundStatement* sequenceSecond;
    BoundFunction* currentFunction;
    BoundCompoundStatement* currentBlock;
    BoundCompoundStatement* breakTargetBlock;
    BoundCompoundStatement* continueTargetBlock;
    std::unordered_map<BoundStatement*, void*> labeledStatementMap;
    std::unordered_map<BoundCompoundStatement*, std::vector<std::unique_ptr<BoundFunctionCall>>> blockDestructionMap;
    std::vector<BoundCompoundStatement*> blocks;
    std::vector<std::unique_ptr<Cleanup>> cleanups;
    std::unordered_map<int, Cleanup*> cleanupMap;
    std::unordered_map<int, void*> tryIndexCleanupTryBlockMap;
    std::unordered_map<int, int> tryIndexMap;
    void* lastAlloca;
    BoundClass* currentClass;
    std::stack<BoundClass*> classStack;
    bool basicBlockOpen;
    void* defaultDest;
    std::unordered_map<IntegralValue, void*, IntegralValueHash>* currentCaseMap;
    std::unordered_map<int, void*> utf8stringMap;
    std::unordered_map<int, void*> utf16stringMap;
    std::unordered_map<int, void*> utf32stringMap;
    std::unordered_map<int, void*> uuidMap;
    std::unordered_map<void*, int> unwindBlockMap;
    std::string compileUnitId;
    bool generateLineNumbers;
    int64_t currentTryBlockId;
    int64_t nextTryBlockId;
    void* currentTryNextBlock;
    void* handlerBlock;
    void* cleanupBlock;
    bool inTryBlock;
    int numTriesInCurrentBlock;
    int tryIndex;
    int32_t prevLineNumber;
    std::set<FunctionSymbol*> compileUnitFunctions;
    std::unordered_map<BoundCompoundStatement*, int16_t> scopeIdMap;
    int32_t prevControlFlowGraphNodeId;
};

} } // namespace cmajor::codegencpp

#endif // CMAJOR_CODEGENCPP_CMCPP_CODEGENERATOR_INCLUDED
