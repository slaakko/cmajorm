// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Class.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/Identifier.hpp>

namespace sngcm { namespace ast {

ClassNode::ClassNode(const Span& span_, const boost::uuids::uuid& moduleId_) : 
    Node(NodeType::classNode, span_, moduleId_), specifiers(Specifiers::none), id(), templateParameters(), baseClassOrInterfaces(), members()
{
}

ClassNode::ClassNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, IdentifierNode* id_, AttributesNode* attributes_) :
    Node(NodeType::classNode, span_, moduleId_), specifiers(specifiers_), id(id_), templateParameters(), baseClassOrInterfaces(), members(), attributes(attributes_)
{
}

Node* ClassNode::Clone(CloneContext& cloneContext) const
{
    AttributesNode* clonedAttributes = nullptr;
    if (attributes)
    {
        clonedAttributes = static_cast<AttributesNode*>(attributes->Clone(cloneContext));
    }
    ClassNode* clone = new ClassNode(GetSpan(), ModuleId(), specifiers, static_cast<IdentifierNode*>(id->Clone(cloneContext)), clonedAttributes);
    if (!cloneContext.InstantiateClassNode())
    {
        int tn = templateParameters.Count();
        for (int i = 0; i < tn; ++i)
        {
            clone->AddTemplateParameter(static_cast<TemplateParameterNode*>(templateParameters[i]->Clone(cloneContext)));
        }
    }
    int bn = baseClassOrInterfaces.Count();
    for (int i = 0; i < bn; ++i)
    {
        clone->AddBaseClassOrInterface(baseClassOrInterfaces[i]->Clone(cloneContext));
    }
    if (constraint)
    {
        clone->SetConstraint(static_cast<WhereConstraintNode*>(constraint->Clone(cloneContext)));
    }
    int mn = members.Count();
    for (int i = 0; i < mn; ++i)
    {
        clone->AddMember(members[i]->Clone(cloneContext));
    }
    clone->SetSpecifierSpan(specifierSpan);
    clone->SetClassSpan(classSpan);
    clone->SetBeginBraceSpan(beginBraceSpan);
    clone->SetEndBraceSpan(endBraceSpan);
    return clone;
}

void ClassNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ClassNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    bool hasAttributes = attributes != nullptr;
    writer.GetBinaryWriter().Write(hasAttributes);
    if (hasAttributes)
    {
        writer.Write(attributes.get());
    }
    writer.Write(specifiers);
    writer.Write(id.get());
    templateParameters.Write(writer);
    baseClassOrInterfaces.Write(writer);
    members.Write(writer);
    bool convertExternal = ModuleId() == writer.SpanConversionModuleId();
    writer.Write(specifierSpan, convertExternal);
    writer.Write(classSpan, convertExternal);
    writer.Write(beginBraceSpan, convertExternal);
    writer.Write(endBraceSpan, convertExternal);
}

void ClassNode::Read(AstReader& reader)
{
    Node::Read(reader);
    bool hasAttributes = reader.GetBinaryReader().ReadBool();
    if (hasAttributes)
    {
        attributes.reset(reader.ReadAttributesNode());
    }
    specifiers = reader.ReadSpecifiers();
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    templateParameters.Read(reader);
    templateParameters.SetParent(this);
    baseClassOrInterfaces.Read(reader);
    baseClassOrInterfaces.SetParent(this);
    members.Read(reader);
    members.SetParent(this);
    specifierSpan = reader.ReadSpan();
    classSpan = reader.ReadSpan();
    beginBraceSpan = reader.ReadSpan();
    endBraceSpan = reader.ReadSpan();
}

void ClassNode::AddTemplateParameter(TemplateParameterNode* templateParameter)
{
    templateParameter->SetParent(this);
    templateParameters.Add(templateParameter);
}

void ClassNode::AddBaseClassOrInterface(Node* baseClassOrInterface)
{
    baseClassOrInterface->SetParent(this);
    baseClassOrInterfaces.Add(baseClassOrInterface);
}

void ClassNode::SetConstraint(WhereConstraintNode* whereConstraint)
{
    constraint.reset(whereConstraint);
    constraint->SetParent(this);
}

void ClassNode::AddMember(Node* member)
{
    member->SetParent(this);
    members.Add(member);
}

