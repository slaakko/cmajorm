// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMUPD_CONVERSION_VISITOR_INCLUDED
#define CMUPD_CONVERSION_VISITOR_INCLUDED
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/Node.hpp>
#include <memory>

using namespace sngcm::ast;

void InitMaps();

class ConversionVisitor : public sngcm::ast::Visitor
{
public:
    ConversionVisitor();
    bool Changed() const { return changed; }
    std::unique_ptr<CompileUnitNode> GetConvertedCompileUnit();
    void Visit(BoolNode& boolNode);
    void Visit(SByteNode& sbyteNode);
    void Visit(ByteNode& byteNode);
    void Visit(ShortNode& shortNode);
    void Visit(UShortNode& ushortNode);
    void Visit(IntNode& intNode);
    void Visit(UIntNode& uintNode);
    void Visit(LongNode& longNode);
    void Visit(ULongNode& ulongNode);
    void Visit(FloatNode& floatNode);
    void Visit(DoubleNode& doubleNode);
    void Visit(CharNode& charNode);
    void Visit(WCharNode& wcharNode);
    void Visit(UCharNode& ucharNode);
    void Visit(VoidNode& voidNode);

    void Visit(BooleanLiteralNode& booleanLiteralNode);
    void Visit(SByteLiteralNode& sbyteLiteralNode);
    void Visit(ByteLiteralNode& byteLiteralNode);
    void Visit(ShortLiteralNode& shortLiteralNode);
    void Visit(UShortLiteralNode& ushortLiteralNode);
    void Visit(IntLiteralNode& intLiteralNode);
    void Visit(UIntLiteralNode& uintLiteralNode);
    void Visit(LongLiteralNode& longLiteralNode);
    void Visit(ULongLiteralNode& ulongLiteralNode);
    void Visit(FloatLiteralNode& floatLiteralNode);
    void Visit(DoubleLiteralNode& doubleLiteralNode);
    void Visit(CharLiteralNode& charLiteralNode);
    void Visit(WCharLiteralNode& wcharLiteralNode);
    void Visit(UCharLiteralNode& ucharLiteralNode);
    void Visit(StringLiteralNode& stringLiteralNode);
    void Visit(WStringLiteralNode& wstringLiteralNode);
    void Visit(UStringLiteralNode& ustringLiteralNode);
    void Visit(NullLiteralNode& nullLiteralNode);
    void Visit(ArrayLiteralNode& arrayLiteralNode);
    void Visit(StructuredLiteralNode& structuredLiteralNode);
    void Visit(UuidLiteralNode& uuidLiteralNode);

    void Visit(CompileUnitNode& compileUnitNode);
    void Visit(NamespaceNode& namespaceNode);
    void Visit(AliasNode& aliasNode);
    void Visit(NamespaceImportNode& namespaceImportNode);
    void Visit(IdentifierNode& identifierNode);
    void Visit(CursorIdNode& cursorIdNode);
    void Visit(TemplateIdNode& templateIdNode);
    void Visit(FunctionNode& functionNode);
    void Visit(ClassNode& classNode);
    void Visit(ThisInitializerNode& thisInitializerNode);
    void Visit(BaseInitializerNode& baseInitializerNode);
    void Visit(MemberInitializerNode& memberInitializerNode);
    void Visit(StaticConstructorNode& staticConstructorNode);
    void Visit(ConstructorNode& constructorNode);
    void Visit(DestructorNode& destructorNode);
    void Visit(MemberFunctionNode& memberFunctionNode);
    void Visit(ConversionFunctionNode& conversionFunctionNode);
    void Visit(MemberVariableNode& memberVariableNode);
    void Visit(InterfaceNode& interfaceNode);
    void Visit(DelegateNode& delegateNode);
    void Visit(ClassDelegateNode& classDelegateNode);

