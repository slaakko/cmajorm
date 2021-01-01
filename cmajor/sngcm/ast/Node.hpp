// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_NODE_INCLUDED
#define SNGCM_AST_NODE_INCLUDED
#include <sngcm/ast/AstApi.hpp>
#include <soulng/lexer/Span.hpp>
#include <sngcm/ast/Clone.hpp>
#include <sngcm/ast/Specifier.hpp>
#include <soulng/util/Error.hpp>
#include <boost/uuid/uuid.hpp>
#include <memory>
#include <stdint.h>

namespace sngcm { namespace ast {

using soulng::lexer::Span;

class Visitor;
class AstWriter;
class AstReader;
class ParameterNode;
class TemplateParameterNode;

enum class NodeType : uint8_t
{
    boolNode, sbyteNode, byteNode, shortNode, ushortNode, intNode, uintNode, longNode, ulongNode, floatNode, doubleNode, charNode, wcharNode, ucharNode, voidNode,
    booleanLiteralNode, sbyteLiteralNode, byteLiteralNode, shortLiteralNode, ushortLiteralNode, intLiteralNode, uintLiteralNode, longLiteralNode, ulongLiteralNode,
    floatLiteralNode, doubleLiteralNode, charLiteralNode, wcharLiteralNode, ucharLiteralNode, stringLiteralNode, wstringLiteralNode, ustringLiteralNode, nullLiteralNode, arrayLiteralNode, structuredLiteralNode,
    compileUnitNode, namespaceNode, aliasNode, namespaceImportNode, identifierNode, templateIdNode, functionNode,
    classNode, thisInitializerNode, baseInitializerNode, memberInitializerNode, staticConstructorNode, constructorNode, destructorNode, memberFunctionNode, conversionFunctionNode, 
    memberVariableNode,
    interfaceNode, delegateNode, classDelegateNode,
    parenthesizedConstraintNode, disjunctiveConstraintNode, conjunctiveConstraintNode, whereConstraintNode, predicateConstraintNode, isConstraintNode, multiParamConstraintNode, typeNameConstraintNode,
    constructorConstraintNode, destructorConstraintNode, memberFunctionConstraintNode, functionConstraintNode,
    sameConstraintNode, derivedConstraintNode, convertibleConstraintNode, explicitlyConvertibleConstraintNode, commonConstraintNode, nonreferenceTypeConstraintNode,
    axiomStatementNode, axiomNode, conceptIdNode, conceptNode, 
    sameConceptNode, derivedConceptNode, convertibleConceptNode, explicitlyConvertibleConceptNode, commonConceptNode, nonreferenceTypeConceptNode,
    labelNode, compoundStatementNode, returnStatementNode, ifStatementNode, whileStatementNode, doStatementNode, forStatementNode, breakStatementNode, continueStatementNode,
    gotoStatementNode, constructionStatementNode, deleteStatementNode, destroyStatementNode, assignmentStatementNode, expressionStatementNode, emptyStatementNode,
    rangeForStatementNode, switchStatementNode, caseStatementNode, defaultStatementNode, gotoCaseStatementNode, gotoDefaultStatementNode,
    throwStatementNode, catchNode, tryStatementNode, assertStatementNode,
    typedefNode, constantNode, enumTypeNode, enumConstantNode, parameterNode, templateParameterNode,
    constNode, lvalueRefNode, rvalueRefNode, pointerNode, arrayNode,
    dotNode, arrowNode, equivalenceNode, implicationNode, disjunctionNode, conjunctionNode, bitOrNode, bitXorNode, bitAndNode,
    equalNode, notEqualNode, lessNode, greaterNode, lessOrEqualNode, greaterOrEqualNode, shiftLeftNode, shiftRightNode,
    addNode, subNode, mulNode, divNode, remNode, notNode, unaryPlusNode, unaryMinusNode, prefixIncrementNode, prefixDecrementNode, complementNode, derefNode, addrOfNode,
    isNode, asNode, indexingNode, invokeNode, postfixIncrementNode, postfixDecrementNode, sizeOfNode, typeNameNode, typeIdNode, castNode, constructNode, newNode, thisNode, baseNode,
    conditionalCompilationDisjunctionNode, conditionalCompilationConjunctionNode, conditionalCompilationNotNode, conditionalCompilationPrimaryNode, conditionalCompilationPartNode, conditionalCompilationStatementNode,
    uuidLiteralNode, cursorIdNode, parenthesizedExpressionNode, globalVariableNode, parenthesizedCondCompExpressionNode, labeledStatementNode, commentNode, functionPtrNode,
    maxNode
};

std::string NodeTypeStr(NodeType nodeType);

class SNGCM_AST_API Node
{
public:
    Node(NodeType nodeType_, const Span& span_, const boost::uuids::uuid& moduleId_);
    virtual ~Node();
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    NodeType GetNodeType() const { return nodeType; }
    virtual Node* Clone(CloneContext& cloneContext) const = 0;
    virtual void Accept(Visitor& visitor) = 0;
    virtual void Write(AstWriter& writer);
    virtual void Read(AstReader& reader);
    virtual void SetFullSpan();
    virtual std::string ToString() const { return std::string(); }
    virtual void AddArgument(Node* argument) { Assert(false, "AddArgument not overridden");  }
    virtual void AddParameter(ParameterNode* parameter) { Assert(false, "AddParameter not overridden"); }
    virtual void AddTemplateParameter(TemplateParameterNode* templateParameter) { Assert(false, "AddTemplateParameter not overridden"); }
    virtual bool IsUnsignedTypeNode() const { return false; }
    virtual bool IsStatementNode() const { return false; }
    virtual bool NodeIsConstraintNode() const { return false; }
    virtual bool IsConceptNode() const { return false; }
    virtual bool IsFunctionNode() const { return false; }
    virtual bool IsIntrinsicConceptNode() const { return false; }
    virtual bool IsConditionalCompilationExpressionNode() const { return false; }
    virtual Specifiers GetSpecifiers() const { return Specifiers::none; }
    const Span& GetSpan() const { return span; }
    void SetSpan(const Span& span_) { span = span_; }
    void SetSpanStart(int32_t spanStart) { span.start = spanStart; }
    void SetSpanEnd(int end) { span.end = end; }
    const Node* Parent() const { return parent; }
    Node* Parent() { return parent; }
    void SetParent(Node* parent_);
    const boost::uuids::uuid& ModuleId() const { return moduleId; }
private:
    NodeType nodeType;
    Span span;
    boost::uuids::uuid moduleId;
    Node* parent;
};

class SNGCM_AST_API UnaryNode : public Node
{
public:
    UnaryNode(NodeType nodeType, const Span& span_, const boost::uuids::uuid& moduleId_);
    UnaryNode(NodeType nodeType, const Span& span_, const boost::uuids::uuid& moduleId_, Node* subject_);
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void SetFullSpan() override;
    const Node* Subject() const { return subject.get(); }
    Node* Subject() { return subject.get(); }
private:
    std::unique_ptr<Node> subject;
};

class SNGCM_AST_API BinaryNode : public Node
{
public:
    BinaryNode(NodeType nodeType, const Span& span_, const boost::uuids::uuid& moduleId_);
    BinaryNode(NodeType nodeType, const Span& span_, const boost::uuids::uuid& moduleId_, Node* left_, Node* right_);
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    void SetFullSpan() override;
    const Node* Left() const { return left.get(); }
    Node* Left() { return left.get(); }
    const Node* Right() const { return right.get(); }
    Node* Right() { return right.get(); }
private:
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

class SNGCM_AST_API NodeCreator
{
public:
    NodeCreator();
    NodeCreator(const NodeCreator&) = delete;
    NodeCreator& operator=(const NodeCreator&) = delete;
    virtual ~NodeCreator();
    virtual Node* CreateNode(const Span& span, const boost::uuids::uuid& moduleId) = 0;
};

class SNGCM_AST_API NodeFactory
{
public:
    NodeFactory(const NodeFactory&) = delete;
    NodeFactory& operator=(const NodeFactory&) = delete;
    static NodeFactory& Instance() { Assert(instance, "node factory not initialized"); return *instance; }
    static void Init();
    static void Done();
    void Register(NodeType nodeType, NodeCreator* creator);
    Node* CreateNode(NodeType nodeType, const Span& span, const boost::uuids::uuid& moduleId);
private:
    static std::unique_ptr<NodeFactory> instance;
    std::vector<std::unique_ptr<NodeCreator>> creators;
    NodeFactory();
};

SNGCM_AST_API void NodeInit();
SNGCM_AST_API void NodeDone();

} } // namespace sngcm::ast

#endif // SNGCM_AST_NODE_INCLUDED
