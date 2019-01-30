// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_VISITOR_INCLUDED
#define CMAJOR_PARSING_VISITOR_INCLUDED
#include <cmajor/parsing/ParsingApi.hpp>

namespace cmajor { namespace parsing {

class Namespace;
class UsingObject;
class RuleLink;
class CharParser;
class StringParser;
class CharSetParser;
class EmptyParser;
class SpaceParser;
class LetterParser;
class UpperLetterParser;
class LowerLetterParser;
class TitleLetterParser;
class ModifierLetterParser;
class OtherLetterParser;
class CasedLetterParser;
class DigitParser;
class HexDigitParser;
class MarkParser;
class NonspacingMarkParser;
class SpacingMarkParser;
class EnclosingMarkParser;
class NumberParser;
class DecimalNumberParser;
class LetterNumberParser;
class OtherNumberParser;
class PunctuationParser;
class ConnectorPunctuationParser;
class DashPunctuationParser;
class OpenPunctuationParser;
class ClosePunctuationParser;
class InitialPunctuationParser;
class FinalPunctuationParser;
class OtherPunctuationParser;
class SymbolParser;
class MathSymbolParser;
class CurrencySymbolParser;
class ModifierSymbolParser;
class OtherSymbolParser;
class SeparatorParser;
class SpaceSeparatorParser;
class LineSeparatorParser;
class ParagraphSeparatorParser;
class OtherParser;
class ControlParser;
class FormatParser;
class SurrogateParser;
class PrivateUseParser;
class UnassignedParser;
class GraphicParser;
class BaseCharParser;
class AlphabeticParser;
class IdStartParser;
class IdContParser;
class AnyCharParser;
class OptionalParser;
class PositiveParser;
class KleeneStarParser;
class ActionParser;
class ExpectationParser;
class TokenParser;
class GroupingParser;
class SequenceParser;
class AlternativeParser;
class DifferenceParser;
class ExclusiveOrParser;
class IntersectionParser;
class ListParser;
class Rule;
class NonterminalParser;
class Grammar;
class KeywordParser;
class KeywordListParser;
class RangeParser;

class PARSING_API Visitor
{
public:
    virtual void Visit(CharParser& parser) {}
    virtual void Visit(StringParser& parser) {}
    virtual void Visit(CharSetParser& parser) {}
    virtual void Visit(EmptyParser& parser) {}
    virtual void Visit(SpaceParser& parser) {}
    virtual void Visit(LetterParser& parser) {}
    virtual void Visit(UpperLetterParser& parser) {}
    virtual void Visit(LowerLetterParser& parser) {}
    virtual void Visit(TitleLetterParser& parser) {}
    virtual void Visit(ModifierLetterParser& parser) {}
    virtual void Visit(OtherLetterParser& parser) {}
    virtual void Visit(CasedLetterParser& parser) {}
    virtual void Visit(DigitParser& parser) {}
    virtual void Visit(HexDigitParser& parser) {}
    virtual void Visit(MarkParser& parser) {}
    virtual void Visit(NonspacingMarkParser& parser) {}
    virtual void Visit(SpacingMarkParser& parser) {}
    virtual void Visit(EnclosingMarkParser& parser) {}
    virtual void Visit(NumberParser& parser) {}
    virtual void Visit(DecimalNumberParser& parser) {}
    virtual void Visit(LetterNumberParser& parser) {}
    virtual void Visit(OtherNumberParser& parser) {}
    virtual void Visit(PunctuationParser& parser) {}
    virtual void Visit(ConnectorPunctuationParser& parser) {}
    virtual void Visit(DashPunctuationParser& parser) {}
    virtual void Visit(OpenPunctuationParser& parser) {}
    virtual void Visit(ClosePunctuationParser& parser) {}
    virtual void Visit(InitialPunctuationParser& parser) {}
    virtual void Visit(FinalPunctuationParser& parser) {}
    virtual void Visit(OtherPunctuationParser& parser) {}
    virtual void Visit(SymbolParser& parser) {}
    virtual void Visit(MathSymbolParser& parser) {}
    virtual void Visit(CurrencySymbolParser& parser) {}
    virtual void Visit(ModifierSymbolParser& parser) {}
    virtual void Visit(OtherSymbolParser& parser) {}
    virtual void Visit(SeparatorParser& parser) {}
    virtual void Visit(SpaceSeparatorParser& parser) {}
    virtual void Visit(LineSeparatorParser& parser) {}
    virtual void Visit(ParagraphSeparatorParser& parser) {}
    virtual void Visit(OtherParser& parser) {}
    virtual void Visit(ControlParser& parser) {}
    virtual void Visit(FormatParser& parser) {}
    virtual void Visit(SurrogateParser& parser) {}
    virtual void Visit(PrivateUseParser& parser) {}
    virtual void Visit(UnassignedParser& parser) {}
    virtual void Visit(GraphicParser& parser) {}
    virtual void Visit(BaseCharParser& parser) {}
    virtual void Visit(AlphabeticParser& parser) {}
    virtual void Visit(IdStartParser& parser) {}
    virtual void Visit(IdContParser& parser) {}
    virtual void Visit(AnyCharParser& parser) {}
    virtual void Visit(RangeParser& parser) {}    
    virtual void BeginVisit(OptionalParser& parser) {}
    virtual void EndVisit(OptionalParser& parser) {}
    virtual void BeginVisit(PositiveParser& parser) {}
    virtual void EndVisit(PositiveParser& parser) {}
    virtual void BeginVisit(KleeneStarParser& parser) {}
    virtual void EndVisit(KleeneStarParser& parser) {}
    virtual void BeginVisit(ActionParser& parser) {}
    virtual void EndVisit(ActionParser& parser) {}
    virtual void BeginVisit(ExpectationParser& parser) {}
    virtual void EndVisit(ExpectationParser& parser) {}
    virtual void BeginVisit(TokenParser& parser) {}
    virtual void EndVisit(TokenParser& parser) {}
    virtual void BeginVisit(GroupingParser& parser) {}
    virtual void EndVisit(GroupingParser& parser) {}
    virtual void BeginVisit(SequenceParser& parser) {}
    virtual void Visit(SequenceParser& parser) {}
    virtual void EndVisit(SequenceParser& parser) {}
    virtual void BeginVisit(AlternativeParser& parser) {}
    virtual void Visit(AlternativeParser& parser) {}
    virtual void EndVisit(AlternativeParser& parser) {}
    virtual void BeginVisit(DifferenceParser& parser) {}
    virtual void Visit(DifferenceParser& parser) {}
    virtual void EndVisit(DifferenceParser& parser) {}
    virtual void BeginVisit(ExclusiveOrParser& parser) {}
    virtual void Visit(ExclusiveOrParser& parser) {}
    virtual void EndVisit(ExclusiveOrParser& parser) {}
    virtual void BeginVisit(IntersectionParser& parser) {}
    virtual void Visit(IntersectionParser& parser) {}
    virtual void EndVisit(IntersectionParser& parser) {}
    virtual void BeginVisit(ListParser& parser) {}
    virtual void Visit(ListParser& parser) {}
    virtual void EndVisit(ListParser& parser) {}
    virtual void BeginVisit(Rule& parser) {}
    virtual void EndVisit(Rule& parser) {}
    virtual void Visit(NonterminalParser& parser) {}
    virtual void BeginVisit(Grammar& grammar) {}
    virtual void EndVisit(Grammar& grammar) {}
    virtual void Visit(KeywordParser& parser) {}
    virtual void Visit(KeywordListParser& parser) {}
    virtual void BeginVisit(Namespace& ns) {}
    virtual void EndVisit(Namespace& ns) {}
    virtual void Visit(UsingObject& object) {}
    virtual void Visit(RuleLink& link) {}
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_VISITOR_INCLUDED
