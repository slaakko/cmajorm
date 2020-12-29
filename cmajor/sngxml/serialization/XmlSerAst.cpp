// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlSerAst.hpp>
#include <sngxml/serialization/XmlSerVisitor.hpp>

namespace sngxml { namespace xmlser {

Node::~Node()
{
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

MemberVariableNode::MemberVariableNode(TypeNode* type_, const std::string& id_) : type(type_), id(id_)
{
}

void MemberVariableNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
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
    memberVariables.push_back(std::unique_ptr<MemberVariableNode>(memberVariable));
}

SourceFileNode::SourceFileNode()
{
}

void SourceFileNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void SourceFileNode::AddInclude(const std::string& includeDir)
{
    includeDirs.push_back(includeDir);
}

void SourceFileNode::AddClass(ClassNode* classNode)
{
    classes.push_back(std::unique_ptr<ClassNode>(classNode));
}

} } // namespace sngxml::xmlser
