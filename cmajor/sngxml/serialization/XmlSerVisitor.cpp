// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlSerVisitor.hpp>
#include <sngxml/serialization/XmlSerAst.hpp>
#include <memory>

namespace sngxml { namespace xmlser {

Visitor::~Visitor()
{
}

void DefaultVisitor::Visit(SourceFileNode& node)
{
    node.GlobalNs()->Accept(*this);
}

void DefaultVisitor::Visit(NamespaceNode& node) 
{
    for (const std::unique_ptr<Node>& n : node.Nodes())
    {
        n->Accept(*this);
    }
}

void DefaultVisitor::Visit(ClassNode& node)
{
    for (const std::unique_ptr<Node>& node: node.Nodes())
    {
        node->Accept(*this);
    }
}

void DefaultVisitor::Visit(MemberVariableNode& node)
{
    node.Type()->Accept(*this);
}

void DefaultVisitor::Visit(EnumTypeNode& node)
{
    for (const std::unique_ptr<EnumConstantNode>& enumConstantNode : node.EnumConstants())
    {
        enumConstantNode->Accept(*this);
    }
}

void DefaultVisitor::Visit(ArrayNode& node)
{
    node.Type()->Accept(*this);
}

void DefaultVisitor::Visit(PointerNode& node)
{
    node.Type()->Accept(*this);
}

} } // namespace sngxml::xmlser
