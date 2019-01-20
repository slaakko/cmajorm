// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/codedom/Operator.hpp>
#include <cmajor/util/Unicode.hpp>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace cmajor { namespace codedom {

using namespace cmajor::unicode;

struct OperatorHash
{
    size_t operator()(Operator op) const { return size_t(op); }
};

class OperatorTable
{
public:
    static void Init();
    static void Done();
    static OperatorTable& Instance() 
    {
        return *instance;
    }
    Operator GetOperator(const std::u32string& operatorName);
    std::u32string GetOperatorStr(Operator op);
private:
    static std::unique_ptr<OperatorTable> instance;
    OperatorTable();
    std::unordered_map<std::u32string, Operator> operatorMap;
    std::unordered_map<Operator, std::u32string, OperatorHash> operatorStrMap;
};

std::unique_ptr<OperatorTable>  OperatorTable::instance;

OperatorTable::OperatorTable()
{
    operatorMap[U"*"] = Operator::mul;
    operatorMap[U"&&"] = Operator::and_;
    operatorMap[U"||"] = Operator::or_;
    operatorMap[U"&"] = Operator::bitand_;
    operatorMap[U"^"] = Operator::bitxor;
    operatorMap[U"|"] = Operator::bitor_;
    operatorMap[U"+"] = Operator::plus;
    operatorMap[U"-"] = Operator::minus;
    operatorMap[U"!"] = Operator::not_;
    operatorMap[U"~"] = Operator::neg;
    operatorMap[U".*"] = Operator::dotStar;
    operatorMap[U"->*"] = Operator::arrowStar;
    operatorMap[U"/"] = Operator::div;
    operatorMap[U"%"] = Operator::rem;
    operatorMap[U"<<"] = Operator::shiftLeft;
    operatorMap[U">>"] = Operator::shiftRight;
    operatorMap[U"<"] = Operator::less;
    operatorMap[U">"] = Operator::greater;
    operatorMap[U"<="] = Operator::lessOrEq;
    operatorMap[U">="] = Operator::greaterOrEq;
    operatorMap[U"=="] = Operator::eq;
    operatorMap[U"!="] = Operator::notEq;
    operatorMap[U","] = Operator::comma;
    operatorMap[U"="] = Operator::assign;
    operatorMap[U"*="] = Operator::mulAssing;
    operatorMap[U"/="] = Operator::divAssign;
    operatorMap[U"%="] = Operator::remAssign;
    operatorMap[U"+="] = Operator::plusAssign;
    operatorMap[U"-="] = Operator::minusAssing;
    operatorMap[U"<<="] = Operator::shiftLeftAssign;
    operatorMap[U">>="] = Operator::shiftRightAssign;
    operatorMap[U"&="] = Operator::andAssing;
    operatorMap[U"^="] = Operator::xorAssing;
    operatorMap[U"|="] = Operator::orAssign;
    operatorStrMap[Operator::deref] = U"*";
    operatorStrMap[Operator::addrOf] = U"&";
    operatorStrMap[Operator::plus] = U"+";
    operatorStrMap[Operator::minus] = U"-";
    operatorStrMap[Operator::not_] = U"!";
    operatorStrMap[Operator::neg] = U"~";
    operatorStrMap[Operator::dotStar] = U".*";
    operatorStrMap[Operator::arrowStar] = U"->*";
    operatorStrMap[Operator::and_] = U"&&";
    operatorStrMap[Operator::or_] = U"||";
    operatorStrMap[Operator::bitand_] = U"&";
    operatorStrMap[Operator::bitxor] = U"^";
    operatorStrMap[Operator::bitor_] = U"|";
    operatorStrMap[Operator::mul] = U"*";
    operatorStrMap[Operator::div] = U"/";
    operatorStrMap[Operator::rem] = U"%";
    operatorStrMap[Operator::shiftLeft] = U"<<";
    operatorStrMap[Operator::shiftRight] = U">>";
    operatorStrMap[Operator::less] = U"<";
    operatorStrMap[Operator::greater] = U">";
    operatorStrMap[Operator::lessOrEq] = U"<=";
    operatorStrMap[Operator::greaterOrEq] = U">=";
    operatorStrMap[Operator::eq] = U"==";
    operatorStrMap[Operator::notEq] = U"!=";
    operatorStrMap[Operator::comma] = U",";
    operatorStrMap[Operator::assign] = U"=";
    operatorStrMap[Operator::mulAssing] = U"*=";
    operatorStrMap[Operator::divAssign] = U"/=";
    operatorStrMap[Operator::remAssign] = U"%=";
    operatorStrMap[Operator::plusAssign] = U"+=";
    operatorStrMap[Operator::minusAssing] = U"-=";
    operatorStrMap[Operator::shiftLeftAssign] = U"<<=";
    operatorStrMap[Operator::shiftRightAssign] = U">>=";
    operatorStrMap[Operator::andAssing] = U"&=";
    operatorStrMap[Operator::xorAssing] = U"^=";
    operatorStrMap[Operator::orAssign] = U"|=";
}

void OperatorTable::Init()
{
    instance.reset(new OperatorTable());
}

void OperatorTable::Done()
{
    instance.reset();
}

Operator OperatorTable::GetOperator(const std::u32string& operatorName)
{
    std::unordered_map<std::u32string, Operator>::const_iterator i = operatorMap.find(operatorName);
    if (i != operatorMap.end())
    {
        return i->second;
    }
    throw std::runtime_error("unknown operator '" + ToUtf8(operatorName) + "'");
}

std::u32string OperatorTable::GetOperatorStr(Operator op)
{
    std::unordered_map<Operator, std::u32string, OperatorHash>::const_iterator i = operatorStrMap.find(op);
    if (i != operatorStrMap.end())
    {
        return i->second;
    }
    throw std::runtime_error("unknown operator");
}

Operator GetOperator(const std::u32string& operatorName)
{
    return OperatorTable::Instance().GetOperator(operatorName);
}

std::u32string GetOperatorStr(Operator op)
{
    return OperatorTable::Instance().GetOperatorStr(op);
}

void OperatorInit()
{
    OperatorTable::Init();
}

void OperatorDone()
{
    OperatorTable::Done();
}

} } // namespace cmajor::codedom