void ClassNode::ArrangeMembers()
{
    std::unique_ptr<Node> staticConstructor;
    NodeList<Node> constructors;
    std::unique_ptr<Node> destructor;
    NodeList<Node> virtualMembers;
    NodeList<Node> protectedMembers;
    NodeList<Node> privateMembers;
    int n = members.Count();
    for (int i = 0; i < n; ++i)
    {
        Node* member = members[i];
        if (member->GetNodeType() == NodeType::staticConstructorNode)
        {
            staticConstructor.reset(members.Release(i));
        }
        else if (member->GetNodeType() == NodeType::constructorNode)
        {
            constructors.Add(members.Release(i));
        }
        else if (member->GetNodeType() == NodeType::destructorNode)
        {
            destructor.reset(members.Release(i));
        }
        else
        {
            Specifiers specifiers = member->GetSpecifiers();
            if ((specifiers & (Specifiers::abstract_ | Specifiers::override_ | Specifiers::virtual_)) != Specifiers::none)
            {
                virtualMembers.Add(members.Release(i));
            }
            else if ((specifiers & Specifiers::protected_) != Specifiers::none)
            {
                protectedMembers.Add(members.Release(i));
            }
            else if ((specifiers & Specifiers::private_) != Specifiers::none)
            {
                privateMembers.Add(members.Release(i));
            }
        }
    }
    members.RemoveEmpty();
    int index = 0;
    if (staticConstructor)
    {
        members.Insert(index, staticConstructor.release());
        ++index;
    }
    int nc = constructors.Count();
    if (nc > 0)
    {
        for (int i = 0; i < nc; ++i)
        {
            members.Insert(index, constructors.Release(i));
            ++index;
        }
    }
    if (destructor)
    {
        members.Insert(index, destructor.release());
        ++index;
    }
    if (virtualMembers.Count() > 0 || protectedMembers.Count() > 0 || privateMembers.Count() > 0)
    {
        for (int i = 0; i < virtualMembers.Count(); ++i)
        {
            members.Add(virtualMembers.Release(i));
        }
        for (int i = 0; i < protectedMembers.Count(); ++i)
        {
            members.Add(protectedMembers.Release(i));
        }
        for (int i = 0; i < privateMembers.Count(); ++i)
        {
            members.Add(privateMembers.Release(i));
        }
    }
}

InitializerNode::InitializerNode(NodeType nodeType_, const Span& span_, const boost::uuids::uuid& moduleId_) : Node(nodeType_, span_, moduleId_)
{
}

void InitializerNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    arguments.Write(writer);
}

void InitializerNode::Read(AstReader& reader)
{
    Node::Read(reader);
    arguments.Read(reader);
    arguments.SetParent(this);
}

void InitializerNode::AddArgument(Node* argument)
{
    argument->SetParent(this);
    arguments.Add(argument);
}

ThisInitializerNode::ThisInitializerNode(const Span& span_, const boost::uuids::uuid& moduleId_) : InitializerNode(NodeType::thisInitializerNode, span_, moduleId_)
{
}

Node* ThisInitializerNode::Clone(CloneContext& cloneContext) const
{
    ThisInitializerNode* clone = new ThisInitializerNode(GetSpan(), ModuleId());
    int n = Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        clone->AddArgument(Arguments()[i]->Clone(cloneContext));
    }
    return clone;
}

void ThisInitializerNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BaseInitializerNode::BaseInitializerNode(const Span& span_, const boost::uuids::uuid& moduleId_) : InitializerNode(NodeType::baseInitializerNode, span_, moduleId_)
{
}

Node* BaseInitializerNode::Clone(CloneContext& cloneContext) const
{
    BaseInitializerNode* clone = new BaseInitializerNode(GetSpan(), ModuleId());
    int n = Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        clone->AddArgument(Arguments()[i]->Clone(cloneContext));
    }
    return clone;
}

void BaseInitializerNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

MemberInitializerNode::MemberInitializerNode(const Span& span_, const boost::uuids::uuid& moduleId_) : InitializerNode(NodeType::memberInitializerNode, span_, moduleId_), memberId()
{
}

MemberInitializerNode::MemberInitializerNode(const Span& span_, const boost::uuids::uuid& moduleId_, IdentifierNode* memberId_) : 
    InitializerNode(NodeType::memberInitializerNode, span_, moduleId_), memberId(memberId_)
{
    memberId->SetParent(this);
}

