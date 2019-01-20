// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/parsing/Nonterminal.hpp>
#include <cmajor/parsing/Action.hpp>
#include <cmajor/parsing/Rule.hpp>
#include <cmajor/parsing/Visitor.hpp>

namespace cmajor { namespace parsing {

NonterminalParser::NonterminalParser(const std::u32string& name_, const std::u32string& ruleName_):
    Parser(name_, U"<" + name_ + U">"), name(name_), ruleName(ruleName_), numberOfArguments(0), preCall(), postCall()
{
}

NonterminalParser::NonterminalParser(const std::u32string& name_, const std::u32string& ruleName_, int numberOfArguments_):
    Parser(name_, U"<" + name_ + U">"), name(name_), ruleName(ruleName_), numberOfArguments(numberOfArguments_), preCall(), postCall()
{
}

void NonterminalParser::SetPreCall(PreCall* preCall_) 
{ 
    preCall = std::unique_ptr<PreCall>(preCall_); 
}

void NonterminalParser::SetPostCall(PostCall* postCall_) 
{ 
    postCall = std::unique_ptr<PostCall>(postCall_); 
}

void NonterminalParser::SetArguments(const ArgumentVector& arguments_)
{
    arguments = arguments_; 
    for (cmajor::codedom::CppObject* arg : arguments)
    {
        if (!arg->IsOwned())
        {
            arg->SetOwned();
            ownedArgs.push_back(std::unique_ptr<cmajor::codedom::CppObject>(arg));
        }
    }
}

bool NonterminalParser::Specialized() const 
{ 
    return numberOfArguments > 0 || !arguments.empty() || 
        (rule && rule->TakesOrReturnsValuesOrHasLocals()); 
}

Match NonterminalParser::Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData)
{
    if (rule)
    {
        if (preCall)
        {
            (*preCall)(stack, parsingData);
        }
        Match match = rule->Parse(scanner, stack, parsingData);
        if (postCall)
        {
            (*postCall)(stack, parsingData, match.Hit());
        }
        return match;
    }
    return Match::Nothing();
}

void NonterminalParser::Accept(Visitor& visitor)
{
    visitor.Visit(*this);
}

} } // namespace cmajor::parsing
