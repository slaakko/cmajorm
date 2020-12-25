// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmtrace/TraceNode.hpp>

namespace cmtrace {

TraceNode::TraceNode() : parent(nullptr)
{
}

TraceNode::~TraceNode()
{
}

ContainerNode::ContainerNode()
{
}

void ContainerNode::AddChildNode(TraceNode* childNode)
{
    childNodes.push_back(std::unique_ptr<TraceNode>(childNode));
    childNode->SetParent(this);
}

FunctionNode::FunctionNode(int64_t time_, int32_t level_, const std::string& thread_, const std::string& name_) : time(time_), level(level_), thread(thread_), name(name_), duration(0), included(false)
{
}

void FunctionNode::SetMessage(const std::string& message_)
{
    message = message_;
}

}  // namespace cmtrace
