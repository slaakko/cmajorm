// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ast/Function.hpp>
#include <cmajor/ast/Visitor.hpp>

namespace cmajor { namespace ast {

FunctionNode::FunctionNode(const Span& span_) : Node(NodeType::functionNode, span_), specifiers(Specifiers::none), returnTypeExpr(), groupId(), parameters(), body(), bodySource(), programMain(false)
{
}

FunctionNode::FunctionNode(NodeType nodeType_, const Span& span_) : Node(nodeType_, span_), specifiers(Specifiers::none), returnTypeExpr(), groupId(), parameters(), body(), bodySource(), programMain(false)
{
}

FunctionNode::FunctionNode(const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, const std::u32string& groupId_, Attributes* attributes_) :
    Node(NodeType::functionNode, span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), groupId(groupId_), templateParameters(), parameters(), body(), bodySource(), attributes(attributes_), programMain(false)
{
    if (returnTypeExpr)
    {
        returnTypeExpr->SetParent(this);
    }
}

FunctionNode::FunctionNode(NodeType nodeType_, const Span& span_, Specifiers specifiers_, Node* returnTypeExpr_, const std::u32string& groupId_, Attributes* attributes_) :
    Node(nodeType_, span_), specifiers(specifiers_), returnTypeExpr(returnTypeExpr_), groupId(groupId_), templateParameters(), parameters(), body(), bodySource(), attributes(attributes_), programMain(false)
{
    if (returnTypeExpr)
    {
        returnTypeExpr->SetParent(this);
    }
}

Node* FunctionNode::Clone(CloneContext& cloneContext) const
{
    Attributes* clonedAttributes = nullptr;
    if (attributes)
    {
        clonedAttributes = attributes->Clone();
    }
    Node* clonedReturnTypeExpr = nullptr;
    if (returnTypeExpr)
    {
        clonedReturnTypeExpr = returnTypeExpr->Clone(cloneContext);
    }
    FunctionNode* clone = new FunctionNode(GetSpan(), specifiers, clonedReturnTypeExpr, groupId, clonedAttributes);
    if (!cloneContext.InstantiateFunctionNode())
    {
        int nt = templateParameters.Count();
        for (int i = 0; i < nt; ++i)
        {
            clone->AddTemplateParameter(static_cast<TemplateParameterNode*>(templateParameters[i]->Clone(cloneContext)));
        }
    }
    int np = parameters.Count();
    for (int i = 0; i < np; ++i)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameters[i]->Clone(cloneContext)));
    }
    if (body)
    {
        if (cloneContext.InstantiateClassNode())
        {
            CloneContext bodyCloneContext;
            clone->SetBodySource(static_cast<CompoundStatementNode*>(body->Clone(bodyCloneContext)));
        }
        else
        {
            clone->SetBody(static_cast<CompoundStatementNode*>(body->Clone(cloneContext)));
        }
    }
    if (whereConstraint)
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(whereConstraint->Clone(cloneContext)));
    }
    clone->SetGroupIdSpan(groupIdSpan);
    clone->SetSpecifierSpan(specifierSpan);
    return clone;
}

void FunctionNode::CloneContent(FunctionNode* clone, CloneContext& cloneContext) const
{
    if (attributes)
    {
        clone->attributes.reset(attributes->Clone());
    }
    clone->specifiers = specifiers;
    Node* clonedReturnTypeExpr = nullptr;
    if (returnTypeExpr)
    {
        clone->SetReturnTypeExpr(returnTypeExpr->Clone(cloneContext));
    }
    clone->groupId = groupId;
    int nt = templateParameters.Count();
    for (int i = 0; i < nt; ++i)
    {
        clone->AddTemplateParameter(static_cast<TemplateParameterNode*>(templateParameters[i]->Clone(cloneContext)));
    }
    int np = parameters.Count();
    for (int i = 0; i < np; ++i)
    {
        clone->AddParameter(static_cast<ParameterNode*>(parameters[i]->Clone(cloneContext)));
    }
    if (body)
    {
        if (cloneContext.InstantiateClassNode())
        {
            CloneContext bodyCloneContext;
            clone->SetBodySource(static_cast<CompoundStatementNode*>(body->Clone(bodyCloneContext)));
        }
        else
        {
            clone->SetBody(static_cast<CompoundStatementNode*>(body->Clone(cloneContext)));
        }
    }
    if (whereConstraint)
    { 
        clone->SetConstraint(static_cast<WhereConstraintNode*>(whereConstraint->Clone(cloneContext)));
    }
    clone->SetGroupIdSpan(groupIdSpan);
    clone->SetSpecifierSpan(specifierSpan);
}

void FunctionNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void FunctionNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    bool hasAttributes = attributes != nullptr;
    writer.GetBinaryWriter().Write(hasAttributes);
    if (hasAttributes)
    {
        attributes->Write(writer);
    }
    writer.Write(specifiers);
    bool hasReturnTypeExpr = returnTypeExpr != nullptr;
    writer.GetBinaryWriter().Write(hasReturnTypeExpr);
    if (hasReturnTypeExpr)
    {
        writer.Write(returnTypeExpr.get());
    }
    writer.GetBinaryWriter().Write(groupId);
    templateParameters.Write(writer);
    parameters.Write(writer);
    bool hasConstraint = whereConstraint != nullptr;
    writer.GetBinaryWriter().Write(hasConstraint);
    if (hasConstraint)
    {
        writer.Write(whereConstraint.get());
    }
    bool hasBody = body != nullptr;
    writer.GetBinaryWriter().Write(hasBody);
    if (hasBody)
    {
        writer.Write(body.get());
    }
    bool hasBodySource = bodySource != nullptr;
    writer.GetBinaryWriter().Write(hasBodySource);
    if (hasBodySource)
    {
        writer.Write(bodySource.get());
    }
    writer.Write(groupIdSpan);
    writer.Write(specifierSpan);
}

void FunctionNode::Read(AstReader& reader)
{
    Node::Read(reader);
    bool hasAttributes = reader.GetBinaryReader().ReadBool();
    if (hasAttributes)
    {
        attributes.reset(new Attributes());
        attributes->Read(reader);
    }
    specifiers = reader.ReadSpecifiers();
    bool hasReturnTypeExpr = reader.GetBinaryReader().ReadBool();
    if (hasReturnTypeExpr)
    {
        returnTypeExpr.reset(reader.ReadNode());
        returnTypeExpr->SetParent(this);
    }
    groupId = reader.GetBinaryReader().ReadUtf32String();
    templateParameters.Read(reader);
    templateParameters.SetParent(this);
    parameters.Read(reader);
    parameters.SetParent(this);
    bool hasConstraint = reader.GetBinaryReader().ReadBool();;
    if (hasConstraint)
    {
        whereConstraint.reset(reader.ReadWhereConstraintNode());
    }
    bool hasBody = reader.GetBinaryReader().ReadBool();
    if (hasBody)
    {
        body.reset(reader.ReadCompoundStatementNode());
        body->SetParent(this);
    }
    bool hasBodySource = reader.GetBinaryReader().ReadBool();
    if (hasBodySource)
    {
        bodySource.reset(reader.ReadCompoundStatementNode());
        bodySource->SetParent(this);
    }
    groupIdSpan = reader.ReadSpan();
    specifierSpan = reader.ReadSpan();
}

void FunctionNode::AddTemplateParameter(TemplateParameterNode* templateParameter)
{
    templateParameter->SetParent(this);
    templateParameters.Add(templateParameter);
}

void FunctionNode::AddParameter(ParameterNode* parameter)
{
    parameter->SetParent(this);
    parameters.Add(parameter);
}

void FunctionNode::SwitchToBody()
{
    if (bodySource && !body)
    {
        SetBody(bodySource.release());
    }
}

void FunctionNode::SetReturnTypeExpr(Node* returnTypeExpr_)
{
    returnTypeExpr.reset(returnTypeExpr_);
    returnTypeExpr->SetParent(returnTypeExpr_);
}

void FunctionNode::SetSpecifiers(Specifiers specifiers_)
{
    specifiers = specifiers_;
}

void FunctionNode::SetConstraint(WhereConstraintNode* whereConstraint_)
{
    whereConstraint.reset(whereConstraint_);
    whereConstraint->SetParent(this);
}

void FunctionNode::SetBody(CompoundStatementNode* body_)
{
    body.reset(body_);
    body->SetParent(this);
}

void FunctionNode::SetBodySource(CompoundStatementNode* bodySource_)
{
    bodySource.reset(bodySource_);
    bodySource->SetParent(this);
}

} } // namespace cmajor::ast
