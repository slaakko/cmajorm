// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_PRIMITIVE_INCLUDED
#define CMAJOR_PARSING_PRIMITIVE_INCLUDED
#include <cmajor/parsing/Parser.hpp>
#include <limits.h>
#include <bitset>

namespace cmajor { namespace parsing {

class Visitor;
class Rule;

class CharParser : public Parser
{
public:
    CharParser(char32_t c_);
    char32_t GetChar() const { return c; }
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
private:
    char32_t c;
};

class StringParser : public Parser
{
public:
    StringParser(const std::u32string& s_);
    const std::u32string& GetString() const { return s; }
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string s;
};

struct CharRange
{
    CharRange(char32_t start_, char32_t end_) : start(start_), end(end_) {}
    bool Includes(char32_t c) const { return c >= start && c <= end; }
    char32_t start;
    char32_t end;
};

class CharSetParser : public Parser
{
public:
    CharSetParser(const std::u32string& s_);
    CharSetParser(const std::u32string& s_, bool inverse_);
    const std::u32string& Set() const { return s; }
    bool Inverse() const { return inverse; }
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string s;
    bool inverse;
    std::vector<CharRange> ranges;
    void InitRanges();
};

class EmptyParser : public Parser
{
public:
    EmptyParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class SpaceParser : public Parser
{
public:
    SpaceParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class LetterParser : public Parser
{
public:
    LetterParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class UpperLetterParser : public Parser
{
public:
    UpperLetterParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class LowerLetterParser : public Parser
{
public:
    LowerLetterParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class TitleLetterParser : public Parser
{
public:
    TitleLetterParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class ModifierLetterParser : public Parser
{
public:
    ModifierLetterParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class OtherLetterParser : public Parser
{
public:
    OtherLetterParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class CasedLetterParser : public Parser
{
public:
    CasedLetterParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class DigitParser : public Parser
{
public:
    DigitParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class HexDigitParser : public Parser
{
public:
    HexDigitParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class MarkParser : public Parser
{
public:
    MarkParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class NonspacingMarkParser : public Parser
{
public:
    NonspacingMarkParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class SpacingMarkParser : public Parser
{
public:
    SpacingMarkParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class EnclosingMarkParser : public Parser
{
public:
    EnclosingMarkParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class NumberParser : public Parser
{
public:
    NumberParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class DecimalNumberParser : public Parser
{
public:
    DecimalNumberParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class LetterNumberParser : public Parser
{
public:
    LetterNumberParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class OtherNumberParser : public Parser
{
public:
    OtherNumberParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PunctuationParser : public Parser
{
public:
    PunctuationParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class ConnectorPunctuationParser : public Parser
{
public:
    ConnectorPunctuationParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class DashPunctuationParser : public Parser
{
public:
    DashPunctuationParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class OpenPunctuationParser : public Parser
{
public:
    OpenPunctuationParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class ClosePunctuationParser : public Parser
{
public:
    ClosePunctuationParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class InitialPunctuationParser : public Parser
{
public:
    InitialPunctuationParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class FinalPunctuationParser : public Parser
{
public:
    FinalPunctuationParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class OtherPunctuationParser : public Parser
{
public:
    OtherPunctuationParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class SymbolParser : public Parser
{
public:
    SymbolParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class MathSymbolParser : public Parser
{
public:
    MathSymbolParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class CurrencySymbolParser : public Parser
{
public:
    CurrencySymbolParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class ModifierSymbolParser : public Parser
{
public:
    ModifierSymbolParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class OtherSymbolParser : public Parser
{
public:
    OtherSymbolParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class SeparatorParser : public Parser
{
public:
    SeparatorParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class SpaceSeparatorParser : public Parser
{
public:
    SpaceSeparatorParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class LineSeparatorParser : public Parser
{
public:
    LineSeparatorParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class ParagraphSeparatorParser : public Parser
{
public:
    ParagraphSeparatorParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class OtherParser : public Parser
{
public:
    OtherParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class ControlParser : public Parser
{
public:
    ControlParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class FormatParser : public Parser
{
public:
    FormatParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class SurrogateParser : public Parser
{
public:
    SurrogateParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PrivateUseParser : public Parser
{
public:
    PrivateUseParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class UnassignedParser : public Parser
{
public:
    UnassignedParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class GraphicParser : public Parser
{
public:
    GraphicParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class BaseCharParser : public Parser
{
public:
    BaseCharParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class AlphabeticParser : public Parser
{
public:
    AlphabeticParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class IdStartParser : public Parser
{
public:
    IdStartParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class IdContParser : public Parser
{
public:
    IdContParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class AnyCharParser : public Parser
{
public:
    AnyCharParser();
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class RangeParser : public Parser
{
public:
    RangeParser(uint32_t start_, uint32_t end_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
    uint32_t Start() const { return start; }
    uint32_t End() const { return end; }
private:
    uint32_t start;
    uint32_t end;
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_PRIMITIVE_INCLUDED
