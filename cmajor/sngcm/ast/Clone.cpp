// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Clone.hpp>
#include <sngcm/ast/Function.hpp>
#include <boost/uuid/nil_generator.hpp>

namespace sngcm { namespace ast {

SpanMapper::~SpanMapper()
{
}

CloneContext::CloneContext() : spanMapper(nullptr), instantiateFunctionNode(false), instantiateClassNode(false), makeTestUnits(false)
{
}

CloneContext::~CloneContext()
{
    for (FunctionNode* unitTestFunction : unitTestFunctions)
    {
        delete unitTestFunction;
    }
}

Span CloneContext::MapSpan(const Span& span, const boost::uuids::uuid& rootModuleId)
{
    if (!rootModuleId.is_nil())
    {
        if (!spanMapper)
        {
            throw std::runtime_error("CloneContext: root module not null and span mapper not set");
        }
        return spanMapper->MapSpan(span, rootModuleId);
    }
    else
    {
        return span;
    }
}

void CloneContext::AddUnitTestFunction(FunctionNode* unitTestFunction)
{
    unitTestFunctions.push_back(unitTestFunction);
}

std::vector<FunctionNode*>& CloneContext::UnitTestFunctions()
{
    return unitTestFunctions;
}

} } // namespace sngcm::ast
