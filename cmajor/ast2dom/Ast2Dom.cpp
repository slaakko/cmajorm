// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ast2dom/Ast2Dom.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/Literal.hpp>
#include <sngcm/ast/CompileUnit.hpp>
#include <sngcm/ast/Namespace.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Template.hpp>
#include <sngcm/ast/Function.hpp>
#include <sngcm/ast/Class.hpp>
#include <sngcm/ast/Interface.hpp>
#include <sngcm/ast/Delegate.hpp>
#include <sngcm/ast/Typedef.hpp>
#include <sngcm/ast/Constant.hpp>
#include <sngcm/ast/Enumeration.hpp>
#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/GlobalVariable.hpp>
#include <sngxml/dom/Element.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Path.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace ast2dom {

using namespace soulng::unicode;
using namespace soulng::util;

class Ast2DomVisitor : public sngcm::ast::Visitor
{
public:
    Ast2DomVisitor();
    std::unique_ptr<sngxml::dom::Element> GetCompileUnitElement() { return std::move(compileUnitElement); }
    void Visit(BoolNode& boolNode) override;
    void Visit(SByteNode& sbyteNode) override;
    void Visit(ByteNode& byteNode) override;
    void Visit(ShortNode& shortNode) override;
    void Visit(UShortNode& ushortNode) override;
    void Visit(IntNode& intNode) override;
    void Visit(UIntNode& uintNode) override;
    void Visit(LongNode& longNode) override;
    void Visit(ULongNode& ulongNode) override;
    void Visit(FloatNode& floatNode) override;
    void Visit(DoubleNode& doubleNode) override;
    void Visit(CharNode& charNode) override;
    void Visit(WCharNode& wcharNode) override;
    void Visit(UCharNode& ucharNode) override;
    void Visit(VoidNode& voidNode) override;

    void Visit(BooleanLiteralNode& booleanLiteralNode) override;
    void Visit(SByteLiteralNode& sbyteLiteralNode) override;
    void Visit(ByteLiteralNode& byteLiteralNode) override;
    void Visit(ShortLiteralNode& shortLiteralNode) override;
    void Visit(UShortLiteralNode& ushortLiteralNode) override;
    void Visit(IntLiteralNode& intLiteralNode) override;
    void Visit(UIntLiteralNode& uintLiteralNode) override;
    void Visit(LongLiteralNode& longLiteralNode) override;
    void Visit(ULongLiteralNode& ulongLiteralNode) override;
    void Visit(FloatLiteralNode& floatLiteralNode) override;
    void Visit(DoubleLiteralNode& doubleLiteralNode) override;
    void Visit(CharLiteralNode& charLiteralNode) override;
    void Visit(WCharLiteralNode& wcharLiteralNode) override;
    void Visit(UCharLiteralNode& ucharLiteralNode) override;
    void Visit(StringLiteralNode& stringLiteralNode) override;
    void Visit(WStringLiteralNode& wstringLiteralNode) override;
    void Visit(UStringLiteralNode& ustringLiteralNode) override;
    void Visit(NullLiteralNode& nullLiteralNode) override;
    void Visit(ArrayLiteralNode& arrayLiteralNode) override;
    void Visit(StructuredLiteralNode& structuredLiteralNode) override;
    void Visit(UuidLiteralNode& uuidLiteralNode) override;

    void Visit(CompileUnitNode& compileUnitNode) override;
    void Visit(NamespaceNode& namespaceNode) override;
    void Visit(AliasNode& aliasNode) override;
    void Visit(NamespaceImportNode& namespaceImportNode) override;
    void Visit(IdentifierNode& identifierNode) override;
    void Visit(TemplateIdNode& templateIdNode) override;
    void Visit(FunctionNode& functionNode) override;
    void Visit(ClassNode& classNode) override;
    void Visit(ThisInitializerNode& thisInitializerNode) override;
    void Visit(BaseInitializerNode& baseInitializerNode) override;
    void Visit(MemberInitializerNode& memberInitializerNode) override;
    void Visit(StaticConstructorNode& staticConstructorNode) override;
    void Visit(ConstructorNode& constructorNode) override;
    void Visit(DestructorNode& destructorNode) override;
    void Visit(MemberFunctionNode& memberFunctionNode) override;
    void Visit(ConversionFunctionNode& conversionFunctionNode) override;
    void Visit(MemberVariableNode& memberVariableNode) override;
    void Visit(InterfaceNode& interfaceNode) override;
    void Visit(DelegateNode& delegateNode) override;
    void Visit(ClassDelegateNode& classDelegateNode) override;

