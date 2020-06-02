// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_VISITOR_INCLUDED
#define SNGCM_AST_VISITOR_INCLUDED
#include <sngcm/ast/AstApi.hpp>

namespace sngcm { namespace ast {

class BoolNode;
class SByteNode;
class ByteNode;
class ShortNode;
class UShortNode;
class IntNode;
class UIntNode;
class LongNode;
class ULongNode;
class FloatNode;
class DoubleNode;
class CharNode;
class WCharNode;
class UCharNode;
class VoidNode;

class BooleanLiteralNode;
class SByteLiteralNode;
class ByteLiteralNode;
class ShortLiteralNode;
class UShortLiteralNode;
class IntLiteralNode;
class UIntLiteralNode;
class LongLiteralNode;
class ULongLiteralNode;
class FloatLiteralNode;
class DoubleLiteralNode;
class CharLiteralNode;
class WCharLiteralNode;
class UCharLiteralNode;
class StringLiteralNode;
class WStringLiteralNode;
class UStringLiteralNode;
class NullLiteralNode;
class ArrayLiteralNode;
class StructuredLiteralNode;
class UuidLiteralNode;

class CompileUnitNode;
class NamespaceNode;
class AliasNode;
class NamespaceImportNode;
class IdentifierNode;
class CursorIdNode;
class TemplateIdNode;
class FunctionNode;
class FunctionPtrNode;
class ClassNode;
class ThisInitializerNode;
class BaseInitializerNode;
class MemberInitializerNode;
class StaticConstructorNode;
class ConstructorNode;
class DestructorNode;
class MemberFunctionNode;
class ConversionFunctionNode;
class MemberVariableNode;
class InterfaceNode;
class DelegateNode;
class ClassDelegateNode;

class ParenthesizedConstraintNode;
class DisjunctiveConstraintNode;
class ConjunctiveConstraintNode;
class WhereConstraintNode;
class PredicateConstraintNode;
class IsConstraintNode;
class MultiParamConstraintNode;
class TypeNameConstraintNode;
class ConstructorConstraintNode;
class DestructorConstraintNode;
class MemberFunctionConstraintNode;
class FunctionConstraintNode;
class AxiomStatementNode;
class AxiomNode;
class ConceptIdNode;
class ConceptNode;

class SameConstraintNode;
class DerivedConstraintNode;
class ConvertibleConstraintNode;
class ExplicitlyConvertibleConstraintNode;
class CommonConstraintNode;
class NonreferenceTypeConstraintNode;

class LabelNode;
class LabeledStatementNode;
class CompoundStatementNode;
class ReturnStatementNode;
class IfStatementNode;
class WhileStatementNode;
class DoStatementNode;
class ForStatementNode;
class BreakStatementNode;
class ContinueStatementNode;
class GotoStatementNode;
class ConstructionStatementNode;
class DeleteStatementNode;
class DestroyStatementNode;
class AssignmentStatementNode;
class ExpressionStatementNode;
class EmptyStatementNode;
class RangeForStatementNode;
class SwitchStatementNode;
class CaseStatementNode;
class DefaultStatementNode;
class GotoCaseStatementNode;
class GotoDefaultStatementNode;
class ThrowStatementNode;
class CatchNode;
class TryStatementNode;
class AssertStatementNode;
class ConditionalCompilationPartNode;
class ConditionalCompilationDisjunctionNode;
class ConditionalCompilationConjunctionNode;
class ConditionalCompilationNotNode;
class ConditionalCompilationPrimaryNode;
class ParenthesizedConditionalCompilationExpressionNode;
class ConditionalCompilationStatementNode;

class ConstantNode;
class EnumTypeNode;
class EnumConstantNode;
class ParameterNode;
class TemplateParameterNode;
class TypedefNode;

class ConstNode;
class LValueRefNode;
class RValueRefNode;
class PointerNode;
class ArrayNode;

class DotNode;
class ArrowNode;
class EquivalenceNode;
class ImplicationNode;
class DisjunctionNode;
class ConjunctionNode;
class BitOrNode;
class BitXorNode;
class BitAndNode;
class EqualNode;
class NotEqualNode;
class LessNode;
class GreaterNode;
class LessOrEqualNode;
class GreaterOrEqualNode;
class ShiftLeftNode;
class ShiftRightNode;
class AddNode;
class SubNode;
class MulNode;
class DivNode;
class RemNode;
class NotNode;
class UnaryPlusNode;
class UnaryMinusNode;
class PrefixIncrementNode;
class PrefixDecrementNode;
class DerefNode;
class AddrOfNode;
class ComplementNode;
class IsNode;
class AsNode;
class IndexingNode;
class InvokeNode;
class PostfixIncrementNode;
class PostfixDecrementNode;
class SizeOfNode;
class TypeNameNode;
class TypeIdNode;
class CastNode;
class ConstructNode;
class NewNode;
class ThisNode;
class BaseNode;
class ParenthesizedExpressionNode;
class GlobalVariableNode;
class Attribute;
class Attributes;
class CommentNode;

class SNGCM_AST_API Visitor
{
public:
    virtual ~Visitor();
    virtual void Visit(BoolNode& boolNode) {}
    virtual void Visit(SByteNode& sbyteNode) {}
    virtual void Visit(ByteNode& byteNode) {}
    virtual void Visit(ShortNode& shortNode) {}
    virtual void Visit(UShortNode& ushortNode) {}
    virtual void Visit(IntNode& intNode) {}
    virtual void Visit(UIntNode& uintNode) {}
    virtual void Visit(LongNode& longNode) {}
    virtual void Visit(ULongNode& ulongNode) {}
    virtual void Visit(FloatNode& floatNode) {}
    virtual void Visit(DoubleNode& doubleNode) {}
    virtual void Visit(CharNode& charNode) {}
    virtual void Visit(WCharNode& wcharNode) {}
    virtual void Visit(UCharNode& ucharNode) {}
    virtual void Visit(VoidNode& voidNode) {}

