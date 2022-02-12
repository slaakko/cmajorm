// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_REGION_INCLUDED
#define CMSX_KERNEL_REGION_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Memory.hpp>

namespace cmsx::kernel {

class Process;

enum class RegionId
{
    none, text, data, stack, pool
};

class CMSX_KERNEL_API Region
{
public:
    Region() : id(RegionId::none), start(0), length(0) {}
    Region(RegionId id_, int64_t start_, int64_t length_) : id(id_), start(start_), length(length_) {}
    bool Valid() const { return id != RegionId::none; }
    RegionId Id() const { return id; }
    int64_t Start() const { return start; }
    int64_t Length() const { return length; }
    void SetLength(int64_t length_) { length = length_; }
private:
    RegionId id;
    int64_t start;
    int64_t length;
};

class CMSX_KERNEL_API RegionTable
{
public:
    RegionTable();
    Region GetRegion(RegionId id) const;
    void SetRegion(const Region& region);
    void AddRegion(const Region& region);
    void FreeRegions();
private:
    std::vector<Region> regions;
};

CMSX_KERNEL_API void FreeRegion(Region& region, Process* fromProcess);
CMSX_KERNEL_API void CopyRegion(const Region& region, Process* fromProcess, Process* toProcess);
CMSX_KERNEL_API void ShareRegion(const Region& region, Process* fromProcess, Process* toProcess);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_REGION_INCLUDED
