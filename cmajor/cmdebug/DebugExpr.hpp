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

class DEBUG_API DebugExprNode
{
public:
    DebugExprNode();
    virtual ~DebugExprNode();
    virtual void Accept(DebugExprVisitor& visitor) = 0;
};

class DEBUG_API VariableNameDebugExprNode : public DebugExprNode
{
public:
    VariableNameDebugExprNode(const std::string& identifier_);
    const std::string& Identifier() const { return identifier; }
    void Accept(DebugExprVisitor& visitor) override;
private:
    std::string identifier;
};

class DEBUG_API IntegerDebugExprNode : public DebugExprNode
{
public:
    IntegerDebugExprNode(int64_t value_);
    int64_t Value() const { return value; }
    void Accept(DebugExprVisitor& visitor) override;
private:
    int64_t value;
};

class DEBUG_API ParenthesizedDebugExprNode : public DebugExprNode
{
public:
    ParenthesizedDebugExprNode(DebugExprNode* exprNode_);
    DebugExprNode* ExprNode() const { return exprNode.get(); }
    void Accept(DebugExprVisitor& visitor) override;
private:
    std::unique_ptr<DebugExprNode> exprNode;
};

class DEBUG_API DerefDebugExprNode : public DebugExprNode
{
public:
    DerefDebugExprNode(DebugExprNode* operand_);
    DebugExprNode* Operand() const { return operand.get(); }
    void Accept(DebugExprVisitor& visitor) override;
private:
    std::unique_ptr<DebugExprNode> operand;
};

class DEBUG_API AddrOfDebugExprNode : public DebugExprNode
{
public:
    AddrOfDebugExprNode(DebugExprNode* operand_);
    DebugExprNode* Operand() const { return operand.get(); }
    void Accept(DebugExprVisitor& visitor) override;
private:
    std::unique_ptr<DebugExprNode> operand;
};

class DEBUG_API UnaryMinusDebugExprNode : public DebugExprNode
{
public:
    UnaryMinusDebugExprNode(DebugExprNode* operand_);
    DebugExprNode* Operand() const { return operand.get(); }
    void Accept(DebugExprVisitor& visitor) override;
private:
    std::unique_ptr<DebugExprNode> operand;
};

class DEBUG_API UnaryPlusDebugExprNode : public DebugExprNode
{
public:
    UnaryPlusDebugExprNode(DebugExprNode* operand_);
    DebugExprNode* Operand() const { return operand.get(); }
    void Accept(DebugExprVisitor& visitor) override;
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
private:
    std::unique_ptr<DebugExprNode> subject;
    std::unique_ptr<DebugExprNode> index;
};

class DEBUG_API TypeIdDebugExprNode : public DebugExprNode
{
public:
    TypeIdDebugExprNode(const std::string& typeId_);
    const std::string& TypeId() const { return typeId; }
    void Accept(DebugExprVisitor& visitor) override;
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
private:
    std::unique_ptr<TypeIdDebugExprNode> typeIdNode;
    std::unique_ptr<DebugExprNode> exprNode;
};

class DEBUG_API BaseDebugExprNode : public DebugExprNode
{
public:
    BaseDebugExprNode();
    void Accept(DebugExprVisitor& visitor) override;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_DEBUG_EXPR_INCLUDED
