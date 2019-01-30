// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_GRAMMAR_INCLUDED
#define CMAJOR_PARSING_GRAMMAR_INCLUDED
#include <cmajor/parsing/Parser.hpp>
#include <unordered_set>

namespace cmajor { namespace parsing {

class Grammar;
typedef std::unordered_set<Grammar*> GrammarSet;
typedef GrammarSet::const_iterator GrammarSetIt;

class Rule;
class ParsingDomain;
class Namespace;
class RuleLink;

class PARSING_API Grammar: public ParsingObject
{
public:
    typedef std::vector<Rule*> RuleVec;
    Grammar(const std::u32string& name_, Scope* enclosingScope_);
    Grammar(const std::u32string& name_, Scope* enclosingScope_, ParsingDomain* parsingDomain_);
    void SetStartRuleName(const std::u32string& startRuleName_) { startRuleName = startRuleName_; }
    void SetSkipRuleName(const std::u32string& skipRuleName_) { skipRuleName = skipRuleName_; }
    void AddRule(Rule* rule);
    Rule* GetRule(const std::u32string& ruleName) const;
    virtual void GetReferencedGrammars() {}
    virtual void CreateRules() {}
    virtual void Link();
    void AddGrammarReference(Grammar* grammarReference);
    const GrammarSet& GrammarReferences() const { return grammarReferences; }
    void Accept(Visitor& visitor);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingData* parsingData);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
    Match Parse(Scanner& scanner, ObjectStack& stack);
    const std::u32string& StartRuleName() const { return startRuleName; }
    Rule* StartRule() const { return startRule; }
    const std::u32string& SkipRuleName() const { return skipRuleName; }
    Rule* SkipRule() const { return skipRule; }
    std::ostream* Log() const { return log; }
    void SetLog(std::ostream* log_) { log = log_; }
    int MaxLogLineLength() const { return maxLogLineLength; }
    void SetMaxLogLineLength(int maxLogLineLength_) { maxLogLineLength = maxLogLineLength_; }
    ParsingDomain* GetParsingDomain() const { return parsingDomain; }
    const RuleVec& Rules() const { return rules; }
    Namespace* Ns() const { return ns; }
    void SetNs(Namespace* ns_) { ns = ns_; }
    void AddRuleLink(RuleLink* ruleLink);
    void ResolveStartRule();
    void ResolveSkipRule();
private:
    ParsingDomain* parsingDomain;
    Namespace* ns;
    std::u32string startRuleName;
    std::u32string skipRuleName;
    std::u32string ccRuleName;
    RuleVec rules;
    bool linking;
    bool linked;
    Parser* contentParser;
    GrammarSet grammarReferences;
    typedef std::unordered_set<RuleLink*> RuleLinkSet;
    typedef RuleLinkSet::const_iterator RuleLinkSetIt;
    RuleLinkSet ruleLinks;
    Rule* startRule;
    Rule* skipRule;
    std::ostream* log;
    int maxLogLineLength;
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_GRAMMAR_INCLUDED
