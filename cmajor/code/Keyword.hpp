#ifndef Keyword_hpp_5555
#define Keyword_hpp_5555

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>

namespace cmajor { namespace code {

class Keyword : public cmajor::parsing::Grammar
{
public:
    static Keyword* Create();
    static Keyword* Create(cmajor::parsing::ParsingDomain* parsingDomain);
private:
    std::vector<std::u32string> keywords0;
    Keyword(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
};

} } // namespace cmajor.code

#endif // Keyword_hpp_5555
