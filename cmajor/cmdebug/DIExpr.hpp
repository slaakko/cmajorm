// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_DIEXPR_INCLUDED
#define CMAJOR_DEBUG_DIEXPR_INCLUDED
#include <cmajor/cmdebug/DebugApi.hpp>
#include <memory>
#include <string>

namespace cmajor { namespace debug {

class DIType;
class DIVariable;
class DebugInfo;
class Project;

class DEBUG_API DINode
{
public:
    DINode(DIType* type_);
    virtual ~DINode();
    virtual DINode* Clone() const = 0;
    DIType* Type() const { return type; }
    void SetType(DIType* type_) { type = type_; }
    virtual bool IsTypeIdNode() const { return false; }
    virtual std::string ToString() const = 0;
    virtual std::string GdbExprString() const = 0;
private:
    DIType* type;
};

class DEBUG_API DIVariableReferenceNode : public DINode
{
public:
    DIVariableReferenceNode(DIVariable* variable_);
    std::string ToString() const override;
    DINode* Clone() const override;
    DIVariable* Variable() const { return variable; }
    std::string GdbExprString() const override;
private:
    DIVariable* variable;
};

class DEBUG_API DIIntegerLiteralNode : public DINode
{
public:
    DIIntegerLiteralNode(Project* project_, int64_t value_);
    std::string ToString() const override;
    DINode* Clone() const override;
    std::string GdbExprString() const override;
private:
    Project* project;
    int64_t value;
};

class DEBUG_API DIDerefNode : public DINode
{
public:
    DIDerefNode(DIType* type, DINode* subject_);
    std::string ToString() const override;
    DINode* Clone() const override;
    DINode* Subject() const { return subject.get(); }
    std::string GdbExprString() const override;
private:
    std::unique_ptr<DINode> subject;
};

class DEBUG_API DIAddrOfNode : public DINode
{
public:
    DIAddrOfNode(DIType* type, DINode* subject_);
    std::string ToString() const override;
    DINode* Clone() const override;
    DINode* Subject() const { return subject.get(); }
    std::string GdbExprString() const override;
private:
    std::unique_ptr<DINode> subject;
};

class DEBUG_API DIUnaryMinusNode : public DINode
{
public:
    DIUnaryMinusNode(DINode* subject_);
    std::string ToString() const override;
    DINode* Clone() const override;
    DINode* Subject() const { return subject.get(); }
    std::string GdbExprString() const override;
private:
    std::unique_ptr<DINode> subject;
};

class DEBUG_API DIUnaryPlusNode : public DINode
{
public:
    DIUnaryPlusNode(DINode* subject_);
    std::string ToString() const override;
    DINode* Clone() const override;
    DINode* Subject() const { return subject.get(); }
    std::string GdbExprString() const override;
private:
    std::unique_ptr<DINode> subject;
};

class DEBUG_API DIDotNode : public DINode
{
public:
    DIDotNode(DIType* type_, DINode* subject_, const std::string& member_, bool translateMember_);
    std::string ToString() const override;
    DINode* Clone() const override;
    DINode* Subject() const { return subject.get(); }
    std::string GdbExprString() const override { return gdbExprString; }
private:
    DIType* type;
    std::unique_ptr<DINode> subject;
    std::string member;
    bool translateMember;
    std::string gdbExprString;
};

class DEBUG_API DISubscriptNode : public DINode
{
public:
    DISubscriptNode(DINode* subject_, DINode* index_);
    std::string ToString() const override;
    DINode* Clone() const override;
    DINode* Subject() const { return subject.get(); }
    DINode* Index() const { return index.get(); }
    std::string GdbExprString() const override;
private:
    std::unique_ptr<DINode> subject;
    std::unique_ptr<DINode> index;
};

class DEBUG_API DIParenExprNode : public DINode
{
public:
    DIParenExprNode(DINode* subject_);
    std::string ToString() const override;
    DINode* Clone() const override;
    DINode* Subject() const { return subject.get(); }
    std::string GdbExprString() const override;
private:
    std::unique_ptr<DINode> subject;
};

class DEBUG_API DITypeIdNode : public DINode
{
public:
    DITypeIdNode(DebugInfo* debugInfo_, const std::string& typeId_);
    bool IsTypeIdNode() const override { return true; }
    std::string ToString() const override;
    DINode* Clone() const override;
    const std::string& TypeId() const { return typeId; }
    std::string GdbExprString() const override;
private:
    DebugInfo* debugInfo;
    std::string typeId;
};

class DEBUG_API DICastNode : public DINode
{
public:
    DICastNode(DINode* typeIdNode_, DINode* exprNode_);
    std::string ToString() const override;
    DINode* Clone() const override;
    std::string GdbExprString() const override;
private:
    std::unique_ptr<DINode> typeIdNode;
    std::unique_ptr<DINode> exprNode;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_DIEXPR_INCLUDED
