// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/RegisterAllocator.hpp>
#include <system-x/intermediate/AssemblyConfig.hpp>
#include <system-x/assembler/Constant.hpp>
#include <system-x/machine/Registers.hpp>
#include <stdexcept>

namespace cmsx::intermediate {

ArgLocation::ArgLocation(int index_) : index(index_), constant(nullptr)
{
}

void ArgLocation::SetConstant(cmsx::assembler::HexadecimalConstant* constant_)
{
    constant = constant_;
}

void ArgLocation::SetValue(uint64_t start)
{
    constant->SetValue(start + static_cast<uint64_t>(index + 1) * 8);
}

CallFrame::CallFrame() : saveNumLocals(0)
{
}

void CallFrame::NextArgLocation(cmsx::assembler::HexadecimalConstant* constant)
{
    ArgLocation* argLocation = new ArgLocation(argLocations.size());
    argLocation->SetConstant(constant);
    argLocations.push_back(std::unique_ptr<ArgLocation>(argLocation));
}

void CallFrame::Resolve(int frameSize)
{
    uint64_t start = frameSize;
    start = start + static_cast<uint64_t>(8) * (static_cast<uint64_t>(3) + saveNumLocals);
    for (auto& argLocation : argLocations)
    {
        argLocation->SetValue(start);
    }
}

Frame::Frame() : nextFrameLocationIndex(0)
{
    ResetCallFrame();
}

void Frame::ResetCallFrame()
{
    currentCallFrame.reset(new CallFrame());
}

void Frame::AddCallFrame()
{
    callFrames.push_back(std::move(currentCallFrame));
    ResetCallFrame();
}

void Frame::ResolveCallFrames()
{
    int frameSize = Size();
    for (auto& callFrame : callFrames)
    {
        callFrame->Resolve(frameSize);
    }
}

std::unique_ptr<RegisterPool> RegisterPool::instance;

void RegisterPool::Init()
{
    instance.reset(new RegisterPool());
}

void RegisterPool::Done()
{
    instance.reset();
}

RegisterPool::RegisterPool() : localRegisterCount(GetNumLocalRegs())
{
    for (int i = 0; i < localRegisterCount; ++i)
    {
        localRegisterPool.insert(Register(RegisterKind::local, static_cast<uint8_t>(i)));
    }
    globalRegisterMap[cmsx::machine::regAX] = Register(RegisterKind::global, cmsx::machine::regAX);
    globalRegisterMap[cmsx::machine::regBX] = Register(RegisterKind::global, cmsx::machine::regBX);
    globalRegisterMap[cmsx::machine::regCX] = Register(RegisterKind::global, cmsx::machine::regCX);
    globalRegisterMap[cmsx::machine::regDX] = Register(RegisterKind::global, cmsx::machine::regDX);
    globalRegisterMap[cmsx::machine::regEX] = Register(RegisterKind::global, cmsx::machine::regEX);
    globalRegisterMap[cmsx::machine::regIX] = Register(RegisterKind::global, cmsx::machine::regIX);
    globalRegisterMap[cmsx::machine::regFP] = Register(RegisterKind::global, cmsx::machine::regFP);
    globalRegisterMap[cmsx::machine::regSP] = Register(RegisterKind::global, cmsx::machine::regSP);
}

Register RegisterPool::GetGlobalRegister(uint8_t number)
{
    auto it = globalRegisterMap.find(number);
    if (it != globalRegisterMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("invalid global register number " + std::to_string(number));
    }
}

void RegisterPool::AddLocalRegister(const Register& reg)
{
    localRegisterPool.insert(reg);
}

Register RegisterPool::GetLocalRegister()
{
    if (localRegisterPool.empty())
    {
        throw std::runtime_error("register pool is empty");
    }
    Register reg = *localRegisterPool.begin();
    localRegisterPool.erase(reg);
    return reg;
}

RegisterAllocator::~RegisterAllocator()
{
}

Register GetGlobalRegister(uint8_t registerNumber)
{
    return RegisterPool::Instance().GetGlobalRegister(registerNumber);
}

Register GetLocalRegister()
{
    return RegisterPool::Instance().GetLocalRegister();
}

void InitRegisterAllocator()
{
    RegisterPool::Init();
}

void DoneRegisterAllocator()
{
    RegisterPool::Done();
}

} // cmsx::intermediate
