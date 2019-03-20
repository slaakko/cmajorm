#ifndef Keyword_hpp_10315
#define Keyword_hpp_10315

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>

namespace cmajor { namespace parser {

class Keyword : public cmajor::parsing::Grammar
{
public:
    static Keyword* Create();
    static Keyword* Create(cmajor::parsing::ParsingDomain* parsingDomain);
private:
    std::vector<std::u32string> keywords0;
    std::vector<std::u32string> keywords1;
    Keyword(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
};

} } // namespace cmajor.parser

#endif // Keyword_hpp_10315
