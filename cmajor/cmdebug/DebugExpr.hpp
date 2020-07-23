// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_DEBUG_EXPR_INCLUDED
#define CMAJOR_DEBUG_DEBUG_EXPR_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <memory>
#include <string>

namespace cmajor { namespace debug {

class DebugExprVisitor;

enum class Operator
{
    plus, minus
};

class DEBUG_API DebugExprNode
{
public:
    enum class Kind : int8_t
    {
        address, identifier, integer, parenExpr, add, sub, deref, addrOf, dot, subscript, range, typeId, cast_, var
    };
    DebugExprNode(Kind kind_);
    virtual ~DebugExprNode();
    virtual void Accept(DebugExprVisitor& visitor) = 0;
    virtual std::string ToString() const = 0;
    Kind GetKind() const { return kind; }
private:
    Kind kind;
};

class DEBUG_API AddressDebugExprNode : public DebugExprNode
{
public:
    AddressDebugExprNode(const std::string& value_);
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
    const std::string& Value() const { return value; }
private:
    std::string value;
};

class DEBUG_API IdentifierDebugExprNode : public DebugExprNode
{
public:
    IdentifierDebugExprNode(const std::string& identifier_);
    const std::string& Identifier() const { return identifier; }
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
private:
    std::string identifier;
};

class DEBUG_API IntegerDebugExprNode : public DebugExprNode
{
public:
    IntegerDebugExprNode(int64_t value_);
    int64_t Value() const { return value; }
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
private:
    int64_t value;
};

class DEBUG_API AddDebugExprNode : public DebugExprNode
{
public:
    AddDebugExprNode(DebugExprNode* left_, DebugExprNode* right_);
    DebugExprNode* Left() const { return left.get(); }
    DebugExprNode* Right() const { return right.get(); }
private:
    std::unique_ptr<DebugExprNode> left;
    std::unique_ptr<DebugExprNode> right;
};

class DEBUG_API SubDebugExprNode : public DebugExprNode
{
public:
    SubDebugExprNode(DebugExprNode* left_, DebugExprNode* right_);
    DebugExprNode* Left() const { return left.get(); }
    DebugExprNode* Right() const { return right.get(); }
private:
    std::unique_ptr<DebugExprNode> left;
    std::unique_ptr<DebugExprNode> right;
};

class DEBUG_API ParenthesizedDebugExprNode : public DebugExprNode
{
public:
    ParenthesizedDebugExprNode(DebugExprNode* exprNode_);
    DebugExprNode* ExprNode() const { return exprNode.get(); }
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
private:
    std::unique_ptr<DebugExprNode> exprNode;
};

class DEBUG_API DerefDebugExprNode : public DebugExprNode
{
public:
    DerefDebugExprNode(DebugExprNode* operand_);
    DebugExprNode* Operand() const { return operand.get(); }
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
private:
    std::unique_ptr<DebugExprNode> operand;
};

class DEBUG_API AddrOfDebugExprNode : public DebugExprNode
{
public:
    AddrOfDebugExprNode(DebugExprNode* operand_);
    DebugExprNode* Operand() const { return operand.get(); }
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
private:
    std::unique_ptr<DebugExprNode> operand;
};

class DEBUG_API DotDebugExprNode : public DebugExprNode
{
public:
    DotDebugExprNode(DebugExprNode* subject_, const std::string& member_);
    DebugExprNode* Subject() const { return subject.get(); }
    const std::string& Member() const { return member; }
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
private:
    std::unique_ptr<DebugExprNode> subject;
    std::string member;
};

class DEBUG_API SubscriptDebugExprNode : public DebugExprNode
{
public:
    SubscriptDebugExprNode(DebugExprNode* subject_, DebugExprNode* index_);
    DebugExprNode* Subject() const { return subject.get(); }
    DebugExprNode* Index() const { return index.get(); }
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
private:
    std::unique_ptr<DebugExprNode> subject;
    std::unique_ptr<DebugExprNode> index;
};

class DEBUG_API RangeDebugExprNode : public DebugExprNode
{
public:
    RangeDebugExprNode(DebugExprNode* subject_, DebugExprNode* rangeStart_, DebugExprNode* rangeEnd_);
    DebugExprNode* Subject() const { return subject.get(); }
    DebugExprNode* RangeStart() const { return rangeStart.get(); }
    DebugExprNode* RangeEnd() const { return rangeEnd.get(); }
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
private:
    std::unique_ptr<DebugExprNode> subject;
    std::unique_ptr<DebugExprNode> rangeStart;
    std::unique_ptr<DebugExprNode> rangeEnd;
};

class DEBUG_API TypeIdDebugExprNode : public DebugExprNode
{
public:
    TypeIdDebugExprNode(const std::string& typeId_);
    const std::string& TypeId() const { return typeId; }
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
private:
    std::string typeId;
};

class DEBUG_API CastDebugExprNode : public DebugExprNode
{
public:
    CastDebugExprNode(TypeIdDebugExprNode* typeIdNode_, DebugExprNode* exprNode_);
    TypeIdDebugExprNode* TypeIdNode() const { return typeIdNode.get(); }
    DebugExprNode* ExprNode() const { return exprNode.get(); }
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
private:
    std::unique_ptr<TypeIdDebugExprNode> typeIdNode;
    std::unique_ptr<DebugExprNode> exprNode;
};

class DEBUG_API DebuggerVarExprNode : public DebugExprNode
{
public:
    DebuggerVarExprNode(int variableIndex_);
    int VariableIndex() const { return variableIndex; }
    void Accept(DebugExprVisitor& visitor) override;
    std::string ToString() const override;
private:
    int variableIndex;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_DEBUG_EXPR_INCLUDED
