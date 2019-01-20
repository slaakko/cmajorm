// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ast2dom/Ast2Dom.hpp>
#include <cmajor/ast/Visitor.hpp>
#include <cmajor/ast/Literal.hpp>
#include <cmajor/ast/CompileUnit.hpp>
#include <cmajor/ast/Namespace.hpp>
#include <cmajor/ast/Identifier.hpp>
#include <cmajor/ast/Template.hpp>
#include <cmajor/ast/Function.hpp>
#include <cmajor/ast/Class.hpp>
#include <cmajor/ast/Interface.hpp>
#include <cmajor/ast/Delegate.hpp>
#include <cmajor/ast/Typedef.hpp>
#include <cmajor/ast/Constant.hpp>
#include <cmajor/ast/Enumeration.hpp>
#include <cmajor/ast/TypeExpr.hpp>
#include <cmajor/ast/Expression.hpp>
#include <cmajor/dom/Element.hpp>
#include <cmajor/dom/CharacterData.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <cmajor/util/Path.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace ast2dom {

using namespace cmajor::unicode;
using namespace cmajor::util;

class Ast2DomVisitor : public cmajor::ast::Visitor
{
public:
    Ast2DomVisitor();
    std::unique_ptr<dom::Element> GetCompileUnitElement() { return std::move(compileUnitElement); }
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
    void Visit(CastNode& castNode) override;
    void Visit(ConstructNode& constructNode) override;
    void Visit(NewNode& newNode) override;
    void Visit(ThisNode& thisNode) override;
    void Visit(BaseNode& baseNode) override;
    void Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode) override;
private:
    std::unique_ptr<dom::Element> compileUnitElement;
    dom::Element* currentElement;
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
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"BoolNode")));
}

void Ast2DomVisitor::Visit(SByteNode& sbyteNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"SByteNode")));
}

void Ast2DomVisitor::Visit(ByteNode& byteNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"ByteNode")));
}

void Ast2DomVisitor::Visit(ShortNode& shortNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"ShortNode")));
}

void Ast2DomVisitor::Visit(UShortNode& ushortNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"UShortNode")));
}

void Ast2DomVisitor::Visit(IntNode& intNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"IntNode")));
}

void Ast2DomVisitor::Visit(UIntNode& uintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"UIntNode")));
}

void Ast2DomVisitor::Visit(LongNode& longNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"LongNode")));
}

void Ast2DomVisitor::Visit(ULongNode& ulongNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"ULongNode")));
}

void Ast2DomVisitor::Visit(FloatNode& floatNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"FloatNode")));
}

void Ast2DomVisitor::Visit(DoubleNode& doubleNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"DoubleNode")));
}

void Ast2DomVisitor::Visit(CharNode& charNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"CharNode")));
}

void Ast2DomVisitor::Visit(WCharNode& wcharNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"WCharNode")));
}

void Ast2DomVisitor::Visit(UCharNode& ucharNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"UCharNode")));
}

void Ast2DomVisitor::Visit(VoidNode& voidNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"VoidNode")));
}