    virtual void Visit(BooleanLiteralNode& booleanLiteralNode) {}
    virtual void Visit(SByteLiteralNode& sbyteLiteralNode) {}
    virtual void Visit(ByteLiteralNode& byteLiteralNode) {}
    virtual void Visit(ShortLiteralNode& shortLiteralNode) {}
    virtual void Visit(UShortLiteralNode& ushortLiteralNode) {}
    virtual void Visit(IntLiteralNode& intLiteralNode) {}
    virtual void Visit(UIntLiteralNode& uintLiteralNode) {}
    virtual void Visit(LongLiteralNode& longLiteralNode) {}
    virtual void Visit(ULongLiteralNode& ulongLiteralNode) {}
    virtual void Visit(FloatLiteralNode& floatLiteralNode) {}
    virtual void Visit(DoubleLiteralNode& doubleLiteralNode) {}
    virtual void Visit(CharLiteralNode& charLiteralNode) {}
    virtual void Visit(WCharLiteralNode& wcharLiteralNode) {}
    virtual void Visit(UCharLiteralNode& ucharLiteralNode) {}
    virtual void Visit(StringLiteralNode& stringLiteralNode) {}
    virtual void Visit(WStringLiteralNode& wstringLiteralNode) {}
    virtual void Visit(UStringLiteralNode& ustringLiteralNode) {}
    virtual void Visit(NullLiteralNode& nullLiteralNode) {}
    virtual void Visit(ArrayLiteralNode& arrayLiteralNode) {}
    virtual void Visit(StructuredLiteralNode& structuredLiteralNode) {}
    virtual void Visit(UuidLiteralNode& uuidLiteralNode) {}

    virtual void Visit(CompileUnitNode& compileUnitNode) {}
    virtual void Visit(NamespaceNode& namespaceNode) {}
    virtual void Visit(AliasNode& aliasNode) {}
    virtual void Visit(NamespaceImportNode& namespaceImportNode) {}
    virtual void Visit(IdentifierNode& identifierNode) {}
    virtual void Visit(CursorIdNode& cursorIdNode) {}
    virtual void Visit(TemplateIdNode& templateIdNode) {}
    virtual void Visit(FunctionNode& functionNode) {}
    virtual void Visit(FunctionPtrNode& functionPtrNode) {}
    virtual void Visit(ClassNode& classNode) {}
    virtual void Visit(ThisInitializerNode& thisInitializerNode) {}
    virtual void Visit(BaseInitializerNode& baseInitializerNode) {}
    virtual void Visit(MemberInitializerNode& memberInitializerNode) {}
    virtual void Visit(StaticConstructorNode& staticConstructorNode) {}
    virtual void Visit(ConstructorNode& constructorNode) {}
    virtual void Visit(DestructorNode& destructorNode) {}
    virtual void Visit(MemberFunctionNode& memberFunctionNode) {}
    virtual void Visit(ConversionFunctionNode& conversionFunctionNode) {}
    virtual void Visit(MemberVariableNode& memberVariableNode) {}
    virtual void Visit(InterfaceNode& interfaceNode) {}
    virtual void Visit(DelegateNode& delegateNode) {}
    virtual void Visit(ClassDelegateNode& classDelegateNode) {}

