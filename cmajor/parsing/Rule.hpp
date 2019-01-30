// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_RULE_INCLUDED
#define CMAJOR_PARSING_RULE_INCLUDED
#include <cmajor/parsing/Parser.hpp>

namespace cmajor { namespace parsing {

class PARSING_API AttrOrVariable
{
public:
    AttrOrVariable(const std::u32string& typeName_, const std::u32string& name_);
    const std::u32string& TypeName() const { return typeName; }
    const std::u32string& Name() const { return name; }
private:
    std::u32string typeName;
    std::u32string name;
};

class ActionParser;
class NonterminalParser;
class Grammar;

class PARSING_API Rule: public Parser
{
public:
    typedef std::vector<AttrOrVariable> AttrOrVariableVec;
    typedef std::vector<ActionParser*> ActionVec;
    typedef std::vector<NonterminalParser*> NonterminalVec;
    Rule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_);
    Rule(const std::u32string& name_, Scope* enclosingScope_, Parser* definition_);
    Rule(const std::u32string& name_, Scope* enclosingScope_);
    bool IsRule() const override { return true; }
    Grammar* GetGrammar() const { return grammar; }
    void SetGrammar(Grammar* grammar_) { grammar = grammar_; }
    Parser* Definition() const { return definition; }
    void SetDefinition(Parser* definition_) ;
    const AttrOrVariableVec& InheritedAttributes() const { return inheritedAttributes; }
    void AddInheritedAttribute(const AttrOrVariable& attr) { inheritedAttributes.push_back(attr); }
    const AttrOrVariableVec& LocalVariables() const { return localVariables; }
    void AddLocalVariable(const AttrOrVariable& var) { localVariables.push_back(var); }
    const std::u32string& ValueTypeName() const { return valueTypeName; }
    void SetValueTypeName(const std::u32string& valueTypeName_) { valueTypeName = valueTypeName_; }
    int NumberOfParameters() const { return int(inheritedAttributes.size()); }
    const ActionVec& Actions() const { return actions; }
    void AddAction(ActionParser* action);
    ActionParser* GetAction(const std::u32string& actionName) const;
    const NonterminalVec& Nonterminals() const { return nonterminals; }
    void AddNonterminal(NonterminalParser* nonterminal);
    NonterminalParser* GetNonterminal(const std::u32string& nonterminalName) const;
    bool TakesOrReturnsValuesOrHasLocals() const { return !inheritedAttributes.empty() || !localVariables.empty() || !valueTypeName.empty(); }
    bool Specialized() const { return TakesOrReturnsValuesOrHasLocals() || !actions.empty(); }
    std::u32string SpecializedTypeName() const { return Specialized() ? Name() + U"Rule" : U"Rule"; }
    virtual void Link() {}
    void ExpandCode();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    virtual void Enter(ObjectStack& stack) {}
    virtual void Leave(ObjectStack& stack, bool matched) {}
    virtual void Enter(ObjectStack& stack, ParsingData* parsingData) {}
    virtual void Leave(ObjectStack& stack, ParsingData* parsingData, bool matched) {}
    void Accept(Visitor& visitor) override;
    int Id() const { return id; }
private:
    int id;
    Grammar* grammar;
    Parser* definition;
    AttrOrVariableVec inheritedAttributes;
    AttrOrVariableVec localVariables;
    std::u32string valueTypeName;
    ActionVec actions;
    NonterminalVec nonterminals;
};

class PARSING_API RuleLink: public ParsingObject
{
public:
    RuleLink(const std::u32string& name_, Grammar* grammar_, const std::u32string& linkedRuleName_);
    RuleLink(Grammar* grammar_, const std::u32string& linkedRuleName_);
    bool IsRuleLink() const override { return true; }
    void Accept(Visitor& visitor) override;
    void AddToScope() override;
    const std::u32string& LinkedRuleName() const { return linkedRuleName; }
    void SetRule(Rule* rule_) { rule = rule_; }
    Rule* GetRule() const { return rule; }
private:
    std::u32string linkedRuleName;
    Grammar* grammar;
    Rule* rule;
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_RULE_INCLUDED
