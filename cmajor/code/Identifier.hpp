#ifndef Identifier_hpp_5555
#define Identifier_hpp_5555

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>

namespace cmajor { namespace code {

class Identifier : public cmajor::parsing::Grammar
{
public:
    static Identifier* Create();
    static Identifier* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    std::u32string Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Identifier(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class IdentifierRule;
    class QualifiedIdRule;
};

} } // namespace cmajor.code

#endif // Identifier_hpp_5555
