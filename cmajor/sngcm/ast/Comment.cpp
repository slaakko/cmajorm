// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Comment.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/AstWriter.hpp>
#include <sngcm/ast/AstReader.hpp>

namespace sngcm { namespace ast {

CommentNode::CommentNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::commentNode, span_, moduleId_)
{
}

CommentNode::CommentNode(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& comment_) : Node(NodeType::commentNode, span_, moduleId_), comment(comment_)
{
}

Node* CommentNode::Clone(CloneContext& cloneContext) const
{
    CommentNode* clone = new CommentNode(GetSpan(), ModuleId(), comment);
    return clone;
}

void CommentNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void CommentNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.GetBinaryWriter().Write(comment);
}

void CommentNode::Read(AstReader& reader)
{
    Node::Read(reader);
    comment = reader.GetBinaryReader().ReadUtf32String();
}

} } // namespace sngcm::ast
