// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/SyncNode.hpp>
#include <sngcm/ast/Visitor.hpp>

namespace sngcm { namespace ast {

SyncNode::SyncNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::syncNode, span_, moduleId_)
{
}

Node* SyncNode::Clone(CloneContext& cloneContext) const
{
    return new SyncNode(GetSpan(), ModuleId());
}

void SyncNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace sngcm::ast
