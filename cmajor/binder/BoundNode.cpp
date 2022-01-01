// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundNode.hpp>

namespace cmajor { namespace binder {

BoundNode::BoundNode(const Span& span_, const boost::uuids::uuid& moduleId_, BoundNodeType boundNodeType_) : span(span_), moduleId(moduleId_), boundNodeType(boundNodeType_)
{
}

} } // namespace cmajor::binder
