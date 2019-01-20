#ifndef Constant_hpp_22553
#define Constant_hpp_22553

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Constant.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Constant : public cmajor::parsing::Grammar
{
public:
    static Constant* Create();
    static Constant* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    ConstantNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Constant(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ConstantRule;
};

} } // namespace cmajor.parser

#endif // Constant_hpp_22553
