// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Node.hpp>
#include <sngcm/ast/AstWriter.hpp>
#include <sngcm/ast/AstReader.hpp>
#include <sngcm/ast/BasicType.hpp>
#include <sngcm/ast/Literal.hpp>
#include <sngcm/ast/CompileUnit.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Typedef.hpp>
#include <sngcm/ast/Constant.hpp>
#include <sngcm/ast/Enumeration.hpp>
#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/Parameter.hpp>
#include <sngcm/ast/Template.hpp>
#include <sngcm/ast/Delegate.hpp>
#include <sngcm/ast/Statement.hpp>
#include <sngcm/ast/Function.hpp>
#include <sngcm/ast/Class.hpp>
#include <sngcm/ast/Interface.hpp>
#include <sngcm/ast/Concept.hpp>
#include <sngcm/ast/GlobalVariable.hpp>
#include <sngcm/ast/Comment.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <algorithm>

namespace sngcm { namespace ast {

const char* nodeTypeStr[] = 
{
    "boolNode", "sbyteNode", "byteNode", "shortNode", "ushortNode", "intNode", "uintNode", "longNode", "ulongNode", "floatNode", "doubleNode", "charNode", "wcharNode", "ucharNode", "voidNode",
    "booleanLiteralNode", "sbyteLiteralNode", "byteLiteralNode", "shortLiteralNode", "ushortLiteralNode", "intLiteralNode", "uintLiteralNode", "longLiteralNode", "ulongLiteralNode",
    "floatLiteralNode", "doubleLiteralNode", "charLiteralNode", "wcharLiteralNode", "ucharLiteralNode", "stringLiteralNode", "wstringLiteralNode", "ustringLiteralNode", "nullLiteralNode", "arrayLiteralNode",
    "structuredLiteralNode",
    "compileUnitNode", "namespaceNode", "aliasNode", "namespaceImportNode", "identifierNode", "templateIdNode", "functionNode", 
    "classNode", "thisInitializerNode", "baseInitializerNode", "memberInitializerNode", "staticConstructorNode", "constructorNode", "destructorNode", "memberFunctionNode", "conversionFunctionNode",
    "memberVariableNode",
    "interfaceNode", "delegateNode", "classDelegateNode",
    "parenthesizedConstraintNode", "disjunctiveConstraintNode", "conjunctiveConstraintNode", "whereConstraintNode", "predicateConstraintNode", "isConstraintNode", "multiParamConstraintNode", "typeNameConstraintNode",
    "constructorConstraintNode", "destructorConstraintNode", "memberFunctionConstraintNode", "functionConstraintNode",
    "sameConstraintNode", "derivedConstraintNode", "convertibleConstraintNode", "explicitlyConvertibleConstraintNode", "commonConstraintNode", "nonreferenceTypeConstraintNode",
    "axiomStatementNode", "axiomNode", "conceptIdNode", "conceptNode", 
    "sameConceptNode", "derivedConceptNode", "convertibleConceptNode", "ecxplicitlyConvertibleConceptNode", "commonConceptNode", "nonreferenceTypeConceptNode",
    "labelNode", "compoundStatementNode", "returnStatementNode", "ifStatementNode", "whileStatementNode", "doStatementNode", "forStatementNode", "breakStatementNode", "continueStatementNode",
    "gotoStatementNode", "constructionStatementNode", "deleteStatementNode", "destroyStatementNode", "assignmentStatementNode", "expressionStatementNode", "emptyStatementNode", 
    "rangeForStatementNode", "switchStatementNode", "caseStatementNode", "defaultStatementNode", "gotoCaseStatementNode", 
    "gotoDefaultStatementNode", "throwStatementNode", "catchNode", "tryStatementNode", "assertStatementNode",
    "typedefNode", "constantNode", "enumTypeNode", "enumConstantNode", "parameterNode", "templateParameterNode",
    "constNode", "refNode", "arrayNode",
    "dotNode", "arrowNode", "equivalenceNode", "implicationNode", "disjunctionNode", "conjunctionNode", "bitOrNode", "bitXorNode", "bitAndNode", "equalNode", "notEqualNode", "lessNode", "greaterNode",
    "lessOrEqualNode", "greaterOrEqualNode", "shiftLeftNode", "shiftRightNode", 
    "addNode", "subNode", "mulNode", "divNode", "remNode", "notNode", "unaryPlusNode", "unaryMinusNode", "prefixIncrementNode", "prefixDecrementNode", "complementNode", "derefNode", "addrOfNode",
    "isNode", "asNode", "indexingNode", "invokeNode", "postfixIncrementNode", "postfixDecrementNode", "sizeOfNode", "typeNameNode", "typeIdNode", "castNode", "constructNode", "newNode", "thisNode", "baseNode",
    "conditionalCompilationDisjunctionNode", "conditionalCompilationConjunctionNode", "conditionalCompilationNotNode", "conditionalCompilationPrimaryNode", "conditionalCompilationPartNode", 
    "conditionalCompilationStatementNode", "uuidLiteralNode", "cursorIdNode", "parenthesizedExpressionNode", "globalVariableNode", "parenthesizedCondCompExpressionNode", "labeledStatementNode",
    "commentNode", "functionPtrNode", "syncNode",
    "maxNode"
};

std::string NodeTypeStr(NodeType nodeType)
{
    return nodeTypeStr[static_cast<size_t>(nodeType)];
}

Node::Node(NodeType nodeType_, const Span& span_, const boost::uuids::uuid& moduleId_) : nodeType(nodeType_), span(span_), moduleId(moduleId_), parent(nullptr), lexerFlags()
{
}

Node::~Node()
{
}

void Node::SetParent(Node* parent_)
{
     parent = parent_;
}

void Node::Write(AstWriter& writer)
{
}

void Node::Read(AstReader& reader)
{
}

void Node::SetFullSpan()
{
}

UnaryNode::UnaryNode(NodeType nodeType_, const Span& span_, const boost::uuids::uuid& moduleId_) : Node(nodeType_, span_, moduleId_), subject()
{
}

UnaryNode::UnaryNode(NodeType nodeType_, const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_) : Node(nodeType_, span_, moduleId_), subject(subject_)
{
}

void UnaryNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(subject.get());
}

void UnaryNode::Read(AstReader& reader)
{
    Node::Read(reader);
    subject.reset(reader.ReadNode());
    subject->SetParent(this);
}

void UnaryNode::SetFullSpan()
{
    if (subject)
    {
        const Span& thisSpan = GetSpan();
        subject->SetFullSpan();
        const Span& childSpan = subject->GetSpan();
        SetSpanStart(std::min(thisSpan.start, childSpan.start));
        SetSpanEnd(std::max(thisSpan.end, childSpan.end));
    }
}

BinaryNode::BinaryNode(NodeType nodeType, const Span& span_, const boost::uuids::uuid& moduleId_) : Node(nodeType, span_, moduleId_), left(), right()
{
}

BinaryNode::BinaryNode(NodeType nodeType, const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_) : Node(nodeType, span_, moduleId_), left(left_), right(right_)
{
    left->SetParent(this);
    right->SetParent(this);
}

void BinaryNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.Write(left.get());
    writer.Write(right.get());
}