void Ast2DomVisitor::Visit(BooleanLiteralNode& booleanLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = booleanLiteralNode.Value() ? U"true" : U"false";
    std::unique_ptr<dom::Element> booleanLiteralElement(new dom::Element(U"BooleanLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = booleanLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(booleanLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(SByteLiteralNode& sbyteLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(sbyteLiteralNode.Value()));
    std::unique_ptr<dom::Element> sbyteLiteralElement(new dom::Element(U"SByteLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = sbyteLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(sbyteLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ByteLiteralNode& byteLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(byteLiteralNode.Value()));
    std::unique_ptr<dom::Element> byteLiteralElement(new dom::Element(U"ByteLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = byteLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(byteLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ShortLiteralNode& shortLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(shortLiteralNode.Value()));
    std::unique_ptr<dom::Element> shortLiteralElement(new dom::Element(U"ShortLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = shortLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(shortLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UShortLiteralNode& ushortLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(ushortLiteralNode.Value()));
    std::unique_ptr<dom::Element> ushortLiteralElement(new dom::Element(U"UShortLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = ushortLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(ushortLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IntLiteralNode& intLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(intLiteralNode.Value()));
    std::unique_ptr<dom::Element> intLiteralElement(new dom::Element(U"IntLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = intLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(intLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UIntLiteralNode& uintLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(uintLiteralNode.Value()));
    std::unique_ptr<dom::Element> uintLiteralElement(new dom::Element(U"UIntLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = uintLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(uintLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(LongLiteralNode& longLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(longLiteralNode.Value()));
    std::unique_ptr<dom::Element> longLiteralElement(new dom::Element(U"LongLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = longLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(longLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ULongLiteralNode& ulongLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(ulongLiteralNode.Value()));
    std::unique_ptr<dom::Element> ulongLiteralElement(new dom::Element(U"ULongLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = ulongLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(ulongLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(FloatLiteralNode& floatLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(floatLiteralNode.Value()));
    std::unique_ptr<dom::Element> floatLiteralElement(new dom::Element(U"FloatLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = floatLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(floatLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DoubleLiteralNode& doubleLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(std::to_string(doubleLiteralNode.Value()));
    std::unique_ptr<dom::Element> doubleLiteralElement(new dom::Element(U"DoubleLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = doubleLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(doubleLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(CharLiteralNode& charLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(CharStr(charLiteralNode.Value()));
    std::unique_ptr<dom::Element> charLiteralElement(new dom::Element(U"CharLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = charLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(charLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(WCharLiteralNode& wcharLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = CharStr(static_cast<char32_t>(wcharLiteralNode.Value()));
    std::unique_ptr<dom::Element> wcharLiteralElement(new dom::Element(U"WCharLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = wcharLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(wcharLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UCharLiteralNode& ucharLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = CharStr(ucharLiteralNode.Value());
    std::unique_ptr<dom::Element> ucharLiteralElement(new dom::Element(U"UCharLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = ucharLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(ucharLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(StringLiteralNode& stringLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(StringStr(stringLiteralNode.Value()));
    std::unique_ptr<dom::Element> stringLiteralElement(new dom::Element(U"StringLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = stringLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(stringLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(WStringLiteralNode& wstringLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(StringStr(ToUtf8(wstringLiteralNode.Value())));
    std::unique_ptr<dom::Element> wstringLiteralElement(new dom::Element(U"WStringLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = wstringLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(wstringLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UStringLiteralNode& ustringLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = StringStr(ustringLiteralNode.Value());
    std::unique_ptr<dom::Element> ustringLiteralElement(new dom::Element(U"UStringLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = ustringLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(ustringLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(NullLiteralNode& nullLiteralNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"NullLiteralNode")));
}

void Ast2DomVisitor::Visit(ArrayLiteralNode& arrayLiteralNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"ArrayLiteralNode")));
}

void Ast2DomVisitor::Visit(StructuredLiteralNode& structuredLiteralNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"StructuredLiteralNode")));
}

void Ast2DomVisitor::Visit(UuidLiteralNode& uuidLiteralNode)
{
    CheckCurrentElement();
    std::u32string value = ToUtf32(boost::uuids::to_string(uuidLiteralNode.Uuid()));
    std::unique_ptr<dom::Element> uuidLiteralElement(new dom::Element(U"UuidLiteralNode"));
    dom::Element* prevElement = currentElement;
    currentElement = uuidLiteralElement.get();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(uuidLiteralElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(CompileUnitNode& compileUnitNode)
{
    compileUnitElement.reset(new dom::Element(U"CompileUnitNode"));
    std::u32string value = ToUtf32(Path::GetFileName(compileUnitNode.FilePath()));
    dom::Element* prevElement = currentElement;
    currentElement = compileUnitElement.get();
    currentElement->SetAttribute(U"info", value);
    compileUnitNode.GlobalNs()->Accept(*this);
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(NamespaceNode& namespaceNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> namespaceElement(new dom::Element(U"NamespaceNode"));
    dom::Element* prevElement = currentElement;
    currentElement = namespaceElement.get();
    namespaceNode.Id()->Accept(*this);
    int n = namespaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* member = namespaceNode.Members()[i];
        member->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(namespaceElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AliasNode& aliasNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> aliasElement(new dom::Element(U"AliasNode"));
    dom::Element* prevElement = currentElement;
    currentElement = aliasElement.get();
    aliasNode.Id()->Accept(*this);
    aliasNode.Qid()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(aliasElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(NamespaceImportNode& namespaceImportNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> namespaceImportElement(new dom::Element(U"NamespaceImportNode"));
    dom::Element* prevElement = currentElement;
    currentElement = namespaceImportElement.get();
    namespaceImportNode.Ns()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(namespaceImportElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IdentifierNode& identifierNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> identifierElement(new dom::Element(U"IdentifierNode"));
    dom::Element* prevElement = currentElement;
    currentElement = identifierElement.get();
    currentElement->SetAttribute(U"info", identifierNode.Str());
    prevElement->AppendChild(std::unique_ptr<dom::Node>(identifierElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TemplateIdNode& templateIdNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> templateIdElement(new dom::Element(U"TemplateIdNode"));
    dom::Element* prevElement = currentElement;
    currentElement = templateIdElement.get();
    templateIdNode.Primary()->Accept(*this);
    int n = templateIdNode.TemplateArguments().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* arg = templateIdNode.TemplateArguments()[i];
        arg->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(templateIdElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(FunctionNode& functionNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> functionElement(new dom::Element(U"FunctionNode"));
    dom::Element* prevElement = currentElement;
    currentElement = functionElement.get();
    currentElement->SetAttribute(U"info", functionNode.GroupId());
    int n = functionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameter = functionNode.Parameters()[i];
        parameter->Accept(*this);
    }
    functionNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(functionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ClassNode& classNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> classElement(new dom::Element(U"ClassNode"));
    dom::Element* prevElement = currentElement;
    currentElement = classElement.get();
    classNode.Id()->Accept(*this);
    int n = classNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* member = classNode.Members()[i];
        member->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(classElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ThisInitializerNode& thisInitializerNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> thisInitializerElement(new dom::Element(U"ThisInitializerNode"));
    dom::Element* prevElement = currentElement;
    currentElement = thisInitializerElement.get();
    int n = thisInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* argument = thisInitializerNode.Arguments()[i];
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(thisInitializerElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(BaseInitializerNode& baseInitializerNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> baseInitializerElement(new dom::Element(U"BaseInitializerNode"));
    dom::Element* prevElement = currentElement;
    currentElement = baseInitializerElement.get();
    int n = baseInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* argument = baseInitializerNode.Arguments()[i];
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(baseInitializerElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MemberInitializerNode& memberInitializerNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> memberInitializerElement(new dom::Element(U"MemberInitializerNode"));
    dom::Element* prevElement = currentElement;
    currentElement = memberInitializerElement.get();
    int n = memberInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* argument = memberInitializerNode.Arguments()[i];
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(memberInitializerElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(StaticConstructorNode& staticConstructorNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> staticConstructorElement(new dom::Element(U"StaticConstructorNode"));
    dom::Element* prevElement = currentElement;
    currentElement = staticConstructorElement.get();
    int n = staticConstructorNode.Initializers().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::InitializerNode* initializerNode = staticConstructorNode.Initializers()[i];
        initializerNode->Accept(*this);
    }
    staticConstructorNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(staticConstructorElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstructorNode& constructorNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> constructorElement(new dom::Element(U"ConstructorNode"));
    dom::Element* prevElement = currentElement;
    currentElement = constructorElement.get();
    int n = constructorNode.Initializers().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::InitializerNode* initializerNode = constructorNode.Initializers()[i];
        initializerNode->Accept(*this);
    }
    constructorNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(constructorElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DestructorNode& destructorNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> destructorElement(new dom::Element(U"DestructorNode"));
    dom::Element* prevElement = currentElement;
    currentElement = destructorElement.get();
    destructorNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(destructorElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MemberFunctionNode& memberFunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> memberFunctionElement(new dom::Element(U"MemberFunctionNode"));
    dom::Element* prevElement = currentElement;
    currentElement = memberFunctionElement.get();
    currentElement->SetAttribute(U"info", memberFunctionNode.GroupId());
    int n = memberFunctionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameter = memberFunctionNode.Parameters()[i];
        parameter->Accept(*this);
    }
    memberFunctionNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(memberFunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConversionFunctionNode& conversionFunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> convertionFunctionElement(new dom::Element(U"ConversionFunctionNode"));
    dom::Element* prevElement = currentElement;
    currentElement = convertionFunctionElement.get();
    currentElement->SetAttribute(U"info", conversionFunctionNode.GroupId());
    int n = conversionFunctionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        ParameterNode* parameter = conversionFunctionNode.Parameters()[i];
        parameter->Accept(*this);
    }
    conversionFunctionNode.Body()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(convertionFunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MemberVariableNode& memberVariableNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> memberVariableElement(new dom::Element(U"MemberVariableNode"));
    dom::Element* prevElement = currentElement;
    currentElement = memberVariableElement.get();
    memberVariableNode.Id()->Accept(*this);
    memberVariableNode.TypeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(memberVariableElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(InterfaceNode& interfaceNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> interfaceElement(new dom::Element(U"InterfaceNode"));
    dom::Element* prevElement = currentElement;
    currentElement = interfaceElement.get();
    interfaceNode.Id()->Accept(*this);
    int n = interfaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* member = interfaceNode.Members()[i];
        member->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(interfaceElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DelegateNode& delegateNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> delegateElement(new dom::Element(U"DelegateNode"));
    dom::Element* prevElement = currentElement;
    currentElement = delegateElement.get();
    delegateNode.Id()->Accept(*this);
    int n = delegateNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::ParameterNode* paramter = delegateNode.Parameters()[i];
        paramter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(delegateElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ClassDelegateNode& classDelegateNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> classDelegateElement(new dom::Element(U"ClassDelegateNode"));
    dom::Element* prevElement = currentElement;
    currentElement = classDelegateElement.get();
    classDelegateNode.Id()->Accept(*this);
    int n = classDelegateNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::ParameterNode* parameter = classDelegateNode.Parameters()[i];
        parameter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(classDelegateElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> parenthesizedConstraintElement(new dom::Element(U"ParenthesizedConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = parenthesizedConstraintElement.get();
    parenthesizedConstraintNode.Constraint()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(parenthesizedConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> disjunctiveConstraintElement(new dom::Element(U"DisjunctiveConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = disjunctiveConstraintElement.get();
    disjunctiveConstraintNode.Left()->Accept(*this);
    disjunctiveConstraintNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(disjunctiveConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conjunctiveConstraintElement(new dom::Element(U"ConjunctiveConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = conjunctiveConstraintElement.get();
    conjunctiveConstraintNode.Left()->Accept(*this);
    conjunctiveConstraintNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conjunctiveConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(WhereConstraintNode& whereConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> whereConstraintElement(new dom::Element(U"WhereConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = whereConstraintElement.get();
    whereConstraintNode.Constraint()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(whereConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PredicateConstraintNode& predicateConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> predicateConstraintElement(new dom::Element(U"PredicateConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = predicateConstraintElement.get();
    predicateConstraintNode.InvokeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(predicateConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IsConstraintNode& isConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> isConstraintElement(new dom::Element(U"IsConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = isConstraintElement.get();
    isConstraintNode.TypeExpr()->Accept(*this);
    isConstraintNode.ConceptOrTypeName()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(isConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MultiParamConstraintNode& multiParamConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> multiParamConstraintElement(new dom::Element(U"MultiParamConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = multiParamConstraintElement.get();
    multiParamConstraintNode.ConceptId()->Accept(*this);
    int n = multiParamConstraintNode.TypeExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* typeExpr = multiParamConstraintNode.TypeExprs()[i];
        typeExpr->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(multiParamConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TypeNameConstraintNode& typeNameConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> typeNameConstraintElement(new dom::Element(U"TypeNameConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = typeNameConstraintElement.get();
    typeNameConstraintNode.TypeId()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(typeNameConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstructorConstraintNode& constructorConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> constructorConstraintElement(new dom::Element(U"ConstructorConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = constructorConstraintElement.get();
    constructorConstraintNode.TypeParamId()->Accept(*this);
    int n = constructorConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::ParameterNode* parameter = constructorConstraintNode.Parameters()[i];
        parameter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(constructorConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DestructorConstraintNode& destructorConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> destructorConstraintElement(new dom::Element(U"DestructorConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = destructorConstraintElement.get();
    destructorConstraintNode.TypeParamId()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(destructorConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> memberFunctionConstraintElement(new dom::Element(U"MemberFunctionConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = memberFunctionConstraintElement.get();
    currentElement->SetAttribute(U"info", memberFunctionConstraintNode.GroupId());
    memberFunctionConstraintNode.TypeParamId()->Accept(*this);
    int n = memberFunctionConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::ParameterNode* parameter = memberFunctionConstraintNode.Parameters()[i];
        parameter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(memberFunctionConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(FunctionConstraintNode& functionConstraintNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> functionConstraintElement(new dom::Element(U"FunctionConstraintNode"));
    dom::Element* prevElement = currentElement;
    currentElement = functionConstraintElement.get();
    currentElement->SetAttribute(U"info", functionConstraintNode.GroupId());
    int n = functionConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::ParameterNode* parameter = functionConstraintNode.Parameters()[i];
        parameter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(functionConstraintElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AxiomStatementNode& axiomStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> axiomStatementElement(new dom::Element(U"AxiomStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = axiomStatementElement.get();
    axiomStatementNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(axiomStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AxiomNode& axiomNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> axiomElement(new dom::Element(U"AxiomNode"));
    dom::Element* prevElement = currentElement;
    currentElement = axiomElement.get();
    axiomNode.Id()->Accept(*this);
    int n = axiomNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::ParameterNode* parameter = axiomNode.Parameters()[i];
        parameter->Accept(*this);
    }
    n = axiomNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        AxiomStatementNode* axiomStatementNode = axiomNode.Statements()[i];
        axiomStatementNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(axiomElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConceptIdNode& conceptIdNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conceptIdElement(new dom::Element(U"ConceptIdNode"));
    dom::Element* prevElement = currentElement;
    currentElement = conceptIdElement.get();
    conceptIdNode.Id()->Accept(*this);
    int n = conceptIdNode.TypeParameters().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* typeParameter = conceptIdNode.TypeParameters()[i];
        typeParameter->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conceptIdElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConceptNode& conceptNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conceptElement(new dom::Element(U"ConceptNode"));
    dom::Element* prevElement = currentElement;
    currentElement = conceptElement.get();
    conceptNode.Id()->Accept(*this);
    int n = conceptNode.TypeParameters().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* typeParameter = conceptNode.TypeParameters()[i];
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
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conceptElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(SameConstraintNode& sameConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"SameConstraintNode")));
}

void Ast2DomVisitor::Visit(DerivedConstraintNode& derivedConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"DerivedConstraintNode")));
}

void Ast2DomVisitor::Visit(ConvertibleConstraintNode& convertibleConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"ConvertibleConstraintNode")));
}

void Ast2DomVisitor::Visit(ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"ExplicitlyConvertibleConstraintNode")));
}

void Ast2DomVisitor::Visit(CommonConstraintNode& commonConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"CommonConstraintNode")));
}

void Ast2DomVisitor::Visit(NonreferenceTypeConstraintNode& nonreferenceTypeConstraintNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"NonreferenceTypeConstraintNode")));
}

void Ast2DomVisitor::Visit(LabelNode& labelNode)
{
}

void Ast2DomVisitor::Visit(CompoundStatementNode& compoundStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> compoundStatementElement(new dom::Element(U"CompoundStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = compoundStatementElement.get();
    int n = compoundStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = compoundStatementNode.Statements()[i];
        statementNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(compoundStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ReturnStatementNode& returnStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> returnStatementElement(new dom::Element(U"ReturnStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = returnStatementElement.get();
    if (returnStatementNode.Expression())
    {
        returnStatementNode.Expression()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(returnStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IfStatementNode& ifStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> ifStatementElement(new dom::Element(U"IfStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = ifStatementElement.get();
    ifStatementNode.Condition()->Accept(*this);
    ifStatementNode.ThenS()->Accept(*this);
    if (ifStatementNode.ElseS())
    {
        ifStatementNode.ElseS()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(ifStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(WhileStatementNode& whileStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> whileStatementElement(new dom::Element(U"WhileStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = whileStatementElement.get();
    whileStatementNode.Condition()->Accept(*this);
    whileStatementNode.Statement()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(whileStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DoStatementNode& doStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> doStatementElement(new dom::Element(U"DoStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = doStatementElement.get();
    doStatementNode.Statement()->Accept(*this);
    doStatementNode.Condition()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(doStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ForStatementNode& forStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> forStatementElement(new dom::Element(U"ForStatementNode"));
    dom::Element* prevElement = currentElement;
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
    prevElement->AppendChild(std::unique_ptr<dom::Node>(forStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(BreakStatementNode& breakStatementNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"BreakStatementNode")));
}

void Ast2DomVisitor::Visit(ContinueStatementNode& continueStatementNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"ContinueStatementNode")));
}

void Ast2DomVisitor::Visit(GotoStatementNode& gotoStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> gotoStatementElement(new dom::Element(U"GotoStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = gotoStatementElement.get();
    currentElement->SetAttribute(U"info", gotoStatementNode.Target());
    prevElement->AppendChild(std::unique_ptr<dom::Node>(gotoStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstructionStatementNode& constructionStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> constructionStatementElement(new dom::Element(U"ConstructionStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = constructionStatementElement.get();
    constructionStatementNode.Id()->Accept(*this);
    constructionStatementNode.TypeExpr()->Accept(*this);
    int n = constructionStatementNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* argument = constructionStatementNode.Arguments()[i];
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(constructionStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DeleteStatementNode& deleteStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> deleteStatementElement(new dom::Element(U"DeleteStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = deleteStatementElement.get();
    deleteStatementNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(deleteStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DestroyStatementNode& destroyStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> destroyStatementElement(new dom::Element(U"DestroyStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = destroyStatementElement.get();
    destroyStatementNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(destroyStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AssignmentStatementNode& assignmentStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> assignmentStatementElement(new dom::Element(U"AssignmentStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = assignmentStatementElement.get();
    assignmentStatementNode.TargetExpr()->Accept(*this);
    assignmentStatementNode.SourceExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(assignmentStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ExpressionStatementNode& expressionStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> expressionStatementElement(new dom::Element(U"ExpressionStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = expressionStatementElement.get();
    expressionStatementNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(expressionStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(EmptyStatementNode& emptyStatementNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"EmptyStatementNode")));
}

void Ast2DomVisitor::Visit(RangeForStatementNode& rangeForStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> rangeForStatementElement(new dom::Element(U"RangeForStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = rangeForStatementElement.get();
    rangeForStatementNode.Id()->Accept(*this);
    rangeForStatementNode.TypeExpr()->Accept(*this);
    rangeForStatementNode.Container()->Accept(*this);
    rangeForStatementNode.Action()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(rangeForStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(SwitchStatementNode& switchStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> switchStatementElement(new dom::Element(U"SwitchStatementNode"));
    dom::Element* prevElement = currentElement;
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
    prevElement->AppendChild(std::unique_ptr<dom::Node>(switchStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(CaseStatementNode& caseStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> caseStatementElement(new dom::Element(U"CaseStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = caseStatementElement.get();
    int n = caseStatementNode.CaseExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* caseExprNode = caseStatementNode.CaseExprs()[i];
        caseExprNode->Accept(*this);
    }
    n = caseStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = caseStatementNode.Statements()[i];
        statementNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(caseStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DefaultStatementNode& defaultStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> defaultStatementElement(new dom::Element(U"DefaultStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = defaultStatementElement.get();
    int n = defaultStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statementNode = defaultStatementNode.Statements()[i];
        statementNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(defaultStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(GotoCaseStatementNode& gotoCaseStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> gotoCaseStatementElement(new dom::Element(U"GotoCaseStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = gotoCaseStatementElement.get();
    gotoCaseStatementNode.CaseExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(gotoCaseStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(GotoDefaultStatementNode& gotoDefaultStatementNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"GotoDefaultStatementNode")));
}

void Ast2DomVisitor::Visit(ThrowStatementNode& throwStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> throwStatementElement(new dom::Element(U"ThrowStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = throwStatementElement.get();
    if (throwStatementNode.Expression())
    {
        throwStatementNode.Expression()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(throwStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TryStatementNode& tryStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> tryStatementElement(new dom::Element(U"TryStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = tryStatementElement.get();
    tryStatementNode.TryBlock()->Accept(*this);
    int n = tryStatementNode.Catches().Count();
    for (int i = 0; i < n; ++i)
    {
        CatchNode* catchNode = tryStatementNode.Catches()[i];
        catchNode->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(tryStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(CatchNode& catchNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> catchElement(new dom::Element(U"CatchNode"));
    dom::Element* prevElement = currentElement;
    currentElement = catchElement.get();
    catchNode.Id()->Accept(*this);
    catchNode.TypeExpr()->Accept(*this);
    catchNode.CatchBlock()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(catchElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AssertStatementNode& assertStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> assertStatementElement(new dom::Element(U"AssertStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = assertStatementElement.get();
    assertStatementNode.AssertExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(assertStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conditionalCompilationPartElement(new dom::Element(U"ConditionalCompilationPartNode"));
    dom::Element* prevElement = currentElement;
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
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conditionalCompilationPartElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conditionalCompilationDisjunctionElement(new dom::Element(U"ConditionalCompilationDisjunctionNode"));
    dom::Element* prevElement = currentElement;
    currentElement = conditionalCompilationDisjunctionElement.get();
    conditionalCompilationDisjunctionNode.Left()->Accept(*this);
    conditionalCompilationDisjunctionNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conditionalCompilationDisjunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conditionalCompilationConjunctionElement(new dom::Element(U"ConditionalCompilationConjunctionNode"));
    dom::Element* prevElement = currentElement;
    currentElement = conditionalCompilationConjunctionElement.get();
    conditionalCompilationConjunctionNode.Left()->Accept(*this);
    conditionalCompilationConjunctionNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conditionalCompilationConjunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conditionalCompilationNotElement(new dom::Element(U"ConditionalCompilationNotNode"));
    dom::Element* prevElement = currentElement;
    currentElement = conditionalCompilationNotElement.get();
    conditionalCompilationNotNode.Expr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conditionalCompilationNotElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conditionalCompilationPrimaryElement(new dom::Element(U"ConditionalCompilationPrimaryNode"));
    dom::Element* prevElement = currentElement;
    currentElement = conditionalCompilationPrimaryElement.get();
    currentElement->SetAttribute(U"info", conditionalCompilationPrimaryNode.Symbol());
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conditionalCompilationPrimaryElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conditionalCompilationStatementElement(new dom::Element(U"ConditionalCompilationStatementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = conditionalCompilationStatementElement.get();
    conditionalCompilationStatementNode.IfPart()->Accept(*this);
    int n = conditionalCompilationStatementNode.ElifParts().Count();
    for (int i = 0; i < n; ++i)
    {
        ConditionalCompilationPartNode* elifPart = conditionalCompilationStatementNode.ElifParts()[i];
        elifPart->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conditionalCompilationStatementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TypedefNode& typedefNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> typedefElement(new dom::Element(U"TypedefNode"));
    dom::Element* prevElement = currentElement;
    currentElement = typedefElement.get();
    typedefNode.Id()->Accept(*this);
    typedefNode.TypeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(typedefElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstantNode& constantNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> constantElement(new dom::Element(U"ConstantNode"));
    dom::Element* prevElement = currentElement; 
    currentElement = constantElement.get();
    constantNode.Id()->Accept(*this);
    constantNode.TypeExpr()->Accept(*this);
    if (constantNode.Value())
    {
        constantNode.Value()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(constantElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(EnumTypeNode& enumTypeNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> enumTypeElement(new dom::Element(U"EnumTypeNode"));
    dom::Element* prevElement = currentElement; 
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
    prevElement->AppendChild(std::unique_ptr<dom::Node>(enumTypeElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(EnumConstantNode& enumConstantNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> enumConstantElement(new dom::Element(U"EnumConstantNode"));
    dom::Element* prevElement = currentElement;
    enumConstantNode.Id()->Accept(*this);
    enumConstantNode.GetValue()->Accept(*this);
    currentElement = enumConstantElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(enumConstantElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ParameterNode& parameterNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> parameterElement(new dom::Element(U"ParameterNode"));
    dom::Element* prevElement = currentElement;
    currentElement = parameterElement.get();
    if (parameterNode.Id())
    {
        parameterNode.Id()->Accept(*this);
    }
    parameterNode.TypeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(parameterElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TemplateParameterNode& templateParameterNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> templateParameterElement(new dom::Element(U"TemplateParameterNode"));
    dom::Element* prevElement = currentElement;
    currentElement = templateParameterElement.get();
    templateParameterNode.Id()->Accept(*this);
    if (templateParameterNode.DefaultTemplateArgument())
    {
        templateParameterNode.DefaultTemplateArgument()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(templateParameterElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstNode& constNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> constElement(new dom::Element(U"ConstNode"));
    dom::Element* prevElement = currentElement;
    currentElement = constElement.get();
    constNode.Subject()->Accept(*this); 
    prevElement->AppendChild(std::unique_ptr<dom::Node>(constElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(LValueRefNode& lvalueRefNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> lvalueRefElement(new dom::Element(U"LValueRefNode"));
    dom::Element* prevElement = currentElement;
    currentElement = lvalueRefElement.get();
    lvalueRefNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(lvalueRefElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(RValueRefNode& rvalueRefNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> rvalueRefElement(new dom::Element(U"RValueRefNode"));
    dom::Element* prevElement = currentElement;
    currentElement = rvalueRefElement.get();
    rvalueRefNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(rvalueRefElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PointerNode& pointerNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> pointerElement(new dom::Element(U"PointerNode"));
    dom::Element* prevElement = currentElement;
    currentElement = pointerElement.get();
    pointerNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(pointerElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ArrayNode& arrayNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> arrayElement(new dom::Element(U"ArrayNode"));
    dom::Element* prevElement = currentElement;
    currentElement = arrayElement.get();
    arrayNode.Subject()->Accept(*this);
    if (arrayNode.Size())
    {
        arrayNode.Size()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(arrayElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DotNode& dotNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> dotElement(new dom::Element(U"DotNode"));
    dom::Element* prevElement = currentElement;
    currentElement = dotElement.get();
    dotNode.MemberId()->Accept(*this);
    dotNode.Subject()->Accept(*this); 
    prevElement->AppendChild(std::unique_ptr<dom::Node>(dotElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ArrowNode& arrowNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> arrowElement(new dom::Element(U"ArrowNode"));
    dom::Element* prevElement = currentElement;
    currentElement = arrowElement.get();
    arrowNode.MemberId()->Accept(*this);
    arrowNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(arrowElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(EquivalenceNode& equivalenceNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> equivalenceElement(new dom::Element(U"EquivalenceNode"));
    dom::Element* prevElement = currentElement;
    currentElement = equivalenceElement.get();
    equivalenceNode.Left()->Accept(*this);
    equivalenceNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(equivalenceElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ImplicationNode& implicationNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> implicationElement(new dom::Element(U"ImplicationNode"));
    dom::Element* prevElement = currentElement;
    currentElement = implicationElement.get();
    implicationNode.Left()->Accept(*this);
    implicationNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(implicationElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DisjunctionNode& disjunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> disjunctionElement(new dom::Element(U"DisjunctionNode"));
    dom::Element* prevElement = currentElement;
    currentElement = disjunctionElement.get();
    disjunctionNode.Left()->Accept(*this);
    disjunctionNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(disjunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConjunctionNode& conjunctionNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conjunctionElement(new dom::Element(U"ConjunctionNode"));
    dom::Element* prevElement = currentElement;
    currentElement = conjunctionElement.get();
    conjunctionNode.Left()->Accept(*this);
    conjunctionNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conjunctionElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(BitOrNode& bitOrNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> bitOrElement(new dom::Element(U"BitOrNode"));
    dom::Element* prevElement = currentElement;
    currentElement = bitOrElement.get();
    bitOrNode.Left()->Accept(*this);
    bitOrNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(bitOrElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(BitXorNode& bitXorNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> bitXorElement(new dom::Element(U"BitXorNode"));
    dom::Element* prevElement = currentElement;
    currentElement = bitXorElement.get();
    bitXorNode.Left()->Accept(*this);
    bitXorNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(bitXorElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(BitAndNode& bitAndNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> bitAndElement(new dom::Element(U"BitAndNode"));
    dom::Element* prevElement = currentElement;
    currentElement = bitAndElement.get();
    bitAndNode.Left()->Accept(*this);
    bitAndNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(bitAndElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(EqualNode& equalNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> equalElement(new dom::Element(U"EqualNode"));
    dom::Element* prevElement = currentElement;
    currentElement = equalElement.get();
    equalNode.Left()->Accept(*this);
    equalNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(equalElement.release()));
    currentElement = prevElement;

}

void Ast2DomVisitor::Visit(NotEqualNode& notEqualNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> notEqualElement(new dom::Element(U"NotEqualNode"));
    dom::Element* prevElement = currentElement;
    currentElement = notEqualElement.get();
    notEqualNode.Left()->Accept(*this);
    notEqualNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(notEqualElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(LessNode& lessNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> lessElement(new dom::Element(U"LessNode"));
    dom::Element* prevElement = currentElement;
    currentElement = lessElement.get();
    lessNode.Left()->Accept(*this);
    lessNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(lessElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(GreaterNode& greaterNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> greaterElement(new dom::Element(U"GreaterNode"));
    dom::Element* prevElement = currentElement;
    currentElement = greaterElement.get();
    greaterNode.Left()->Accept(*this);
    greaterNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(greaterElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(LessOrEqualNode& lessOrEqualNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> lessOrEqualElement(new dom::Element(U"LessOrEqualNode"));
    dom::Element* prevElement = currentElement;
    currentElement = lessOrEqualElement.get();
    lessOrEqualNode.Left()->Accept(*this);
    lessOrEqualNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(lessOrEqualElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(GreaterOrEqualNode& greaterOrEqualNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> greaterOrEqualElement(new dom::Element(U"GreaterOrEqualNode"));
    dom::Element* prevElement = currentElement;
    currentElement = greaterOrEqualElement.get();
    greaterOrEqualNode.Left()->Accept(*this);
    greaterOrEqualNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(greaterOrEqualElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ShiftLeftNode& shiftLeftNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> shiftLeftElement(new dom::Element(U"ShiftLeftNode"));
    dom::Element* prevElement = currentElement;
    currentElement = shiftLeftElement.get();
    shiftLeftNode.Left()->Accept(*this);
    shiftLeftNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(shiftLeftElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ShiftRightNode& shiftRightNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> shiftRightElement(new dom::Element(U"ShiftRightNode"));
    dom::Element* prevElement = currentElement;
    currentElement = shiftRightElement.get();
    shiftRightNode.Left()->Accept(*this);
    shiftRightNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(shiftRightElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AddNode& addNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> addElement(new dom::Element(U"AddNode"));
    dom::Element* prevElement = currentElement;
    currentElement = addElement.get();
    addNode.Left()->Accept(*this);
    addNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(addElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(SubNode& subNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> subElement(new dom::Element(U"SubNode"));
    dom::Element* prevElement = currentElement;
    currentElement = subElement.get();
    subNode.Left()->Accept(*this);
    subNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(subElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(MulNode& mulNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> mulElement(new dom::Element(U"MulNode"));
    dom::Element* prevElement = currentElement;
    currentElement = mulElement.get();
    mulNode.Left()->Accept(*this);
    mulNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(mulElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DivNode& divNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> divElement(new dom::Element(U"DivNode"));
    dom::Element* prevElement = currentElement;
    currentElement = divElement.get();
    divNode.Left()->Accept(*this);
    divNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(divElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(RemNode& remNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> remElement(new dom::Element(U"RemNode"));
    dom::Element* prevElement = currentElement;
    currentElement = remElement.get();
    remNode.Left()->Accept(*this);
    remNode.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(remElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(NotNode& notNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> notElement(new dom::Element(U"NotNode"));
    dom::Element* prevElement = currentElement;
    currentElement = notElement.get();
    notNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(notElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UnaryPlusNode& unaryPlusNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> unaryPlusElement(new dom::Element(U"UnaryPlusNode"));
    dom::Element* prevElement = currentElement;
    currentElement = unaryPlusElement.get();
    unaryPlusNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(unaryPlusElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(UnaryMinusNode& unaryMinusNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> unaryMinusElement(new dom::Element(U"UnaryMinusNode"));
    dom::Element* prevElement = currentElement;
    currentElement = unaryMinusElement.get();
    unaryMinusNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(unaryMinusElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PrefixIncrementNode& prefixIncrementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> prefixIncrementElement(new dom::Element(U"PrefixIncrementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = prefixIncrementElement.get();
    prefixIncrementNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(prefixIncrementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PrefixDecrementNode& prefixDecrementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> prefixDecrementElement(new dom::Element(U"PrefixDecrementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = prefixDecrementElement.get();
    prefixDecrementNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(prefixDecrementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(DerefNode& derefNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> derefElement(new dom::Element(U"DerefNode"));
    dom::Element* prevElement = currentElement;
    currentElement = derefElement.get();
    derefNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(derefElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AddrOfNode& addrOfNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> addrOfElement(new dom::Element(U"AddrOfNode"));
    dom::Element* prevElement = currentElement;
    currentElement = addrOfElement.get();
    addrOfNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(addrOfElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ComplementNode& complementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> complementElement(new dom::Element(U"ComplementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = complementElement.get();
    complementNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(complementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IsNode& isNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> isElement(new dom::Element(U"IsNode"));
    dom::Element* prevElement = currentElement;
    currentElement = isElement.get();
    isNode.Expr()->Accept(*this);
    isNode.TargetTypeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(isElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(AsNode& asNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> asElement(new dom::Element(U"AsNode"));
    dom::Element* prevElement = currentElement;
    currentElement = asElement.get();
    asNode.Expr()->Accept(*this);
    asNode.TargetTypeExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(asElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(IndexingNode& indexingNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> indexingElement(new dom::Element(U"IndexingNode"));
    dom::Element* prevElement = currentElement;
    currentElement = indexingElement.get();
    indexingNode.Subject()->Accept(*this);
    indexingNode.Index()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(indexingElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(InvokeNode& invokeNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> invokeElement(new dom::Element(U"InvokeNode"));
    dom::Element* prevElement = currentElement;
    currentElement = invokeElement.get();
    invokeNode.Subject()->Accept(*this);
    int n = invokeNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* arg = invokeNode.Arguments()[i];
        arg->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(invokeElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PostfixIncrementNode& postfixIncrementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> postfixIncrementElement(new dom::Element(U"PostfixIncrementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = postfixIncrementElement.get();
    postfixIncrementNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(postfixIncrementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(PostfixDecrementNode& postfixDecrementNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> postfixDecrementElement(new dom::Element(U"PostfixDecrementNode"));
    dom::Element* prevElement = currentElement;
    currentElement = postfixDecrementElement.get();
    postfixDecrementNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(postfixDecrementElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(SizeOfNode& sizeOfNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> sizeOfElement(new dom::Element(U"SizeOfNode"));
    dom::Element* prevElement = currentElement;
    currentElement = sizeOfElement.get();
    sizeOfNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(sizeOfElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(TypeNameNode& typeNameNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> typeNameElement(new dom::Element(U"TypeNameNode"));
    dom::Element* prevElement = currentElement;
    currentElement = typeNameElement.get();
    typeNameNode.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(typeNameElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(CastNode& castNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> castElement(new dom::Element(U"CastNode"));
    dom::Element* prevElement = currentElement;
    currentElement = castElement.get();
    castNode.TargetTypeExpr()->Accept(*this);
    castNode.SourceExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(castElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ConstructNode& constructNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> constructElement(new dom::Element(U"ConstructNode"));
    dom::Element* prevElement = currentElement;
    currentElement = constructElement.get();
    constructNode.TypeExpr()->Accept(*this);
    int n = constructNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* arg = constructNode.Arguments()[i];
        arg->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(constructElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(NewNode& newNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> newElement(new dom::Element(U"NewNode"));
    dom::Element* prevElement = currentElement;
    currentElement = newElement.get();
    newNode.TypeExpr()->Accept(*this);
    int n = newNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        cmajor::ast::Node* arg = newNode.Arguments()[i];
        arg->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(newElement.release()));
    currentElement = prevElement;
}

void Ast2DomVisitor::Visit(ThisNode& thisNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"ThisNode")));
}

void Ast2DomVisitor::Visit(BaseNode& baseNode)
{
    CheckCurrentElement();
    currentElement->AppendChild(std::unique_ptr<dom::Node>(new dom::Element(U"BaseNode")));
}

void Ast2DomVisitor::Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> parenElement(new dom::Element(U"ParenthesizedExpressionNode"));
    dom::Element* prevElement = currentElement;
    currentElement = parenElement.get();
    parenthesizedExpressionNode.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(parenElement.release()));
    currentElement = prevElement;
}

std::unique_ptr<dom::Document> GenerateAstDocument(cmajor::ast::Node* node)
{
    std::unique_ptr<dom::Document> doc(new dom::Document());
    Ast2DomVisitor visitor;
    node->Accept(visitor);
    std::unique_ptr<dom::Element> compileUnitElement = visitor.GetCompileUnitElement();
    doc->AppendChild(std::unique_ptr<dom::Node>(compileUnitElement.release()));
    return doc;
}

} } // namespace cmajor::ast2dom
