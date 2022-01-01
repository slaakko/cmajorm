// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/AstWriter.hpp>
#include <sngcm/ast/AstReader.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngcm { namespace ast {

using namespace soulng::unicode;

IdentifierNode::IdentifierNode(const Span& span_, const boost::uuids::uuid& moduleId_) : Node(NodeType::identifierNode, span_, moduleId_), identifier()
{
}

IdentifierNode::IdentifierNode(const Span& span_, const boost::uuids::uuid& moduleId_, NodeType nodeType_) : Node(nodeType_, span_, moduleId_), identifier()
{
}

IdentifierNode::IdentifierNode(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& identifier_) : Node(NodeType::identifierNode, span_, moduleId_), identifier(identifier_)
{
}

IdentifierNode::IdentifierNode(const Span& span_, const boost::uuids::uuid& moduleId_, NodeType nodeType_, const std::u32string& identifier_) : Node(nodeType_, span_, moduleId_), identifier(identifier_)
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

IdentifierNode::IdentifierNode(const Span& span_, const boost::uuids::uuid& moduleId_, const soulng::lexer::Token& token) : Node(NodeType::identifierNode, span_, moduleId_)
{
    identifier = std::u32string(token.match.begin, token.match.end);
}

Node* IdentifierNode::Clone(CloneContext& cloneContext) const
{
    IdentifierNode* clone = new IdentifierNode(GetSpan(), ModuleId(), identifier);
    return clone;
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

bool IdentifierNode::IsInternal() const
{
    return !identifier.empty() && identifier.front() == '@';
}

CursorIdNode::CursorIdNode(const Span& span_, const boost::uuids::uuid& moduleId_) : IdentifierNode(span_, moduleId_, NodeType::cursorIdNode)
{
}

CursorIdNode::CursorIdNode(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& identifier_) : IdentifierNode(span_, moduleId_, NodeType::cursorIdNode, identifier_)
{
}

Node* CursorIdNode::Clone(CloneContext& cloneContext) const 
{
    CursorIdNode* clone = new CursorIdNode(GetSpan(), ModuleId(), Str());
    return clone;
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

} } // namespace sngcm::ast
