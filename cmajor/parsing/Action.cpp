// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/Action.hpp>
#include <cmajor/parsing/Visitor.hpp>
#include <cctype>

namespace cmajor { namespace parsing {

ParsingAction::~ParsingAction()
{
}

FailureAction::~FailureAction()
{
}

PreCall::~PreCall()
{
}

PostCall::~PostCall()
{
}

ActionParser::ActionParser(const std::u32string& name_, Parser* child_):
    UnaryParser(name_, child_, child_->Info()), successCode(), failCode(), action(), failureAction() 
{
}

ActionParser::ActionParser(const std::u32string& name_, cmajor::codedom::CompoundStatement* successCode_, Parser* child_): 
    UnaryParser(name_, child_, child_->Info()), successCode(successCode_), failCode(), action(), failureAction() 
{
}

ActionParser::ActionParser(const std::u32string& name_, cmajor::codedom::CompoundStatement* successCode_, cmajor::codedom::CompoundStatement* failCode_, 
    Parser* child_): UnaryParser(name_, child_, child_->Info()), successCode(successCode_), failCode(failCode_), action(), failureAction() 
{
}

std::u32string ActionParser::MethodName() const 
{ 
    return Name() + U"Action"; 
}

std::u32string ActionParser::VariableName() const 
{ 
    return (Name().length() > 0 ? std::u32string(1, std::tolower(Name()[0])) + Name().substr(1) : Name()) + U"ActionParser"; 
}

Match ActionParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    Span actionSpan = scanner.GetSpan();
    Match match = Child()->Parse(scanner, stack, parsingData);
    if (match.Hit())
    {
        if (action)
        {
            bool pass = true;
            actionSpan.SetEnd(scanner.GetSpan().Start());
            const char32_t* matchBegin = scanner.Start() + actionSpan.Start();
            const char32_t* matchEnd = scanner.Start() + actionSpan.End();
            (*action)(matchBegin, matchEnd, actionSpan, scanner.FileName(), parsingData, pass);
            if (!pass)
            {
                return Match::Nothing();
            }
        }
    }
    else if (failureAction)
    {
        (*failureAction)(parsingData);
    }
    return match;
}

void ActionParser::Accept(Visitor& visitor)
{
    visitor.BeginVisit(*this);
    Child()->Accept(visitor);
    visitor.EndVisit(*this);
}

} } // namespace cmajor::parsing
