// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmtrace/Trace.hpp>
#include <cmajor/cmtrace/TraceExpr.hpp>
#include <sngxml/dom/Element.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Trace.hpp>
#include <soulng/util/Unicode.hpp>
#include <memory>
#include <map>
#include <fstream>

namespace cmtrace {

using namespace soulng::util;
using namespace soulng::unicode;

std::string TimeStr(int64_t nanosecs)
{
    std::string timeStr;
    int64_t ns = nanosecs % 1000;
    int64_t us = (nanosecs / 1000) % 1000;
    int64_t ms = (nanosecs / (1000ll * 1000)) % 1000;
    int64_t s = (nanosecs / (1000ll * 1000 * 1000)) % 60;
    int64_t m = (nanosecs / (1000ll * 1000 * 1000 * 60)) % 60;
    int64_t h = (nanosecs / (1000ll * 1000 * 1000 * 60 * 60));
    timeStr.append(std::to_string(h / 10)).append(std::to_string(h % 10)).append(1, ':').append(std::to_string(m / 10)).append(std::to_string(m % 10)).append(1, ':');
    timeStr.append(std::to_string(s / 10)).append(std::to_string(s % 10)).append(1, '.').append(std::to_string(ms / 100)).append(std::to_string((ms % 100) / 10)).append(std::to_string(ms % 10));
    timeStr.append(1, '.').append(std::to_string(us / 100)).append(std::to_string((us % 100) / 10).append(std::to_string(us % 10)));
    timeStr.append(1, '.').append(std::to_string(ns / 100)).append(std::to_string((ns % 100) / 10).append(std::to_string(ns % 10)));
    return timeStr;
}

struct Thread
{
    Thread(char id_) : id(id_), indent(0) {}
    char id;
    int indent;
    std::vector<TraceEntry*> startEntries;
};

class ThreadMap
{
public:
    Thread* GetThread(char threadId);
private:
    std::vector<std::unique_ptr<Thread>> threads;
    std::map<char, Thread*> threadMap;
};

Thread* ThreadMap::GetThread(char threadId)
{
    auto it = threadMap.find(threadId);
    if (it != threadMap.cend())
    {
        return it->second;
    }
    Thread* thread = new Thread(threadId);
    threads.push_back(std::unique_ptr<Thread>(thread));
    threadMap[threadId] = thread;
    return thread;
}

std::unique_ptr<ContainerNode> ReadTrace(const std::string& traceFilePath)
{
    std::unique_ptr<ContainerNode> root(new ContainerNode());
    ContainerNode* currentContainer = root.get();
    ThreadMap threadMap;
    ReadTraceTable();
    std::vector<TraceEntry> trace = soulng::util::ReadTrace(traceFilePath);
    std::vector<ContainerNode*> containers;
    int32_t n = trace.size();
    for (int32_t i = 0; i < n; ++i)
    {
        TraceEntry& entry = trace[i];
        int64_t time = entry.nanosecs;
        Thread* thread = threadMap.GetThread(entry.threadId);
        if (entry.kind == EntryKind::begin)
        {
            int32_t level = thread->indent;
            std::string name = GetTraceFunctionName(entry.id);
            while (thread->indent >= thread->startEntries.size())
            {
                thread->startEntries.push_back(nullptr);
            }
            thread->startEntries[thread->indent] = &entry;
            std::string threadName(1, thread->id);
            std::unique_ptr<FunctionNode> functionPtr(new FunctionNode(time, level, threadName, name));
            FunctionNode* function = functionPtr.get();
            currentContainer->AddChildNode(functionPtr.release());
            containers.push_back(currentContainer);
            currentContainer = function;
            ++thread->indent;
        }
        else if (entry.kind == EntryKind::end)
        {
            if (thread->indent > 0)
            {
                --thread->indent;
            }
            FunctionNode* function = static_cast<FunctionNode*>(currentContainer);
            if (thread->startEntries.size() > thread->indent)
            {
                TraceEntry* start = thread->startEntries[thread->indent];
                if (start)
                {
                    if (start->id == entry.id)
                    {
                        int64_t duration = entry.nanosecs - start->nanosecs;
                        function->SetDuration(duration);
                        thread->startEntries[thread->indent] = nullptr;
                        currentContainer = containers.back();
                        containers.pop_back();
                    }
                }
            }
        }
        else if (entry.kind == EntryKind::message)
        {
            FunctionNode* function = static_cast<FunctionNode*>(currentContainer);
            function->SetMessage(GetTraceMessageName(entry.id));
        }
    }
    return root;
}

void Filter(ContainerNode* container, TraceExpr* filter)
{
    int32_t count = container->Count();
    for (int32_t i = 0; i < count; ++i)
    {
        TraceNode* child = (*container)[i];
        if (child->IsFunctionNode())
        {
            FunctionNode* function = static_cast<FunctionNode*>(child);
            Stack stack;
            filter->Evaluate(stack, *function);
            std::unique_ptr<TraceExpr> value = stack.Pop();
            Boolean* booleanValue = static_cast<Boolean*>(value.get());
            if (booleanValue->Value() == true)
            {
                function->SetIncluded();
            }
            Filter(function, filter);
        }
    }
}

void Pick(ContainerNode* container, ContainerNode* target)
{
    int32_t count = container->Count();
    for (int32_t i = 0; i < count; ++i)
    {
        TraceNode* child = (*container)[i];
        if (child->IsFunctionNode())
        {
            FunctionNode* function = static_cast<FunctionNode*>(child);
            ContainerNode* tgt = target;
            if (function->Included())
            {
                TraceNode* child = container->Release(i);
                target->AddChildNode(child);
                if (child->IsFunctionNode())
                {
                    tgt = static_cast<FunctionNode*>(child);
                }
            }
            Pick(function, tgt);
        }
    }
}

std::unique_ptr<ContainerNode> FilterTrace(ContainerNode* root, const std::string& filterExpr)
{
    std::unique_ptr<TraceExpr> filter = ParseTraceExpr(filterExpr);
    TypeCheckTraceExpr(filter.get());
    Filter(root, filter.get());
    std::unique_ptr<ContainerNode> filteredTrace(new ContainerNode());
    Pick(root, filteredTrace.get());
    return filteredTrace;
}

void WriteText(ContainerNode* container, std::ostream& stream, bool filtered)
{
    int32_t count = container->Count();
    for (int32_t i = 0; i < count; ++i)
    {
        TraceNode* child = (*container)[i];
        if (!child) continue;
        if (child->IsFunctionNode())
        {
            FunctionNode* function = static_cast<FunctionNode*>(child);
            if (!filtered || function->Included())
            {
                std::string levelStr = std::to_string(function->Level() / 10) + std::to_string(function->Level() % 10);
                stream << TimeStr(function->Time()) << " " << function->Thread() << " " << levelStr << " " << std::string(function->Level(), ' ') << ">" << function->Name() <<
                    ":duration=" << TimeStr(function->Duration());
                if (!function->Message().empty())
                {
                    stream << ":message=" << function->Message();
                }
                stream << std::endl;
            }
            WriteText(function, stream, filtered);
            if (!filtered || function->Included())
            {
                std::string levelStr = std::to_string(function->Level() / 10) + std::to_string(function->Level() % 10);
                stream << TimeStr(function->Time() + function->Duration()) << " " << function->Thread() << " " << levelStr << " " << std::string(function->Level(), ' ') << "<" << function->Name() <<
                    ":duration=" << TimeStr(function->Duration());
                if (!function->Message().empty())
                {
                    stream << ":message=" << function->Message();
                }
                stream << std::endl;
            }
        }
    }
}

void WriteTextFile(ContainerNode* root, const std::string& textFilePath, const std::string& filterExpr)
{
    std::ofstream textFile(textFilePath);
    if (!filterExpr.empty())
    {
        textFile << "filter=\"" << filterExpr << "\"" << std::endl;
    }
    WriteText(root, textFile, !filterExpr.empty());
}

void ToXml(ContainerNode* container, sngxml::dom::Element* element, bool filtered)
{
    int32_t count = container->Count();
    for (int32_t i = 0; i < count; ++i)
    {
        TraceNode* child = (*container)[i];
        if (!child) continue;
        sngxml::dom::Element* containerElement = element;
        if (child->IsFunctionNode())
        {
            FunctionNode* function = static_cast<FunctionNode*>(child);
            if (!filtered || function->Included())
            {
                sngxml::dom::Element* functionElement = new sngxml::dom::Element(U"function");
                functionElement->SetAttribute(U"time", ToUtf32(TimeStr(function->Time())));
                functionElement->SetAttribute(U"level", ToUtf32(std::to_string(function->Level())));
                functionElement->SetAttribute(U"thread", ToUtf32(function->Thread()));
                functionElement->SetAttribute(U"name", ToUtf32(function->Name()));
                if (!function->Message().empty())
                {
                    functionElement->SetAttribute(U"message", ToUtf32(function->Message()));
                }
                functionElement->SetAttribute(U"duration", ToUtf32(TimeStr(function->Duration())));
                containerElement->AppendChild(std::unique_ptr<sngxml::dom::Element>(functionElement));
                containerElement = functionElement;
            }
            ToXml(function, containerElement, filtered);
        }
    }
}

std::unique_ptr<sngxml::dom::Document> TraceToXml(ContainerNode* root, const std::string& filterExpr)
{
    std::unique_ptr<sngxml::dom::Document> doc(new sngxml::dom::Document());
    doc->AppendChild(std::unique_ptr<sngxml::dom::Element>(new sngxml::dom::Element(U"trace")));
    if (!filterExpr.empty())
    {
        doc->DocumentElement()->SetAttribute(U"filter", ToUtf32(filterExpr));
    }
    ToXml(root, doc->DocumentElement(), !filterExpr.empty());
    return doc;
}

}  // namespace cmtrace
