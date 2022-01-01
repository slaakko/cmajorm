// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmtrace/TraceExpr.hpp>
#include <cmajor/cmtrace/TraceLexer.hpp>
#include <cmajor/cmtrace/TraceParser.hpp>
#include <soulng/rex/Match.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <chrono>

namespace cmtrace {

using namespace soulng::unicode;

TraceExpr::~TraceExpr()
{
}

Stack::Stack()
{
}

void Stack::Push(TraceExpr* expr)
{
    stack.push_back(std::unique_ptr<TraceExpr>(expr));
}

std::unique_ptr<TraceExpr> Stack::Pop()
{
    std::unique_ptr<TraceExpr> top = std::move(stack.back());
    stack.pop_back();
    return top;
}

String::String(const std::string& value_) : value(value_)
{
}

TraceExpr* String::Clone() const
{
    return new String(value);
}

void String::Evaluate(Stack& stack, FunctionNode& function) const
{
    stack.Push(Clone());
}

std::string MakeString(const std::string& value)
{
    return value.substr(1, value.length() - 2);
}

std::string MakePattern(const std::string& value)
{
    return value.substr(1, value.length() - 2);
}

Pattern::Pattern(const std::string& value_) : value(value_)
{
}

TraceExpr* Pattern::Clone() const
{
    return new Pattern(value);
}

void Pattern::Evaluate(Stack& stack, FunctionNode& function) const
{
    stack.Push(Clone());
}

int64_t ToInteger(const std::u32string& s)
{
    std::string str = ToUtf8(s);
    std::string n;
    for (char c : str)
    {
        if (c != '\'')
        {
            n.append(1, c);
        }
    }
    return boost::lexical_cast<int64_t>(n);
}

Integer::Integer(int64_t value_) : value(value_)
{
}

TraceExpr* Integer::Clone() const
{
    return new Integer(value);
}

void Integer::Evaluate(Stack& stack, FunctionNode& function) const
{
    stack.Push(Clone());
}

Duration::Duration(int64_t value_, Unit unit_) : value(value_), unit(unit_)
{
}

TraceExpr* Duration::Clone() const
{
    return new Duration(value, unit);
}

void Duration::Evaluate(Stack& stack, FunctionNode& function) const
{
    switch (unit)
    {
        case Unit::seconds:
        {
            std::chrono::seconds duration{ value };
            stack.Push(new Integer(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()));
            break;
        }
        case Unit::milliseconds:
        {
            std::chrono::milliseconds duration{ value };
            stack.Push(new Integer(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()));
            break;
        }
        case Unit::microseconds:
        {
            std::chrono::microseconds duration{ value};
            stack.Push(new Integer(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()));
            break;
        }
        case Unit::nanoseconds:
        {
            std::chrono::nanoseconds duration{ value };
            stack.Push(new Integer(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count()));
            break;
        }
    }
}

Boolean::Boolean(bool value_) : value(value_)
{
}

TraceExpr* Boolean::Clone() const
{
    return new Boolean(value);
}

void Boolean::Evaluate(Stack& stack, FunctionNode& function) const
{
    stack.Push(Clone());
}

TraceExpr* NameField::Clone() const
{
    return new NameField();
}

void NameField::Evaluate(Stack& stack, FunctionNode& function) const
{
    stack.Push(new String(function.Name()));
}

TraceExpr* ThreadField::Clone() const
{
    return new ThreadField();
}

void ThreadField::Evaluate(Stack& stack, FunctionNode& function) const
{
    stack.Push(new String(function.Thread()));
}

TraceExpr* MessageField::Clone() const
{
    return new MessageField();
}

void MessageField::Evaluate(Stack& stack, FunctionNode& function) const
{
    stack.Push(new String(function.Message()));
}

TraceExpr* DurationField::Clone() const
{
    return new DurationField();
}

void DurationField::Evaluate(Stack& stack, FunctionNode& function) const
{
    stack.Push(new Integer(function.Duration()));
}

TraceExpr* TimeField::Clone() const
{
    return new TimeField();
}

void TimeField::Evaluate(Stack& stack, FunctionNode& function) const
{
    stack.Push(new Integer(function.Time()));
}

TraceExpr* LevelField::Clone() const
{
    return new LevelField();
}

void LevelField::Evaluate(Stack& stack, FunctionNode& function) const
{
    stack.Push(new Integer(function.Level()));
}

TraceExpr* Parent::Clone() const
{
    return new Parent();
}

void Parent::Evaluate(Stack& stack, FunctionNode& function) const
{
    TraceNode* parent = function.Parent();
    if (parent)
    {
        stack.Push(new Boolean(parent->Included()));
    }
    else
    {
        stack.Push(new Boolean(false));
    }
}

BinaryExpr::BinaryExpr(TraceExpr* left_, TraceExpr* right_) : left(left_), right(right_)
{
}

void BinaryExpr::TypeCheck() const
{
    left->TypeCheck();
    right->TypeCheck();
}

OrExpr::OrExpr(TraceExpr* left, TraceExpr* right) : BinaryExpr(left, right)
{
}

TraceExpr* OrExpr::Clone() const
{
    return new OrExpr(Left()->Clone(), Right()->Clone());
}

void OrExpr::Evaluate(Stack& stack, FunctionNode& function) const
{
    Left()->Evaluate(stack, function);
    std::unique_ptr<TraceExpr> left = stack.Pop();
    Right()->Evaluate(stack, function);
    std::unique_ptr<TraceExpr> right = stack.Pop();
    Boolean* leftVal = static_cast<Boolean*>(left.get());
    Boolean* rightVal = static_cast<Boolean*>(right.get());
    stack.Push(new Boolean(leftVal->Value() || rightVal->Value()));
}

void OrExpr::TypeCheck() const
{
    BinaryExpr::TypeCheck();
    if (Left()->GetType() != Type::boolean)
    {
        throw std::runtime_error("type checking error: left expression of 'or' expression is not a Boolean expression");
    }
    if (Right()->GetType() != Type::boolean)
    {
        throw std::runtime_error("type checking error: right expression of 'or' expression is not a Boolean expression");
    }
}

AndExpr::AndExpr(TraceExpr* left, TraceExpr* right) : BinaryExpr(left, right)
{
}

TraceExpr* AndExpr::Clone() const
{
    return new AndExpr(Left()->Clone(), Right()->Clone());
}

void AndExpr::Evaluate(Stack& stack, FunctionNode& function) const
{
    Left()->Evaluate(stack, function);
    std::unique_ptr<TraceExpr> left = stack.Pop();
    Right()->Evaluate(stack, function);
    std::unique_ptr<TraceExpr> right = stack.Pop();
    Boolean* leftVal = static_cast<Boolean*>(left.get());
    Boolean* rightVal = static_cast<Boolean*>(right.get());
    stack.Push(new Boolean(leftVal->Value() && rightVal->Value()));
}

void AndExpr::TypeCheck() const
{
    BinaryExpr::TypeCheck();
    if (Left()->GetType() != Type::boolean)
    {
        throw std::runtime_error("type checking error: left expression of 'and' expression is not a Boolean expression");
    }
    if (Right()->GetType() != Type::boolean)
    {
        throw std::runtime_error("type checking error: right expression of 'and' expression is not a Boolean expression");
    }
}

MatchExpr::MatchExpr(TraceExpr* left, TraceExpr* right) : BinaryExpr(left, right)
{
}

TraceExpr* MatchExpr::Clone() const
{
    return new MatchExpr(Left()->Clone(), Right()->Clone());
}

void MatchExpr::Evaluate(Stack& stack, FunctionNode& function) const
{
    Left()->Evaluate(stack, function);
    std::unique_ptr<TraceExpr> left = stack.Pop();
    Right()->Evaluate(stack, function);
    std::unique_ptr<TraceExpr> right = stack.Pop();
    Type leftType = left->GetType();
    Type rightType = right->GetType();
    if (leftType == Type::string && rightType == Type::pattern)
    {
        String* leftString = static_cast<String*>(left.get());
        Pattern* rightPattern = static_cast<Pattern*>(right.get());
        stack.Push(new Boolean(soulng::rex::FilePatternMatch(ToUtf32(leftString->Value()), ToUtf32(rightPattern->Value()))));
    }
    else if (leftType == Type::pattern && rightType == Type::string)
    {
        Pattern* leftPattern = static_cast<Pattern*>(left.get());
        String* rightString = static_cast<String*>(right.get());
        stack.Push(new Boolean(soulng::rex::FilePatternMatch(ToUtf32(rightString->Value()), ToUtf32(leftPattern->Value()))));
    }
}

void MatchExpr::TypeCheck() const
{
    BinaryExpr::TypeCheck();
    if (Left()->GetType() == Type::pattern && Right()->GetType() == Type::string)
    {
        return;
    }
    if (Left()->GetType() == Type::string && Right()->GetType() == Type::pattern)
    {
        return;
    }
    if (Left()->GetType() != Type::pattern && Left()->GetType() != Type::string)
    {
        throw std::runtime_error("type checking error: type of left expression of 'match' expression is not a pattern or string");
    }
    if (Right()->GetType() != Type::pattern && Right()->GetType() != Type::string)
    {
        throw std::runtime_error("type checking error: type of right expression of 'match' expression is not a pattern or string");
    }
    throw std::runtime_error("type checking error: the left operand of a match expression must be a pattern and right operand must be a string, or " 
        "the left operand of a match expression must be a string and right operand must be a pattern");
}

RelationalExpr::RelationalExpr(TraceExpr* left, TraceExpr* right, Operator op_) : BinaryExpr(left, right), op(op_)
{
}

TraceExpr* RelationalExpr::Clone() const
{
    return new RelationalExpr(Left()->Clone(), Right()->Clone(), op);
}

void RelationalExpr::Evaluate(Stack& stack, FunctionNode& function) const
{
    Left()->Evaluate(stack, function);
    std::unique_ptr<TraceExpr> left = stack.Pop();
    Right()->Evaluate(stack, function);
    std::unique_ptr<TraceExpr> right = stack.Pop();
    Type leftType = left->GetType();
    Type rightType = right->GetType();
    if (leftType == Type::string && rightType == Type::string)
    {
        String* leftString = static_cast<String*>(left.get());
        String* rightString = static_cast<String*>(right.get());
        switch (op)
        {
            case Operator::eq:
            {
                stack.Push(new Boolean(leftString->Value() == rightString->Value()));
                break;
            }
            case Operator::neq:
            {
                stack.Push(new Boolean(leftString->Value() != rightString->Value()));
                break;
            }
            case Operator::less:
            {
                stack.Push(new Boolean(leftString->Value() < rightString->Value()));
                break;
            }
            case Operator::greater:
            {
                stack.Push(new Boolean(leftString->Value() > rightString->Value()));
                break;
            }
            case Operator::lessEq:
            {
                stack.Push(new Boolean(leftString->Value() <= rightString->Value()));
                break;
            }
            case Operator::greaterEq:
            {
                stack.Push(new Boolean(leftString->Value() >= rightString->Value()));
                break;
            }
        }
    }
    else if (leftType == Type::integer && rightType == Type::integer)
    {
    Integer* leftInt = static_cast<Integer*>(left.get());
    Integer* rightInt = static_cast<Integer*>(right.get());
    switch (op)
    {
    case Operator::eq:
    {
        stack.Push(new Boolean(leftInt->Value() == rightInt->Value()));
        break;
    }
    case Operator::neq:
    {
        stack.Push(new Boolean(leftInt->Value() != rightInt->Value()));
        break;
    }
    case Operator::less:
    {
        stack.Push(new Boolean(leftInt->Value() < rightInt->Value()));
        break;
    }
    case Operator::greater:
    {
        stack.Push(new Boolean(leftInt->Value() > rightInt->Value()));
        break;
    }
    case Operator::lessEq:
    {
        stack.Push(new Boolean(leftInt->Value() <= rightInt->Value()));
        break;
    }
    case Operator::greaterEq:
    {
        stack.Push(new Boolean(leftInt->Value() >= rightInt->Value()));
        break;
    }
    }
    }
    else if (leftType == Type::boolean && rightType == Type::boolean)
    {
    Boolean* leftBool = static_cast<Boolean*>(left.get());
    Boolean* rightBool = static_cast<Boolean*>(right.get());
    switch (op)
    {
    case Operator::eq:
    {
        stack.Push(new Boolean(leftBool->Value() == rightBool->Value()));
        break;
    }
    case Operator::neq:
    {
        stack.Push(new Boolean(leftBool->Value() != rightBool->Value()));
        break;
    }
    }
    }
    else
    {
    throw std::runtime_error("invalid relational expression");
    }
}

void RelationalExpr::TypeCheck() const
{
    BinaryExpr::TypeCheck();
    Type leftType = Left()->GetType();
    Type rightType = Right()->GetType();
    if (leftType == Type::boolean && rightType == Type::boolean)
    {
        if (op == Operator::eq || op == Operator::neq)
        {
            return;
        }
        else
        {
            throw std::runtime_error("type checking error: Boolean expressions can compared only for equality or inequality");
        }
    }
    else if (leftType == Type::integer && rightType == Type::integer)
    {
        return;
    }
    else if (leftType == Type::string && rightType == Type::string)
    {
        return;
    }
    throw std::runtime_error("type checking error: operands of a relational expression must both be of Boolean, integer or string type");
}

ParenExpr::ParenExpr(TraceExpr* expr_) : expr(expr_)
{
}

TraceExpr* ParenExpr::Clone() const
{
    return new ParenExpr(expr->Clone());
}

void ParenExpr::Evaluate(Stack& stack, FunctionNode& function) const
{
    expr->Evaluate(stack, function);
}

void ParenExpr::TypeCheck() const
{
    expr->TypeCheck();
}

std::unique_ptr<TraceExpr> ParseTraceExpr(const std::string& traceExpr)
{
    std::u32string content = ToUtf32(traceExpr);
    TraceLexer lexer(content, "", 0);
    return TraceParser::Parse(lexer);
}

void TypeCheckTraceExpr(TraceExpr* traceExpr)
{
    if (traceExpr->GetType() != Type::boolean)
    {
        throw std::runtime_error("type checking error: trace expression must be a Boolean expression");
    }
    traceExpr->TypeCheck();
}

}  // namespace cmtrace
