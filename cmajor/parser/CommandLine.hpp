#ifndef CommandLine_hpp_8344
#define CommandLine_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>

namespace cmajor { namespace parser {

class CommandLine : public cmajor::parsing::Grammar
{
public:
    static CommandLine* Create();
    static CommandLine* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    std::vector<std::string> Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    CommandLine(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class CommandLineRule;
    class ArgumentRule;
    class ArgElementRule;
    class OddBackslashesAndLiteralQuotationMarkRule;
    class EvenBackslashesAndQuotationMarkRule;
    class StringCharRule;
};

} } // namespace cmajor.parser

#endif // CommandLine_hpp_8344
