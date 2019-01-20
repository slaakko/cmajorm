#ifndef Expression_hpp_22807
#define Expression_hpp_22807

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Expression.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Expression : public cmajor::parsing::Grammar
{
public:
    static Expression* Create();
    static Expression* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    Node* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Expression(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ExpressionRule;
    class EquivalenceRule;
    class ImplicationRule;
    class DisjunctionRule;
    class ConjunctionRule;
    class BitOrRule;
    class BitXorRule;
    class BitAndRule;
    class EqualityRule;
    class RelationalRule;
    class ShiftRule;
    class AdditiveRule;
    class MultiplicativeRule;
    class PrefixRule;
    class PostfixRule;
    class PrimaryRule;
    class SizeOfExprRule;
    class TypeNameExprRule;
    class CastExprRule;
    class ConstructExprRule;
    class NewExprRule;
    class ArgumentListRule;
    class ExpressionListRule;
    class InvokeExprRule;
};

} } // namespace cmajor.parser

#endif // Expression_hpp_22807
