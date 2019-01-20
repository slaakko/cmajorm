// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_XPATH_XPATH_EXPR
#define CMAJOR_XPATH_XPATH_EXPR
#include <cmajor/xpath/XPathObject.hpp>
#include <cmajor/xpath/XPathContext.hpp>
#include <string>
#include <memory>
#include <vector>

namespace cmajor { namespace xpath {

using dom::Axis;

enum class Operator
{
    equal, notEqual, less, greater, lessOrEqual, greaterOrEqual, plus, minus, mul, div, mod, slash, slashSlash
};

class XPathExpr
{
public:
    virtual ~XPathExpr();
    virtual std::unique_ptr<XPathObject> Evaluate(XPathContext& context) { return std::unique_ptr<XPathObject>();  }
    virtual std::u32string TextValue() const { return std::u32string(); }
    virtual std::unique_ptr<dom::Node> ToDom() const = 0;
};

class XPathUnaryExpr : public XPathExpr
{
public:
    XPathUnaryExpr(XPathExpr* operand_);
    XPathExpr* Operand() const { return operand.get(); }
private:
    std::unique_ptr<XPathExpr> operand;
};

class XPathBinaryExpr : public XPathExpr
{
public:
    XPathBinaryExpr(XPathExpr* left_, XPathExpr* right_);
    XPathExpr* Left() const { return left.get(); }
    XPathExpr* Right() const { return right.get(); }
private:
    std::unique_ptr<XPathExpr> left;
    std::unique_ptr<XPathExpr> right;
};

class XPathOrExpr : public XPathBinaryExpr
{
public:
    XPathOrExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathAndExpr : public XPathBinaryExpr
{
public:
    XPathAndExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathEqualExpr : public XPathBinaryExpr
{
public:
    XPathEqualExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathNotEqualExpr : public XPathBinaryExpr
{
public:
    XPathNotEqualExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathLessExpr : public XPathBinaryExpr
{
public:
    XPathLessExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathGreaterExpr : public XPathBinaryExpr
{
public:
    XPathGreaterExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathLessOrEqualExpr : public XPathBinaryExpr
{
public:
    XPathLessOrEqualExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathGreaterOrEqualExpr : public XPathBinaryExpr
{
public:
    XPathGreaterOrEqualExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathAddExpr : public XPathBinaryExpr
{
public:
    XPathAddExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathSubExpr : public XPathBinaryExpr
{
public:
    XPathSubExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathMulExpr : public XPathBinaryExpr
{
public:
    XPathMulExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathDivExpr : public XPathBinaryExpr
{
public:
    XPathDivExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathModExpr : public XPathBinaryExpr
{
public:
    XPathModExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathUnaryMinusExpr : public XPathUnaryExpr
{
public:
    XPathUnaryMinusExpr(XPathExpr* operand_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathUnionExpr : public XPathBinaryExpr
{
public:
    XPathUnionExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathCombineStepExpr : public XPathBinaryExpr
{
public:
    XPathCombineStepExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathRootNodeExpr : public XPathExpr
{
public:
    XPathRootNodeExpr();
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathFilterExpr : public XPathUnaryExpr
{
public:
    XPathFilterExpr(XPathExpr* expr_, XPathExpr* predicate_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::unique_ptr<XPathExpr> predicate;
};

class XPathNodeTestExpr : public XPathExpr
{
public:
    virtual bool Select(cmajor::dom::Node* node, Axis axis) const { return true; }
};

class XPathLocationStepExpr : public XPathExpr
{
public:
    XPathLocationStepExpr(Axis axis_, XPathNodeTestExpr* nodeTest_);
    void AddPredicate(XPathExpr* predicate);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    Axis axis;
    std::unique_ptr<XPathNodeTestExpr> nodeTest;
    std::vector<std::unique_ptr<XPathExpr>> predicates;
};

Axis GetAxis(const std::u32string& axisName);

class XPathPILiteralTest : public XPathNodeTestExpr
{
public:
    XPathPILiteralTest(XPathExpr* literal_);
    bool Select(cmajor::dom::Node* node, Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::unique_ptr<XPathExpr> literal;
};

class XPathCommentNodeTest : public XPathNodeTestExpr
{
public:
    bool Select(cmajor::dom::Node* node, Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathTextNodeTest : public XPathNodeTestExpr
{
public:
    bool Select(cmajor::dom::Node* node, Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathPINodeTest : public XPathNodeTestExpr
{
public:
    bool Select(cmajor::dom::Node* node, Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathAnyNodeTest : public XPathNodeTestExpr
{
public:
    bool Select(cmajor::dom::Node* node, Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathPrincipalNodeTest : public XPathNodeTestExpr
{
public:
    bool Select(cmajor::dom::Node* node, Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class XPathPrefixTest : public XPathNodeTestExpr
{
public:
    XPathPrefixTest(const std::u32string& name_);
    bool Select(cmajor::dom::Node* node, Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string name;
};

class XPathNameTest : public XPathNodeTestExpr
{
public:
    XPathNameTest(const std::u32string& name_);
    bool Select(cmajor::dom::Node* node, Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string name;
};

class XPathVariableReference : public XPathExpr
{
public:
    XPathVariableReference(const std::u32string& name_);
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string name;
};

class XPathLiteral : public XPathExpr
{
public:
    XPathLiteral(const std::u32string& value_);
    std::u32string TextValue() const override { return value; }
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string value;
};

class XPathNumberExpr : public XPathExpr
{
public:
    XPathNumberExpr(const std::u32string& value_);
    XPathNumberExpr(double value_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    double value;
};

class XPathFunctionCall : public XPathExpr
{
public:
    XPathFunctionCall(const std::u32string& functionName_);
    void AddArgument(XPathExpr* argument);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context);
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string functionName;
    std::vector<std::unique_ptr<XPathExpr>> arguments;
};

} } // namespace cmajor::xpath

#endif // CMAJOR_XPATH_XPATH_EXPR
