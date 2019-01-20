// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_LINKING_INCLUDED
#define CMAJOR_PARSING_LINKING_INCLUDED
#include <cmajor/parsing/Visitor.hpp>

namespace cmajor { namespace parsing {

class Grammar;
class Rule;
class ParsingDomain;

class LinkerVisitor : public Visitor
{
public:
    LinkerVisitor();
    void BeginVisit(Grammar& grammar) override;
    void EndVisit(Grammar& grammar) override;
    void Visit(RuleLink& link) override;
    void BeginVisit(Rule& rule) override;
    void EndVisit(Rule& rule) override;
    void BeginVisit(ActionParser& parser) override;
    void Visit(NonterminalParser& parser) override;
    void Visit(KeywordParser& parser) override;
    void Visit(KeywordListParser& parser) override;
private:
    Grammar* currentGrammar;
    Rule* currentRule;
    int actionNumber;
};

void Link(ParsingDomain* parsingDomain);
void ExpandCode(ParsingDomain* parsingDomain);

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_LINKING_INCLUDED
