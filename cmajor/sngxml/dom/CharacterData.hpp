// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_DOM_CHARACTER_DATA_INCLUDED
#define SNGXML_DOM_CHARACTER_DATA_INCLUDED
#include <sngxml/dom/Node.hpp>

namespace sngxml { namespace dom {

SNGXML_DOM_API std::u32string XmlCharDataEscape(const std::u32string& charData);

class SNGXML_DOM_API CharacterData : public Node
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

class SNGXML_DOM_API Text : public CharacterData
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

class SNGXML_DOM_API EntityReference : public CharacterData
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

class SNGXML_DOM_API CDataSection : public Text
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

class SNGXML_DOM_API Comment : public CharacterData
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

class SNGXML_DOM_API ProcessingInstruction : public Node
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

} } // namespace sngxml::dom

#endif // SNGXML_DOM_CHARACTER_DATA_INCLUDED
