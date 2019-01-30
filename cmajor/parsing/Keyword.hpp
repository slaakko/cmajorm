// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_KEYWORD_INCLUDED
#define CMAJOR_PARSING_KEYWORD_INCLUDED
#include <cmajor/parsing/Parser.hpp>
#include <set>

namespace cmajor { namespace parsing {

class Rule;

class PARSING_API KeywordParser : public Parser
{
public:
    KeywordParser(const std::u32string& keyword_);
    KeywordParser(const std::u32string& keyword_, const std::u32string& continuationRuleName_);
    const std::u32string& Keyword() const { return keyword; }
    const std::u32string& ContinuationRuleName() const { return continuationRuleName; }
    void SetContinuationRule(Rule* continuationRule_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string keyword;
    std::u32string continuationRuleName;
    Rule* continuationRule;
    Parser* keywordStringParser;
    Rule* keywordRule;
    void CreateKeywordRule();
};

class PARSING_API KeywordListParser : public Parser
{
public:
    typedef std::set<std::u32string> KeywordSet;
    typedef KeywordSet::const_iterator KeywordSetIt;
    KeywordListParser(const std::u32string& selectorRuleName_, const std::vector<std::u32string>& keywords_);
    const std::u32string& SelectorRuleName() const { return selectorRuleName; }
    const std::u32string& KeywordVecName() const { return keywordVecName; }
    std::u32string& KeywordVecName() { return keywordVecName; }
    const KeywordSet& Keywords() const { return keywords; }
    void SetSelectorRule(Rule* selectorRule_) { selectorRule = selectorRule_; }
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string selectorRuleName;
    std::u32string keywordVecName;
    KeywordSet keywords;
    Rule* selectorRule;
};

PARSING_API void KeywordInit();
PARSING_API void KeywordDone();

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_KEYWORD_INCLUDED