    void Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode) override;
    void Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode) override;
    void Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode) override;
    void Visit(WhereConstraintNode& whereConstraintNode) override;
    void Visit(PredicateConstraintNode& predicateConstraintNode) override;
    void Visit(IsConstraintNode& isConstraintNode) override;
    void Visit(MultiParamConstraintNode& multiParamConstraintNode) override;
    void Visit(TypeNameConstraintNode& typeNameConstraintNode) override;
    void Visit(ConstructorConstraintNode& constructorConstraintNode) override;
    void Visit(DestructorConstraintNode& destructorConstraintNode) override;
    void Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode) override;
    void Visit(FunctionConstraintNode& functionConstraintNode) override;
    void Visit(AxiomStatementNode& axiomStatementNode) override;
    void Visit(AxiomNode& axiomNode) override;
    void Visit(ConceptIdNode& conceptIdNode) override;
    void Visit(ConceptNode& conceptNode) override;

    void Visit(SameConstraintNode& sameConstraintNode) override;
    void Visit(DerivedConstraintNode& derivedConstraintNode) override;
    void Visit(ConvertibleConstraintNode& convertibleConstraintNode) override;
    void Visit(ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode) override;
    void Visit(CommonConstraintNode& commonConstraintNode) override;
    void Visit(NonreferenceTypeConstraintNode& nonreferenceTypeConstraintNode) override;

    void Visit(LabelNode& labelNode) override;
    void Visit(CompoundStatementNode& compoundStatementNode) override;
    void Visit(LabeledStatementNode& labeledStatementNode) override;
    void Visit(ReturnStatementNode& returnStatementNode) override;
    void Visit(IfStatementNode& ifStatementNode) override;
    void Visit(WhileStatementNode& whileStatementNode) override;
    void Visit(DoStatementNode& doStatementNode) override;
    void Visit(ForStatementNode& forStatementNode) override;
    void Visit(BreakStatementNode& breakStatementNode) override;
    void Visit(ContinueStatementNode& continueStatementNode) override;
    void Visit(GotoStatementNode& gotoStatementNode) override;
    void Visit(ConstructionStatementNode& constructionStatementNode) override;
    void Visit(DeleteStatementNode& deleteStatementNode) override;
    void Visit(DestroyStatementNode& destroyStatementNode) override;
    void Visit(AssignmentStatementNode& assignmentStatementNode) override;
    void Visit(ExpressionStatementNode& expressionStatementNode) override;
    void Visit(EmptyStatementNode& emptyStatementNode) override;
    void Visit(RangeForStatementNode& rangeForStatementNode) override;
    void Visit(SwitchStatementNode& switchStatementNode) override;
    void Visit(CaseStatementNode& caseStatementNode) override;
    void Visit(DefaultStatementNode& defaultStatementNode) override;
    void Visit(GotoCaseStatementNode& gotoCaseStatementNode) override;
    void Visit(GotoDefaultStatementNode& gotoDefaultStatementNode) override;
    void Visit(ThrowStatementNode& throwStatementNode) override;
    void Visit(TryStatementNode& tryStatementNode) override;
    void Visit(CatchNode& catchNode) override;
    void Visit(AssertStatementNode& assertStatementNode) override;
    void Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode) override;
    void Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode) override;
    void Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode) override;
    void Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode) override;
    void Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode) override;
    void Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode) override;

    void Visit(TypedefNode& typedefNode) override;
    void Visit(ConstantNode& constantNode) override;
    void Visit(EnumTypeNode& enumTypeNode) override;
    void Visit(EnumConstantNode& enumConstantNode) override;
    void Visit(ParameterNode& parameterNode) override;
    void Visit(TemplateParameterNode& templateParameterNode) override;

    void Visit(ConstNode& constNode) override;
    void Visit(LValueRefNode& lvalueRefNode) override;
    void Visit(RValueRefNode& rvalueRefNode) override;
    void Visit(PointerNode& pointerNode) override;
    void Visit(ArrayNode& arrayNode) override;

    void Visit(DotNode& dotNode) override;
    void Visit(ArrowNode& arrowNode) override;
    void Visit(EquivalenceNode& equivalenceNode) override;
    void Visit(ImplicationNode& implicationNode) override;
    void Visit(DisjunctionNode& disjunctionNode) override;
    void Visit(ConjunctionNode& conjunctionNode) override;
    void Visit(BitOrNode& bitOrNode) override;
    void Visit(BitXorNode& bitXorNode) override;
    void Visit(BitAndNode& bitAndNode) override;
    void Visit(EqualNode& equalNode) override;
    void Visit(NotEqualNode& notEqualNode) override;
    void Visit(LessNode& lessNode) override;
    void Visit(GreaterNode& greaterNode) override;
    void Visit(LessOrEqualNode& lessOrEqualNode) override;
    void Visit(GreaterOrEqualNode& greaterOrEqualNode) override;
    void Visit(ShiftLeftNode& shiftLeftNode) override;
    void Visit(ShiftRightNode& shiftRightNode) override;
    void Visit(AddNode& addNode) override;
    void Visit(SubNode& subNode) override;
    void Visit(MulNode& mulNode) override;
    void Visit(DivNode& divNode) override;
    void Visit(RemNode& remNode) override;
    void Visit(NotNode& notNode) override;
    void Visit(UnaryPlusNode& unaryPlusNode) override;
    void Visit(UnaryMinusNode& unaryMinusNode) override;
    void Visit(PrefixIncrementNode& prefixIncrementNode) override;
    void Visit(PrefixDecrementNode& prefixDecrementNode) override;
    void Visit(DerefNode& derefNode) override;
    void Visit(AddrOfNode& addrOfNode) override;
    void Visit(ComplementNode& complementNode) override;
    void Visit(IsNode& isNode) override;
    void Visit(AsNode& asNode) override;
    void Visit(IndexingNode& indexingNode) override;
    void Visit(InvokeNode& invokeNode) override;
    void Visit(PostfixIncrementNode& postfixIncrementNode) override;
    void Visit(PostfixDecrementNode& postfixDecrementNode) override;
    void Visit(SizeOfNode& sizeOfNode) override;
    void Visit(TypeNameNode& typeNameNode) override;
    void Visit(TypeIdNode& typeIdNode) override;
    void Visit(CastNode& castNode) override;
    void Visit(ConstructNode& constructNode) override;
    void Visit(NewNode& newNode) override;
    void Visit(ThisNode& thisNode) override;
    void Visit(BaseNode& baseNode) override;
    void Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode) override;
    void Visit(GlobalVariableNode& globalVariableNode) override;
private:
    std::unique_ptr<sngxml::dom::Element> compileUnitElement;
    sngxml::dom::Element* currentElement;
    void CheckCurrentElement();
};

Ast2DomVisitor::Ast2DomVisitor() : currentElement(nullptr)
{
}

void Ast2DomVisitor::CheckCurrentElement()
{
    if (!currentElement)
    {
        throw std::runtime_error("current element not set");
    }
}

void Ast2DomVisitor::Visit(BoolNode& boolNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"BoolNode")));
}

void Ast2DomVisitor::Visit(SByteNode& sbyteNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"SByteNode")));
}

void Ast2DomVisitor::Visit(ByteNode& byteNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"ByteNode")));
}

void Ast2DomVisitor::Visit(ShortNode& shortNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"ShortNode")));
}

void Ast2DomVisitor::Visit(UShortNode& ushortNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"UShortNode")));
}

void Ast2DomVisitor::Visit(IntNode& intNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"IntNode")));
}

void Ast2DomVisitor::Visit(UIntNode& uintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"UIntNode")));
}

void Ast2DomVisitor::Visit(LongNode& longNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"LongNode")));
}

void Ast2DomVisitor::Visit(ULongNode& ulongNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"ULongNode")));
}

void Ast2DomVisitor::Visit(FloatNode& floatNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"FloatNode")));
}

void Ast2DomVisitor::Visit(DoubleNode& doubleNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"DoubleNode")));
}

void Ast2DomVisitor::Visit(CharNode& charNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"CharNode")));
}

