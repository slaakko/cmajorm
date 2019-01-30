// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_PARSER_INCLUDED
#define CMAJOR_PARSING_PARSER_INCLUDED
#include <cmajor/parsing/Match.hpp>
#include <cmajor/parsing/ParsingData.hpp>
#include <cmajor/parsing/ParsingObject.hpp>
#include <stack>

namespace cmajor { namespace parsing {

class PARSING_API Object
{
public:
    virtual ~Object();
};

typedef std::stack<std::unique_ptr<Object>> ObjectStack;

template<typename ValueType>
class ValueObject: public Object
{
public:
    ValueObject(): value() {}
    ValueObject(const ValueType& value_): value(value_) {}
    ValueObject<ValueType>& operator=(const ValueType& v) { value = v; return *this; }
    operator ValueType() const { return value; }
private:
    ValueType value;
};

class Scanner;

class PARSING_API Parser: public ParsingObject
{
public:
    Parser(const std::u32string& name_, const std::u32string& info_);
    Parser(const std::u32string& name_, const std::u32string& info_, ObjectKind kind_);
    const std::u32string& Info() const { return info; }
    virtual Match Parse(Scanner& scanner, ObjectStack& stack, ParsingData* parsingData) = 0;
private:
    std::u32string info;
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_PARSER_INCLUDED

