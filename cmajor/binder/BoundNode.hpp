// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_BOUND_NODE_INCLUDED
#define CMAJOR_BINDER_BOUND_NODE_INCLUDED
#include <cmajor/binder/BinderApi.hpp>
#include <cmajor/ir/GenObject.hpp>
#include <cmajor/symbols/Module.hpp>

namespace cmajor { namespace binder {

using soulng::lexer::Span;
using namespace cmajor::ir;
using namespace cmajor::symbols;

enum class BoundNodeType : uint8_t
{
    boundCompileUnit, boundClass, boundFunction, 
    boundSequenceStatement, boundCompoundStatement, boundReturnStatement, boundIfStatement, boundWhileStatement, boundDoStatement, boundForStatement, 
    boundSwitchStatement, boundCaseStatement, boundDefaultStatement, boundGotoCaseStatement, boundGotoDefaultStatement, boundBreakStatement, boundContinueStatement,
    boundGotoStatement, boundConstructionStatement, boundAssignmentStatement, boundExpressionStatement, boundInitializationStatement, boundEmptyStatement,
    boundSetVmtPtrStatement, boundThrowStatement, boundRethrowStatement, boundTryStatement, boundCatchStatement,
    boundParameter, boundLocalVariable, boundMemberVariable, boundConstant, boundEnumConstant, boundLiteral, boundTemporary, boundBitCast, boundFunctionPtr, boundConstructAndReturnTemporary,
    boundClassOrClassDelegateConversionResult,
    boundFunctionCall, boundDelegateCall, boundClassDelegateCall, boundConversion, boundSizeOfExpression, boundAddressOfExpression, boundDereferenceExpression, boundReferenceToPointerExpression,
    boundConstructExpression, boundIsExpression, boundAsExpression, boundTypeNameExpression, boundTypeIdExpression, boundDisjunction, boundConjunction,
    boundAtomicConstraint, boundDisjunctiveConstraint, boundConjunctiveConstraint, boundConcept,
    boundTypeExpression, boundNamespaceExpression, boundFunctionGroupExpression, boundMemberExpression,
    boundNamespace, boundEnumTypeDefinition, boundGlobalVariable
};

class BoundNodeVisitor;

class BINDER_API BoundNode : public GenObject
{
public:
    BoundNode(const Span& span_, const boost::uuids::uuid& moduleId, BoundNodeType boundNodeType_);
    virtual void Accept(BoundNodeVisitor& visitor) = 0;
    const Span& GetSpan() const { return span; }
    const boost::uuids::uuid& ModuleId() const { return moduleId; }
    BoundNodeType GetBoundNodeType() const { return boundNodeType; }
private:
    Span span;
    boost::uuids::uuid moduleId;
    BoundNodeType boundNodeType;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_BOUND_NODE_INCLUDED
