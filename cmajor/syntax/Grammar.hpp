#ifndef Grammar_hpp_5581
#define Grammar_hpp_5581

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/parsing/Grammar.hpp>

namespace cmajor { namespace syntax {

class GrammarGrammar : public cmajor::parsing::Grammar
{
public:
    static GrammarGrammar* Create();
    static GrammarGrammar* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    cmajor::parsing::Grammar* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, cmajor::parsing::Scope* enclosingScope);
private:
    GrammarGrammar(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class GrammarRule;
    class GrammarContentRule;
    class StartClauseRule;
    class SkipClauseRule;
};

} } // namespace cmajor.syntax

#endif // Grammar_hpp_5581
