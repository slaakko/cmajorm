#ifndef Attribute_hpp_8344
#define Attribute_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Attribute.hpp>

namespace cmajor { namespace parser {

class Attribute : public cmajor::parsing::Grammar
{
public:
    static Attribute* Create();
    static Attribute* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    cmajor::ast::Attributes* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Attribute(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class AttributesRule;
    class AttributeRule;
};

} } // namespace cmajor.parser

#endif // Attribute_hpp_8344
