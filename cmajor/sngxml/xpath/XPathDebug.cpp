// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/xpath/XPathDebug.hpp>

namespace sngxml { namespace xpath {

class XPathDebugInfo
{
public:
    static void Init();
    static void Done();
    static XPathDebugInfo& Instance();
    bool DebugParsing() const { return debugParsing; }
    void SetDebugParsing() { debugParsing = true; }
    bool DebugQuery() const { return debugQuery; }
    void SetDebugQuery() { debugQuery = true; }
    void SetQueryDom(std::unique_ptr<sngxml::dom::Node>&& queryDom_) { queryDom = std::move(queryDom_); }
    std::unique_ptr<sngxml::dom::Node> GetQueryDom() { return std::move(queryDom); }
    void SetQueryDuration(std::chrono::nanoseconds duration) { queryDuration = duration; }
    std::chrono::nanoseconds GetQueryDuration() const { return queryDuration; }
    void SetExecuteDuration(std::chrono::nanoseconds duration) { executeDuration = duration; }
    std::chrono::nanoseconds GetExecuteDuration() const { return executeDuration; }
private:
    static std::unique_ptr<XPathDebugInfo> instance;
    XPathDebugInfo();
    bool debugParsing;
    bool debugQuery;
    std::unique_ptr<sngxml::dom::Node> queryDom;
    std::chrono::nanoseconds queryDuration;
    std::chrono::nanoseconds executeDuration;
};

std::unique_ptr<XPathDebugInfo> XPathDebugInfo::instance;

XPathDebugInfo::XPathDebugInfo() : debugParsing(false), debugQuery(false)
{
}

void XPathDebugInfo::Init()
{
    instance.reset(new XPathDebugInfo());
}

void XPathDebugInfo::Done()
{
    instance.reset();
}

XPathDebugInfo& XPathDebugInfo::Instance()
{
    return *instance;
}

bool XPathDebugParsing()
{
    return XPathDebugInfo::Instance().DebugParsing();
}

void SetXPathDebugParsing()
{
    XPathDebugInfo::Instance().SetDebugParsing();
}

bool XPathDebugQuery()
{
    return XPathDebugInfo::Instance().DebugQuery();
}

void SetXPathDebugQuery()
{
    XPathDebugInfo::Instance().SetDebugQuery();
}

void SetXPathQueryDom(std::unique_ptr<sngxml::dom::Node>&& queryDom)
{
    XPathDebugInfo::Instance().SetQueryDom(std::move(queryDom));
}

std::unique_ptr<sngxml::dom::Node> GetXPathQueryDom()
{
    return XPathDebugInfo::Instance().GetQueryDom();
}

void SetXPathQueryDuration(std::chrono::nanoseconds duration)
{
    XPathDebugInfo::Instance().SetQueryDuration(duration);
}

std::chrono::nanoseconds XPathQueryDuration()
{
    return XPathDebugInfo::Instance().GetQueryDuration();
}

void SetXPathExecuteDuration(std::chrono::nanoseconds duration)
{
    XPathDebugInfo::Instance().SetExecuteDuration(duration);
}

std::chrono::nanoseconds XPathExecuteDuration()
{
    return XPathDebugInfo::Instance().GetExecuteDuration();
}

void InitDebug()
{
    XPathDebugInfo::Init();
}

void DoneDebug()
{
    XPathDebugInfo::Done();
}

} } // namespace sngxml::xpath
