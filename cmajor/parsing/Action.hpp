// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_ACTION_INCLUDED
#define CMAJOR_PARSING_ACTION_INCLUDED
#include <cmajor/parsing/Composite.hpp>
#include <cmajor/parsing/Scanner.hpp>
#include <cmajor/codedom/Statement.hpp>

namespace cmajor { namespace parsing {

class PARSING_API ParsingAction
{
public:
    virtual ~ParsingAction();
    virtual void operator()(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass) const = 0;
};

template<typename Type>
class MemberParsingAction: public ParsingAction
{
public:
    MemberParsingAction(Type* type_, void (Type::*pm)(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)) :
        type(type_), memfun(pm)
    {
    }
    void operator()(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass) const override
    {
        (type->*memfun)(matchBegin, matchEnd, span, fileName, parsingData, pass);
    }
private:
    Type* type;
    void (Type::*memfun)(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass);
};

class PARSING_API FailureAction
{
public:
    virtual ~FailureAction();
    virtual void operator()(ParsingData* parsingData) const = 0;
};

template<typename Type>
class MemberFailureAction: public FailureAction
{
public:
    MemberFailureAction(Type* type_, void (Type::*pm)(ParsingData* parsingData)) :
        type(type_), memfun(pm)
    {
    }
    void operator()(ParsingData* parsingData) const override
    {
        (type->*memfun)(parsingData);
    }
private:
    Type* type;
    void (Type::*memfun)(ParsingData* parsingData);
};

class PARSING_API PreCall
{
public:
    virtual ~PreCall();
    virtual void operator()(ObjectStack& stack, ParsingData* parsingData) const = 0;
};

template <typename Type>
class MemberPreCall : public PreCall
{
public:
    MemberPreCall(Type* type_, void (Type::*pm)(ObjectStack& stack, ParsingData* parsingData)) :
        type(type_), memfun(pm)
    {
    }
    void operator()(ObjectStack& stack, ParsingData* parsingData) const override
    {
        (type->*memfun)(stack, parsingData);
    }
private:
    Type* type;
    void (Type::*memfun)(ObjectStack& stack, ParsingData* parsingData);
};

class PARSING_API PostCall
{
public:
    virtual ~PostCall();
    virtual void operator()(ObjectStack& stack, ParsingData* parsingData, bool matched) const = 0;
};

template <typename Type>
class MemberPostCall: public PostCall
{
public:
    MemberPostCall(Type* type_, void (Type::*pm)(ObjectStack& stack, ParsingData* parsingData, bool matched)) :
        type(type_), memfun(pm)
    {
    }
    void operator()(ObjectStack& stack, ParsingData* parsingData, bool matched) const override
    {
        (type->*memfun)(stack, parsingData, matched);
    }
private:
    Type* type;
    void (Type::*memfun)(ObjectStack& stack, ParsingData* parsingData, bool matched);
};

class PARSING_API ActionParser : public UnaryParser
{
public:
    ActionParser(const std::u32string& name_, Parser* child_);
    ActionParser(const std::u32string& name_, cmajor::codedom::CompoundStatement* successCode_, Parser* child_);
    ActionParser(const std::u32string& name_, cmajor::codedom::CompoundStatement* successCode_, cmajor::codedom::CompoundStatement* failCode_, 
        Parser* child_);
    bool IsActionParser() const override { return true; }
    std::u32string MethodName() const;
    std::u32string VariableName() const;
    cmajor::codedom::CompoundStatement* SuccessCode() const { return successCode.get(); }
    cmajor::codedom::CompoundStatement* FailCode() const { return failCode.get(); }
    void SetAction(ParsingAction* action_) { action = std::unique_ptr<ParsingAction>(action_); }
    void SetFailureAction(FailureAction* failureAction_) { failureAction = std::unique_ptr<FailureAction>(failureAction_); }
    Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) override;
    void Accept(Visitor& visitor) override;
private:
    std::unique_ptr<cmajor::codedom::CompoundStatement> successCode;
    std::unique_ptr<cmajor::codedom::CompoundStatement> failCode;
    std::unique_ptr<ParsingAction> action;
    std::unique_ptr<FailureAction> failureAction;
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_ACTION_INCLUDED