Node* MemberInitializerNode::Clone(CloneContext& cloneContext) const
{
    MemberInitializerNode* clone = new MemberInitializerNode(GetSpan(), ModuleId(), static_cast<IdentifierNode*>(memberId->Clone(cloneContext)));
    int n = Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        clone->AddArgument(Arguments()[i]->Clone(cloneContext));
    }
    return clone;
}

void MemberInitializerNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void MemberInitializerNode::Write(AstWriter& writer)
{
    InitializerNode::Write(writer);
    writer.Write(memberId.get());
}

void MemberInitializerNode::Read(AstReader& reader)
{
    InitializerNode::Read(reader);
    memberId.reset(reader.ReadIdentifierNode());
    memberId->SetParent(this);
}

StaticConstructorNode::StaticConstructorNode(const Span& span_, const boost::uuids::uuid& moduleId_) : 
    FunctionNode(NodeType::staticConstructorNode, span_, moduleId_, Specifiers::none, nullptr, U"@static_constructor", nullptr), initializers()
{
}

StaticConstructorNode::StaticConstructorNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, AttributesNode* attributes_) :
    FunctionNode(NodeType::staticConstructorNode, span_, moduleId_, specifiers_, nullptr, U"@static_constructor", attributes_), initializers()
{
}

Node* StaticConstructorNode::Clone(CloneContext& cloneContext) const
{
    StaticConstructorNode* clone = new StaticConstructorNode(GetSpan(), ModuleId(), GetSpecifiers(), nullptr);
    int n = initializers.Count();
    for (int i = 0; i < n; ++i)
    {
        clone->AddInitializer(static_cast<InitializerNode*>(initializers[i]->Clone(cloneContext)));
    }
    clone->classId.reset(static_cast<IdentifierNode*>(classId->Clone(cloneContext)));
    CloneContent(clone, cloneContext);
    return clone;
}

void StaticConstructorNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void StaticConstructorNode::Write(AstWriter& writer)
{
    FunctionNode::Write(writer);
    initializers.Write(writer);
    writer.Write(classId.get());
}

void StaticConstructorNode::Read(AstReader& reader)
{
    FunctionNode::Read(reader);
    initializers.Read(reader);
    initializers.SetParent(this);
    classId.reset(reader.ReadIdentifierNode());
}

void StaticConstructorNode::AddInitializer(InitializerNode* initializer)
{
    initializer->SetParent(this);
    initializers.Add(initializer);
}

ConstructorNode::ConstructorNode(const Span& span_, const boost::uuids::uuid& moduleId_) : FunctionNode(NodeType::constructorNode, span_, moduleId_, Specifiers::none, nullptr, U"@constructor", nullptr), initializers()
{
}

ConstructorNode::ConstructorNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, AttributesNode* attributes_) : FunctionNode(NodeType::constructorNode, span_, moduleId_, specifiers_, nullptr, U"@constructor", attributes_), initializers()
{
}

Node* ConstructorNode::Clone(CloneContext& cloneContext) const
{
    ConstructorNode* clone = new ConstructorNode(GetSpan(), ModuleId(), GetSpecifiers(), nullptr);
    int n = initializers.Count();
    for (int i = 0; i < n; ++i)
    {
        clone->AddInitializer(static_cast<InitializerNode*>(initializers[i]->Clone(cloneContext)));
    }
    clone->classId.reset(static_cast<IdentifierNode*>(classId->Clone(cloneContext)));
    CloneContent(clone, cloneContext);
    return clone;
}

void ConstructorNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ConstructorNode::Write(AstWriter& writer)
{
    FunctionNode::Write(writer);
    initializers.Write(writer);
    writer.Write(classId.get());
}

void ConstructorNode::Read(AstReader& reader)
{
    FunctionNode::Read(reader);
    initializers.Read(reader);
    initializers.SetParent(this);
    classId.reset(reader.ReadIdentifierNode());
}

void ConstructorNode::AddInitializer(InitializerNode* initializer)
{
    initializer->SetParent(this);
    initializers.Add(initializer);
}

DestructorNode::DestructorNode(const Span& span_, const boost::uuids::uuid& moduleId_) : FunctionNode(NodeType::destructorNode, span_, moduleId_)
{
}

DestructorNode::DestructorNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, AttributesNode* attributes_) : 
    FunctionNode(NodeType::destructorNode, span_, moduleId_, specifiers_, nullptr, U"@destructor", attributes_)
{
}

