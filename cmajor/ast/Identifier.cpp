// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ast/Identifier.hpp>
#include <cmajor/ast/Visitor.hpp>
#include <cmajor/ast/AstWriter.hpp>
#include <cmajor/ast/AstReader.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace ast {

using namespace cmajor::unicode;

IdentifierNode::IdentifierNode(const Span& span_) : Node(NodeType::identifierNode, span_), identifier()
{
}

IdentifierNode::IdentifierNode(const Span& span_, NodeType nodeType_) : Node(NodeType::cursorIdNode, span_), identifier()
{
}

IdentifierNode::IdentifierNode(const Span& span_, const std::u32string& identifier_) : Node(NodeType::identifierNode, span_), identifier(identifier_)
{
}

IdentifierNode::IdentifierNode(const Span& span_, NodeType nodeType_, const std::u32string& identifier_) : Node(NodeType::cursorIdNode, span_), identifier(identifier_)
{
    std::u32string result;
    for (char32_t c : identifier)
    {
        if (c != '`')
        {
            result.append(1, c);
        }
    }
    std::swap(result, identifier);
}

Node* IdentifierNode::Clone(CloneContext& cloneContext) const
{
    return new IdentifierNode(GetSpan(), identifier);
}

void IdentifierNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void IdentifierNode::Write(AstWriter& writer)
{
    Node::Write(writer);
    writer.GetBinaryWriter().Write(identifier);
}

void IdentifierNode::Read(AstReader& reader)
{
    Node::Read(reader);
    identifier = reader.GetBinaryReader().ReadUtf32String();
}

std::string IdentifierNode::ToString() const
{
    return ToUtf8(identifier);
}

CursorIdNode::CursorIdNode(const Span& span_) : IdentifierNode(span_, NodeType::cursorIdNode)
{
}

CursorIdNode::CursorIdNode(const Span& span_, const std::u32string& identifier_) : IdentifierNode(span_, NodeType::cursorIdNode, identifier_)
{
}

Node* CursorIdNode::Clone(CloneContext& cloneContext) const 
{
    return new CursorIdNode(GetSpan(), Str());
}

void CursorIdNode::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

void CursorIdNode::Write(AstWriter& writer)
{
    IdentifierNode::Write(writer);
}

void CursorIdNode::Read(AstReader& reader)
{
    IdentifierNode::Read(reader);
}

} } // namespace cmajor::ast
