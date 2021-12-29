// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/LinearScanRegisterAllocator.hpp>
#include <system-x/intermediate/Error.hpp>

namespace cmsx::intermediate {

LiveRange GetLiveRange(Instruction* inst)
{
    if (inst->RequiresLocalRegister())
    {
        int lastUserIndex = inst->Index();
        for (Instruction* user : inst->Users())
        {
            if (user->Index() > lastUserIndex)
            {
                lastUserIndex = user->Index();
            }
        }
        LiveRange range(inst->Index(), lastUserIndex);
        return range;
    }
    else
    {
        return LiveRange(inst->Index(), inst->Index());
    }
}

LinearScanRegisterAllocator::LinearScanRegisterAllocator(Function& function) : frame(), liveRanges(), active(), frameLocations(), registers()
{
    ComputeLiveRanges(function);
}

void LinearScanRegisterAllocator::AddLiveRange(const LiveRange& liveRange, Instruction* inst)
{
    liveRanges.insert(liveRange);
    instructionRangeMap[inst] = liveRange;
    rangeInstructionMap[liveRange].push_back(inst);
    if (inst->IsParamInstruction() || inst->IsLocalInstruction())
    {
        AllocateFrameLocation(inst);
    }
}

void LinearScanRegisterAllocator::AddFreeRegToPool(Instruction* inst)
{
    Register reg = GetRegister(inst);
    if (reg.Valid())
    {
        RegisterPool::Instance().AddLocalRegister(reg);
        RemoveRegister(inst);
    }
}

void LinearScanRegisterAllocator::RemoveFromActive(const LiveRange& range)
{
    active.erase(range);
    for (Instruction* inst : GetInstructions(range))
    {
        locations[inst] = locations[inst] & ~Locations::reg;
    }
}

bool LinearScanRegisterAllocator::NoFreeRegs() const
{
    return RegisterPool::Instance().NumFreeLocalRegisters() == 0;
}

Register LinearScanRegisterAllocator::GetRegister(Instruction* inst) const
{
    auto it = registers.find(inst);
    if (it != registers.cend())
    {
        return it->second;
    }
    else
    {
        return Register();
    }
}

void LinearScanRegisterAllocator::RemoveRegister(Instruction* inst)
{
    registers.erase(inst);
}

FrameLocation LinearScanRegisterAllocator::GetFrameLocation(Instruction* inst) const
{
    auto it = frameLocations.find(inst);
    if (it != frameLocations.cend())
    {
        return it->second;
    }
    else
    {
        return FrameLocation();
    }
}

void LinearScanRegisterAllocator::AllocateRegister(Instruction* inst)
{
    registers[inst] = RegisterPool::Instance().GetLocalRegister();
    LiveRange range = GetLiveRange(inst);
    active.insert(range);
    locations[inst] = locations[inst] | Locations::reg;
}

void LinearScanRegisterAllocator::AllocateFrameLocation(Instruction* inst)
{
    frameLocations[inst] = frame.NextFrameLocation();
    locations[inst] = locations[inst] | Locations::frame;
}

void LinearScanRegisterAllocator::Spill(Instruction* inst)
{
    spillDataVec.clear();
    LiveRange spill = *--active.cend();
    LiveRange range = GetLiveRange(inst);
    for (Instruction* instToSpill : GetInstructions(spill))
    {
        registers[inst] = registers[instToSpill];
        AllocateFrameLocation(instToSpill);
        locations[instToSpill] = Locations::frame;
        locations[inst] = locations[inst] | Locations::reg;
        active.erase(spill);
        active.insert(range);
        SpillData spillData;
        spillData.registerToSpill = registers[instToSpill];
        spillData.spillToFrameLocation = frameLocations[instToSpill];
        spillDataVec.push_back(spillData);
    }
}

LiveRange LinearScanRegisterAllocator::GetLiveRange(Instruction* inst) const
{
    auto it = instructionRangeMap.find(inst);
    if (it != instructionRangeMap.cend())
    {
        return it->second;
    }
    else
    {
        return LiveRange(inst->Index(), inst->Index());
    }
}

const std::vector<Instruction*>& LinearScanRegisterAllocator::GetInstructions(const LiveRange& range) const
{
    static std::vector<Instruction*> empty;
    auto it = rangeInstructionMap.find(range);
    if (it != rangeInstructionMap.cend())
    {
        return it->second;
    }
    else
    {
        return empty;
    }
}

void LinearScanRegisterAllocator::ComputeLiveRanges(Function& function)
{
    BasicBlock* block = function.FirstBasicBlock();
    while (block)
    {
        Instruction* inst = block->FirstInstruction();
        while (inst)
        {
            LiveRange liveRange = cmsx::intermediate::GetLiveRange(inst);
            if (liveRange.Valid())
            {
                AddLiveRange(liveRange, inst);
            }
            inst = inst->Next();
        }
        block = block->Next();
    }
}

void LinearScanRegisterAllocator::ExpireOldRanges(const LiveRange& range)
{
    std::vector<LiveRange> toRemove;
    for (const auto& activeRange : active)
    {
        if (activeRange.end >= range.start) break;
        toRemove.push_back(activeRange);
        for (Instruction* inst : GetInstructions(activeRange))
        {
            AddFreeRegToPool(inst);
        }
    }
    for (const auto& r : toRemove)
    {
        RemoveFromActive(r);
    }
}

Locations LinearScanRegisterAllocator::GetLocations(Instruction* inst) const
{
    auto it = locations.find(inst);
    if (it != locations.cend())
    {
        return it->second;
    }
    return Locations::none;
}

int LinearScanRegisterAllocator::LastActiveLocalReg() const
{
    int lastActiveLocalReg = -1;
    for (const auto& instReg : registers)
    {
        const Register& reg = instReg.second;
        if (reg.IsLocal())
        {
            if (reg.number > lastActiveLocalReg)
            {
                lastActiveLocalReg = reg.number;
            }
        }
    }
    return lastActiveLocalReg;
}

RegisterAllocationAction LinearScanRegisterAllocator::Run(Instruction* inst)
{
    LiveRange liveRange = GetLiveRange(inst);
    ExpireOldRanges(liveRange);
    if (inst->RequiresLocalRegister())
    {
        if (NoFreeRegs())
        {
            Spill(inst);
            return RegisterAllocationAction::spill;
        }
        else
        {
            AllocateRegister(inst);
            return RegisterAllocationAction::registerAllocated;
        }
    }
    else
    {
        return RegisterAllocationAction::none;
    }
}

const std::vector<SpillData>& LinearScanRegisterAllocator::GetSpillData() const
{
    return spillDataVec;
}

std::unique_ptr<LinearScanRegisterAllocator> LinearScanRegisterAllocation(Function& function)
{
    std::unique_ptr<LinearScanRegisterAllocator> registerAllocator(new LinearScanRegisterAllocator(function));
    return registerAllocator;
}

} // cmsx::intermediate
