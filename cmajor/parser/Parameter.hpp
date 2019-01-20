#ifndef Parameter_hpp_8344
#define Parameter_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Parameter.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Parameter : public cmajor::parsing::Grammar
{
public:
    static Parameter* Create();
    static Parameter* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx, Node* owner);
private:
    Parameter(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ParameterListRule;
    class ParameterRule;
};

} } // namespace cmajor.parser

#endif // Parameter_hpp_8344