Node* DestructorNode::Clone(CloneContext& cloneContext) const
{
    DestructorNode* clone = new DestructorNode(GetSpan(), ModuleId(), GetSpecifiers(), nullptr);
    clone->classId.reset(static_cast<IdentifierNode*>(classId->Clone(cloneContext)));
    CloneContent(clone, cloneContext);
    return clone;
}

void DestructorNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void DestructorNode::Write(AstWriter& writer)
{
    FunctionNode::Write(writer);
    writer.Write(classId.get());
}

void DestructorNode::Read(AstReader& reader)
{
    FunctionNode::Read(reader);
    classId.reset(reader.ReadIdentifierNode());
}

MemberFunctionNode::MemberFunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_) : FunctionNode(NodeType::memberFunctionNode, span_, moduleId_)
{
}

MemberFunctionNode::MemberFunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, Node* returnTypeExpr_, const std::u32string& groupId_, AttributesNode* attributes_) :
    FunctionNode(NodeType::memberFunctionNode, span_, moduleId_, specifiers_, returnTypeExpr_, groupId_, attributes_)
{
}

Node* MemberFunctionNode::Clone(CloneContext& cloneContext) const
{
    MemberFunctionNode* clone = new MemberFunctionNode(GetSpan(), ModuleId());
    CloneContent(clone, cloneContext);
    if (IsConst())
    {
        clone->SetConst();
    }
    return clone;
}

void MemberFunctionNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ConversionFunctionNode::ConversionFunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_) : FunctionNode(NodeType::conversionFunctionNode, span_, moduleId_)
{
}

ConversionFunctionNode::ConversionFunctionNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, Node* returnTypeExpr_, AttributesNode* attributes_) :
    FunctionNode(NodeType::conversionFunctionNode, span_, moduleId_, specifiers_, returnTypeExpr_, U"@operator_conv", attributes_)
{
}

Node* ConversionFunctionNode::Clone(CloneContext& cloneContext) const
{
    ConversionFunctionNode* clone = new ConversionFunctionNode(GetSpan(), ModuleId());
    CloneContent(clone, cloneContext);
    if (IsConst())
    {
        clone->SetConst();
    }
    return clone;
}

void ConversionFunctionNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

MemberVariableNode::MemberVariableNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::memberVariableNode, span_, moduleId_), specifiers()
{
}

MemberVariableNode::MemberVariableNode(const Span& span_, const boost::uuids::uuid& moduleId_, Specifiers specifiers_, Node* typeExpr_, IdentifierNode* id_, AttributesNode* attributes_) :
    Node(NodeType::memberVariableNode, span_, moduleId_), specifiers(specifiers_), typeExpr(typeExpr_), id(id_), attributes(attributes_)
{
    typeExpr->SetParent(this);
    id->SetParent(this);
}

Node* MemberVariableNode::Clone(CloneContext& cloneContext) const
{
    AttributesNode* clonedAttributes = nullptr;
    if (attributes)
    {
        clonedAttributes = static_cast<AttributesNode*>(attributes->Clone(cloneContext));
    }
    MemberVariableNode* clone =  new MemberVariableNode(GetSpan(), ModuleId(), specifiers, typeExpr->Clone(cloneContext), static_cast<IdentifierNode*>(id->Clone(cloneContext)), clonedAttributes);
    clone->SetSpecifierSpan(specifierSpan);
    return clone;
}

void MemberVariableNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void MemberVariableNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    bool hasAttributes = attributes != nullptr;
    writer.GetBinaryWriter().Write(hasAttributes);
    if (hasAttributes)
    {
        writer.Write(attributes.get());
    }
    writer.Write(specifiers);
    writer.Write(typeExpr.get());
    writer.Write(id.get());
    bool convertExternal = ModuleId() == writer.SpanConversionModuleId();
    writer.Write(specifierSpan, convertExternal);
}

void MemberVariableNode::Read(AstReader& reader)
{
    Node::Read(reader);
    bool hasAttributes = reader.GetBinaryReader().ReadBool();
    if (hasAttributes)
    {
        attributes.reset(reader.ReadAttributesNode());
    }
    specifiers = reader.ReadSpecifiers();
    typeExpr.reset(reader.ReadNode());
    typeExpr->SetParent(this);
    id.reset(reader.ReadIdentifierNode());
    id->SetParent(this);
    specifierSpan = reader.ReadSpan();
}

} } // namespace sngcm::ast
