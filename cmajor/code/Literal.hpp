#ifndef Literal_hpp_5555
#define Literal_hpp_5555

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/codedom/Literal.hpp>

namespace cmajor { namespace code {

class Literal : public cmajor::parsing::Grammar
{
public:
    static Literal* Create();
    static Literal* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    cmajor::codedom::Literal* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Literal(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class LiteralRule;
    class IntegerLiteralRule;
    class CharacterLiteralRule;
    class CCharSequenceRule;
    class FloatingLiteralRule;
    class StringLiteralRule;
    class BooleanLiteralRule;
    class PointerLiteralRule;
};

} } // namespace cmajor.code

#endif // Literal_hpp_5555
