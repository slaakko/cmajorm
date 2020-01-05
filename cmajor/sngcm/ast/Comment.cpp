// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Comment.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/AstWriter.hpp>
#include <sngcm/ast/AstReader.hpp>

namespace sngcm { namespace ast {

CommentNode::CommentNode(const Span& span_) : Node(NodeType::commentNode, span_)
{
}

CommentNode::CommentNode(const Span& span_, const std::u32string& comment_) : Node(NodeType::commentNode, span_), comment(comment_)
{
}

Node* CommentNode::Clone(CloneContext& cloneContext) const
{
    return new CommentNode(GetSpan(), comment);
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
