// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundNode.hpp>

namespace cmajor { namespace binder {

BoundNode::BoundNode(Module* module_, const Span& span_, BoundNodeType boundNodeType_) : module(module_), span(span_), boundNodeType(boundNodeType_)
{
}

} } // namespace cmajor::binder
