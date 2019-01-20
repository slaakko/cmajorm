// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/Composite.hpp>
#include <cmajor/parsing/Scanner.hpp>
#include <cmajor/parsing/Visitor.hpp>
#include <cmajor/parsing/Exception.hpp>

namespace cmajor { namespace parsing {

UnaryParser::UnaryParser(const std::u32string& name_, Parser* child_, const std::u32string& info_): Parser(name_, info_), child(child_) 
{
    Own(child);
}

OptionalParser::OptionalParser(Parser* child_): UnaryParser(U"optional", child_, child_->Info() + U"?") 
{
}

Match OptionalParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Span save = scanner.GetSpan();
    Match match = Child()->Parse(scanner, stack, parsingData);
    if (match.Hit())
    {
        return match;
    }
    scanner.SetSpan(save);
    return Match::Empty();
}

void OptionalParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Child()->Accept(visitor);
    visitor.EndVisit(*this);
}

PositiveParser::PositiveParser(Parser* child_): UnaryParser(U"positive", child_, child_->Info() + U"+")
{
}

Match PositiveParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Match match = Child()->Parse(scanner, stack, parsingData);
    if (match.Hit())
    {
        for (;;)
        {
            Span save = scanner.GetSpan();
            scanner.Skip();
            Match next = Child()->Parse(scanner, stack, parsingData);
            if (next.Hit())
            {
                match.Concatenate(next);
            }
            else
            {
                scanner.SetSpan(save);
                break;
            }
        }
    }
    return match;
}

void PositiveParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Child()->Accept(visitor);
    visitor.EndVisit(*this);
}

KleeneStarParser::KleeneStarParser(Parser* child_): UnaryParser(U"kleene", child_, child_->Info() + U"*")
{
}

Match KleeneStarParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Match match = Match::Empty();
    bool first = true;
    for (;;)
    {
        Span save = scanner.GetSpan();
        if (first)
        {
            first = false;
        }
        else
        {
            scanner.Skip();
        }
        Match next = Child()->Parse(scanner, stack, parsingData);
        if (next.Hit())
        {
            match.Concatenate(next);
        }
        else
        {
            scanner.SetSpan(save);
            break;
        }
    }
    return match;
}

void KleeneStarParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Child()->Accept(visitor);
    visitor.EndVisit(*this);
}

ExpectationParser::ExpectationParser(Parser* child_): UnaryParser(U"expectation", child_, child_->Info() + U"!") 
{
}

Match ExpectationParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Span expectationSpan = scanner.GetSpan();
    Match match = Match::Nothing();
    try
    {
        match = Child()->Parse(scanner, stack, parsingData);
    }
    catch (const ExpectationFailure& ex)
    {
        throw ExpectationFailure(Child()->Info() + ex.Info(), ex.FileName(), ex.GetSpan(), ex.Start(), ex.End());
    }
    if (match.Hit())
    {
        return match;
    }
    else
    {
        throw ExpectationFailure(Child()->Info(), scanner.FileName(), expectationSpan, scanner.Start(), scanner.End());
    }
}

void ExpectationParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Child()->Accept(visitor);
    visitor.EndVisit(*this);
}


TokenParser::TokenParser(Parser* child_): UnaryParser(U"token", child_, child_->Info()) 
{
}

Match TokenParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    scanner.BeginToken();
    Match match = Child()->Parse(scanner, stack, parsingData);
    scanner.EndToken();
    return match;
}

void TokenParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Child()->Accept(visitor);
    visitor.EndVisit(*this);
}

GroupingParser::GroupingParser(Parser* child_) : UnaryParser(U"grouping", child_, child_->Info())
{
}

Match GroupingParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    return Child()->Parse(scanner, stack, parsingData);
}

void GroupingParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Child()->Accept(visitor);
    visitor.EndVisit(*this);
}

BinaryParser::BinaryParser(const std::u32string& name_, Parser* left_, Parser* right_, const std::u32string& info_): Parser(name_, info_), left(left_), right(right_) 
{
    Own(left);
    Own(right);
}

SequenceParser::SequenceParser(Parser* left_, Parser* right_): BinaryParser(U"sequence", left_, right_, left_->Info()) 
{
}

