// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DOM_CHARACTER_DATA_INCLUDED
#define CMAJOR_DOM_CHARACTER_DATA_INCLUDED
#include <cmajor/dom/Node.hpp>

namespace cmajor { namespace dom {

std::u32string XmlCharDataEscape(const std::u32string& charData);

class CharacterData : public Node
{
public:
    CharacterData(NodeType nodeType_, const std::u32string& name_);
    CharacterData(NodeType nodeType_, const std::u32string& name_, const std::u32string& data_);
    CharacterData(const CharacterData&) = delete;
    CharacterData& operator=(const CharacterData&) = delete;
    CharacterData(CharacterData&&) = delete;
    CharacterData& operator=(CharacterData&&) = delete;
    void Write(CodeFormatter& formatter) override;
    bool ValueContainsNewLine() const override;
    const std::u32string& Data() const { return data; }
    std::u32string& Data() { return data; }
private:
    std::u32string data;
};

class Text : public CharacterData
{
public:
    Text();
    Text(const std::u32string& data_);
    Text(const Text&) = delete;
    Text& operator=(const Text&) = delete;
    Text(Text&&) = delete;
    Text& operator=(Text&&) = delete;
    Text(NodeType nodeType_, const std::u32string& name_);
    Text(NodeType nodeType_, const std::u32string& name_, const std::u32string& data_);
    std::unique_ptr<Node> CloneNode(bool deep) override;
    void Accept(Visitor& visitor) override;
};

class EntityReference : public CharacterData
{
public:
    EntityReference();
    EntityReference(const std::u32string& entityRef_);
    EntityReference(const EntityReference&) = delete;
    EntityReference& operator=(const EntityReference&) = delete;
    EntityReference(EntityReference&&) = delete;
    EntityReference& operator=(EntityReference&&) = delete;
    void Write(CodeFormatter& formatter) override;
    std::unique_ptr<Node> CloneNode(bool deep) override;
    void Accept(Visitor& visitor) override;
};

class CDataSection : public Text
{
public:
    CDataSection();
    CDataSection(const std::u32string& data_);
    CDataSection(const CDataSection&) = delete;
    CDataSection& operator=(const CDataSection&) = delete;
    CDataSection(CDataSection&&) = delete;
    CDataSection& operator=(CDataSection&&) = delete;
    void Write(CodeFormatter& formatter) override;
    std::unique_ptr<Node> CloneNode(bool deep) override;
    void Accept(Visitor& visitor) override;
};

class Comment : public CharacterData
{
public:
    Comment();
    Comment(const std::u32string& data_);
    Comment(const Comment&) = delete;
    Comment& operator=(const Comment&) = delete;
    Comment(Comment&&) = delete;
    Comment& operator=(Comment&&) = delete;
    void Write(CodeFormatter& formatter) override;
    std::unique_ptr<Node> CloneNode(bool deep) override;
    void Accept(Visitor& visitor) override;
};

class ProcessingInstruction : public Node
{
public:
    ProcessingInstruction(const std::u32string& target_, const std::u32string& data_);
    ProcessingInstruction(const ProcessingInstruction&) = delete;
    ProcessingInstruction& operator=(const ProcessingInstruction&) = delete;
    ProcessingInstruction(ProcessingInstruction&&) = delete;
    ProcessingInstruction& operator=(ProcessingInstruction&&) = delete;
    void Write(CodeFormatter& formatter) override;
    std::unique_ptr<Node> CloneNode(bool deep) override;
    void Accept(Visitor& visitor) override;
    const std::u32string& Target() const { return target; }
    const std::u32string& Data() const { return data; }
private:
    std::u32string target;
    std::u32string data;
};

} } // namespace cmajor::dom

#endif // CMAJOR_DOM_CHARACTER_DATA_INCLUDED
