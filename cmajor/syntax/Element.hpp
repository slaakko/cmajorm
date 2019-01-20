#ifndef Element_hpp_5581
#define Element_hpp_5581

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/parsing/Scope.hpp>
#include <cmajor/parsing/Grammar.hpp>

namespace cmajor { namespace syntax {

class Element : public cmajor::parsing::Grammar
{
public:
    static Element* Create();
    static Element* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, cmajor::parsing::Grammar* grammar);
private:
    std::vector<std::u32string> keywords0;
    Element(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class RuleLinkRule;
    class SignatureRule;
    class ParameterListRule;
    class VariableRule;
    class ParameterRule;
    class ReturnTypeRule;
    class IdentifierRule;
    class QualifiedIdRule;
    class StringArrayRule;
};

} } // namespace cmajor.syntax

#endif // Element_hpp_5581
