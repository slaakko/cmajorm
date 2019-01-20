#ifndef SourceToken_hpp_4502
#define SourceToken_hpp_4502

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/SourceToken.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class SourceToken : public cmajor::parsing::Grammar
{
public:
    static SourceToken* Create();
    static SourceToken* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, SourceTokenFormatter* formatter);
private:
    SourceToken(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class SourceTokensRule;
    class SourceTokenRule;
    class SpacesRule;
    class OtherRule;
};

} } // namespace cmajor.parser

#endif // SourceToken_hpp_4502
