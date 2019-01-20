#ifndef Rule_hpp_5581
#define Rule_hpp_5581

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/parsing/Rule.hpp>

namespace cmajor { namespace syntax {

class Rule : public cmajor::parsing::Grammar
{
public:
    static Rule* Create();
    static Rule* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    cmajor::parsing::Rule* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, cmajor::parsing::Scope* enclosingScope);
private:
    Rule(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class RuleRule;
    class RuleHeaderRule;
    class RuleBodyRule;
};

} } // namespace cmajor.syntax

#endif // Rule_hpp_5581
