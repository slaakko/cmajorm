#ifndef TypeExpr_hpp_8344
#define TypeExpr_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/TypeExpr.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class TypeExpr : public cmajor::parsing::Grammar
{
public:
    static TypeExpr* Create();
    static TypeExpr* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    Node* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    TypeExpr(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class TypeExprRule;
    class PrefixTypeExprRule;
    class PostfixTypeExprRule;
    class PrimaryTypeExprRule;
};

} } // namespace cmajor.parser

#endif // TypeExpr_hpp_8344
