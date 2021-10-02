// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlSerAst.hpp>
#include <sngxml/serialization/XmlSerVisitor.hpp>

namespace sngxml { namespace xmlser {

Node::~Node()
{
}

IncludeDirectiveNode::IncludeDirectiveNode(const std::string& fileTag_, const std::string& filePath_) : fileTag(fileTag_), filePath(filePath_)
{
}

void IncludeDirectiveNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ForwardClassDeclarationNode::ForwardClassDeclarationNode(const std::string& classId_) : classId(classId_)
{
}

void ForwardClassDeclarationNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void BoolNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void SByteNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ByteNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ShortNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void UShortNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void IntNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void UIntNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void LongNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ULongNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void FloatNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void DoubleNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void CharNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void WCharNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void UCharNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void UuidNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void StringNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void WStringNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void UStringNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void DateNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void DateTimeNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void TimestampNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void TimePointNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void DurationNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ClassIdNode::ClassIdNode(const std::string& classId_) : classId(classId_)
{
}

void ClassIdNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

PointerNode::PointerNode(TypeNode* type_) : type(type_)
{
}

void PointerNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ArrayNode::ArrayNode(TypeNode* type_) : type(type_)
{
}

void ArrayNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

TemplateIdNode::TemplateIdNode(const std::string& typeId_, const std::string& typeParamId_) : typeId(typeId_), typeParamId(typeParamId_)
{
}

void TemplateIdNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

MemberVariableNode::MemberVariableNode(TypeNode* type_, const std::string& id_) : type(type_), id(id_)
{
}

void MemberVariableNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

NamespaceNode::NamespaceNode(const std::string& id_) : id(id_)
{
}

void NamespaceNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void NamespaceNode::AddNode(Node* node)
{
    nodes.push_back(std::unique_ptr<Node>(node));
}

bool NamespaceNode::ContainsNamespaces() const
{
    for (const std::unique_ptr<Node>& node : nodes)
    {
        if (node->IsNamespaceNode() || node->ContainsNamespaces()) return true;
    }
    return false;
}

ClassNode::ClassNode(Key key_, const std::string& api_, const std::string& id_) : key(key_), api(api_), id(id_)
{
}

void ClassNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void ClassNode::SetBaseClassId(const std::string& baseClassId_)
{
    baseClassId = baseClassId_;
}

void ClassNode::AddMemberVariable(MemberVariableNode* memberVariable)
{
    nodes.push_back(std::unique_ptr<Node>(memberVariable));
}

void ClassNode::AddCppBlock(CppBlockNode* cppBlock)
{
    nodes.push_back(std::unique_ptr<Node>(cppBlock));
}

EnumConstantNode::EnumConstantNode(const std::string& id_) : id(id_)
{
}

void EnumConstantNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

EnumTypeNode::EnumTypeNode(const std::string& id_) : id(id_)
{
}

void EnumTypeNode::AddEnumConstant(EnumConstantNode* enumConstant)
{
    enumConstants.push_back(std::unique_ptr<EnumConstantNode>(enumConstant));
}

void EnumTypeNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SourceFileNode::SourceFileNode() : globalNs(std::string())
{
}

NamespaceNode* SourceFileNode::GlobalNs()
{
    return &globalNs;
}

CppBlockNode::CppBlockNode(const std::string& cppText_) : cppText(cppText_), source(false)
{
}

void CppBlockNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void SourceFileNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void SourceFileNode::AddIncludeDirective(IncludeDirectiveNode* includeDirectiveNode)
{
    includeDirectives.push_back(std::unique_ptr<IncludeDirectiveNode>(includeDirectiveNode));
}

} } // namespace sngxml::xmlser
