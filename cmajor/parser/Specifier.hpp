#ifndef Specifier_hpp_8344
#define Specifier_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Specifier.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Specifier : public cmajor::parsing::Grammar
{
public:
    static Specifier* Create();
    static Specifier* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    Specifiers Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Specifier(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class SpecifiersRule;
    class SpecifierRule;
};

} } // namespace cmajor.parser

#endif // Specifier_hpp_8344
