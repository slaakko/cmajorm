#ifndef Literal_hpp_15318
#define Literal_hpp_15318

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Literal.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Literal : public cmajor::parsing::Grammar
{
public:
    static Literal* Create();
    static Literal* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    LiteralNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Literal(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class LiteralRule;
    class BooleanLiteralRule;
    class FloatingLiteralRule;
    class FloatingLiteralValueRule;
    class IntegerLiteralRule;
    class IntegerLiteralValueRule;
    class HexIntegerLiteralRule;
    class DecIntegerLiteralRule;
    class CharLiteralRule;
    class StringLiteralRule;
    class NullLiteralRule;
    class ArrayLiteralRule;
    class StructuredLiteralRule;
    class CharEscapeRule;
    class DecDigitSequenceRule;
    class HexDigitSequenceRule;
    class HexDigit4Rule;
    class HexDigit8Rule;
    class OctalDigitSequenceRule;
};

} } // namespace cmajor.parser

#endif // Literal_hpp_15318
