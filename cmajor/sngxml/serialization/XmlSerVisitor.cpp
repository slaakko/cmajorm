// =================================
// Copyright (c) 2020 Seppo Laakko
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
    for (const std::unique_ptr<ClassNode>& classNode : node.Classes())
    {
        classNode->Accept(*this);
    }
}

void DefaultVisitor::Visit(ClassNode& node)
{
    for (const std::unique_ptr<MemberVariableNode>& memberVariableNode : node.MemberVariables())
    {
        memberVariableNode->Accept(*this);
    }
}

void DefaultVisitor::Visit(MemberVariableNode& node)
{
    node.Type()->Accept(*this);
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