void BinaryNode::Read(AstReader& reader)
{
    Node::Read(reader);
    left.reset(reader.ReadNode());
    left->SetParent(this);
    right.reset(reader.ReadNode());
    right->SetParent(this);
}

void BinaryNode::SetFullSpan()
{
    const Span& thisSpan = GetSpan();
    left->SetFullSpan();
    right->SetFullSpan();
    const Span& leftSpan = left->GetSpan();
    const Span& rightSpan = right->GetSpan();
    SetSpanStart(std::min(thisSpan.start, std::min(leftSpan.start, rightSpan.start)));
    SetSpanEnd(std::max(thisSpan.end, std::max(leftSpan.end, rightSpan.end)));
}

NodeCreator::NodeCreator()
{
}

NodeCreator::~NodeCreator()
{
}

template<typename T>
class ConcreteNodeCreator : public NodeCreator
{
public:
    ConcreteNodeCreator() : NodeCreator() {}
    ConcreteNodeCreator(const ConcreteNodeCreator&) = delete;
    ConcreteNodeCreator& operator=(const ConcreteNodeCreator&) = delete;
    Node* CreateNode(const Span& span, const boost::uuids::uuid& moduleId) override
    {
        return new T(span, moduleId);
    }
};

std::unique_ptr<NodeFactory> NodeFactory::instance;

