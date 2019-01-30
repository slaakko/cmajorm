// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_COMPOSITE_INCLUDED
#define CMAJOR_PARSING_COMPOSITE_INCLUDED
#include <cmajor/parsing/Parser.hpp>

namespace cmajor { namespace parsing {

class Visitor;

class PARSING_API UnaryParser : public Parser
{
public:
    UnaryParser(const std::u32string& name_, Parser* child_, const std::u32string& info_);
    Parser* Child() const { return child; }
private:
    Parser* child;
};

class PARSING_API OptionalParser : public UnaryParser
{
public:
    OptionalParser(Parser* child_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PARSING_API PositiveParser : public UnaryParser
{
public:
    PositiveParser(Parser* child_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PARSING_API KleeneStarParser : public UnaryParser
{
public:
    KleeneStarParser(Parser* child_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PARSING_API ExpectationParser : public UnaryParser
{
public:
    ExpectationParser(Parser* child_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PARSING_API TokenParser : public UnaryParser
{
public:
    TokenParser(Parser* child_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PARSING_API GroupingParser : public UnaryParser
{
public:
    GroupingParser(Parser* child_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PARSING_API BinaryParser : public Parser
{
public:
    BinaryParser(const std::u32string& name_, Parser* left_, Parser* right_, const std::u32string& info_);
    Parser* Left() const { return left; }
    Parser* Right() const { return right; }
private:
    Parser* left;
    Parser* right;
};

class PARSING_API SequenceParser : public BinaryParser
{
public:
    SequenceParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PARSING_API AlternativeParser : public BinaryParser
{
public:
    AlternativeParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PARSING_API DifferenceParser : public BinaryParser
{
public:
    DifferenceParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PARSING_API ExclusiveOrParser : public BinaryParser
{
public:
    ExclusiveOrParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PARSING_API IntersectionParser : public BinaryParser
{
public:
    IntersectionParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
};

class PARSING_API ListParser : public UnaryParser
{
public:
    ListParser(Parser* left_, Parser* right_);
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
private:
    Parser* left;
    Parser* right;
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_COMPOSITE_INCLUDED
