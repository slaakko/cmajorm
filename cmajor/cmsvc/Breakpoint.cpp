// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/Breakpoint.hpp>
#include <algorithm>

namespace cmajor { namespace service {

Breakpoint::Breakpoint(int line_) : line(line_), condition(), disabled(false), info(), list(nullptr)
{
}

Breakpoint::Breakpoint(int line_, const std::string& condition_) : line(line_), condition(condition_), disabled(false), info(), list(nullptr)
{
}

Breakpoint::Breakpoint(int line_, const std::string& condition_, bool disabled_) : line(line_), condition(condition_), disabled(disabled_), info(), list(nullptr)
{
}

BreakpointList::BreakpointList() : breakpointCollection(nullptr), filePath(), breakpoints()
{
}

BreakpointList::~BreakpointList()
{
    Clear();
}

void BreakpointList::SetFilePath(const std::string& filePath_)
{
    filePath = filePath_;
}

void BreakpointList::Clear()
{
    for (Breakpoint* bp : breakpoints)
    {
        delete bp;
    }
    breakpoints.clear();
    breakpointCollection->SetChanged();
}

void BreakpointList::RemoveBreakpoint(Breakpoint* breakpoint)
{
    Breakpoint* bp = GetBreakpoint(breakpoint->line);
    if (bp)
    {
        breakpoints.erase(std::remove(breakpoints.begin(), breakpoints.end(), breakpoint), breakpoints.end());
        delete breakpoint;
        breakpointCollection->SetChanged();
    }
}

struct ByLine
{
    bool operator()(Breakpoint* left, Breakpoint* right) const
    {
        return left->line < right->line;
    }
};

Breakpoint* BreakpointList::GetBreakpoint(int line) const
{
    if (IsEmpty()) return nullptr;
    Breakpoint bp(line);
    auto it = std::lower_bound(breakpoints.cbegin(), breakpoints.cend(), &bp, ByLine());
    if (it != breakpoints.cend())
    {
        Breakpoint* foundBp = *it;
        if (foundBp->line == line)
        {
            return foundBp;
        }
    }
    return nullptr;
}

void BreakpointList::AddBreakpoint(Breakpoint* breakpoint)
{
    Breakpoint* prev = GetBreakpoint(breakpoint->line);
    if (prev)
    {
        throw std::runtime_error("already has breakpoint on line " + std::to_string(breakpoint->line));
    }
    breakpoint->list = this;
    breakpoints.push_back(breakpoint);
    std::sort(breakpoints.begin(), breakpoints.end(), ByLine());
    breakpointCollection->SetChanged();
}

BreakpointCollection::BreakpointCollection() : changed(false)
{
}

BreakpointList& BreakpointCollection::GetBreakpointList(const std::string& filePath)
{
    BreakpointList& breakpointList = breakpointListMap[filePath];
    breakpointList.SetBreakpointCollection(this);
    if (breakpointList.FilePath().empty())
    {
        breakpointList.SetFilePath(filePath);
    }
    return breakpointList;
}

void BreakpointCollection::Clear()
{
    breakpointListMap.clear();
    changed = true;
}

} } // namespace cmajor::service