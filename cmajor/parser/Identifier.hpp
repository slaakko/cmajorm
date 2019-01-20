#ifndef Identifier_hpp_8344
#define Identifier_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Identifier.hpp>

namespace cmajor { namespace parser {

using cmajor::ast::IdentifierNode;
class Identifier : public cmajor::parsing::Grammar
{
public:
    static Identifier* Create();
    static Identifier* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    IdentifierNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Identifier(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class IdentifierRule;
    class QualifiedIdRule;
};

} } // namespace cmajor.parser

#endif // Identifier_hpp_8344
