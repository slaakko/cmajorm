#ifndef Typedef_hpp_8344
#define Typedef_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Typedef.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Typedef : public cmajor::parsing::Grammar
{
public:
    static Typedef* Create();
    static Typedef* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    TypedefNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Typedef(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class TypedefRule;
};

} } // namespace cmajor.parser

#endif // Typedef_hpp_8344
