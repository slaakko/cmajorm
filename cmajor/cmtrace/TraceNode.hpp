// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMTRACE_TRACE_NODE_INCLUDED
#define CMAJOR_CMTRACE_TRACE_NODE_INCLUDED
#include <cmajor/cmtrace/TraceApi.hpp>
#include <memory>
#include <string>
#include <vector>
#include <stdint.h>

namespace cmtrace {

class ContainerNode;

class TRACE_API TraceNode
{
public:
    TraceNode();
    virtual ~TraceNode();
    ContainerNode* Parent() const { return parent; }
    void SetParent(ContainerNode* parent_) { parent = parent_; }
    virtual bool Included() const { return false; }
    virtual bool IsFunctionNode() const { return false; }
private:
    ContainerNode* parent;
};

class TRACE_API ContainerNode : public TraceNode
{
public:
    ContainerNode();
    ContainerNode(const ContainerNode&) = delete;
    ContainerNode(ContainerNode&&) = delete;
    ContainerNode& operator=(const ContainerNode&) = delete;
    ContainerNode& operator=(ContainerNode&&) = delete;
    void AddChildNode(TraceNode* childNode);
    TraceNode* Release(int32_t i) { return childNodes[i].release(); }
    int32_t Count() const { return childNodes.size(); }
    TraceNode* operator[](int32_t i) const { return childNodes[i].get(); }
private:
    std::vector<std::unique_ptr<TraceNode>> childNodes;
};

class TRACE_API FunctionNode : public ContainerNode
{
public:
    FunctionNode(int64_t time_, int32_t level_, const std::string& thread_, const std::string& name_);
    void SetDuration(int64_t duration_) { duration = duration_; }
    const std::string& Thread() const { return thread; }
    const std::string& Name() const { return name; }
    const std::string& Message() const { return message; }
    void SetMessage(const std::string& message_);
    int64_t Duration() const { return duration; }
    int64_t Time() const { return time; }
    int64_t Level() const { return level; }
    bool Included() const override { return included; }
    void SetIncluded() { included = true; }
    bool IsFunctionNode() const override { return true; }
private:
    int64_t time;
    int32_t level;
    std::string thread;
    std::string name;
    std::string message;
    int64_t duration;
    bool included;
};

}  // namespace cmtrace

#endif // CMAJOR_CMTRACE_TRACE_NODE_INCLUDED
