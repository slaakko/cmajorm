#ifndef Declarator_hpp_5555
#define Declarator_hpp_5555

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/codedom/Declaration.hpp>

namespace cmajor { namespace code {

class Declarator : public cmajor::parsing::Grammar
{
public:
    static Declarator* Create();
    static Declarator* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    cmajor::codedom::InitDeclaratorList* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Declarator(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class InitDeclaratorListRule;
    class InitDeclaratorRule;
    class DeclaratorRule;
    class DirectDeclaratorRule;
    class DeclaratorIdRule;
    class TypeIdRule;
    class TypeRule;
    class TypeSpecifierSeqRule;
    class AbstractDeclaratorRule;
    class DirectAbstractDeclaratorRule;
    class CVQualifierSeqRule;
    class InitializerRule;
    class InitializerClauseRule;
    class InitializerListRule;
};

} } // namespace cmajor.code

#endif // Declarator_hpp_5555
