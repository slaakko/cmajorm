#ifndef Template_hpp_8344
#define Template_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Template.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Template : public cmajor::parsing::Grammar
{
public:
    static Template* Create();
    static Template* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    Node* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Template(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class TemplateIdRule;
    class TemplateParameterRule;
    class TemplateParameterListRule;
};

} } // namespace cmajor.parser

#endif // Template_hpp_8344