Match SequenceParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Match leftMatch = Left()->Parse(scanner, stack, parsingData);
    if (leftMatch.Hit())
    {
        scanner.Skip();
        Match rightMatch = Right()->Parse(scanner, stack, parsingData);
        if (rightMatch.Hit())
        {
            leftMatch.Concatenate(rightMatch);
            return leftMatch;
        }
    }
    return Match::Nothing();
}

void SequenceParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Left()->Accept(visitor);
    visitor.Visit(*this);
    Right()->Accept(visitor);
    visitor.EndVisit(*this);
}

AlternativeParser::AlternativeParser(Parser* left_, Parser* right_): BinaryParser(U"alternative", left_, right_, left_->Info() + U" | " + right_->Info()) 
{
}

Match AlternativeParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Span save = scanner.GetSpan();
    Match leftMatch = Left()->Parse(scanner, stack, parsingData);
    if (leftMatch.Hit())
    {
        return leftMatch;
    }
    scanner.SetSpan(save);
    return Right()->Parse(scanner, stack, parsingData);
}

void AlternativeParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Left()->Accept(visitor);
    visitor.Visit(*this);
    Right()->Accept(visitor);
    visitor.EndVisit(*this);
}

DifferenceParser::DifferenceParser(Parser* left_, Parser* right_): BinaryParser(U"difference", left_, right_, left_->Info() + U" - " + right_->Info()) 
{
}

Match DifferenceParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Span save = scanner.GetSpan();
    Match leftMatch = Left()->Parse(scanner, stack, parsingData);
    if (leftMatch.Hit())
    {
        Span tmp = scanner.GetSpan();
        scanner.SetSpan(save);
        save = tmp;
        Match rightMatch = Right()->Parse(scanner, stack, parsingData);
        if (!rightMatch.Hit() || rightMatch.Length() < leftMatch.Length())
        {
            scanner.SetSpan(save);
            return leftMatch;
        }
    }
    return Match::Nothing();
}

void DifferenceParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Left()->Accept(visitor);
    visitor.Visit(*this);
    Right()->Accept(visitor);
    visitor.EndVisit(*this);
}

ExclusiveOrParser::ExclusiveOrParser(Parser* left_, Parser* right_): BinaryParser(U"exclusiveOr", left_, right_, left_->Info() + U" ^ " + right_->Info()) 
{
}

Match ExclusiveOrParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Span save = scanner.GetSpan();
    Match leftMatch = Left()->Parse(scanner, stack, parsingData);
    Span temp = scanner.GetSpan();
    scanner.SetSpan(save);
    save = temp;
    Match rightMatch = Right()->Parse(scanner, stack, parsingData);
    bool match = leftMatch.Hit() ? !rightMatch.Hit() : rightMatch.Hit();
    if (match)
    {
        if (leftMatch.Hit())
        {
            scanner.SetSpan(save);
        }
        return leftMatch.Hit() ? leftMatch : rightMatch;
    }
    return Match::Nothing();
}

void ExclusiveOrParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Left()->Accept(visitor);
    visitor.Visit(*this);
    Right()->Accept(visitor);
    visitor.EndVisit(*this);
}

IntersectionParser::IntersectionParser(Parser* left_, Parser* right_): BinaryParser(U"intersection", left_, right_, left_->Info() + U" & " + right_->Info()) 
{
}

Match IntersectionParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Span save = scanner.GetSpan();
    Match leftMatch = Left()->Parse(scanner, stack, parsingData);
    if (leftMatch.Hit())
    {
        scanner.SetSpan(save);
        Match rightMatch = Right()->Parse(scanner, stack, parsingData);
        if (leftMatch.Length() == rightMatch.Length())
        {
            return leftMatch;
        }
    }
    return Match::Nothing();
}

void IntersectionParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Left()->Accept(visitor);
    visitor.Visit(*this);
    Right()->Accept(visitor);
    visitor.EndVisit(*this);
}

ListParser::ListParser(Parser* left_, Parser* right_): 
    UnaryParser(U"list", new SequenceParser(left_, new KleeneStarParser(new SequenceParser(right_, left_))), 
        left_->Info() + U" % " + right_->Info()), left(left_), right(right_)
{
}

Match ListParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    return Child()->Parse(scanner, stack, parsingData);
}

void ListParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    left->Accept(visitor);
    visitor.Visit(*this);
    right->Accept(visitor);
    visitor.EndVisit(*this);
}

} } // namespace cmajor::parsing
