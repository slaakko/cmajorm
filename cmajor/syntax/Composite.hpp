#ifndef Composite_hpp_5581
#define Composite_hpp_5581

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/parsing/Scope.hpp>
#include <cmajor/parsing/Parser.hpp>

namespace cmajor { namespace syntax {

class Composite : public cmajor::parsing::Grammar
{
public:
    static Composite* Create();
    static Composite* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    cmajor::parsing::Parser* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, cmajor::parsing::Scope* enclosingScope);
private:
    Composite(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class AlternativeRule;
    class SequenceRule;
    class DifferenceRule;
    class ExclusiveOrRule;
    class IntersectionRule;
    class ListRule;
    class PostfixRule;
};

} } // namespace cmajor.syntax

#endif // Composite_hpp_5581
