// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_BREAKPOINT_INCLUDED
#define CMAJOR_SERVICE_BREAKPOINT_INCLUDED
#include <cmajor/cmsvc/ServiceApi.hpp>
#include <cmajor/cmmsg/CmdbMessage.hpp>
#include <map>
#include <string>
#include <vector>

namespace cmajor { namespace service {

struct CMSVC_API Breakpoint
{
    Breakpoint(int line_);
    Breakpoint(int line_, const std::string& condition_);
    Breakpoint(int line_, const std::string& condition_, bool disabled_);
    int line;
    std::string condition;
    bool disabled;
    BreakpointInfo info;
};

class CMSVC_API BreakpointList
{
public:
    BreakpointList();
    ~BreakpointList();
    bool IsEmpty() const { return breakpoints.empty(); }
    const std::string& FilePath() const { return filePath; }
    void SetFilePath(const std::string& filePath_);
    const std::vector<Breakpoint*>& Breakpoints() const { return breakpoints; }
    std::vector<Breakpoint*>& Breakpoints() { return breakpoints; }
    void Clear();
    void RemoveBreakpoint(Breakpoint* breakpoint);
    Breakpoint* GetBreakpoint(int line) const;
    void AddBreakpoint(Breakpoint* breakpoint);
private:
    std::string filePath;
    std::vector<Breakpoint*> breakpoints;
};

class CMSVC_API BreakpointCollection
{
public:
    BreakpointList& GetBreakpointList(const std::string& filePath);
    void Clear();
private:
    std::map<std::string, BreakpointList> breakpointListMap;
};

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_BREAKPOINT_INCLUDED
