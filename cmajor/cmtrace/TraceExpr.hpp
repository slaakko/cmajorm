// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMTRACE_TRACE_EXPR_INCLUDED
#define CMAJOR_CMTRACE_TRACE_EXPR_INCLUDED
#include <cmajor/cmtrace/TraceApi.hpp>
#include <cmajor/cmtrace/TraceNode.hpp>
#include <cmajor/soulng/rex/Nfa.hpp>

namespace cmtrace {

enum class Operator : int8_t
{
    eq, neq, less, greater, lessEq, greaterEq
};

enum class Type : int8_t
{
    string, pattern, integer, boolean
};

enum class Unit : int8_t
{
    seconds, milliseconds, microseconds, nanoseconds
};

class Stack;

using soulng::rex::Context;
using soulng::rex::Nfa;

class TRACE_API TraceExpr
{
public:
    virtual ~TraceExpr();
    virtual TraceExpr* Clone() const = 0;
    virtual void Evaluate(Stack& stack, FunctionNode& function) const = 0;
    virtual Type GetType() const = 0;
    virtual void TypeCheck() const {}
};

class TRACE_API Stack
{
public:
    Stack();
    Stack(const Stack&) = delete;
    Stack(Stack&&) = delete;
    Stack& operator=(const Stack&) = delete;
    Stack& operator=(Stack&&) = delete;
    void Push(TraceExpr* expr);
    std::unique_ptr<TraceExpr> Pop();
private:
    std::vector<std::unique_ptr<TraceExpr>> stack;
};

std::string MakeString(const std::string& value);

class TRACE_API String: public TraceExpr
{
public:
    String(const std::string& value_);
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::string; }
    const std::string& Value() const { return value; }
private:
    std::string value;
};

std::string MakePattern(const std::string& value);

class TRACE_API Pattern : public TraceExpr
{
public:
    Pattern(const std::string& value_);
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::pattern; }
    const std::string& Value() const { return value; }
private:
    std::string value;
};

TRACE_API int64_t ToInteger(const std::u32string& s);

class TRACE_API Integer : public TraceExpr
{
public:
    Integer(int64_t value_);
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::integer; }
    int64_t Value() const { return value; }
private:
    int64_t value;
};

class TRACE_API Duration : public TraceExpr
{
public:
    Duration(int64_t value_, Unit unit_);
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::integer; }
    int64_t Value() const { return value; }
private:
    int64_t value;
    Unit unit;
};

class TRACE_API Boolean : public TraceExpr
{
public:
    Boolean(bool value_);
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::boolean; }
    bool Value() const { return value; }
private:
    bool value;
};

class TRACE_API NameField : public TraceExpr
{
public:
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::string; }
};

class TRACE_API ThreadField : public TraceExpr
{
public:
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::string; }
};

class TRACE_API MessageField : public TraceExpr
{
public:
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::string; }
};

class TRACE_API DurationField : public TraceExpr
{
public:
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::integer; }
};

class TRACE_API TimeField : public TraceExpr
{
public:
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::integer; }
};

class TRACE_API LevelField : public TraceExpr
{
public:
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::integer; }
};

class TRACE_API Parent : public TraceExpr
{
public:
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::boolean; }
};

class TRACE_API BinaryExpr : public TraceExpr
{
public:
    BinaryExpr(TraceExpr* left_, TraceExpr* right_);
    BinaryExpr(const BinaryExpr&) = delete;
    BinaryExpr(BinaryExpr&&) = delete;
    BinaryExpr& operator=(const BinaryExpr&) = delete;
    BinaryExpr& operator=(BinaryExpr&&) = delete;
    TraceExpr* Left() const { return left.get(); }
    TraceExpr* Right() const { return right.get(); }
    void TypeCheck() const override;
private:
    std::unique_ptr<TraceExpr> left;
    std::unique_ptr<TraceExpr> right;
};

class TRACE_API OrExpr : public BinaryExpr
{
public:
    OrExpr(TraceExpr* left, TraceExpr* right);
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::boolean; }
    void TypeCheck() const override;
};

class TRACE_API AndExpr : public BinaryExpr
{
public:
    AndExpr(TraceExpr* left, TraceExpr* right);
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::boolean; }
    void TypeCheck() const override;
};

class TRACE_API MatchExpr : public BinaryExpr
{
public:
    MatchExpr(TraceExpr* left, TraceExpr* right);
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::boolean; }
    void TypeCheck() const override;
};

class TRACE_API RelationalExpr : public BinaryExpr
{
public:
    RelationalExpr(TraceExpr* left, TraceExpr* right, Operator op_);
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return Type::boolean; }
    void TypeCheck() const override;
private:
    Operator op;
};

class TRACE_API ParenExpr : public TraceExpr
{
public:
    ParenExpr(TraceExpr* expr_);
    TraceExpr* Clone() const override;
    void Evaluate(Stack& stack, FunctionNode& function) const override;
    Type GetType() const override { return expr->GetType(); }
    void TypeCheck() const override;
private:
    std::unique_ptr<TraceExpr> expr;
};

TRACE_API std::unique_ptr<TraceExpr> ParseTraceExpr(const std::string& traceExpr);
TRACE_API void TypeCheckTraceExpr(TraceExpr* traceExpr);

}  // namespace cmtrace

#endif // CMAJOR_CMTRACE_TRACE_EXPR_INCLUDED
