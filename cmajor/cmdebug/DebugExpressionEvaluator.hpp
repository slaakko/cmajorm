// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_DEBUG_DEBUG_EXPRESSION_EVALUATOR_INCLUDED
#define CMAJOR_DEBUG_DEBUG_EXPRESSION_EVALUATOR_INCLUDED
#include <cmajor/cmdebug/BoundDebugNodeVisitor.hpp>
#include <cmajor/cmdebug/Container.hpp>
#include <soulng/util/Json.hpp>

namespace cmajor { namespace debug {

using namespace soulng::util;

class Debugger;
class BoundDebugNode;
class DIType;

class DebugExpressionEvaluator : public BoundDebugNodeVisitor
{
public:
    DebugExpressionEvaluator(Debugger& debugger_, const std::string& expression_);
    void Visit(BoundIntegerLiteralNode& node) override;
    void Visit(BoundDebugExpression& expr) override;
    void Visit(BoundSubscriptNode& node) override;
    void Visit(BoundRangeNode& node) override;
    JsonValue* ReleaseResult() { return result.release(); }
    ContainerClassTemplateKind GetContainerKind(DIType* type);
    void Evaluate(BoundDebugNode* node);
    void AddTypes(BoundDebugNode* node);
private:
    Debugger& debugger;
    std::string expression;
    std::unique_ptr<JsonValue> result;
    int64_t integer;
    int64_t rangeStart;
    int64_t rangeEnd;
};

} } // namespace cmajor::debug

#endif // CMAJOR_DEBUG_DEBUG_EXPRESSION_EVALUATOR_INCLUDED
