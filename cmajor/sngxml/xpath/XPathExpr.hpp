// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XPATH_XPATH_EXPR
#define SNGXML_XPATH_XPATH_EXPR
#include <sngxml/xpath/XPathObject.hpp>
#include <sngxml/xpath/XPathContext.hpp>
#include <string>
#include <memory>
#include <vector>

namespace sngxml { namespace xpath {

enum class Operator
{
    equal, notEqual, less, greater, lessOrEqual, greaterOrEqual, plus, minus, mul, div, mod, slash, slashSlash
};

class SNGXML_XPATH_API XPathExpr
{
public:
    XPathExpr();
    XPathExpr(const XPathExpr&) = delete;
    XPathExpr& operator=(const XPathExpr&) = delete;
    XPathExpr(XPathExpr&&) = delete;
    XPathExpr&& operator=(XPathExpr&&) = delete;
    virtual ~XPathExpr();
    virtual std::unique_ptr<XPathObject> Evaluate(XPathContext& context) { return std::unique_ptr<XPathObject>(); }
    virtual std::u32string TextValue() const { return std::u32string(); }
    virtual std::unique_ptr<dom::Node> ToDom() const = 0;
};

class SNGXML_XPATH_API XPathUnaryExpr : public XPathExpr
{
public:
    XPathUnaryExpr(XPathExpr* operand_);
    XPathExpr* Operand() const { return operand.get(); }
private:
    std::unique_ptr<XPathExpr> operand;
};

class SNGXML_XPATH_API XPathBinaryExpr : public XPathExpr
{
public:
    XPathBinaryExpr(XPathExpr* left_, XPathExpr* right_);
    XPathExpr* Left() const { return left.get(); }
    XPathExpr* Right() const { return right.get(); }
private:
    std::unique_ptr<XPathExpr> left;
    std::unique_ptr<XPathExpr> right;
};

class SNGXML_XPATH_API XPathOrExpr : public XPathBinaryExpr
{
public:
    XPathOrExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathAndExpr : public XPathBinaryExpr
{
public:
    XPathAndExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathEqualExpr : public XPathBinaryExpr
{
public:
    XPathEqualExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathNotEqualExpr : public XPathBinaryExpr
{
public:
    XPathNotEqualExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathLessExpr : public XPathBinaryExpr
{
public:
    XPathLessExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathGreaterExpr : public XPathBinaryExpr
{
public:
    XPathGreaterExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathLessOrEqualExpr : public XPathBinaryExpr
{
public:
    XPathLessOrEqualExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathGreaterOrEqualExpr : public XPathBinaryExpr
{
public:
    XPathGreaterOrEqualExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathAddExpr : public XPathBinaryExpr
{
public:
    XPathAddExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathSubExpr : public XPathBinaryExpr
{
public:
    XPathSubExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathMulExpr : public XPathBinaryExpr
{
public:
    XPathMulExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathDivExpr : public XPathBinaryExpr
{
public:
    XPathDivExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathModExpr : public XPathBinaryExpr
{
public:
    XPathModExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathUnaryMinusExpr : public XPathUnaryExpr
{
public:
    XPathUnaryMinusExpr(XPathExpr* operand_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathUnionExpr : public XPathBinaryExpr
{
public:
    XPathUnionExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathCombineStepExpr : public XPathBinaryExpr
{
public:
    XPathCombineStepExpr(XPathExpr* left_, XPathExpr* right_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathRootNodeExpr : public XPathExpr
{
public:
    XPathRootNodeExpr();
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathFilterExpr : public XPathUnaryExpr
{
public:
    XPathFilterExpr(XPathExpr* expr_, XPathExpr* predicate_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::unique_ptr<XPathExpr> predicate;
};

class SNGXML_XPATH_API XPathNodeTestExpr : public XPathExpr
{
public:
    virtual bool Select(sngxml::dom::Node* node, sngxml::dom::Axis axis) const { return true; }
};

class SNGXML_XPATH_API XPathLocationStepExpr : public XPathExpr
{
public:
    XPathLocationStepExpr(sngxml::dom::Axis axis_, XPathNodeTestExpr* nodeTest_);
    XPathLocationStepExpr(const XPathLocationStepExpr&) = delete;
    XPathLocationStepExpr& operator=(const XPathLocationStepExpr&) = delete;
    XPathLocationStepExpr(XPathLocationStepExpr&&) = delete;
    XPathLocationStepExpr&& operator=(XPathLocationStepExpr&&) = delete;
    void AddPredicate(XPathExpr* predicate);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    sngxml::dom::Axis axis;
    std::unique_ptr<XPathNodeTestExpr> nodeTest;
    std::vector<std::unique_ptr<XPathExpr>> predicates;
};

SNGXML_XPATH_API sngxml::dom::Axis GetAxis(const std::u32string& axisName);

class SNGXML_XPATH_API XPathPILiteralTest : public XPathNodeTestExpr
{
public:
    XPathPILiteralTest(XPathExpr* literal_);
    bool Select(sngxml::dom::Node* node, sngxml::dom::Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::unique_ptr<XPathExpr> literal;
};

class SNGXML_XPATH_API XPathCommentNodeTest : public XPathNodeTestExpr
{
public:
    bool Select(sngxml::dom::Node* node, sngxml::dom::Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathTextNodeTest : public XPathNodeTestExpr
{
public:
    bool Select(sngxml::dom::Node* node, sngxml::dom::Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathPINodeTest : public XPathNodeTestExpr
{
public:
    bool Select(sngxml::dom::Node* node, sngxml::dom::Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathAnyNodeTest : public XPathNodeTestExpr
{
public:
    bool Select(sngxml::dom::Node* node, sngxml::dom::Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathPrincipalNodeTest : public XPathNodeTestExpr
{
public:
    bool Select(sngxml::dom::Node* node, sngxml::dom::Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
};

class SNGXML_XPATH_API XPathPrefixTest : public XPathNodeTestExpr
{
public:
    XPathPrefixTest(const std::u32string& name_);
    bool Select(sngxml::dom::Node* node, sngxml::dom::Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string name;
};

class SNGXML_XPATH_API XPathNameTest : public XPathNodeTestExpr
{
public:
    XPathNameTest(const std::u32string& name_);
    bool Select(sngxml::dom::Node* node, sngxml::dom::Axis axis) const override;
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string name;
};

class SNGXML_XPATH_API XPathVariableReference : public XPathExpr
{
public:
    XPathVariableReference(const std::u32string& name_);
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string name;
};

class SNGXML_XPATH_API XPathLiteral : public XPathExpr
{
public:
    XPathLiteral(const std::u32string& value_);
    std::u32string TextValue() const override { return value; }
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string value;
};

class SNGXML_XPATH_API XPathNumberExpr : public XPathExpr
{
public:
    XPathNumberExpr(const std::u32string& value_);
    XPathNumberExpr(double value_);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    double value;
};

class SNGXML_XPATH_API XPathFunctionCall : public XPathExpr
{
public:
    XPathFunctionCall(const std::u32string& functionName_);
    void AddArgument(XPathExpr* argument);
    std::unique_ptr<XPathObject> Evaluate(XPathContext& context) override;
    std::unique_ptr<dom::Node> ToDom() const override;
private:
    std::u32string functionName;
    std::vector<std::unique_ptr<XPathExpr>> arguments;
};

} } // namespace sngxml::xpath

#endif // SNGXML_XPATH_XPATH_EXPR