void NodeFactory::Init()
{
    instance.reset(new NodeFactory());
}

void NodeFactory::Done()
{
    instance.reset();
}

NodeFactory::NodeFactory()
{
    creators.resize(static_cast<size_t>(NodeType::maxNode));

    Register(NodeType::boolNode, new ConcreteNodeCreator<BoolNode>());
    Register(NodeType::sbyteNode, new ConcreteNodeCreator<SByteNode>());
    Register(NodeType::byteNode, new ConcreteNodeCreator<ByteNode>());
    Register(NodeType::shortNode, new ConcreteNodeCreator<ShortNode>());
    Register(NodeType::ushortNode, new ConcreteNodeCreator<UShortNode>());
    Register(NodeType::intNode, new ConcreteNodeCreator<IntNode>());
    Register(NodeType::uintNode, new ConcreteNodeCreator<UIntNode>());
    Register(NodeType::longNode, new ConcreteNodeCreator<LongNode>());
    Register(NodeType::ulongNode, new ConcreteNodeCreator<ULongNode>());
    Register(NodeType::floatNode, new ConcreteNodeCreator<FloatNode>());
    Register(NodeType::doubleNode, new ConcreteNodeCreator<DoubleNode>());
    Register(NodeType::charNode, new ConcreteNodeCreator<CharNode>());
    Register(NodeType::wcharNode, new ConcreteNodeCreator<WCharNode>());
    Register(NodeType::ucharNode, new ConcreteNodeCreator<UCharNode>());
    Register(NodeType::voidNode, new ConcreteNodeCreator<VoidNode>());

    Register(NodeType::booleanLiteralNode, new ConcreteNodeCreator<BooleanLiteralNode>());
    Register(NodeType::sbyteLiteralNode, new ConcreteNodeCreator<SByteLiteralNode>());
    Register(NodeType::byteLiteralNode, new ConcreteNodeCreator<ByteLiteralNode>());
    Register(NodeType::shortLiteralNode, new ConcreteNodeCreator<ShortLiteralNode>());
    Register(NodeType::ushortLiteralNode, new ConcreteNodeCreator<UShortLiteralNode>());
    Register(NodeType::intLiteralNode, new ConcreteNodeCreator<IntLiteralNode>());
    Register(NodeType::uintLiteralNode, new ConcreteNodeCreator<UIntLiteralNode>());
    Register(NodeType::longLiteralNode, new ConcreteNodeCreator<LongLiteralNode>());
    Register(NodeType::ulongLiteralNode, new ConcreteNodeCreator<ULongLiteralNode>());
    Register(NodeType::floatLiteralNode, new ConcreteNodeCreator<FloatLiteralNode>());
    Register(NodeType::doubleLiteralNode, new ConcreteNodeCreator<DoubleLiteralNode>());
    Register(NodeType::charLiteralNode, new ConcreteNodeCreator<CharLiteralNode>());
    Register(NodeType::wcharLiteralNode, new ConcreteNodeCreator<WCharLiteralNode>());
    Register(NodeType::ucharLiteralNode, new ConcreteNodeCreator<UCharLiteralNode>());
    Register(NodeType::stringLiteralNode, new ConcreteNodeCreator<StringLiteralNode>());
    Register(NodeType::wstringLiteralNode, new ConcreteNodeCreator<WStringLiteralNode>());
    Register(NodeType::ustringLiteralNode, new ConcreteNodeCreator<UStringLiteralNode>());
    Register(NodeType::nullLiteralNode, new ConcreteNodeCreator<NullLiteralNode>());
    Register(NodeType::arrayLiteralNode, new ConcreteNodeCreator<ArrayLiteralNode>());
    Register(NodeType::structuredLiteralNode, new ConcreteNodeCreator<StructuredLiteralNode>());

    Register(NodeType::compileUnitNode, new ConcreteNodeCreator<CompileUnitNode>());
    Register(NodeType::namespaceNode, new ConcreteNodeCreator<NamespaceNode>());
    Register(NodeType::aliasNode, new ConcreteNodeCreator<AliasNode>());
    Register(NodeType::namespaceImportNode, new ConcreteNodeCreator<NamespaceImportNode>());
    Register(NodeType::identifierNode, new ConcreteNodeCreator<IdentifierNode>());
    Register(NodeType::templateIdNode, new ConcreteNodeCreator<TemplateIdNode>());
    Register(NodeType::functionNode, new ConcreteNodeCreator<FunctionNode>());
    Register(NodeType::classNode, new ConcreteNodeCreator<ClassNode>());
    Register(NodeType::thisInitializerNode, new ConcreteNodeCreator<ThisInitializerNode>());
    Register(NodeType::baseInitializerNode, new ConcreteNodeCreator<BaseInitializerNode>());
    Register(NodeType::memberInitializerNode, new ConcreteNodeCreator<MemberInitializerNode>());
    Register(NodeType::staticConstructorNode, new ConcreteNodeCreator<StaticConstructorNode>());
    Register(NodeType::constructorNode, new ConcreteNodeCreator<ConstructorNode>());
    Register(NodeType::destructorNode, new ConcreteNodeCreator<DestructorNode>());
    Register(NodeType::memberFunctionNode, new ConcreteNodeCreator<MemberFunctionNode>());
    Register(NodeType::conversionFunctionNode, new ConcreteNodeCreator<ConversionFunctionNode>());
    Register(NodeType::memberVariableNode, new ConcreteNodeCreator<MemberVariableNode>());
    Register(NodeType::interfaceNode, new ConcreteNodeCreator<InterfaceNode>());
    Register(NodeType::delegateNode, new ConcreteNodeCreator<DelegateNode>());
    Register(NodeType::classDelegateNode, new ConcreteNodeCreator<ClassDelegateNode>());

    Register(NodeType::parenthesizedConstraintNode, new ConcreteNodeCreator<ParenthesizedConstraintNode>());
    Register(NodeType::disjunctiveConstraintNode, new ConcreteNodeCreator<DisjunctiveConstraintNode>());
    Register(NodeType::conjunctiveConstraintNode, new ConcreteNodeCreator<ConjunctiveConstraintNode>());
    Register(NodeType::whereConstraintNode, new ConcreteNodeCreator<WhereConstraintNode>());
    Register(NodeType::predicateConstraintNode, new ConcreteNodeCreator<PredicateConstraintNode>());
    Register(NodeType::isConstraintNode, new ConcreteNodeCreator<IsConstraintNode>());
    Register(NodeType::multiParamConstraintNode, new ConcreteNodeCreator<MultiParamConstraintNode>());
    Register(NodeType::typeNameConstraintNode, new ConcreteNodeCreator<TypeNameConstraintNode>());
    Register(NodeType::constructorConstraintNode, new ConcreteNodeCreator<ConstructorConstraintNode>());
    Register(NodeType::destructorConstraintNode, new ConcreteNodeCreator<DestructorConstraintNode>()); 
    Register(NodeType::memberFunctionConstraintNode, new ConcreteNodeCreator<MemberFunctionConstraintNode>());
    Register(NodeType::functionConstraintNode, new ConcreteNodeCreator<FunctionConstraintNode>());
    Register(NodeType::sameConstraintNode, new ConcreteNodeCreator<SameConstraintNode>());
    Register(NodeType::derivedConstraintNode, new ConcreteNodeCreator<DerivedConstraintNode>());
    Register(NodeType::convertibleConstraintNode, new ConcreteNodeCreator<ConvertibleConstraintNode>());
    Register(NodeType::explicitlyConvertibleConstraintNode, new ConcreteNodeCreator<ExplicitlyConvertibleConstraintNode>());
    Register(NodeType::commonConstraintNode, new ConcreteNodeCreator<CommonConstraintNode>());
    Register(NodeType::nonreferenceTypeConstraintNode, new ConcreteNodeCreator<NonreferenceTypeConstraintNode>());
    Register(NodeType::axiomStatementNode, new ConcreteNodeCreator<AxiomStatementNode>());
    Register(NodeType::axiomNode, new ConcreteNodeCreator<AxiomNode>());
    Register(NodeType::conceptIdNode, new ConcreteNodeCreator<ConceptIdNode>());
    Register(NodeType::conceptNode, new ConcreteNodeCreator<ConceptNode>());
    Register(NodeType::sameConceptNode, new ConcreteNodeCreator<SameConceptNode>());
    Register(NodeType::derivedConceptNode, new ConcreteNodeCreator<DerivedConceptNode>());
    Register(NodeType::convertibleConceptNode, new ConcreteNodeCreator<ConvertibleConceptNode>());
    Register(NodeType::explicitlyConvertibleConceptNode, new ConcreteNodeCreator<ExplicitlyConvertibleConceptNode>());
    Register(NodeType::commonConceptNode, new ConcreteNodeCreator<CommonConceptNode>());
    Register(NodeType::nonreferenceTypeConceptNode, new ConcreteNodeCreator<NonreferenceTypeConceptNode>());

    Register(NodeType::labelNode, new ConcreteNodeCreator<LabelNode>());
    Register(NodeType::labeledStatementNode, new ConcreteNodeCreator<LabeledStatementNode>());
    Register(NodeType::compoundStatementNode, new ConcreteNodeCreator<CompoundStatementNode>());
    Register(NodeType::returnStatementNode, new ConcreteNodeCreator<ReturnStatementNode>());
    Register(NodeType::ifStatementNode, new ConcreteNodeCreator<IfStatementNode>()); 
    Register(NodeType::whileStatementNode, new ConcreteNodeCreator<WhileStatementNode>());
    Register(NodeType::doStatementNode, new ConcreteNodeCreator<DoStatementNode>());
    Register(NodeType::forStatementNode, new ConcreteNodeCreator<ForStatementNode>());
    Register(NodeType::breakStatementNode, new ConcreteNodeCreator<BreakStatementNode>());
    Register(NodeType::continueStatementNode, new ConcreteNodeCreator<ContinueStatementNode>());
    Register(NodeType::gotoStatementNode, new ConcreteNodeCreator<GotoStatementNode>());
    Register(NodeType::constructionStatementNode, new ConcreteNodeCreator<ConstructionStatementNode>());
    Register(NodeType::deleteStatementNode, new ConcreteNodeCreator<DeleteStatementNode>());
    Register(NodeType::destroyStatementNode, new ConcreteNodeCreator<DestroyStatementNode>());
    Register(NodeType::assignmentStatementNode, new ConcreteNodeCreator<AssignmentStatementNode>());
    Register(NodeType::expressionStatementNode, new ConcreteNodeCreator<ExpressionStatementNode>());
    Register(NodeType::emptyStatementNode, new ConcreteNodeCreator<EmptyStatementNode>());
    Register(NodeType::rangeForStatementNode, new ConcreteNodeCreator<RangeForStatementNode>());
    Register(NodeType::switchStatementNode, new ConcreteNodeCreator<SwitchStatementNode>());
    Register(NodeType::caseStatementNode, new ConcreteNodeCreator<CaseStatementNode>());
    Register(NodeType::defaultStatementNode, new ConcreteNodeCreator<DefaultStatementNode>());
    Register(NodeType::gotoCaseStatementNode, new ConcreteNodeCreator<GotoCaseStatementNode>());
    Register(NodeType::gotoDefaultStatementNode, new ConcreteNodeCreator<GotoDefaultStatementNode>());
    Register(NodeType::throwStatementNode, new ConcreteNodeCreator<ThrowStatementNode>());
    Register(NodeType::catchNode, new ConcreteNodeCreator<CatchNode>());
    Register(NodeType::tryStatementNode, new ConcreteNodeCreator<TryStatementNode>());
    Register(NodeType::assertStatementNode, new ConcreteNodeCreator<AssertStatementNode>());

    Register(NodeType::typedefNode, new ConcreteNodeCreator<TypedefNode>());
    Register(NodeType::constantNode, new ConcreteNodeCreator<ConstantNode>());
    Register(NodeType::enumTypeNode, new ConcreteNodeCreator<EnumTypeNode>());
    Register(NodeType::enumConstantNode, new ConcreteNodeCreator<EnumConstantNode>());
    Register(NodeType::parameterNode, new ConcreteNodeCreator<ParameterNode>());
    Register(NodeType::templateParameterNode, new ConcreteNodeCreator<TemplateParameterNode>());

    Register(NodeType::constNode, new ConcreteNodeCreator<ConstNode>());
    Register(NodeType::lvalueRefNode, new ConcreteNodeCreator<LValueRefNode>());
    Register(NodeType::rvalueRefNode, new ConcreteNodeCreator<RValueRefNode>());
    Register(NodeType::pointerNode, new ConcreteNodeCreator<PointerNode>());
    Register(NodeType::dotNode, new ConcreteNodeCreator<DotNode>());
    Register(NodeType::arrowNode, new ConcreteNodeCreator<ArrowNode>());
    Register(NodeType::arrayNode, new ConcreteNodeCreator<ArrayNode>());

    Register(NodeType::equivalenceNode, new ConcreteNodeCreator<EquivalenceNode>());
    Register(NodeType::implicationNode, new ConcreteNodeCreator<ImplicationNode>());
    Register(NodeType::disjunctionNode, new ConcreteNodeCreator<DisjunctionNode>());
    Register(NodeType::conjunctionNode, new ConcreteNodeCreator<ConjunctionNode>());
    Register(NodeType::bitOrNode, new ConcreteNodeCreator<BitOrNode>());
    Register(NodeType::bitXorNode, new ConcreteNodeCreator<BitXorNode>());
    Register(NodeType::bitAndNode, new ConcreteNodeCreator<BitAndNode>());
    Register(NodeType::equalNode, new ConcreteNodeCreator<EqualNode>());
    Register(NodeType::notEqualNode, new ConcreteNodeCreator<NotEqualNode>());
    Register(NodeType::lessNode, new ConcreteNodeCreator<LessNode>());
    Register(NodeType::greaterNode, new ConcreteNodeCreator<GreaterNode>());
    Register(NodeType::lessOrEqualNode, new ConcreteNodeCreator<LessOrEqualNode>());
    Register(NodeType::greaterOrEqualNode, new ConcreteNodeCreator<GreaterOrEqualNode>());
    Register(NodeType::shiftLeftNode, new ConcreteNodeCreator<ShiftLeftNode>());
    Register(NodeType::shiftRightNode, new ConcreteNodeCreator<ShiftRightNode>());
    Register(NodeType::addNode, new ConcreteNodeCreator<AddNode>());
    Register(NodeType::subNode, new ConcreteNodeCreator<SubNode>());
    Register(NodeType::mulNode, new ConcreteNodeCreator<MulNode>());
    Register(NodeType::divNode, new ConcreteNodeCreator<DivNode>());
    Register(NodeType::remNode, new ConcreteNodeCreator<RemNode>());
    Register(NodeType::notNode, new ConcreteNodeCreator<NotNode>());
    Register(NodeType::unaryPlusNode, new ConcreteNodeCreator<UnaryPlusNode>());
    Register(NodeType::unaryMinusNode, new ConcreteNodeCreator<UnaryMinusNode>());
    Register(NodeType::prefixIncrementNode, new ConcreteNodeCreator<PrefixIncrementNode>());
    Register(NodeType::prefixDecrementNode, new ConcreteNodeCreator<PrefixDecrementNode>());
    Register(NodeType::complementNode, new ConcreteNodeCreator<ComplementNode>());
    Register(NodeType::derefNode, new ConcreteNodeCreator<DerefNode>());
    Register(NodeType::addrOfNode, new ConcreteNodeCreator<AddrOfNode>());
    Register(NodeType::isNode, new ConcreteNodeCreator<IsNode>());
    Register(NodeType::asNode, new ConcreteNodeCreator<AsNode>());
    Register(NodeType::indexingNode, new ConcreteNodeCreator<IndexingNode>());
    Register(NodeType::invokeNode, new ConcreteNodeCreator<InvokeNode>());
    Register(NodeType::postfixIncrementNode, new ConcreteNodeCreator<PostfixIncrementNode>());
    Register(NodeType::postfixDecrementNode, new ConcreteNodeCreator<PostfixDecrementNode>());
    Register(NodeType::sizeOfNode, new ConcreteNodeCreator<SizeOfNode>());
    Register(NodeType::typeNameNode, new ConcreteNodeCreator<TypeNameNode>());
    Register(NodeType::typeIdNode, new ConcreteNodeCreator<TypeIdNode>());
    Register(NodeType::castNode, new ConcreteNodeCreator<CastNode>());
    Register(NodeType::constructNode, new ConcreteNodeCreator<ConstructNode>());
    Register(NodeType::newNode, new ConcreteNodeCreator<NewNode>());
    Register(NodeType::thisNode, new ConcreteNodeCreator<ThisNode>());
    Register(NodeType::baseNode, new ConcreteNodeCreator<BaseNode>());
    Register(NodeType::conditionalCompilationDisjunctionNode, new ConcreteNodeCreator<ConditionalCompilationDisjunctionNode>());
    Register(NodeType::conditionalCompilationConjunctionNode, new ConcreteNodeCreator<ConditionalCompilationConjunctionNode>());
    Register(NodeType::conditionalCompilationNotNode, new ConcreteNodeCreator<ConditionalCompilationNotNode>());
    Register(NodeType::conditionalCompilationPrimaryNode, new ConcreteNodeCreator<ConditionalCompilationPrimaryNode>());
    Register(NodeType::conditionalCompilationPartNode, new ConcreteNodeCreator<ConditionalCompilationPartNode>());
    Register(NodeType::parenthesizedCondCompExpressionNode, new ConcreteNodeCreator<ParenthesizedConditionalCompilationExpressionNode>());
    Register(NodeType::conditionalCompilationStatementNode, new ConcreteNodeCreator<ConditionalCompilationStatementNode>());
    Register(NodeType::uuidLiteralNode, new ConcreteNodeCreator<UuidLiteralNode>());
    Register(NodeType::cursorIdNode, new ConcreteNodeCreator<CursorIdNode>());
    Register(NodeType::parenthesizedExpressionNode, new ConcreteNodeCreator<ParenthesizedExpressionNode>());
    Register(NodeType::globalVariableNode, new ConcreteNodeCreator<GlobalVariableNode>());
    Register(NodeType::commentNode, new ConcreteNodeCreator<CommentNode>());
    Register(NodeType::functionPtrNode, new ConcreteNodeCreator<FunctionPtrNode>());
}

void NodeFactory::Register(NodeType nodeType, NodeCreator* creator)
{
    creators[static_cast<size_t>(nodeType)] = std::unique_ptr<NodeCreator>(creator);
}

Node* NodeFactory::CreateNode(NodeType nodeType, const Span& span, const boost::uuids::uuid& moduleId)
{
    const std::unique_ptr<NodeCreator>& creator = creators[static_cast<size_t>(nodeType)];
    if (creator)
    {
        Node* value = creator->CreateNode(span, moduleId);
        if (value)
        {
            return value;
        }
        else
        {
            throw std::runtime_error("could not create node");
        }
    }
    else
    {
        throw std::runtime_error("no creator for node type '" + NodeTypeStr(nodeType) + "'");
    }
}

void NodeInit()
{
    NodeFactory::Init();
}

void NodeDone()
{
    NodeFactory::Done();
}

} } // namespace sngcm::ast
