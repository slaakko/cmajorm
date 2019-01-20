// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/Primitive.hpp>
#include <cmajor/parsing/Scanner.hpp>
#include <cmajor/parsing/Visitor.hpp>
#include <cmajor/parsing/Rule.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cctype>

namespace cmajor { namespace parsing {

using namespace cmajor::util;
using namespace cmajor::unicode;

CharParser::CharParser(char32_t c_): Parser(U"char", U"\"" + std::u32string(1, c_) + U"\""), c(c_)
{
}

Match CharParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (scanner.GetChar() == c)
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void CharParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

StringParser::StringParser(const std::u32string& s_): Parser(U"string", U"\"" + s_ + U"\""), s(s_) 
{
}

Match StringParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Match match = Match::Empty();
    int i = 0;
    int n = int(s.length());
    while (i < n && !scanner.AtEnd() && scanner.GetChar() == s[i])
    {
        ++scanner;
        ++i;
        match.Concatenate(Match::One());
    }
    if (i == n)
    {
        return match;
    }
    return Match::Nothing();
}

void StringParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

CharSetParser::CharSetParser(const std::u32string& s_): Parser(U"charSet", U"[" + s_ + U"]"), s(s_), inverse(false) 
{ 
    InitRanges(); 
}

CharSetParser::CharSetParser(const std::u32string& s_, bool inverse_): Parser(U"charSet", U"[" + s_ + U"]"), s(s_), inverse(inverse_) 
{ 
    InitRanges();
}

void CharSetParser::InitRanges()
{
    int i = 0;
    int n = int(s.length());
    while (i < n)
    {
        char32_t first = s[i];
        char32_t last = first;
        ++i;
        if (i < n)
        {
            if (s[i] == '-')
            {
                ++i;
                if (i < n)
                {
                    last = s[i];
                    ++i;
                }
                else
                {
                    ranges.push_back(CharRange(first, first));
                    first = '-';
                    last = '-';
                }
            }
        }
        ranges.push_back(CharRange(first, last));
    }
}

Match CharSetParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        char32_t c = scanner.GetChar();
        bool found = false;
        for (const CharRange& range : ranges)
        {
            if (range.Includes(c))
            {
                found = true;
                break;
            }
        }
        if (found != inverse)
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void CharSetParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

EmptyParser::EmptyParser(): Parser(U"empty", U"") 
{
}

Match EmptyParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    return Match::Empty();
}

void EmptyParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SpaceParser::SpaceParser(): Parser(U"space", U"space") 
{
}