    void Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode);
    void Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode);
    void Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode);
    void Visit(WhereConstraintNode& whereConstraintNode);
    void Visit(PredicateConstraintNode& predicateConstraintNode);
    void Visit(IsConstraintNode& isConstraintNode);
    void Visit(MultiParamConstraintNode& multiParamConstraintNode);
    void Visit(TypeNameConstraintNode& typeNameConstraintNode);
    void Visit(ConstructorConstraintNode& constructorConstraintNode);
    void Visit(DestructorConstraintNode& destructorConstraintNode);
    void Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode);
    void Visit(FunctionConstraintNode& functionConstraintNode);
    void Visit(AxiomStatementNode& axiomStatementNode);
    void Visit(AxiomNode& axiomNode);
    void Visit(ConceptIdNode& conceptIdNode);
    void Visit(ConceptNode& conceptNode);

    void Visit(LabelNode& labelNode);
    void Visit(LabeledStatementNode& labeledStatementNode);
    void Visit(CompoundStatementNode& compoundStatementNode);
    void Visit(ReturnStatementNode& returnStatementNode);
    void Visit(IfStatementNode& ifStatementNode);
    void Visit(WhileStatementNode& whileStatementNode);
    void Visit(DoStatementNode& doStatementNode);
    void Visit(ForStatementNode& forStatementNode);
    void Visit(BreakStatementNode& breakStatementNode);
    void Visit(ContinueStatementNode& continueStatementNode);
    void Visit(GotoStatementNode& gotoStatementNode);
    void Visit(ConstructionStatementNode& constructionStatementNode);
    void Visit(DeleteStatementNode& deleteStatementNode);
    void Visit(DestroyStatementNode& destroyStatementNode);
    void Visit(AssignmentStatementNode& assignmentStatementNode);
    void Visit(ExpressionStatementNode& expressionStatementNode);
    void Visit(EmptyStatementNode& emptyStatementNode);
    void Visit(RangeForStatementNode& rangeForStatementNode);
    void Visit(SwitchStatementNode& switchStatementNode);
    void Visit(CaseStatementNode& caseStatementNode);
    void Visit(DefaultStatementNode& defaultStatementNode);
    void Visit(GotoCaseStatementNode& gotoCaseStatementNode);
    void Visit(GotoDefaultStatementNode& gotoDefaultStatementNode);
    void Visit(ThrowStatementNode& throwStatementNode);
    void Visit(TryStatementNode& tryStatementNode);
    void Visit(CatchNode& catchNode);
    void Visit(AssertStatementNode& assertStatementNode);
    void Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode);
    void Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode);
    void Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode);
    void Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode);
    void Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode);
    void Visit(ParenthesizedConditionalCompilationExpressionNode& parenthesizeCondCompExprNode);
    void Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode);

    void Visit(TypedefNode& typedefNode);
    void Visit(ConstantNode& constantNode);
    void Visit(EnumTypeNode& enumTypeNode);
    void Visit(EnumConstantNode& enumConstantNode);
    void Visit(ParameterNode& parameterNode);
    void Visit(TemplateParameterNode& templateParameterNode);

    void Visit(ConstNode& constNode);
    void Visit(LValueRefNode& lvalueRefNode);
    void Visit(RValueRefNode& rvalueRefNode);
    void Visit(PointerNode& pointerNode);
    void Visit(ArrayNode& arrayNode);

    void Visit(DotNode& dotNode);
    void Visit(ArrowNode& arrowNode);
    void Visit(EquivalenceNode& equivalenceNode);
    void Visit(ImplicationNode& implicationNode);
    void Visit(DisjunctionNode& disjunctionNode);
    void Visit(ConjunctionNode& conjunctionNode);
    void Visit(BitOrNode& bitOrNode);
    void Visit(BitXorNode& bitXorNode);
    void Visit(BitAndNode& bitAndNode);
    void Visit(EqualNode& equalNode);
    void Visit(NotEqualNode& notEqualNode);
    void Visit(LessNode& lessNode);
    void Visit(GreaterNode& greaterNode);
    void Visit(LessOrEqualNode& lessOrEqualNode);
    void Visit(GreaterOrEqualNode& greaterOrEqualNode);
    void Visit(ShiftLeftNode& shiftLeftNode);
    void Visit(ShiftRightNode& shiftRightNode);
    void Visit(AddNode& addNode);
    void Visit(SubNode& subNode);
    void Visit(MulNode& mulNode);
    void Visit(DivNode& divNode);
    void Visit(RemNode& remNode);
    void Visit(NotNode& notNode);
    void Visit(UnaryPlusNode& unaryPlusNode);
    void Visit(UnaryMinusNode& unaryMinusNode);
    void Visit(PrefixIncrementNode& prefixIncrementNode);
    void Visit(PrefixDecrementNode& prefixDecrementNode);
    void Visit(DerefNode& derefNode);
    void Visit(AddrOfNode& addrOfNode);
    void Visit(ComplementNode& complementNode);
    void Visit(IsNode& isNode);
    void Visit(AsNode& asNode);
    void Visit(IndexingNode& indexingNode);
    void Visit(InvokeNode& invokeNode);
    void Visit(PostfixIncrementNode& postfixIncrementNode);
    void Visit(PostfixDecrementNode& postfixDecrementNode);
    void Visit(SizeOfNode& sizeOfNode);
    void Visit(TypeNameNode& typeNameNode);
    void Visit(TypeIdNode& typeIdNode);
    void Visit(CastNode& castNode);
    void Visit(ConstructNode& constructNode);
    void Visit(NewNode& newNode);
    void Visit(ThisNode& thisNode);
    void Visit(BaseNode& baseNode);
    void Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode);
    void Visit(GlobalVariableNode& globalVariableNode);
private:
    std::unique_ptr<Node> node;
    CloneContext ctx;
    bool changed;
    bool baseClassId;
    bool derivesFromXmlContentHandler;
};


#endif // CMUPD_CONVERSION_VISITOR_INCLUDED