void Ast2DomVisitor::Visit(WCharNode& wcharNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"WCharNode")));
}

void Ast2DomVisitor::Visit(UCharNode& ucharNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"UCharNode")));
}

void Ast2DomVisitor::Visit(VoidNode& voidNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"VoidNode")));
}

void Ast2DomVisitor::Visit(BooleanLiteralNode& booleanLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = booleanLiteralNode.Value() ? U"true" : U"false";
    std::unique_ptr<sngxml::dom::Element> booleanLiteralElement(new sngxml::dom::Element(U"BooleanLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = booleanLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(booleanLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(SByteLiteralNode& sbyteLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(sbyteLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> sbyteLiteralElement(new sngxml::dom::Element(U"SByteLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = sbyteLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(sbyteLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ByteLiteralNode& byteLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(byteLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> byteLiteralElement(new sngxml::dom::Element(U"ByteLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = byteLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(byteLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ShortLiteralNode& shortLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(shortLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> shortLiteralElement(new sngxml::dom::Element(U"ShortLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = shortLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(shortLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UShortLiteralNode& ushortLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(ushortLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> ushortLiteralElement(new sngxml::dom::Element(U"UShortLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = ushortLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(ushortLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IntLiteralNode& intLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(intLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> intLiteralElement(new sngxml::dom::Element(U"IntLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = intLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(intLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UIntLiteralNode& uintLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(uintLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> uintLiteralElement(new sngxml::dom::Element(U"UIntLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = uintLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(uintLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(LongLiteralNode& longLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(longLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> longLiteralElement(new sngxml::dom::Element(U"LongLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = longLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(longLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ULongLiteralNode& ulongLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(ulongLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> ulongLiteralElement(new sngxml::dom::Element(U"ULongLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = ulongLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(ulongLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(FloatLiteralNode& floatLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(floatLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> floatLiteralElement(new sngxml::dom::Element(U"FloatLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = floatLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(floatLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DoubleLiteralNode& doubleLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(doubleLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> doubleLiteralElement(new sngxml::dom::Element(U"DoubleLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = doubleLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(doubleLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(CharLiteralNode& charLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(CharStr(charLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> charLiteralElement(new sngxml::dom::Element(U"CharLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = charLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(charLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(WCharLiteralNode& wcharLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = CharStr(static_cast<char32_t>(wcharLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> wcharLiteralElement(new sngxml::dom::Element(U"WCharLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = wcharLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(wcharLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UCharLiteralNode& ucharLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = CharStr(ucharLiteralNode.Value());
    std::unique_ptr<sngxml::dom::Element> ucharLiteralElement(new sngxml::dom::Element(U"UCharLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = ucharLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(ucharLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(StringLiteralNode& stringLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(StringStr(stringLiteralNode.Value()));
    std::unique_ptr<sngxml::dom::Element> stringLiteralElement(new sngxml::dom::Element(U"StringLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = stringLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(stringLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(WStringLiteralNode& wstringLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(StringStr(ToUtf8(wstringLiteralNode.Value())));
    std::unique_ptr<sngxml::dom::Element> wstringLiteralElement(new sngxml::dom::Element(U"WStringLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = wstringLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(wstringLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UStringLiteralNode& ustringLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = StringStr(ustringLiteralNode.Value());
    std::unique_ptr<sngxml::dom::Element> ustringLiteralElement(new sngxml::dom::Element(U"UStringLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = ustringLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(ustringLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(NullLiteralNode& nullLiteralNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"NullLiteralNode")));
}

void Ast2DomVisitor::Visit(ArrayLiteralNode& arrayLiteralNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"ArrayLiteralNode")));
}

void Ast2DomVisitor::Visit(StructuredLiteralNode& structuredLiteralNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"StructuredLiteralNode")));
}

void Ast2DomVisitor::Visit(UuidLiteralNode& uuidLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(boost::uuids::to_string(uuidLiteralNode.GetUuid()));
    std::unique_ptr<sngxml::dom::Element> uuidLiteralElement(new sngxml::dom::Element(U"UuidLiteralNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = uuidLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(uuidLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(CompileUnitNode& compileUnitNode)
{
    compileUnitElement.reset(new sngxml::dom::Element(U"CompileUnitNode"));
    std::u32string value = ToUtf32(Path::GetFileName(compileUnitNode.FilePath()));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = compileUnitElement.get();
    currentElement->SetAttribute(U"info", value);
    compileUnitNode.GlobalNs()->Accept(*this);
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(NamespaceNode& namespaceNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> namespaceElement(new sngxml::dom::Element(U"NamespaceNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = namespaceElement.get();
    namespaceNode.Id()->Accept(*this);
    int n = namespaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* member = namespaceNode.Members()[i];
        member->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(namespaceElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AliasNode& aliasNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> aliasElement(new sngxml::dom::Element(U"AliasNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = aliasElement.get();
    aliasNode.Id()->Accept(*this);
    aliasNode.Qid()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(aliasElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(NamespaceImportNode& namespaceImportNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> namespaceImportElement(new sngxml::dom::Element(U"NamespaceImportNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = namespaceImportElement.get();
    namespaceImportNode.Ns()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(namespaceImportElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IdentifierNode& identifierNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> identifierElement(new sngxml::dom::Element(U"IdentifierNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = identifierElement.get();
    currentElement->SetAttribute(U"info", identifierNode.Str());
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(identifierElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TemplateIdNode& templateIdNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> templateIdElement(new sngxml::dom::Element(U"TemplateIdNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = templateIdElement.get();
    templateIdNode.Primary()->Accept(*this);
    int n = templateIdNode.TemplateArguments().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* arg = templateIdNode.TemplateArguments()[i];
        arg->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(templateIdElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(FunctionNode& functionNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> functionElement(new sngxml::dom::Element(U"FunctionNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = functionElement.get();
    currentElement->SetAttribute(U"info", functionNode.GroupId());
    int n = functionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameter = functionNode.Parameters()[i];
        parameter->Accept(*this);
    }
    functionNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ClassNode& classNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> classElement(new sngxml::dom::Element(U"ClassNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = classElement.get();
    classNode.Id()->Accept(*this);
    int n = classNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* member = classNode.Members()[i];
        member->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(classElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ThisInitializerNode& thisInitializerNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> thisInitializerElement(new sngxml::dom::Element(U"ThisInitializerNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = thisInitializerElement.get();
    int n = thisInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* argument = thisInitializerNode.Arguments()[i];
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(thisInitializerElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(BaseInitializerNode& baseInitializerNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> baseInitializerElement(new sngxml::dom::Element(U"BaseInitializerNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = baseInitializerElement.get();
    int n = baseInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* argument = baseInitializerNode.Arguments()[i];
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(baseInitializerElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MemberInitializerNode& memberInitializerNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> memberInitializerElement(new sngxml::dom::Element(U"MemberInitializerNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = memberInitializerElement.get();
    int n = memberInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* argument = memberInitializerNode.Arguments()[i];
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(memberInitializerElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(StaticConstructorNode& staticConstructorNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> staticConstructorElement(new sngxml::dom::Element(U"StaticConstructorNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = staticConstructorElement.get();
    int n = staticConstructorNode.Initializers().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::InitializerNode* initializerNode = staticConstructorNode.Initializers()[i];
        initializerNode->Accept(*this);
    }
    staticConstructorNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(staticConstructorElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstructorNode& constructorNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> constructorElement(new sngxml::dom::Element(U"ConstructorNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = constructorElement.get();
    int n = constructorNode.Initializers().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::InitializerNode* initializerNode = constructorNode.Initializers()[i];
        initializerNode->Accept(*this);
    }
    constructorNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constructorElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DestructorNode& destructorNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> destructorElement(new sngxml::dom::Element(U"DestructorNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = destructorElement.get();
    destructorNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(destructorElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MemberFunctionNode& memberFunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> memberFunctionElement(new sngxml::dom::Element(U"MemberFunctionNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = memberFunctionElement.get();
    currentElement->SetAttribute(U"info", memberFunctionNode.GroupId());
    int n = memberFunctionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameter = memberFunctionNode.Parameters()[i];
        parameter->Accept(*this);
    }
    memberFunctionNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(memberFunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConversionFunctionNode& conversionFunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> convertionFunctionElement(new sngxml::dom::Element(U"ConversionFunctionNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = convertionFunctionElement.get();
    currentElement->SetAttribute(U"info", conversionFunctionNode.GroupId());
    int n = conversionFunctionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameter = conversionFunctionNode.Parameters()[i];
        parameter->Accept(*this);
    }
    conversionFunctionNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(convertionFunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MemberVariableNode& memberVariableNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> memberVariableElement(new sngxml::dom::Element(U"MemberVariableNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = memberVariableElement.get();
    memberVariableNode.Id()->Accept(*this);
    memberVariableNode.TypeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(memberVariableElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(InterfaceNode& interfaceNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> interfaceElement(new sngxml::dom::Element(U"InterfaceNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = interfaceElement.get();
    interfaceNode.Id()->Accept(*this);
    int n = interfaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* member = interfaceNode.Members()[i];
        member->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(interfaceElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DelegateNode& delegateNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> delegateElement(new sngxml::dom::Element(U"DelegateNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = delegateElement.get();
    delegateNode.Id()->Accept(*this);
    int n = delegateNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::ParameterNode* paramter = delegateNode.Parameters()[i];
        paramter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(delegateElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ClassDelegateNode& classDelegateNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> classDelegateElement(new sngxml::dom::Element(U"ClassDelegateNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = classDelegateElement.get();
    classDelegateNode.Id()->Accept(*this);
    int n = classDelegateNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::ParameterNode* parameter = classDelegateNode.Parameters()[i];
        parameter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(classDelegateElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> parenthesizedConstraintElement(new sngxml::dom::Element(U"ParenthesizedConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = parenthesizedConstraintElement.get();
    parenthesizedConstraintNode.Constraint()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(parenthesizedConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> disjunctiveConstraintElement(new sngxml::dom::Element(U"DisjunctiveConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = disjunctiveConstraintElement.get();
    disjunctiveConstraintNode.Left()->Accept(*this);
    disjunctiveConstraintNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(disjunctiveConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conjunctiveConstraintElement(new sngxml::dom::Element(U"ConjunctiveConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conjunctiveConstraintElement.get();
    conjunctiveConstraintNode.Left()->Accept(*this);
    conjunctiveConstraintNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conjunctiveConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(WhereConstraintNode& whereConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> whereConstraintElement(new sngxml::dom::Element(U"WhereConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = whereConstraintElement.get();
    whereConstraintNode.Constraint()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(whereConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PredicateConstraintNode& predicateConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> predicateConstraintElement(new sngxml::dom::Element(U"PredicateConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = predicateConstraintElement.get();
    predicateConstraintNode.InvokeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(predicateConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IsConstraintNode& isConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> isConstraintElement(new sngxml::dom::Element(U"IsConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = isConstraintElement.get();
    isConstraintNode.TypeExpr()->Accept(*this);
    isConstraintNode.ConceptOrTypeName()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(isConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MultiParamConstraintNode& multiParamConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> multiParamConstraintElement(new sngxml::dom::Element(U"MultiParamConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = multiParamConstraintElement.get();
    multiParamConstraintNode.ConceptId()->Accept(*this);
    int n = multiParamConstraintNode.TypeExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* typeExpr = multiParamConstraintNode.TypeExprs()[i];
        typeExpr->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(multiParamConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TypeNameConstraintNode& typeNameConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> typeNameConstraintElement(new sngxml::dom::Element(U"TypeNameConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = typeNameConstraintElement.get();
    typeNameConstraintNode.TypeId()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeNameConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstructorConstraintNode& constructorConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> constructorConstraintElement(new sngxml::dom::Element(U"ConstructorConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = constructorConstraintElement.get();
    constructorConstraintNode.TypeParamId()->Accept(*this);
    int n = constructorConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::ParameterNode* parameter = constructorConstraintNode.Parameters()[i];
        parameter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constructorConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DestructorConstraintNode& destructorConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> destructorConstraintElement(new sngxml::dom::Element(U"DestructorConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = destructorConstraintElement.get();
    destructorConstraintNode.TypeParamId()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(destructorConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> memberFunctionConstraintElement(new sngxml::dom::Element(U"MemberFunctionConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = memberFunctionConstraintElement.get();
    currentElement->SetAttribute(U"info", memberFunctionConstraintNode.GroupId());
    memberFunctionConstraintNode.TypeParamId()->Accept(*this);
    int n = memberFunctionConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::ParameterNode* parameter = memberFunctionConstraintNode.Parameters()[i];
        parameter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(memberFunctionConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(FunctionConstraintNode& functionConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> functionConstraintElement(new sngxml::dom::Element(U"FunctionConstraintNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = functionConstraintElement.get();
    currentElement->SetAttribute(U"info", functionConstraintNode.GroupId());
    int n = functionConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::ParameterNode* parameter = functionConstraintNode.Parameters()[i];
        parameter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AxiomStatementNode& axiomStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> axiomStatementElement(new sngxml::dom::Element(U"AxiomStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = axiomStatementElement.get();
    axiomStatementNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(axiomStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AxiomNode& axiomNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> axiomElement(new sngxml::dom::Element(U"AxiomNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = axiomElement.get();
    axiomNode.Id()->Accept(*this);
    int n = axiomNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::ParameterNode* parameter = axiomNode.Parameters()[i];
        parameter->Accept(*this);
    }
    n = axiomNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        AxiomStatementNode* axiomStatementNode = axiomNode.Statements()[i];
        axiomStatementNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(axiomElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConceptIdNode& conceptIdNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conceptIdElement(new sngxml::dom::Element(U"ConceptIdNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conceptIdElement.get();
    conceptIdNode.Id()->Accept(*this);
    int n = conceptIdNode.TypeParameters().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* typeParameter = conceptIdNode.TypeParameters()[i];
        typeParameter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conceptIdElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConceptNode& conceptNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conceptElement(new sngxml::dom::Element(U"ConceptNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conceptElement.get();
    conceptNode.Id()->Accept(*this);
    int n = conceptNode.TypeParameters().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* typeParameter = conceptNode.TypeParameters()[i];
        typeParameter->Accept(*this);
    }
    n = conceptNode.Constraints().Count();
    for (int i = 0; i < n; ++i)
    {
        ConstraintNode* constraintNode = conceptNode.Constraints()[i];
        constraintNode->Accept(*this);
    }
    n = conceptNode.Axioms().Count();
    for (int i = 0; i < n; ++i)
    {
        AxiomNode* axiomNode = conceptNode.Axioms()[i];
        axiomNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conceptElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(SameConstraintNode& sameConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"SameConstraintNode")));
}

void Ast2DomVisitor::Visit(DerivedConstraintNode& derivedConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"DerivedConstraintNode")));
}

void Ast2DomVisitor::Visit(ConvertibleConstraintNode& convertibleConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"ConvertibleConstraintNode")));
}

void Ast2DomVisitor::Visit(ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"ExplicitlyConvertibleConstraintNode")));
}

void Ast2DomVisitor::Visit(CommonConstraintNode& commonConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"CommonConstraintNode")));
}

void Ast2DomVisitor::Visit(NonreferenceTypeConstraintNode& nonreferenceTypeConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"NonreferenceTypeConstraintNode")));
}

void Ast2DomVisitor::Visit(LabelNode& labelNode)
{
}

void Ast2DomVisitor::Visit(CompoundStatementNode& compoundStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> compoundStatementElement(new sngxml::dom::Element(U"CompoundStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = compoundStatementElement.get();
    int n = compoundStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = compoundStatementNode.Statements()[i];
        statementNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(compoundStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(LabeledStatementNode& labeledStatementNode)
{
    labeledStatementNode.Stmt()->Accept(*this);
}

void Ast2DomVisitor::Visit(ReturnStatementNode& returnStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> returnStatementElement(new sngxml::dom::Element(U"ReturnStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = returnStatementElement.get();
    if (returnStatementNode.Expression())
    {
        returnStatementNode.Expression()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(returnStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IfStatementNode& ifStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> ifStatementElement(new sngxml::dom::Element(U"IfStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = ifStatementElement.get();
    ifStatementNode.Condition()->Accept(*this);
    ifStatementNode.ThenS()->Accept(*this);
    if (ifStatementNode.ElseS())
    {
        ifStatementNode.ElseS()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(ifStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(WhileStatementNode& whileStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> whileStatementElement(new sngxml::dom::Element(U"WhileStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = whileStatementElement.get();
    whileStatementNode.Condition()->Accept(*this);
    whileStatementNode.Statement()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(whileStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DoStatementNode& doStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> doStatementElement(new sngxml::dom::Element(U"DoStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = doStatementElement.get();
    doStatementNode.Statement()->Accept(*this);
    doStatementNode.Condition()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(doStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ForStatementNode& forStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> forStatementElement(new sngxml::dom::Element(U"ForStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = forStatementElement.get();
    if (forStatementNode.InitS())
    {
        forStatementNode.InitS()->Accept(*this);
    }
    if (forStatementNode.Condition())
    {
        forStatementNode.Condition()->Accept(*this);
    }
    if (forStatementNode.LoopS())
    {
        forStatementNode.LoopS()->Accept(*this);
    }
    forStatementNode.ActionS()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(forStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(BreakStatementNode& breakStatementNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"BreakStatementNode")));
}

void Ast2DomVisitor::Visit(ContinueStatementNode& continueStatementNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"ContinueStatementNode")));
}

void Ast2DomVisitor::Visit(GotoStatementNode& gotoStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> gotoStatementElement(new sngxml::dom::Element(U"GotoStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = gotoStatementElement.get();
    currentElement->SetAttribute(U"info", gotoStatementNode.Target());
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(gotoStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstructionStatementNode& constructionStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> constructionStatementElement(new sngxml::dom::Element(U"ConstructionStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = constructionStatementElement.get();
    constructionStatementNode.Id()->Accept(*this);
    constructionStatementNode.TypeExpr()->Accept(*this);
    int n = constructionStatementNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* argument = constructionStatementNode.Arguments()[i];
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constructionStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DeleteStatementNode& deleteStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> deleteStatementElement(new sngxml::dom::Element(U"DeleteStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = deleteStatementElement.get();
    deleteStatementNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(deleteStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DestroyStatementNode& destroyStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> destroyStatementElement(new sngxml::dom::Element(U"DestroyStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = destroyStatementElement.get();
    destroyStatementNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(destroyStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AssignmentStatementNode& assignmentStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> assignmentStatementElement(new sngxml::dom::Element(U"AssignmentStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = assignmentStatementElement.get();
    assignmentStatementNode.TargetExpr()->Accept(*this);
    assignmentStatementNode.SourceExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(assignmentStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ExpressionStatementNode& expressionStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> expressionStatementElement(new sngxml::dom::Element(U"ExpressionStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = expressionStatementElement.get();
    expressionStatementNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(expressionStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(EmptyStatementNode& emptyStatementNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"EmptyStatementNode")));
}

void Ast2DomVisitor::Visit(RangeForStatementNode& rangeForStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> rangeForStatementElement(new sngxml::dom::Element(U"RangeForStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = rangeForStatementElement.get();
    rangeForStatementNode.Id()->Accept(*this);
    rangeForStatementNode.TypeExpr()->Accept(*this);
    rangeForStatementNode.Container()->Accept(*this);
    rangeForStatementNode.Action()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rangeForStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(SwitchStatementNode& switchStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> switchStatementElement(new sngxml::dom::Element(U"SwitchStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = switchStatementElement.get();
    switchStatementNode.Condition()->Accept(*this);
    int n = switchStatementNode.Cases().Count();
    for (int i = 0; i < n; ++i)
    {
        CaseStatementNode* caseStatementNode = switchStatementNode.Cases()[i];
        caseStatementNode->Accept(*this);
    }
    if (switchStatementNode.Default())
    {
        switchStatementNode.Default()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(switchStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(CaseStatementNode& caseStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> caseStatementElement(new sngxml::dom::Element(U"CaseStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = caseStatementElement.get();
    int n = caseStatementNode.CaseExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* caseExprNode = caseStatementNode.CaseExprs()[i];
        caseExprNode->Accept(*this);
    }
    n = caseStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = caseStatementNode.Statements()[i];
        statementNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(caseStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DefaultStatementNode& defaultStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> defaultStatementElement(new sngxml::dom::Element(U"DefaultStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = defaultStatementElement.get();
    int n = defaultStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = defaultStatementNode.Statements()[i];
        statementNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(defaultStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(GotoCaseStatementNode& gotoCaseStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> gotoCaseStatementElement(new sngxml::dom::Element(U"GotoCaseStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = gotoCaseStatementElement.get();
    gotoCaseStatementNode.CaseExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(gotoCaseStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(GotoDefaultStatementNode& gotoDefaultStatementNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"GotoDefaultStatementNode")));
}

void Ast2DomVisitor::Visit(ThrowStatementNode& throwStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> throwStatementElement(new sngxml::dom::Element(U"ThrowStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = throwStatementElement.get();
    if (throwStatementNode.Expression())
    {
        throwStatementNode.Expression()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(throwStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TryStatementNode& tryStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> tryStatementElement(new sngxml::dom::Element(U"TryStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = tryStatementElement.get();
    tryStatementNode.TryBlock()->Accept(*this);
    int n = tryStatementNode.Catches().Count();
    for (int i = 0; i < n; ++i)
    {
        CatchNode* catchNode = tryStatementNode.Catches()[i];
        catchNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tryStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(CatchNode& catchNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> catchElement(new sngxml::dom::Element(U"CatchNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = catchElement.get();
    catchNode.Id()->Accept(*this);
    catchNode.TypeExpr()->Accept(*this);
    catchNode.CatchBlock()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(catchElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AssertStatementNode& assertStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> assertStatementElement(new sngxml::dom::Element(U"AssertStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = assertStatementElement.get();
    assertStatementNode.AssertExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(assertStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conditionalCompilationPartElement(new sngxml::dom::Element(U"ConditionalCompilationPartNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conditionalCompilationPartElement.get();
    if (conditionalCompilationPartNode.Expr())
    {
        conditionalCompilationPartNode.Expr()->Accept(*this);
    }
    int n = conditionalCompilationPartNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = conditionalCompilationPartNode.Statements()[i];
        statementNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conditionalCompilationPartElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conditionalCompilationDisjunctionElement(new sngxml::dom::Element(U"ConditionalCompilationDisjunctionNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conditionalCompilationDisjunctionElement.get();
    conditionalCompilationDisjunctionNode.Left()->Accept(*this);
    conditionalCompilationDisjunctionNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conditionalCompilationDisjunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conditionalCompilationConjunctionElement(new sngxml::dom::Element(U"ConditionalCompilationConjunctionNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conditionalCompilationConjunctionElement.get();
    conditionalCompilationConjunctionNode.Left()->Accept(*this);
    conditionalCompilationConjunctionNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conditionalCompilationConjunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conditionalCompilationNotElement(new sngxml::dom::Element(U"ConditionalCompilationNotNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conditionalCompilationNotElement.get();
    conditionalCompilationNotNode.Expr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conditionalCompilationNotElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conditionalCompilationPrimaryElement(new sngxml::dom::Element(U"ConditionalCompilationPrimaryNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conditionalCompilationPrimaryElement.get();
    currentElement->SetAttribute(U"info", conditionalCompilationPrimaryNode.Symbol());
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conditionalCompilationPrimaryElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conditionalCompilationStatementElement(new sngxml::dom::Element(U"ConditionalCompilationStatementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conditionalCompilationStatementElement.get();
    conditionalCompilationStatementNode.IfPart()->Accept(*this);
    int n = conditionalCompilationStatementNode.ElifParts().Count();
    for (int i = 0; i < n; ++i)
    {
        ConditionalCompilationPartNode* elifPart = conditionalCompilationStatementNode.ElifParts()[i];
        elifPart->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conditionalCompilationStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TypedefNode& typedefNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> typedefElement(new sngxml::dom::Element(U"TypedefNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = typedefElement.get();
    typedefNode.Id()->Accept(*this);
    typedefNode.TypeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typedefElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstantNode& constantNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> constantElement(new sngxml::dom::Element(U"ConstantNode"));
    sngxml::dom::Element* prevElement = currentElement; 
    currentElement = constantElement.get();
    constantNode.Id()->Accept(*this);
    constantNode.TypeExpr()->Accept(*this);
    if (constantNode.Value())
    {
        constantNode.Value()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constantElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(EnumTypeNode& enumTypeNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> enumTypeElement(new sngxml::dom::Element(U"EnumTypeNode"));
    sngxml::dom::Element* prevElement = currentElement; 
    currentElement = enumTypeElement.get();
    enumTypeNode.Id()->Accept(*this);
    if (enumTypeNode.GetUnderlyingType())
    {
        enumTypeNode.GetUnderlyingType()->Accept(*this);
    }
    int n = enumTypeNode.Constants().Count();
    for (int i = 0; i < n; ++i)
    {
        EnumConstantNode* enumConstant = enumTypeNode.Constants()[i];
        enumConstant->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(enumTypeElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(EnumConstantNode& enumConstantNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> enumConstantElement(new sngxml::dom::Element(U"EnumConstantNode"));
    sngxml::dom::Element* prevElement = currentElement;
    enumConstantNode.Id()->Accept(*this);
    enumConstantNode.GetValue()->Accept(*this);
    currentElement = enumConstantElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(enumConstantElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ParameterNode& parameterNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> parameterElement(new sngxml::dom::Element(U"ParameterNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = parameterElement.get();
    if (parameterNode.Id())
    {
        parameterNode.Id()->Accept(*this);
    }
    parameterNode.TypeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(parameterElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TemplateParameterNode& templateParameterNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> templateParameterElement(new sngxml::dom::Element(U"TemplateParameterNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = templateParameterElement.get();
    templateParameterNode.Id()->Accept(*this);
    if (templateParameterNode.DefaultTemplateArgument())
    {
        templateParameterNode.DefaultTemplateArgument()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(templateParameterElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstNode& constNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> constElement(new sngxml::dom::Element(U"ConstNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = constElement.get();
    constNode.Subject()->Accept(*this); 
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(LValueRefNode& lvalueRefNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> lvalueRefElement(new sngxml::dom::Element(U"LValueRefNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = lvalueRefElement.get();
    lvalueRefNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(lvalueRefElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(RValueRefNode& rvalueRefNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> rvalueRefElement(new sngxml::dom::Element(U"RValueRefNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = rvalueRefElement.get();
    rvalueRefNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rvalueRefElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PointerNode& pointerNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> pointerElement(new sngxml::dom::Element(U"PointerNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = pointerElement.get();
    pointerNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(pointerElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ArrayNode& arrayNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> arrayElement(new sngxml::dom::Element(U"ArrayNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = arrayElement.get();
    arrayNode.Subject()->Accept(*this);
    if (arrayNode.Size())
    {
        arrayNode.Size()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(arrayElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DotNode& dotNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> dotElement(new sngxml::dom::Element(U"DotNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = dotElement.get();
    dotNode.MemberId()->Accept(*this);
    dotNode.Subject()->Accept(*this); 
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(dotElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ArrowNode& arrowNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> arrowElement(new sngxml::dom::Element(U"ArrowNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = arrowElement.get();
    arrowNode.MemberId()->Accept(*this);
    arrowNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(arrowElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(EquivalenceNode& equivalenceNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> equivalenceElement(new sngxml::dom::Element(U"EquivalenceNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = equivalenceElement.get();
    equivalenceNode.Left()->Accept(*this);
    equivalenceNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(equivalenceElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ImplicationNode& implicationNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> implicationElement(new sngxml::dom::Element(U"ImplicationNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = implicationElement.get();
    implicationNode.Left()->Accept(*this);
    implicationNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(implicationElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DisjunctionNode& disjunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> disjunctionElement(new sngxml::dom::Element(U"DisjunctionNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = disjunctionElement.get();
    disjunctionNode.Left()->Accept(*this);
    disjunctionNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(disjunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConjunctionNode& conjunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conjunctionElement(new sngxml::dom::Element(U"ConjunctionNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conjunctionElement.get();
    conjunctionNode.Left()->Accept(*this);
    conjunctionNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conjunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(BitOrNode& bitOrNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> bitOrElement(new sngxml::dom::Element(U"BitOrNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = bitOrElement.get();
    bitOrNode.Left()->Accept(*this);
    bitOrNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(bitOrElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(BitXorNode& bitXorNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> bitXorElement(new sngxml::dom::Element(U"BitXorNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = bitXorElement.get();
    bitXorNode.Left()->Accept(*this);
    bitXorNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(bitXorElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(BitAndNode& bitAndNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> bitAndElement(new sngxml::dom::Element(U"BitAndNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = bitAndElement.get();
    bitAndNode.Left()->Accept(*this);
    bitAndNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(bitAndElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(EqualNode& equalNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> equalElement(new sngxml::dom::Element(U"EqualNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = equalElement.get();
    equalNode.Left()->Accept(*this);
    equalNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(equalElement.release()));
    currentElement = prevElement;

}

void Ast2DomVisitor::Visit(NotEqualNode& notEqualNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> notEqualElement(new sngxml::dom::Element(U"NotEqualNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = notEqualElement.get();
    notEqualNode.Left()->Accept(*this);
    notEqualNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(notEqualElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(LessNode& lessNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> lessElement(new sngxml::dom::Element(U"LessNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = lessElement.get();
    lessNode.Left()->Accept(*this);
    lessNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(lessElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(GreaterNode& greaterNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> greaterElement(new sngxml::dom::Element(U"GreaterNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = greaterElement.get();
    greaterNode.Left()->Accept(*this);
    greaterNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(greaterElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(LessOrEqualNode& lessOrEqualNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> lessOrEqualElement(new sngxml::dom::Element(U"LessOrEqualNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = lessOrEqualElement.get();
    lessOrEqualNode.Left()->Accept(*this);
    lessOrEqualNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(lessOrEqualElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(GreaterOrEqualNode& greaterOrEqualNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> greaterOrEqualElement(new sngxml::dom::Element(U"GreaterOrEqualNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = greaterOrEqualElement.get();
    greaterOrEqualNode.Left()->Accept(*this);
    greaterOrEqualNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(greaterOrEqualElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ShiftLeftNode& shiftLeftNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> shiftLeftElement(new sngxml::dom::Element(U"ShiftLeftNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = shiftLeftElement.get();
    shiftLeftNode.Left()->Accept(*this);
    shiftLeftNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(shiftLeftElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ShiftRightNode& shiftRightNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> shiftRightElement(new sngxml::dom::Element(U"ShiftRightNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = shiftRightElement.get();
    shiftRightNode.Left()->Accept(*this);
    shiftRightNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(shiftRightElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AddNode& addNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> addElement(new sngxml::dom::Element(U"AddNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = addElement.get();
    addNode.Left()->Accept(*this);
    addNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(addElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(SubNode& subNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> subElement(new sngxml::dom::Element(U"SubNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = subElement.get();
    subNode.Left()->Accept(*this);
    subNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(subElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MulNode& mulNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> mulElement(new sngxml::dom::Element(U"MulNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = mulElement.get();
    mulNode.Left()->Accept(*this);
    mulNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(mulElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DivNode& divNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> divElement(new sngxml::dom::Element(U"DivNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = divElement.get();
    divNode.Left()->Accept(*this);
    divNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(divElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(RemNode& remNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> remElement(new sngxml::dom::Element(U"RemNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = remElement.get();
    remNode.Left()->Accept(*this);
    remNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(remElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(NotNode& notNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> notElement(new sngxml::dom::Element(U"NotNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = notElement.get();
    notNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(notElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UnaryPlusNode& unaryPlusNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> unaryPlusElement(new sngxml::dom::Element(U"UnaryPlusNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = unaryPlusElement.get();
    unaryPlusNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(unaryPlusElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UnaryMinusNode& unaryMinusNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> unaryMinusElement(new sngxml::dom::Element(U"UnaryMinusNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = unaryMinusElement.get();
    unaryMinusNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(unaryMinusElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PrefixIncrementNode& prefixIncrementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> prefixIncrementElement(new sngxml::dom::Element(U"PrefixIncrementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = prefixIncrementElement.get();
    prefixIncrementNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(prefixIncrementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PrefixDecrementNode& prefixDecrementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> prefixDecrementElement(new sngxml::dom::Element(U"PrefixDecrementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = prefixDecrementElement.get();
    prefixDecrementNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(prefixDecrementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DerefNode& derefNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> derefElement(new sngxml::dom::Element(U"DerefNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = derefElement.get();
    derefNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(derefElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AddrOfNode& addrOfNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> addrOfElement(new sngxml::dom::Element(U"AddrOfNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = addrOfElement.get();
    addrOfNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(addrOfElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ComplementNode& complementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> complementElement(new sngxml::dom::Element(U"ComplementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = complementElement.get();
    complementNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(complementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IsNode& isNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> isElement(new sngxml::dom::Element(U"IsNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = isElement.get();
    isNode.Expr()->Accept(*this);
    isNode.TargetTypeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(isElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AsNode& asNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> asElement(new sngxml::dom::Element(U"AsNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = asElement.get();
    asNode.Expr()->Accept(*this);
    asNode.TargetTypeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(asElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IndexingNode& indexingNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> indexingElement(new sngxml::dom::Element(U"IndexingNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = indexingElement.get();
    indexingNode.Subject()->Accept(*this);
    indexingNode.Index()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(indexingElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(InvokeNode& invokeNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> invokeElement(new sngxml::dom::Element(U"InvokeNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = invokeElement.get();
    invokeNode.Subject()->Accept(*this);
    int n = invokeNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* arg = invokeNode.Arguments()[i];
        arg->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(invokeElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PostfixIncrementNode& postfixIncrementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> postfixIncrementElement(new sngxml::dom::Element(U"PostfixIncrementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = postfixIncrementElement.get();
    postfixIncrementNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(postfixIncrementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PostfixDecrementNode& postfixDecrementNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> postfixDecrementElement(new sngxml::dom::Element(U"PostfixDecrementNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = postfixDecrementElement.get();
    postfixDecrementNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(postfixDecrementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(SizeOfNode& sizeOfNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> sizeOfElement(new sngxml::dom::Element(U"SizeOfNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = sizeOfElement.get();
    sizeOfNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(sizeOfElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TypeNameNode& typeNameNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> typeNameElement(new sngxml::dom::Element(U"TypeNameNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = typeNameElement.get();
    typeNameNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeNameElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TypeIdNode& typeIdNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> typeIdElement(new sngxml::dom::Element(U"TypeIdNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = typeIdElement.get();
    typeIdNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeIdElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(CastNode& castNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> castElement(new sngxml::dom::Element(U"CastNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = castElement.get();
    castNode.TargetTypeExpr()->Accept(*this);
    castNode.SourceExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(castElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstructNode& constructNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> constructElement(new sngxml::dom::Element(U"ConstructNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = constructElement.get();
    constructNode.TypeExpr()->Accept(*this);
    int n = constructNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* arg = constructNode.Arguments()[i];
        arg->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constructElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(NewNode& newNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> newElement(new sngxml::dom::Element(U"NewNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = newElement.get();
    newNode.TypeExpr()->Accept(*this);
    int n = newNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        sngcm::ast::Node* arg = newNode.Arguments()[i];
        arg->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(newElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ThisNode& thisNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"ThisNode")));
}

void Ast2DomVisitor::Visit(BaseNode& baseNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"BaseNode")));
}

void Ast2DomVisitor::Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> parenElement(new sngxml::dom::Element(U"ParenthesizedExpressionNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = parenElement.get();
    parenthesizedExpressionNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(parenElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(GlobalVariableNode& globalVariableNode)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> globalVariableElement(new sngxml::dom::Element(U"GlobalVariableNode"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = globalVariableElement.get();
    globalVariableNode.Id()->Accept(*this); 
    globalVariableNode.TypeExpr()->Accept(*this);
    if (globalVariableNode.Initializer())
    {
        globalVariableNode.Initializer()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(globalVariableElement.release()));
    currentElement = prevElement;
}

std::unique_ptr<sngxml::dom::Document> GenerateAstDocument(sngcm::ast::Node* node)
{
    std::unique_ptr<sngxml::dom::Document> doc(new sngxml::dom::Document());
    Ast2DomVisitor visitor;
    node->Accept(visitor);
    std::unique_ptr<sngxml::dom::Element> compileUnitElement = visitor.GetCompileUnitElement();
    doc->AppendChild(std::unique_ptr<sngxml::dom::Node>(compileUnitElement.release()));
    return doc;
}

} } // namespace cmajor::ast2dom