Match SpaceParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsWhiteSpace(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void SpaceParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LetterParser::LetterParser(): Parser(U"letter", U"letter") 
{
}

Match LetterParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsLetter(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void LetterParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UpperLetterParser::UpperLetterParser() : Parser(U"upper_letter", U"upper_letter")
{
}

Match UpperLetterParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsUpperLetter(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void UpperLetterParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LowerLetterParser::LowerLetterParser() : Parser(U"lower_letter", U"lower_letter")
{
}

Match LowerLetterParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsLowerLetter(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void LowerLetterParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

TitleLetterParser::TitleLetterParser() : Parser(U"title_letter", U"title_letter")
{
}

Match TitleLetterParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsTitleLetter(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void TitleLetterParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ModifierLetterParser::ModifierLetterParser() : Parser(U"modifier_letter", U"modifier_letter")
{
}

Match ModifierLetterParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsModifierLetter(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void ModifierLetterParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

OtherLetterParser::OtherLetterParser() : Parser(U"other_letter", U"other_letter")
{
}

Match OtherLetterParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsOtherLetter(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void OtherLetterParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

CasedLetterParser::CasedLetterParser() : Parser(U"cased_letter", U"cased_letter")
{
}

Match CasedLetterParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsCasedLetter(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void CasedLetterParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DigitParser::DigitParser(): Parser(U"digit", U"digit")
{
}

Match DigitParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsAsciiDigit(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void DigitParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

HexDigitParser::HexDigitParser(): Parser(U"hexdigit", U"hexdigit") 
{
}

Match HexDigitParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsAsciiHexDigit(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void HexDigitParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

MarkParser::MarkParser() : Parser(U"mark", U"mark")
{
}

Match MarkParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsMark(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void MarkParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

NonspacingMarkParser::NonspacingMarkParser() : Parser(U"nonspacing_mark", U"nonspacing_mark")
{
}

Match NonspacingMarkParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsNonspacingMark(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void NonspacingMarkParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SpacingMarkParser::SpacingMarkParser() : Parser(U"spacing_mark", U"spacing_mark")
{
}

Match SpacingMarkParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsSpacingMark(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void SpacingMarkParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

EnclosingMarkParser::EnclosingMarkParser() : Parser(U"enclosing_mark", U"enclosing_mark")
{
}

Match EnclosingMarkParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsEnclosingMark(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void EnclosingMarkParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

NumberParser::NumberParser() : Parser(U"number", U"number")
{
}

Match NumberParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsNumber(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void NumberParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DecimalNumberParser::DecimalNumberParser() : Parser(U"decimal_number", U"decimal_number")
{
}

Match DecimalNumberParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsDecimalNumber(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void DecimalNumberParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LetterNumberParser::LetterNumberParser() : Parser(U"letter_number", U"letter_number")
{
}

Match LetterNumberParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsLetterNumber(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void LetterNumberParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

OtherNumberParser::OtherNumberParser() : Parser(U"other_number", U"other_number")
{
}

Match OtherNumberParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsOtherNumber(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void OtherNumberParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

PunctuationParser::PunctuationParser(): Parser(U"punctuation", U"punctuation")
{
}

Match PunctuationParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsPunctuation(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void PunctuationParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ConnectorPunctuationParser::ConnectorPunctuationParser() : Parser(U"connector_punctuation", U"connector_punctuation")
{
}

Match ConnectorPunctuationParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsConnectorPunctuation(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void ConnectorPunctuationParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

DashPunctuationParser::DashPunctuationParser() : Parser(U"dash_punctuation", U"dash_punctuation")
{
}

Match DashPunctuationParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsDashPunctuation(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void DashPunctuationParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

OpenPunctuationParser::OpenPunctuationParser() : Parser(U"open_punctuation", U"open_punctuation")
{
}

Match OpenPunctuationParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsOpenPunctuation(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void OpenPunctuationParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ClosePunctuationParser::ClosePunctuationParser() : Parser(U"close_punctuation", U"close_punctuation")
{
}

Match ClosePunctuationParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsClosePunctuation(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void ClosePunctuationParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

InitialPunctuationParser::InitialPunctuationParser() : Parser(U"initial_punctuation", U"initial_punctuation")
{
}

Match InitialPunctuationParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsInitialPunctuation(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void InitialPunctuationParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

FinalPunctuationParser::FinalPunctuationParser() : Parser(U"final_punctuation", U"final_punctuation")
{
}

Match FinalPunctuationParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsFinalPunctuation(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void FinalPunctuationParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

OtherPunctuationParser::OtherPunctuationParser() : Parser(U"other_punctuation", U"other_punctuation")
{
}

Match OtherPunctuationParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsOtherPunctuation(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void OtherPunctuationParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SymbolParser::SymbolParser() : Parser(U"symbol", U"symbol")
{
}

Match SymbolParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsSymbol(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void SymbolParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

MathSymbolParser::MathSymbolParser() : Parser(U"math_symbol", U"math_symbol")
{
}

Match MathSymbolParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsMathSymbol(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void MathSymbolParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

CurrencySymbolParser::CurrencySymbolParser() : Parser(U"currency_symbol", U"currency_symbol")
{
}

Match CurrencySymbolParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsCurrencySymbol(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void CurrencySymbolParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ModifierSymbolParser::ModifierSymbolParser() : Parser(U"modifier_symbol", U"modifier_symbol")
{
}

Match ModifierSymbolParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsModifierSymbol(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void ModifierSymbolParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

OtherSymbolParser::OtherSymbolParser() : Parser(U"other_symbol", U"other_symbol")
{
}

Match OtherSymbolParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsOtherSymbol(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void OtherSymbolParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SeparatorParser::SeparatorParser() : Parser(U"separator", U"separator")
{
}

Match SeparatorParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsSeparator(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void SeparatorParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SpaceSeparatorParser::SpaceSeparatorParser() : Parser(U"space_separator", U"space_separator")
{
}

Match SpaceSeparatorParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsSpaceSeparator(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void SpaceSeparatorParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

LineSeparatorParser::LineSeparatorParser() : Parser(U"line_separator", U"line_separator")
{
}

Match LineSeparatorParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsLineSeparator(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void LineSeparatorParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ParagraphSeparatorParser::ParagraphSeparatorParser() : Parser(U"paragraph_separator", U"paragraph_separator")
{
}

Match ParagraphSeparatorParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsParagraphSeparator(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void ParagraphSeparatorParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

OtherParser::OtherParser() : Parser(U"other", U"other")
{
}

Match OtherParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsOther(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void OtherParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

ControlParser::ControlParser() : Parser(U"control", U"control")
{
}

Match ControlParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsControl(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void ControlParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

FormatParser::FormatParser() : Parser(U"format", U"format")
{
}

Match FormatParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsFormat(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void FormatParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

SurrogateParser::SurrogateParser() : Parser(U"surrogate", U"surrogate")
{
}

Match SurrogateParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsSurrogate(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void SurrogateParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

PrivateUseParser::PrivateUseParser() : Parser(U"private_use", U"private_use")
{
}

Match PrivateUseParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsPrivateUse(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void PrivateUseParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

UnassignedParser::UnassignedParser() : Parser(U"unassigned", U"unassigned")
{
}

Match UnassignedParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsUnassigned(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void UnassignedParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

GraphicParser::GraphicParser() : Parser(U"graphic", U"graphic")
{
}

Match GraphicParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsGraphic(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void GraphicParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

BaseCharParser::BaseCharParser() : Parser(U"basechar", U"basechar")
{
}

Match BaseCharParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsBase(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void BaseCharParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

AlphabeticParser::AlphabeticParser() : Parser(U"alphabetic", U"alphabetic")
{
}

Match AlphabeticParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsAlphabetic(scanner.GetChar()))
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void AlphabeticParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

IdStartParser::IdStartParser() : Parser(U"idstart", U"idstart")
{
}

Match IdStartParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsIdStart(scanner.GetChar()) || scanner.GetChar() == '_')
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void IdStartParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

IdContParser::IdContParser() : Parser(U"idcont", U"idcont")
{
}

Match IdContParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        if (IsIdCont(scanner.GetChar()) || scanner.GetChar() == '_')
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void IdContParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

AnyCharParser::AnyCharParser(): Parser(U"anychar", U"anychar")
{
}

Match AnyCharParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        ++scanner;
        return Match::One();
    }
    return Match::Nothing();
}

void AnyCharParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

RangeParser::RangeParser(uint32_t start_, uint32_t end_): Parser(U"range", U"range(" + ToUtf32(std::to_string(start_)) + U"," + ToUtf32(std::to_string(end_)) + U")"), start(start_), end(end_)
{
}

Match RangeParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (!scanner.AtEnd())
    {
        char32_t c = scanner.GetChar();
        if (c >= start && c <= end)
        {
            ++scanner;
            return Match::One();
        }
    }
    return Match::Nothing();
}

void RangeParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace cmajor::parsing
