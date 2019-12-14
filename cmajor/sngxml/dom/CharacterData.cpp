// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/dom/CharacterData.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngxml { namespace dom {

using namespace soulng::unicode;

std::u32string XmlCharDataEscape(const std::u32string& charData)
{
    std::u32string result;
    for (char32_t c : charData)
    {
        switch (c)
        {
        case '<': result.append(U"&lt;"); break;
        case '&': result.append(U"&amp;"); break;
        default: result.append(1, c); break;
        }
    }
    return result;
}

CharacterData::CharacterData(NodeType nodeType_, const std::u32string& name_) : Node(nodeType_, name_)
{
}

CharacterData::CharacterData(NodeType nodeType_, const std::u32string& name_, const std::u32string& data_) : Node(nodeType_, name_), data(data_)
{
}

void CharacterData::Write(CodeFormatter& formatter)
{
    formatter.Write(ToUtf8(XmlCharDataEscape(data)));
}

bool CharacterData::ValueContainsNewLine() const
{
    return data.find('\n') != std::u32string::npos;
}

Text::Text() : CharacterData(NodeType::textNode, U"text")
{
}

Text::Text(const std::u32string& data_) : CharacterData(NodeType::textNode, U"text", data_)
{
}

Text::Text(NodeType nodeType_, const std::u32string& name_) : CharacterData(nodeType_, name_)
{
}

Text::Text(NodeType nodeType_, const std::u32string& name_, const std::u32string& data_) : CharacterData(nodeType_, name_, data_)
{
}

std::unique_ptr<Node> Text::CloneNode(bool deep)
{
    return std::unique_ptr<Node>(new Text(Data()));
}

void Text::Accept(Visitor& visitor)
{
    visitor.Visit(this);
}

EntityReference::EntityReference() : CharacterData(NodeType::entityReferenceNode, U"entity_refefrence")
{
}

EntityReference::EntityReference(const std::u32string& entityRef_) : CharacterData(NodeType::entityReferenceNode, U"entity_refefrence", entityRef_)
{
}

void EntityReference::Write(CodeFormatter& formatter)
{
    formatter.Write("&");
    formatter.Write(ToUtf8(Data()));
    formatter.Write(";");
}

std::unique_ptr<Node> EntityReference::CloneNode(bool deep)
{
    return std::unique_ptr<Node>(new EntityReference(Data()));
}

void EntityReference::Accept(Visitor& visitor)
{
    visitor.Visit(this);
}

CDataSection::CDataSection() : Text(NodeType::cdataSectionNode, U"cdata_section")
{
}

CDataSection::CDataSection(const std::u32string& data_) : Text(NodeType::cdataSectionNode, U"cdata_section", data_)
{
}

void CDataSection::Write(CodeFormatter& formatter)
{
    formatter.Write("<![CDATA[");
    formatter.Write(ToUtf8(Data()));
    formatter.Write("]]>");
}

std::unique_ptr<Node> CDataSection::CloneNode(bool deep)
{
    return std::unique_ptr<Node>(new CDataSection(Data()));
}

void CDataSection::Accept(Visitor& visitor)
{
    visitor.Visit(this);
}

Comment::Comment() : CharacterData(NodeType::commentNode, U"comment")
{
}

Comment::Comment(const std::u32string& data_) : CharacterData(NodeType::commentNode, U"comment", data_)
{
}

void Comment::Write(CodeFormatter& formatter)
{
    formatter.Write("<!-- ");
    formatter.Write(ToUtf8(Data()));
    formatter.Write(" -->");
}

std::unique_ptr<Node> Comment::CloneNode(bool deep)
{
    return std::unique_ptr<Node>(new Comment(Data()));
}

void Comment::Accept(Visitor& visitor)
{
    visitor.Visit(this);
}

ProcessingInstruction::ProcessingInstruction(const std::u32string& target_, const std::u32string& data_) : Node(NodeType::processingInstructionNode, U"processing_instruction"), target(target_), data(data_)
{
}

void ProcessingInstruction::Write(CodeFormatter& formatter)
{
    formatter.WriteLine("<?" + ToUtf8(target) + " " + ToUtf8(data) + "?>");
}

std::unique_ptr<Node> ProcessingInstruction::CloneNode(bool deep)
{
    return std::unique_ptr<Node>(new ProcessingInstruction(target, data));
}

void ProcessingInstruction::Accept(Visitor& visitor)
{
    visitor.Visit(this);
}

} } // namespace sngxml::dom
