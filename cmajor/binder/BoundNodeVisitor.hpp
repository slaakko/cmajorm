// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_BOUND_NODE_VISITOR_INCLUDED
#define CMAJOR_BINDER_BOUND_NODE_VISITOR_INCLUDED
#include <cmajor/binder/BinderApi.hpp>

namespace cmajor { namespace binder {

class BoundCompileUnit;
class BoundNamespace;
class BoundClass;
class BoundEnumTypeDefinition;
class BoundFunction;
class BoundSequenceStatement;
class BoundCompoundStatement;
class BoundReturnStatement;
class BoundIfStatement;
class BoundWhileStatement;
class BoundDoStatement;
class BoundForStatement;
class BoundSwitchStatement;
class BoundCaseStatement;
class BoundGotoCaseStatement;
class BoundGotoDefaultStatement;
class BoundDefaultStatement;
class BoundBreakStatement;
class BoundContinueStatement;
class BoundGotoStatement;
class BoundConstructionStatement;
class BoundAssignmentStatement;
class BoundExpressionStatement;
class BoundInitializationStatement;
class BoundEmptyStatement;
class BoundSetVmtPtrStatement;
class BoundThrowStatement;
class BoundRethrowStatement;
class BoundTryStatement;
class BoundCatchStatement;
class BoundParameter;
class BoundLocalVariable;
class BoundMemberVariable;
class BoundConstant;
class BoundEnumConstant;
class BoundLiteral;
class BoundTemporary;
class BoundSizeOfExpression;
class BoundAddressOfExpression;
class BoundDereferenceExpression;
class BoundReferenceToPointerExpression;
class BoundFunctionCall;
class BoundDelegateCall;
class BoundClassDelegateCall;
class BoundInterfaceCall;
class BoundConversion;
class BoundConstructExpression;
class BoundConstructAndReturnTemporaryExpression;
class BoundClassOrClassDelegateConversionResult;
class BoundIsExpression;
class BoundAsExpression;
class BoundTypeNameExpression;
class BoundTypeIdExpression;
class BoundBitCast;
class BoundFunctionPtr;
class BoundDisjunction;
class BoundConjunction;
class BoundAtomicConstraint;
class BoundDisjunctiveConstraint;
class BoundConjunctiveConstraint;
class BoundGlobalVariable;

class BINDER_API BoundNodeVisitor
{
public:
    virtual ~BoundNodeVisitor();
    virtual void Visit(BoundCompileUnit& boundCompileUnit) {}
    virtual void Visit(BoundNamespace& boundNamespace) {}
    virtual void Visit(BoundClass& boundClass) {}
    virtual void Visit(BoundEnumTypeDefinition& boundEnumTypeDefinition) {}
    virtual void Visit(BoundFunction& boundFunction) {}
    virtual void Visit(BoundSequenceStatement& boundSequenceStatement) {}
    virtual void Visit(BoundCompoundStatement& boundCompoundStatement) {}
    virtual void Visit(BoundReturnStatement& boundReturnStatement) {}
    virtual void Visit(BoundIfStatement& boundIfStatement) {}
    virtual void Visit(BoundWhileStatement& boundWhileStatement) {}
    virtual void Visit(BoundDoStatement& boundDoStatement) {}
    virtual void Visit(BoundForStatement& boundForStatement) {}
    virtual void Visit(BoundSwitchStatement& boundSwitchStatement) {}
    virtual void Visit(BoundCaseStatement& boundCaseStatement) {}
    virtual void Visit(BoundDefaultStatement& boundDefaultStatement) {}
    virtual void Visit(BoundGotoCaseStatement& boundGotoCaseStatement) {}
    virtual void Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement) {}
    virtual void Visit(BoundBreakStatement& boundBreakStatement) {}
    virtual void Visit(BoundContinueStatement& boundContinueStatement) {}
    virtual void Visit(BoundGotoStatement& boundGotoStatement) {}
    virtual void Visit(BoundConstructionStatement& boundConstructionStatement) {}
    virtual void Visit(BoundAssignmentStatement& boundAssignmentStatement) {}
    virtual void Visit(BoundExpressionStatement& boundExpressionStatement) {}
    virtual void Visit(BoundInitializationStatement& boundInitializationStatement) {}
    virtual void Visit(BoundEmptyStatement& boundEmptyStatement) {}
    virtual void Visit(BoundSetVmtPtrStatement& boundSetVmtPtrStatement) {}
    virtual void Visit(BoundThrowStatement& boundThrowStatement) {}
    virtual void Visit(BoundRethrowStatement& boundRethrowStatement) {}
    virtual void Visit(BoundTryStatement& boundTryStatement) {}
    virtual void Visit(BoundCatchStatement& boundCatchStatement) {}
    virtual void Visit(BoundParameter& boundParameter) {}
    virtual void Visit(BoundLocalVariable& boundLocalVariable) {}
    virtual void Visit(BoundMemberVariable& boundMemberVariable) {}
    virtual void Visit(BoundConstant& boundConstant) {}
    virtual void Visit(BoundEnumConstant& boundEnumConstant) {}
    virtual void Visit(BoundLiteral& boundLiteral) {}
    virtual void Visit(BoundTemporary& boundTemporary) {}
    virtual void Visit(BoundSizeOfExpression& boundSizeOfExpression) {}
    virtual void Visit(BoundAddressOfExpression& boundAddressOfExpression) {}
    virtual void Visit(BoundDereferenceExpression& boundDereferenceExpression) {}
    virtual void Visit(BoundReferenceToPointerExpression& boundReferenceToPointerExpression) {}
    virtual void Visit(BoundFunctionCall& boundFunctionCall) {}
    virtual void Visit(BoundDelegateCall& boundDelegateCall) {}
    virtual void Visit(BoundClassDelegateCall& boundClassDelegateCall) {}
    virtual void Visit(BoundInterfaceCall& boundInterfaceCall) {}
    virtual void Visit(BoundConversion& boundConversion) {}
    virtual void Visit(BoundConstructExpression& boundConstructExpression) {}
    virtual void Visit(BoundConstructAndReturnTemporaryExpression& boundConstructAndReturnTemporaryExpression) {}
    virtual void Visit(BoundClassOrClassDelegateConversionResult& boundClassOrClassDelegateConversionResult) {}
    virtual void Visit(BoundIsExpression& boundIsExpression) {}
    virtual void Visit(BoundAsExpression& boundAsExpression) {}
    virtual void Visit(BoundTypeNameExpression& boundTypeNameExpression) {}
    virtual void Visit(BoundTypeIdExpression& boundTypeIdExpression) {}
    virtual void Visit(BoundBitCast& boundBitCast) {}
    virtual void Visit(BoundFunctionPtr& boundFunctionPtr) {}
    virtual void Visit(BoundDisjunction& boundDisjunction) {}
    virtual void Visit(BoundConjunction& boundConjunction) {}
    virtual void Visit(BoundAtomicConstraint& boundAtomicConstraint) {}
    virtual void Visit(BoundDisjunctiveConstraint& boundDisjunctiveConstraint) {}
    virtual void Visit(BoundConjunctiveConstraint& boundConjunctiveConstraint) {}
    virtual void Visit(BoundGlobalVariable& boundGlobalVariable) {}
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_BOUND_NODE_VISITOR_INCLUDED
