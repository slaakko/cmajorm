// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYNTAX_CODEGENERATOR_VISITOR_INCLUDED
#define CMAJOR_SYNTAX_CODEGENERATOR_VISITOR_INCLUDED

#include <cmajor/parsing/Primitive.hpp>
#include <cmajor/parsing/Composite.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/parsing/Visitor.hpp>
#include <soulng/util/CodeFormatter.hpp>

namespace cmajor { namespace syntax {

using cmajor::util::CodeFormatter;
using namespace cmajor::parsing;

class CodeGeneratorVisitor : public cmajor::parsing::Visitor
{
public:
    CodeGeneratorVisitor(CodeFormatter& cppFormatter_, CodeFormatter& hppFormatter_);
    void BeginVisit(Grammar& grammar) override;
    void EndVisit(Grammar& grammar) override;
    void Visit(CharParser& parser) override;
    void Visit(StringParser& parser) override;
    void Visit(CharSetParser& parser) override;
    void Visit(KeywordParser& parser) override;
    void Visit(KeywordListParser& parser) override;
    void Visit(EmptyParser& parser) override;
    void Visit(SpaceParser& parser) override;
    void Visit(LetterParser& parser) override;
    void Visit(UpperLetterParser& parser) override;
    void Visit(LowerLetterParser& parser) override;
    void Visit(TitleLetterParser& parser) override;
    void Visit(ModifierLetterParser& parser) override;
    void Visit(OtherLetterParser& parser) override;
    void Visit(CasedLetterParser& parser) override;
    void Visit(DigitParser& parser) override;
    void Visit(HexDigitParser& parser) override;
    void Visit(MarkParser& parser) override;
    void Visit(NonspacingMarkParser& parser) override;
    void Visit(SpacingMarkParser& parser) override;
    void Visit(EnclosingMarkParser& parser) override;
    void Visit(NumberParser& parser) override;
    void Visit(DecimalNumberParser& parser) override;
    void Visit(LetterNumberParser& parser) override;
    void Visit(OtherNumberParser& parser) override;
    void Visit(PunctuationParser& parser) override;
    void Visit(ConnectorPunctuationParser& parser) override;
    void Visit(DashPunctuationParser& parser) override;
    void Visit(OpenPunctuationParser& parser) override;
    void Visit(ClosePunctuationParser& parser) override;
    void Visit(InitialPunctuationParser& parser) override;
    void Visit(FinalPunctuationParser& parser) override;
    void Visit(OtherPunctuationParser& parser) override;
    void Visit(SymbolParser& parser) override;
    void Visit(MathSymbolParser& parser) override;
    void Visit(CurrencySymbolParser& parser) override;
    void Visit(ModifierSymbolParser& parser) override;
    void Visit(OtherSymbolParser& parser) override;
    void Visit(SeparatorParser& parser) override;
    void Visit(SpaceSeparatorParser& parser) override;
    void Visit(LineSeparatorParser& parser) override;
    void Visit(ParagraphSeparatorParser& parser) override;
    void Visit(OtherParser& parser) override;
    void Visit(ControlParser& parser) override;
    void Visit(FormatParser& parser) override;
    void Visit(SurrogateParser& parser) override;
    void Visit(PrivateUseParser& parser) override;
    void Visit(UnassignedParser& parser) override;
    void Visit(GraphicParser& parser) override;
    void Visit(BaseCharParser& parser) override;
    void Visit(AlphabeticParser& parser) override;
    void Visit(IdStartParser& parser) override;
    void Visit(IdContParser& parser) override;
    void Visit(AnyCharParser& parser) override;
    void Visit(RangeParser& parser) override;
    void Visit(NonterminalParser& parser) override;
    void Visit(RuleLink& link) override;
    void BeginVisit(Rule& parser) override;
    void EndVisit(Rule& parser) override;
    void BeginVisit(OptionalParser& parser) override;
    void EndVisit(OptionalParser& parser) override;
    void BeginVisit(PositiveParser& parser) override;
    void EndVisit(PositiveParser& parser) override;
    void BeginVisit(KleeneStarParser& parser) override;
    void EndVisit(KleeneStarParser& parser) override;
    void BeginVisit(ActionParser& parser) override;
    void EndVisit(ActionParser& parser) override;
    void BeginVisit(ExpectationParser& parser) override;
    void EndVisit(ExpectationParser& parser) override;
    void BeginVisit(TokenParser& parser) override;
    void EndVisit(TokenParser& parser) override;
    void BeginVisit(GroupingParser& parser) override;
    void EndVisit(GroupingParser& parser) override;
    void BeginVisit(SequenceParser& parser) override;
    void Visit(SequenceParser& parser) override;
    void EndVisit(SequenceParser& parser) override;
    void BeginVisit(AlternativeParser& parser) override;
    void Visit(AlternativeParser& parser) override;
    void EndVisit(AlternativeParser& parser) override;
    void BeginVisit(DifferenceParser& parser) override;
    void Visit(DifferenceParser& parser) override;
    void EndVisit(DifferenceParser& parser) override;
    void BeginVisit(IntersectionParser& parser) override;
    void Visit(IntersectionParser& parser) override;
    void EndVisit(IntersectionParser& parser) override;
    void BeginVisit(ExclusiveOrParser& parser) override;
    void Visit(ExclusiveOrParser& parser) override;
    void EndVisit(ExclusiveOrParser& parser) override;
    void BeginVisit(ListParser& parser) override;
    void Visit(ListParser& parser) override;
    void EndVisit(ListParser& parser) override;
private:
    CodeFormatter& cppFormatter;
    CodeFormatter& hppFormatter;
    std::string CharSetStr();
};

} } // namespace cmajor::syntax

#endif // CMAJOR_SYNTAX_CODEGENERATOR_VISITOR_INCLUDED
