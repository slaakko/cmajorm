// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/LinearScanRegisterAllocator.hpp>
#include <system-x/intermediate/Context.hpp>
#include <system-x/intermediate/Error.hpp>
#include <soulng/util/Util.hpp>

namespace cmsx::intermediate {

using namespace soulng::util;

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

LinearScanRegisterAllocator::LinearScanRegisterAllocator(Function& function, Context* context_) : frame(), liveRanges(), active(), frameLocations(), registers(), context(context_)
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
        context->GetRegisterPool().AddLocalRegister(reg);
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
    return context->GetRegisterPool().NumFreeLocalRegisters() == 0;
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
    registers[inst] = context->GetRegisterPool().GetLocalRegister();
    LiveRange range = GetLiveRange(inst);
    active.insert(range);
    locations[inst] = locations[inst] | Locations::reg;
}

void LinearScanRegisterAllocator::AllocateFrameLocation(Instruction* inst)
{
    if (inst->IsParamInstruction())
    {
        ParamInstruction* paramInst = static_cast<ParamInstruction*>(inst);
        int64_t size = Align(paramInst->GetType()->Size(), 8); 
        if (size != 8)
        {
            int x = 0;
        }
        frameLocations[paramInst] = frame.GetFrameLocation(size);
        locations[paramInst] = locations[paramInst] | Locations::frame;
    }
    else if (inst->IsLocalInstruction())
    {
        LocalInstruction* localInst = static_cast<LocalInstruction*>(inst);
        int64_t size = Align(localInst->LocalType()->Size(), 8);
        frameLocations[localInst] = frame.GetFrameLocation(size);
        locations[localInst] = locations[localInst] | Locations::frame;
    }
    else
    {
        frameLocations[inst] = frame.GetFrameLocation(8);
        locations[inst] = locations[inst] | Locations::frame;
    }
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

void LinearScanRegisterAllocator::AddRegisterLocation(Instruction* inst, const Register& reg)
{
    locations[inst] = locations[inst] | Locations::reg;
    registers[inst] = reg;
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

std::unique_ptr<LinearScanRegisterAllocator> LinearScanRegisterAllocation(Function& function, Context* context)
{
    std::unique_ptr<LinearScanRegisterAllocator> registerAllocator(new LinearScanRegisterAllocator(function, context));
    return registerAllocator;
}

} // cmsx::intermediate