    virtual void Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode) {}
    virtual void Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode) {}
    virtual void Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode) {}
    virtual void Visit(WhereConstraintNode& whereConstraintNode) {}
    virtual void Visit(PredicateConstraintNode& predicateConstraintNode) {}
    virtual void Visit(IsConstraintNode& isConstraintNode) {}
    virtual void Visit(MultiParamConstraintNode& multiParamConstraintNode) {}
    virtual void Visit(TypeNameConstraintNode& typeNameConstraintNode) {}
    virtual void Visit(ConstructorConstraintNode& constructorConstraintNode) {}
    virtual void Visit(DestructorConstraintNode& destructorConstraintNode) {}
    virtual void Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode) {}
    virtual void Visit(FunctionConstraintNode& functionConstraintNode) {}
    virtual void Visit(AxiomStatementNode& axiomStatementNode) {}
    virtual void Visit(AxiomNode& axiomNode) {}
    virtual void Visit(ConceptIdNode& conceptIdNode) {}
    virtual void Visit(ConceptNode& conceptNode) {}

    virtual void Visit(SameConstraintNode& sameConstraintNode) {}
    virtual void Visit(DerivedConstraintNode& derivedConstraintNode) {}
    virtual void Visit(ConvertibleConstraintNode& convertibleConstraintNode) {}
    virtual void Visit(ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode) {}
    virtual void Visit(CommonConstraintNode& commonConstraintNode) {}
    virtual void Visit(NonreferenceTypeConstraintNode& nonreferenceTypeConstraintNode) {}

    virtual void Visit(LabelNode& labelNode) {}
    virtual void Visit(LabeledStatementNode& labeledStatementNode) {}
    virtual void Visit(CompoundStatementNode& compoundStatementNode) {}
    virtual void Visit(ReturnStatementNode& returnStatementNode) {}
    virtual void Visit(IfStatementNode& ifStatementNode) {}
    virtual void Visit(WhileStatementNode& whileStatementNode) {}
    virtual void Visit(DoStatementNode& doStatementNode) {}
    virtual void Visit(ForStatementNode& forStatementNode) {}
    virtual void Visit(BreakStatementNode& breakStatementNode) {}
    virtual void Visit(ContinueStatementNode& continueStatementNode) {}
    virtual void Visit(GotoStatementNode& gotoStatementNode) {}
    virtual void Visit(ConstructionStatementNode& constructionStatementNode) {}
    virtual void Visit(DeleteStatementNode& deleteStatementNode) {}
    virtual void Visit(DestroyStatementNode& destroyStatementNode) {}
    virtual void Visit(AssignmentStatementNode& assignmentStatementNode) {}
    virtual void Visit(ExpressionStatementNode& expressionStatementNode) {}
    virtual void Visit(EmptyStatementNode& emptyStatementNode) {}
    virtual void Visit(RangeForStatementNode& rangeForStatementNode) {}
    virtual void Visit(SwitchStatementNode& switchStatementNode) {}
    virtual void Visit(CaseStatementNode& caseStatementNode) {}
    virtual void Visit(DefaultStatementNode& defaultStatementNode) {}
    virtual void Visit(GotoCaseStatementNode& gotoCaseStatementNode) {}
    virtual void Visit(GotoDefaultStatementNode& gotoDefaultStatementNode) {}
    virtual void Visit(ThrowStatementNode& throwStatementNode) {}
    virtual void Visit(TryStatementNode& tryStatementNode) {}
    virtual void Visit(CatchNode& catchNode) {}
    virtual void Visit(AssertStatementNode& assertStatementNode) {}
    virtual void Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode) {}
    virtual void Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode) {}
    virtual void Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode) {}
    virtual void Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode) {}
    virtual void Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode) {}
    virtual void Visit(ParenthesizedConditionalCompilationExpressionNode& parenthesizeCondCompExprNode) {}
    virtual void Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode) {}

    virtual void Visit(TypedefNode& typedefNode) {}
    virtual void Visit(ConstantNode& constantNode) {}
    virtual void Visit(EnumTypeNode& enumTypeNode) {}
    virtual void Visit(EnumConstantNode& enumConstantNode) {}
    virtual void Visit(ParameterNode& parameterNode) {}
    virtual void Visit(TemplateParameterNode& templateParameterNode) {}

    virtual void Visit(ConstNode& constNode) {}
    virtual void Visit(LValueRefNode& lvalueRefNode) {}
    virtual void Visit(RValueRefNode& rvalueRefNode) {}
    virtual void Visit(PointerNode& pointerNode) {}
    virtual void Visit(ArrayNode& arrayNode) {}

    virtual void Visit(DotNode& dotNode) {}
    virtual void Visit(ArrowNode& arrowNode) {}
    virtual void Visit(EquivalenceNode& equivalenceNode) {}
    virtual void Visit(ImplicationNode& implicationNode) {}
    virtual void Visit(DisjunctionNode& disjunctionNode) {}
    virtual void Visit(ConjunctionNode& conjunctionNode) {}
    virtual void Visit(BitOrNode& bitOrNode) {}
    virtual void Visit(BitXorNode& bitXorNode) {}
    virtual void Visit(BitAndNode& bitAndNode) {}
    virtual void Visit(EqualNode& equalNode) {}
    virtual void Visit(NotEqualNode& notEqualNode) {}
    virtual void Visit(LessNode& lessNode) {}
    virtual void Visit(GreaterNode& greaterNode) {}
    virtual void Visit(LessOrEqualNode& lessOrEqualNode) {}
    virtual void Visit(GreaterOrEqualNode& greaterOrEqualNode) {}
    virtual void Visit(ShiftLeftNode& shiftLeftNode) {}
    virtual void Visit(ShiftRightNode& shiftRightNode) {}
    virtual void Visit(AddNode& addNode) {}
    virtual void Visit(SubNode& subNode) {}
    virtual void Visit(MulNode& mulNode) {}
    virtual void Visit(DivNode& divNode) {}
    virtual void Visit(RemNode& remNode) {}
    virtual void Visit(NotNode& notNode) {}
    virtual void Visit(UnaryPlusNode& unaryPlusNode) {}
    virtual void Visit(UnaryMinusNode& unaryMinusNode) {}
    virtual void Visit(PrefixIncrementNode& prefixIncrementNode) {}
    virtual void Visit(PrefixDecrementNode& prefixDecrementNode) {}
    virtual void Visit(DerefNode& derefNode) {}
    virtual void Visit(AddrOfNode& addrOfNode) {}
    virtual void Visit(ComplementNode& complementNode) {}
    virtual void Visit(IsNode& isNode) {}
    virtual void Visit(AsNode& asNode) {}
    virtual void Visit(IndexingNode& indexingNode) {}
    virtual void Visit(InvokeNode& invokeNode) {}
    virtual void Visit(PostfixIncrementNode& postfixIncrementNode) {}
    virtual void Visit(PostfixDecrementNode& postfixDecrementNode) {}
    virtual void Visit(SizeOfNode& sizeOfNode) {}
    virtual void Visit(TypeNameNode& typeNameNode) {}
    virtual void Visit(TypeIdNode& typeIdNode) {}
    virtual void Visit(CastNode& castNode) {}
    virtual void Visit(ConstructNode& constructNode) {}
    virtual void Visit(NewNode& newNode) {}
    virtual void Visit(ThisNode& thisNode) {}
    virtual void Visit(BaseNode& baseNode) {}
    virtual void Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode) {}
    virtual void Visit(GlobalVariableNode& globalVariableNode) {}

    virtual void Visit(Attribute& attribute) {}
    virtual void Visit(Attributes& attributes) {}

    virtual void Visit(CommentNode& commentNode) {}
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_VISITOR_INCLUDED
