// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Region.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Processor.hpp>

namespace cmsx::kernel {

std::string RegionIdStr(RegionId id)
{
    switch (id)
    {
        case RegionId::none: return "none";
        case RegionId::text: return "text";
        case RegionId::data: return "data";
        case RegionId::stack: return "stack";
        case RegionId::pool: return "pool";
    }
    return std::string();
}

RegionTable::RegionTable()
{
}

Region RegionTable::GetRegion(RegionId id) const
{
    for (const auto& reg : regions)
    {
        if (reg.Id() == id)
        {
            return reg;
        }
    }
    return Region();
}

void RegionTable::SetRegion(const Region& region)
{
    for (auto& reg : regions)
    {
        if (reg.Id() == region.Id())
        {
            reg = region;
        }
    }
}

void RegionTable::AddRegion(const Region& region)
{
    for (const auto& reg : regions)
    {
        if (reg.Id() == region.Id())
        {
            throw SystemError(EALREADYEXISTS, "region '" + RegionIdStr(region.Id()) + "' already exists", __FUNCTION__);
       }
    }
    regions.push_back(region);
}

void RegionTable::FreeRegions()
{
    regions.clear();
}

void FreeRegion(Region& region, Process* fromProcess)
{
    cmsx::machine::Memory& mem = fromProcess->GetProcessor()->GetMachine()->Mem();
    mem.FreeRange(fromProcess->RV(), region.Start(), region.Length());
    region = Region();
}

void CopyRegion(const Region& region, Process* fromProcess, Process* toProcess)
{
    cmsx::machine::Memory& mem = fromProcess->GetProcessor()->GetMachine()->Mem();
    mem.CopyRange(fromProcess->RV(), toProcess->RV(), region.Start(), region.Length());
    toProcess->GetRegionTable().AddRegion(region);
}

void ShareRegion(const Region& region, Process* fromProcess, Process* toProcess)
{
    cmsx::machine::Memory& mem = fromProcess->GetProcessor()->GetMachine()->Mem();
    mem.ShareRange(fromProcess->RV(), toProcess->RV(), region.Start(), region.Length());
    toProcess->GetRegionTable().AddRegion(region);
}

} // namespace cmsx::kernel
