#ifndef Primitive_hpp_5581
#define Primitive_hpp_5581

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/parsing/Primitive.hpp>
#include <cmajor/parsing/Keyword.hpp>

namespace cmajor { namespace syntax {

class Primitive : public cmajor::parsing::Grammar
{
public:
    static Primitive* Create();
    static Primitive* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    cmajor::parsing::Parser* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Primitive(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class PrimitiveRule;
    class CharRule;
    class StringRule;
    class CharSetRule;
    class CharSetRangeRule;
    class CharSetCharRule;
    class KeywordRule;
    class KeywordBodyRule;
    class KeywordListRule;
    class KeywordListBodyRule;
    class RangeRule;
    class CodePointRule;
    class EmptyRule;
    class SpaceRule;
    class AnyCharRule;
    class LetterRule;
    class DigitRule;
    class HexDigitRule;
    class UpperLetterRule;
    class LowerLetterRule;
    class TitleLetterRule;
    class ModifierLetterRule;
    class OtherLetterRule;
    class CasedLetterRule;
    class MarkRule;
    class NonspacingMarkRule;
    class SpacingMarkRule;
    class EnclosingMarkRule;
    class NumberRule;
    class DecimalNumberRule;
    class LetterNumberRule;
    class OtherNumberRule;
    class PunctuationRule;
    class ConnectorPunctuationRule;
    class DashPunctuationRule;
    class OpenPunctuationRule;
    class ClosePunctuationRule;
    class InitialPunctuationRule;
    class FinalPunctuationRule;
    class OtherPunctuationRule;
    class SymbolRule;
    class MathSymbolRule;
    class CurrencySymbolRule;
    class ModifierSymbolRule;
    class OtherSymbolRule;
    class SeparatorRule;
    class SpaceSeparatorRule;
    class LineSeparatorRule;
    class ParagraphSeparatorRule;
    class OtherRule;
    class ControlRule;
    class FormatRule;
    class SurrogateRule;
    class PrivateUseRule;
    class UnassignedRule;
    class GraphicRule;
    class BaseCharRule;
    class AlphabeticRule;
    class IdStartRule;
    class IdContRule;
};

} } // namespace cmajor.syntax

#endif // Primitive_hpp_5581
